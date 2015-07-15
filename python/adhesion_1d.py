import sys, os, time
import numpy as np
import pandas as pd
import pickle
import multiprocessing as mp
import datetime

import math
import matplotlib.pyplot as plt

import simulator as s
from simulator import DVector

from diffusion import *
from heat import *
from randomWalk_db import *
from swig import *
from log import *

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
    def setDiffusionCoeff(self, D): self.swig_param.setDiffusion(D)
    def setDriftCoeff(self, c): self.swig_param.setDrift(c)
    def setStepSize(self, h): self.swig_param.setDiscreteX(h)
    def setSensingRadius(self, R): self.swig_param.setSensingRadius(R)
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
    def getDB(self): return self.db
    def getNewConnection(self): return RandomWalkDB()
    def setNewDB(self): self.db = self.getNewConnection()

    def getCellPathFromDB(self, simId=None):
        return self.db.returnPathsForSim(simId)

    def doHistogram(self, simId=None, bar_width=None, Compare=False):
        if not bar_width: bar_width = self.getStepSize()

        sim = self.db.getSimulationFromId(simId)
        ddf = self.getCellPathFromDB(simId)
        self.param = sim.Parameters

        assert isinstance(ddf, dict), "df is expected to be a dict"

        if not len(ddf) > 0:
            print('Data returned from simulation %d is empty!' % self.runId)
            return

        print('Plotting for times ', end='')
        print(" ".join(str(x) for x in ddf.keys()))
        print('.')

        for key, df in ddf.items():
            try:
                x = df['avg']
            except KeyError:
                print('Data returned from simulation %d is empty!' % self.runId)
                return df
            except:
                raise

            total = np.sum(x)
            x = x / total

            bins=np.arange(-self.getDomainLeftBoundary(),
                       self.getDomainRightBoundary(),
                       bar_width)

            print('shape bins=', np.shape(bins))
            print('shape x=', np.shape(x))

            if Compare:

                # get diffusion soln
                xd, u = self.computeDiffusionSoln(key, self.getDiffusionCoeff())

                # try yet another diffusion solver
                N = self.getDomainSize() * self.param.DomainN

                if N<2000:
                    print('Using a larger N')
                    N = 2**12

                x2 = np.arange(-self.getDomainLeftBoundary(),
                               self.getDomainRightBoundary(),
                               self.getDomainSize()/N)
                assert len(x2)==N, ""
                u_ic = np.zeros(N)

                print(len(bins))

                # index is zero based -> midpoint one lower
                mid = int(N/2)-1

                print('start=', mid-1, ' end=', mid+2, ' N=',N)

                #u_ic[mid-1:mid+2]=0.33
                u_ic = vg(x2, 1.0 / self.param.DomainN)
                print('u_ic total=', np.sum(u_ic))
                assert len(u_ic)==N, ""

                solver = FFTHeat1D_test(u_ic, key, self.getDomainSize(), \
                                        self.getDiffusionCoeff(), \
                                        self.getDriftCoeff())
                solver.time_step()
                u2 = solver.get_x()

            if Compare:
                y_max = max(np.max(x), np.max(u2))
            else:
                y_max = np.max(x)

            print('y_max=', y_max, ' u2=', np.max(u2))
            plt.ylim(0, 1.1 * y_max)

            plt.plot(xd, x, color='r')

            if Compare:
                print('x2=', np.shape(x2), ' u2=', np.shape(u2))
                plt.plot(x2, u2, color='g')
                #plt.plot(x2, u_ic, color='k')
                print('u2 total=', np.sum(u2))

            print('total sim=', np.sum(x))

            plt.title('Results of space-jump process with %d players at %f'\
                      % (total, key))

            plt.savefig('plot_'+str(simId)+'_'+str(key)+'.png')
            #ax.xlim(min(bins), max(bins))
            plt.show()

    def plotIC(self, sim):
        bar_width = self.h
        x=sim.getCellPath()
        bins=np.arange(-self.L, self.L + self.h, bar_width)

        print('shape bins=', np.shape(bins))
        print('shape x=', np.shape(x))

        # get diffusion soln
        xd, u = self.computeDiffusionSoln(FinalTime=0.0)

        N = 100
        x2 = np.arange(-self.getDomainLeftBoundary(),
                       self.getDomainRightBoundary(),
                       self.getDomainSize()/N)
        assert len(x2)==N, ""
        u_ic = np.zeros(N)

        print(len(bins))

        # index is zero based -> midpoint one lower
        mid = int(N/2)-1

        print('start=', mid-1, ' end=', mid+2, ' N=',N)

        u_ic[mid-1:mid+2]=0.33

        assert len(u_ic)==N, ""

        time_step = key * self.getDomainSize()* self.getDiffusionCoeff()
        solver = FFTHeat1D_test(u_ic, time_step, self.getDomainSizeL())
        solver.time_step()
        u2 = solver.get_x()

        # plotting
        plt.bar(bins[:-1], x, width=bar_width)
        plt.plot(xd, u, color='k')

        # plot error
        error = u - x
        plt.plot(xd, error, color='r')

        print('shape u=', np.shape(u))
        print('shape x=', np.shape(x))
        self.print_error(u, x)

        plt.ylim(0,1000)
        plt.show()

    def computeDiffusionSoln(self, T, D):
        N=self.getDomainSizeL()
        Nhalf = N/2

        print('Solving heat equation using the FFT')
        print('N=',N,' T=',T,' D=', self.getDiffusionCoeff())
        print('The domain is [', self.getDomainLeftBoundary(), ', ', \
              self.getDomainRightBoundary(),']')

        # domain is [-L, L]
        xd=np.linspace(self.getDomainLeftBoundary(), self.getDomainRightBoundary(), N)
        u0=np.zeros(N)
        u0[N/2]=1.0/2.0
        u0[N/2+1]=1.0/2.0

        if T==0.0:
            return xd, u0

        # IC
        F = np.fft.fft(u0)
        k = np.append(np.arange(Nhalf), np.arange(-Nhalf,0,1))
        k2 = self.getDiffusionCoeff() * k**2
        W=np.exp(-k2 * T)
        Ft = F * W
        u = np.real(np.fft.ifft(Ft))

        return xd, u

    # Compute norm between vectors x, y
    def norm1(self, x, y):
        return np.linalg.norm(x-y, ord=1)

    # compute l2 between vectors x, y
    def norm2(self, x, y):
        return np.linalg.norm(x-y, ord=2)

    def print_error(self, x, y):
        print('The L1 error=', self.norm1(x, y))
        print('The L2 error=', self.norm2(x, y))

    def checkParametersInDB(self):
        self.db.param_create_if_not_exist(self.param)

    @timeit
    def runSimulations(self, FinalTimes, NoPlayers=500):

        feedback_q = mp.Queue()
        p = mp.Process(target=self.__runSimulations__,
                       args=(feedback_q, FinalTimes, NoPlayers,))
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
    def __runSimulations__(self, feedback_q, FinalTimes, NoPlayers=500):

        print('Preparing simulation...')
        # NoPaths, number of paths to generate for each config
        NoPaths = 10

        # create sim object in database
        self.prepareSimulation()

        # starts consumers
        num = 1
        if self.multiprocessing:
            num = mp.cpu_count() + 1

        print('Simulation id is %d. Running %d paths for each time. The '\
              'total number of simulations is %d. The final times are ' \
              % (self.runId, NoPaths, len(FinalTimes) * NoPaths), end='')
        print(" ".join(str(x) for x in FinalTimes), end='')
        print('. Executing %d simulations in parallel, on %d CPUs.' \
              % (num, mp.cpu_count()))

        print('Starting simulating...')

        tasks = mp.JoinableQueue()
        results = mp.Queue()

        print('Creating %d simulations' % num)
        simulations = [Consumer(tasks, results, feedback_q)
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
            tasks.put(Simulation(NoPlayers, FinalTimes, self.dparam))

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

    def __init__(self, feedback_queue):
        mp.Process.__init__(self)
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
            next_result = self.q.get()
            if next_result == None:
                print('%s: Exiting' % proc_name)
                return

            print('%s: Storing path for time %f and simulation %d'
                  % (proc_name, next_result[0], self.runId))

            self.db.storePath(next_result[1], next_result[0],
                              self.runId, self.version, next_result[2])
        return

class Consumer(SafeProcess):

    def __init__(self, task_queue, result_queue, feedback_queue):
        mp.Process.__init__(self)
        self.task_queue = task_queue
        self.result_queue = result_queue
        self.feedback_queue = feedback_queue

    def saferun(self):
        proc_name = self.name
        while True:
            next_task = self.task_queue.get()
            if next_task is None:
                print('%s: Exiting' % proc_name)
                self.task_queue.task_done()
                break
            print('%s: %s' % (proc_name, next_task))
            next_task(self.result_queue)
            self.task_queue.task_done()
        return

class Simulation(object):

    def __init__(self, NoPlayers, FinalTimes, parameters):

        assert (NoPlayers>0), "Number of players has to be larger 0"
        assert (np.max(FinalTimes)>0), "Number of steps has to be larger 0"
        if np.max(FinalTimes)>1E3: print ('WARNING number of steps is very large!!')

        self.NoPlayers = NoPlayers
        self.FinalTimes = FinalTimes
        self.parameters = parameters

    def __str__(self):
        return "Executing Simulation up to '%f' with '%d' players." \
                % (np.max(self.FinalTimes), self.NoPlayers)

    def __call__(self, result_queue):

        # domainN stores points per unit length
        stepSize = 1.0 / self.parameters["DomainN"]

        # FIXME
        domainSizeHalf = int(self.parameters["DomainSize"]/2)
        domainShape = DVector([-domainSizeHalf, domainSizeHalf])

        if isinstance(self.FinalTimes, np.ndarray):
            self.FinalTimes = self.FinalTimes.tolist()

        try:

            swig_param = PickalableParameters(domainShape, stepSize,
                                          DVector(self.FinalTimes),
                                          self.NoPlayers)

            swig_param.setDiffusion(self.parameters["diffusion_coeff"])
            swig_param.setDrift(self.parameters["drift_coeff"])
            swig_param.setSensingRadius(self.parameters["R"])

            # always use uniform ic here
            swig_param.setIcType(self.parameters["ic_type"])
            swig_param.setRandomWalkType(self.parameters["rw_type"])

            print('create simulator')
            # create sim object
            sim = PickalableSimulator(swig_param)

        except:
            raise

        # the callback function
        def storePath(*args, **kwargs):
            # get steps + path
            data = kwargs
            steps = data["steps"]
            FinalTime = np.around(data["time"], decimals=2)
            path = np.asarray(data["states"])

            result_queue.put((FinalTime, path, steps, ))

        # register callback function
        sim.registerPyListener(storePath)

        # run the simulation
        sim.run()

        return

if __name__ == '__main__':

    # TODO read this from an XML file
    param = dict(DomainSize=10, DomainN=10,
                 diffusion_coeff=1.0, drift_coeff=20,
                 R=1.0, omega_type=1, omega_p=0.42, g_type=1,
                 u0=0.8, bcs='pp', ic_type=s.IC_TYPE_UNIFORM, ic_p=0.1,
                 rw_type=s.RANDOMWALK_TYPE_ADHESION)

    player = Player(param)
    db=player.getDB()

    #player.runSimulation()
