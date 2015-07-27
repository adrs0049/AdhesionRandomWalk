import sys, os, time
import numpy as np
import pickle
import multiprocessing as mp
import queue
import datetime

import math
import logging

import simulator as s
from simulator import DVector

from randomWalk_db import *
from swig import *
from log import *

import plot as plt

logging.basicConfig(format='[%(asctime)s] %(message)s', \
                    datefmt='%d/%m/%Y %H:%M:%S',\
                    level=logging.INFO)

def timeit(method):

    def timed(*args, **kwargs):
        ts = time.time()
        result = method(*args, **kwargs)
        te = time.time()

        print ('%r (%r, %r) %2.2f sec' % \
               (method.__name__, args, kwargs, te-ts))

        return result

    return timed

class Player(object):
    def __init__(self, parameters):

        # open database connection
        self.db = RandomWalkDB()

        # parameters are a dict. First check if it exists in database
        self.param = self.db.param_create_if_not_exist(parameters)
        self.dparam = parameters

        # store id to the most recent randomwalk
        self.runId = -1

        # number of paths to generate
        self.NoPaths = 64

        # use multiprocessors
        self.multiprocessing = True

        # some temp vars
        self.Diffusion = 0
        self.Drift = 0
        self.R = 0
        self.DomainN = 0

    # TODO REVIEW ALL THOSE getter and setters!!
    def getDomainSize(self): return float(self.param.DomainSize)
    def getDomainSizeL(self): return float(self.param.DomainSize) * float(self.param.DomainN)
    def getDomainShape(self): return (0.0, self.getDomainSize)
    def getDomainLeftBoundary(self): return 0.0
    def getDomainRightBoundary(self): return self.getDomainSize()
    def getDiffusionCoeff(self): return float(self.param.diffusion_coeff)
    def getDriftCoeff(self): return float(self.param.drift_coeff)
    def getStepSize(self): return float(1.0 / self.param.DomainN)
    def getSensingRadius(self): return float(self.param.R)
    def getSensingRadiusL(self): return self.param.getSensingRadiusL()
    def getLatticeSize(self): return self.getDomainSizeL()
    def getFinalTime(self): return self.FinalTime
    # TODO do something that is actually python standard
    def getVersion(self): return '0.1'
    def setSimId(self, simId):
        try:
            self.db.getSimulationFromId(simId)
        except InvalidEntry as e:
            print('Simulation id %d does not exist in database!' % simId)
        except:
            raise

        self.runId = simId

    # DEBUG STUFF
    def getParameters(self): return self.param
    def getDB(self):
        # TODO figure out why i have to do this, otherwise database
        # returns an old state??
        self.db = RandomWalkDB()
        return self.db

    def getNewConnection(self): return RandomWalkDB()
    def setNewDB(self): self.db = self.getNewConnection()

    def getCellPathFromDB(self, simId=None):
        return self.db.returnPathsForSim(simId)

    def doHistogram(self, simId=None, Compare=False):
        p = plt.Plotter(simId, Compare=Compare)
        p()

    def checkParametersInDB(self):
        self.db.param_create_if_not_exist(self.param)

    @timeit
    def runSimulations(self, FinalTimes):

        feedback_q = mp.Queue()
        p = mp.Process(target=self.__runSimulations__,
                       args=(feedback_q, FinalTimes,))
        try:
            p.start()

            while p.is_alive():
                fb = feedback_q.get()

                # Poison pill!
                if fb == 'DONE':
                    break

                if fb["type"] == "error":
                    print("ERROR in " + str(fb["process"]) + ": "\
                       + fb["message"] + "\n" )
                    p.terminate()
                    for child in mp.active_children():
                        child.terminate()
                else:
                    print(datetime.datetime.fromtimestamp(fb["timestamp"]).\
                          strftime('%Y-%m-%d %H:%M:%S') + " " + \
                          str(fb["process"]) + ": " + fb["message"])

        except KeyboardInterrupt:
            print('keyboard interrupt')
            p.terminate()
            for child in mp.active_children():
                child.terminate()

        except:
            raise

        finally:
            p.join()

    """ run several simulations """
    def __runSimulations__(self, feedback_q, FinalTimes):

        print('Preparing simulation...')
        # NoPaths, number of paths to generate for each config
        NoPaths = self.NoPaths

        # create sim object in database
        self.prepareSimulation()

        # starts consumers
        num = 1
        if self.multiprocessing:
            num = mp.cpu_count()

        print('Simulation id is %d. Running %d paths for each time. The '\
              'total number of simulations is %d. The final times are ' \
              % (self.runId, NoPaths, len(FinalTimes) * NoPaths), end='')
        print(" ".join(str(x) for x in FinalTimes), end='')
        print('. Executing %d simulations in parallel, on %d CPUs.' \
              % (num, mp.cpu_count()))

        print('Starting simulating...')

        lock = mp.Lock()
        tasks = mp.JoinableQueue()
        # Why does this work better?
        m = mp.Manager()
        results = m.Queue()
        #results = mp.Queue()

        print('Creating %d simulations' % num)
        simulations = [Consumer(tasks, results, feedback_q, lock)
                       for i in range(num) ]
        writers = [storePath(results, feedback_q, self.runId, self.getVersion())
                    for i in range(1)]

        print('Starting processes...')
        for sim in simulations:
            sim.start()

        print('Starting writers...')
        for writer in writers:
            writer.start()

        # enqueue jobs
        print('Enqueuing simulations')
        for i in range(NoPaths):
            tasks.put(Simulation(FinalTimes, self.dparam, i))

        print('Appending poison pill...')
        # send poison pill to all simulations
        for sim in simulations:
            tasks.put(None)

        print('Waiting for simulations to finish...')
        tasks.join()

        print('Killing mysql thread...')
        for writer in writers:
            results.put(None)
            writer.join()

        print('Finished Simulation %d' %(self.runId))
        feedback_q.put('DONE')

        return

    """ create simulation object in database """
    def prepareSimulation(self):
        # create new record in the database
        if self.runId == -1:
            self.runId = self.db.createSimulation('space-jump simulation', self.param.id)

        self.DomainN = self.param.DomainN
        self.Diffusion = self.getDiffusionCoeff()
        self.Drift = self.getDriftCoeff()
        self.R = self.getSensingRadius()

class SafeProcess(mp.Process):

    def __init__(self, feedback_queue, lock = None):
        mp.Process.__init__(self)
        self.lock = lock
        self.feedback_queue = feedback_queue

    def log_info(self, message):
        log_info(self.feedback_queue, message, self.name)

    def log_error(self, err):
        log_error(self.feedback_queue, err, self.name)

    def log_debug(self, message):
        log_debug(self.feedback_queue, message, self.name)

    def saferun(self):
        if self._target:
            self._target(*self._args, **self._kwargs)

    def run(self):
        try:
            self.saferun()
        except Exception as e:
            self.log_error(e)
            raise e
        return

class storePath(SafeProcess):

    def __init__(self, q, feedback_queue, runId, version):
        SafeProcess.__init__(self, feedback_queue)
        self.runId = runId
        self.version = version
        self.db = RandomWalkDB()
        self.q = q

    def __str__(self):
        return "Executing storePath with sim_id '%d' and on version '%s'." \
                % (self.runId, self.version)

    def saferun(self):
        proc_name = self.name
        while True:
            try:
                next_result = self.q.get(timeout=100)
            except queue.Empty:
                logging.info('%s: Result queue is still empty!' % proc_name)
                continue

            if next_result == None:
                logging.info('%s: Exiting' % proc_name)
                return

            logging.info('%s: Storing path for time %2.2f and simulation '
                         '%d' % (proc_name, next_result[0], \
                                 self.runId))

            self.db.storePath(next_result[1], next_result[0],
                              self.runId, self.version, next_result[2])
        return

class Consumer(SafeProcess):

    def __init__(self, task_queue, result_queue, feedback_queue, lock):
        SafeProcess.__init__(self, feedback_queue, lock)
        self.task_queue = task_queue
        self.result_queue = result_queue
        self.feedback_queue = feedback_queue

    def saferun(self):
        proc_name = self.name
        while True:
            next_task = self.task_queue.get()
            if next_task is None:
                logging.info('%s: Exiting' % proc_name)
                self.task_queue.task_done()
                break
            logging.info('%s: %s' % (proc_name, next_task))
            try:
                next_task(self)
            except:
                raise
            finally:
                self.task_queue.task_done()
        return

    def __call__(self, result):
        self.lock.acquire()
        try:
            self.put_in_queue(result)
        finally:
            self.lock.release()

    def put_in_queue(self, result):
        #print('HELLO putting in queue')
        try:
            self.result_queue.put(result)
        except queue.Full:
            print('QUEUE WAS FULL')
        except:
            raise

class Simulation(object):

    def __init__(self, FinalTimes, parameters, simulation_id):

        assert (np.max(FinalTimes)>0), "Number of steps has to be larger 0"
        if np.max(FinalTimes)>1E3:
            print ('WARNING number of steps is very large!!')

        self.FinalTimes = FinalTimes
        self.parameters = parameters
        self.simulation_id = simulation_id

        self.call = None

    def __str__(self):
        return "Executing Simulation %d up to '%f'." \
                % (self.simulation_id, np.max(self.FinalTimes))

    # FIXME use a unidirectional pipe here!!!
    # the callback function
    def callback(self, *args, **kwargs):
        # get steps + path
        data = kwargs
        steps = data["steps"]
        FinalTime = np.around(data["time"], decimals=2)
        path = np.asarray(data["states"])

        # package up the data
        test_data = (FinalTime, path, steps, )

        logging.info('Simulator %d: Send path for time %2.2f with %d steps ' \
                     'to MySQL writer.' %(self.simulation_id, FinalTime, steps))

        # call consumer callback
        self.call(test_data)

    def __call__(self, callback):

        # set callback function!
        self.call = callback

        # FIXME
        domainSizeHalf = int(self.parameters["DomainSize"]/2)
        domainShape = DVector([-domainSizeHalf, domainSizeHalf])

        if isinstance(self.FinalTimes, np.ndarray):
            self.FinalTimes = self.FinalTimes.tolist()

        try:

            swig_param = PickalableParameters(domainShape,
                                              self.parameters["DomainN"],
                                              DVector(self.FinalTimes))

            swig_param.setDiffusion(self.parameters["diffusion_coeff"])
            swig_param.setDrift(self.parameters["drift_coeff"])
            swig_param.setSensingRadius(self.parameters["R"])

            # always use uniform ic here
            swig_param.setIcType(self.parameters["ic_type"])
            swig_param.setRandomWalkType(self.parameters["rw_type"])
            swig_param.setOmegaP(self.parameters["omega_p"])
            swig_param.setLambda(50.0)
            swig_param.setIcP(int(self.parameters["ic_p"]))
            swig_param.update()
            swig_param.print_info()

            logging.info('Creating simulator %d.' % self.simulation_id)
            # create sim object
            sim = PickalableSimulator(swig_param)

        except:
            raise

        # register callback function
        sim.registerPyListener(self)

        # run the simulation
        sim.run()

        return

if __name__ == '__main__':

    # TODO read this from an XML file
    param = dict(DomainSize=10, DomainN=16,
                 diffusion_coeff=1.0, drift_coeff=0.003,
                 R=1.0, omega_type=s.OMEGA_TYPE_UNIFORM, omega_p=0.82, g_type=1,
                 u0=0.8, bcs='pp', ic_type=s.IC_TYPE_TRIG_NOISE, ic_p=100.0,
                 rw_type=s.RANDOMWALK_TYPE_ADHESION,
                 space_type=s.SPACE_TYPE_ALWAYS_FREE,
                 adhesivity_type=s.ADHESIVITY_TYPE_SIMPLE)

    # setup player
    player = Player(param)
