import sys, os, time
import numpy as np
import pandas as pd
import pickle
import multiprocessing as mp

import math
import matplotlib.pyplot as plt

import simulator as s
from simulator import DVector

from diffusion import *
from heat import *
from randomWalk_db import *

class Player(object):
    def __init__(self, parameters):

        # open database connection
        self.db = RandomWalkDB()

        # parameters are a dict. First check if it exists in database
        self.param = self.db.param_create_if_not_exist(parameters)

        # setup database connection
        self.db = RandomWalkDB()

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
            db.getSimulationFromId(simId)
        except InvalidEntry as e:
            print('Simulation id %d does not exist in database!' % simId)
        except:
            raise

        self.runId = simId

    # DEBUG STUFF
    def getParameters(self): return self.param
    def getDB(self): return self.db

    def getCellPathFromDB(self, simId=None):
        return db.returnPathsForSim(simId)

    def doHistogram(self, simId=None, bar_width=None):
        if not bar_width: bar_width = self.getStepSize()

        sim = db.getSimulationFromId(simId)
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
                       self.getDomainRightBoundary() + self.getStepSize(),
                       bar_width)

            print('shape bins=', np.shape(bins))
            print('shape x=', np.shape(x))

            # get diffusion soln
            xd, u = self.computeDiffusionSoln(key, self.getDiffusionCoeff())

            # get yet another exact soln to double check
            #ad = AD(shape=(self.getDomainSizeL(), ), L = self.getDomainSize()/2,
            #        D=self.getDiffusionCoeff(), c=0.0)

            #u2 = ad.eval(key)
            #x2+=5

            # try yet another diffusion solver
            N = 100
            x2 = np.arange(-self.getDomainLeftBoundary(),
                           self.getDomainRightBoundary(),
                           self.getDomainSize()/N)
            assert len(x2)==N, ""
            u_ic = np.zeros(N)

            print(len(bins))

            start=int( (N / self.getDomainSize()) * (5.0))
            finish= int((N / self.getDomainSize()) * (5.0 + bar_width))

            print('start=', start, ' end=', finish, ' N=',N)

            u_ic[start:finish]=0.5

            assert len(u_ic)==N, ""

            time_step = key * self.getDomainSize()* self.getDiffusionCoeff()
            solver = FFTHeat1D_test(u_ic, time_step, self.getDomainSizeL())
            solver.time_step()
            u2 = solver.get_x()

            #return x2, u2

            # plotting
            plt.bar(bins[:-1], x, width=bar_width)
            plt.plot(xd, u, color='k')

            print('x2=', np.shape(x2), ' u2=', np.shape(u2))
            plt.plot(x2, u2, color='g')

            # plot error
            error = u - x
            plt.plot(xd, error, color='r')

            print('shape u=', np.shape(u))
            print('shape x=', np.shape(x))
            self.print_error(u, x)

            plt.title('Results of space-jump process with %d players at %f'\
                      % (total, key))

            #ax.xlim(min(bins), max(bins))
            plt.show()

    def plotIC(self):
        bar_width = self.h
        x=self.getCellPath()
        bins=np.arange(-self.L, self.L + self.h, bar_width)

        print('shape bins=', np.shape(bins))
        print('shape x=', np.shape(x))

        # get diffusion soln
        xd, u = self.computeDiffusionSoln(FinalTime=0.0)

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
        db.param_create_if_not_exist(self.param)

    """ run several simulations """
    def runSimulations(self, FinalTimes, NoPlayers=500):

        print('Preparing simulation...')
        # NoPaths, number of paths to generate for each config
        NoPaths = 10
        ftimes = [t for t in FinalTimes for _ in range(NoPaths) ]

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

        print('Creating %d consumers' % num)
        consumers = [Consumer(tasks, results) for i in range(num) ]

        for w in consumers:
            w.start()

        # enqueue jobs
        for t in ftimes:
            tasks.put(Simulation(NoPlayers, t, self.DomainN, \
                                 self.Diffusion, self.Drift, self.R))

        print('Killing all consumers')
        for i in range(num):
            tasks.put(None)

        print('Waiting for simulations to finish...')
        tasks.join()

        print('Storing results in database...')
        while results.qsize() > 0:
            result = results.get()
            db.storePath(result[1], result[0], self.runId, self.getVersion())

        print('Finished Simulation %d' %(self.runId))

    """ create simulation object in database """
    def prepareSimulation(self):
        # create new record in the database
        if self.runId == -1:
            self.runId = db.createSimulation('space-jump simulation', self.param.id)

        self.DomainN = self.param.DomainN
        self.Diffusion = self.getDiffusionCoeff()
        self.Drift = self.getDriftCoeff()
        self.R = self.getSensingRadius()

class Consumer(mp.Process):

    def __init__(self, task_queue, result_queue):
        mp.Process.__init__(self)
        self.task_queue = task_queue
        self.result_queue = result_queue

    def run(self):
        proc_name = self.name
        while True:
            next_task = self.task_queue.get()
            if next_task is None:
                print('%s: Exiting' % proc_name)
                self.task_queue.task_done()
                break
            print('%s: %s' % (proc_name, next_task))
            answer = next_task()
            self.task_queue.task_done()
            self.result_queue.put(answer)
        return

# move somewhere else
class PickalableSWIG:
    def __setstate__(self, state):
        self.__init__(*state['args'])

    def __getstate__(self):
        return {'args' : self.args}

class PickalableParameters(s.Parameters, PickalableSWIG):

    def __init__(self, *args):
        self.args = args
        s.Parameters.__init__(self, *args)

class PickalableSimulator(s.Simulator, PickalableSWIG):

    def __init__(self, *args):
        self.args = args
        s.Simulator.__init__(self, *args)

class Simulation(object):

    def __init__(self, NoPlayers, FinalTime, DomainN, Diffusion, Drift, R):

        assert (NoPlayers>0), "Number of players has to be larger 0"
        assert (FinalTime>0), "Number of steps has to be larger 0"
        if FinalTime>1E3: print ('WARNING number of steps is very large!!')

        self.NoPlayers = NoPlayers
        self.FinalTime = FinalTime
        self.DomainN = DomainN
        self.Diffusion = Diffusion
        self.Drift = Drift
        self.R = R

    def __str__(self):
        return "Executing Simulation up to '%f' with '%d' players." \
                % (self.FinalTime, self.NoPlayers)

    def __call__(self):

        # domainN stores points per unit length
        stepSize = 1.0 / self.DomainN

        # FIXME
        domainShape = DVector([-5.0, 5.0])

        # get workable swig parameters
        swig_param = PickalableParameters(domainShape, stepSize, self.FinalTime,
                                          self.NoPlayers)

        swig_param.setDiffusion(self.Diffusion)
        swig_param.setDrift(self.Drift)
        swig_param.setSensingRadius(self.R)

        #print(swig_param)
        #print('setup sim')

        # create sim object
        sim = PickalableSimulator(swig_param)
        #self.sim = s.Simulator(self.swig_param)
        #print('print info')
        #sim._print()
        #time.sleep(1)

        # run the simulation
        sim.run()

        # return only the cell path with finaltime
        return self.FinalTime, np.asarray(sim.getPath())

if __name__ == '__main__':

    # TODO read this from an XML file
    param = dict(DomainSize=10, DomainN=10,
                 diffusion_coeff=0.5, drift_coeff=0.0,
                 R=1.0, omega_type=1, omega_p=0.42, g_type=1,
                 u0=0.8, bcs='pp', ic_type=1, ic_p=0.1)

    player = Player(param)
    db=player.getDB()

    #player.runSimulation()
