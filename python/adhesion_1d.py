import math
import matplotlib.pyplot as plt
import sys
from collections import namedtuple
import numpy as np
import simulator as s
from simulator import DVector
import time

from diffusion import *
from randomWalk_db import *

def getValues(l, name):
  return [getattr(r, name) for r in l]

class Player(object):
    def __init__(self, parameters):

        # open database connection
        self.db = RandomWalkDB()

        # parameters are a dict. First check if it exists in database
        self.param = self.db.param_create_if_not_exist(parameters)

        # some variables used later
        self.FinalTime = 0
        # FIXME required for plotting read this value also from db
        self.NoPlayers = 500
        self.swig_param = 0

        # sim object
        self.sim = 0

        # setup database connection
        self.db = RandomWalkDB()

        # store id to the most recent randomwalk
        self.runId = -1

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

    # DEBUG STUFF
    def getParameters(self): return self.param
    def getDB(self): return self.db

    def getCellPath(self):
        return np.asarray(self.sim.getPath())

    def getCellPathFromDB(self):
        if self.runId==-1:
            self.runId = db.getMostRecentRandomWalk()
            print self.runId

        rw = db.getRandomWalkFromId(self.runId)
        path = rw.path

        # set final time otherwise continuum soln is wrong
        self.FinalTime = float(rw.final_time)
        # get parameters belonging to rw
        self.param = rw.parameter

        return [float(x.population) for x in path]

    def getCellLocations(self):
        weight = np.asarray(self.sim.getPath())
        domain = np.arange(-self.L, self.L, self.h)
        #print( np.shape(weight) )
        #print( np.shape(domain))
        #print( weight )
        #print( domain )        
        return np.multiply(weight, domain)

    def doHistogram(self, bar_width=None, fromDb=True):
        if not bar_width: bar_width = self.getStepSize()

        if fromDb:
            x = self.getCellPathFromDB()
        else:
            x=self.getCellPath()

        bins=np.arange(-self.getDomainLeftBoundary(), 
                        self.getDomainRightBoundary() + self.getStepSize(),
                        bar_width)

        print('shape bins=', np.shape(bins))
        print('shape x=', np.shape(x))

        # get diffusion soln
        xd, u = self.computeDiffusionSoln()
        
        # plotting
        plt.bar(bins[:-1], x, width=bar_width)
        plt.plot(xd, u, color='k')

        # plot error
        error = u - x
        plt.plot(xd, error, color='r')

        print('shape u=', np.shape(u))
        print('shape x=', np.shape(x))
        self.print_error(u, x)
    
        #plt.bar(x)
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

    def computeDiffusionSoln(self, FinalTime=None):
        N=self.getDomainSizeL()
        Nhalf = N/2

        if not FinalTime==None:
            T = FinalTime
        else:
            T=self.getFinalTime()

        print('Solving heat equation using the FFT')
        print('N=',N,' T=',T,' D=', self.getDiffusionCoeff())
        print('The domain is [', self.getDomainLeftBoundary(), ', ', \
                self.getDomainRightBoundary(),']')

        # domain is [-L, L]
        xd=np.linspace(self.getDomainLeftBoundary(), self.getDomainRightBoundary(), N)
        u0=np.zeros(N)
        u0[N/2]=self.NoPlayers/2.0
        u0[N/2+1]=self.NoPlayers/2.0

        if FinalTime==0.0:
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

    def setupSim(self, NoPlayers=100, NoSteps=1000):
        self.updateValues(NoPlayers, NoSteps)
        self.sim = s.Simulator(self.param)

    def printSim(self):
        self.sim._print()

    def getSwigParameters(self):
        swig_param = s.Parameters

        stepSize = 1.0 / self.param.DomainN

        # FIXME
        domainShape = DVector([-5.0, 5.0])

        swig_param = s.Parameters(domainShape, stepSize, self.FinalTime,
                self.NoPlayers)

        swig_param.setDiffusion(self.getDiffusionCoeff())
        swig_param.setDrift(self.getDriftCoeff())
        swig_param.setSensingRadius(self.getSensingRadius())

        return swig_param

    def checkParametersInDB(self):
        db.param_create_if_not_exist(self.param)

    def prepareSimulation(self, NoPlayers, FinalTime):

        self.NoPlayers = NoPlayers
        self.FinalTime = FinalTime

        self.swig_param = self.getSwigParameters()

        # create new record in the database
        self.runId = db.createRandomWalk(self.getVersion(), self.param.id,
                FinalTime)

        # create sim object
        self.sim = s.Simulator(self.swig_param)
        self.sim._print()
        time.sleep(1)

    """ Method to safe generated path to the database """
    def finishSimulation(self, FinalTime):

        # get stateVector
        stateVector = self.getCellPath()

        # send the stateVector to the database
        # TODO actually save the current time!
        db.storePath(stateVector, FinalTime, self.runId)

        # Don't ever overwrite the database -> unset runId
        # self.runId = -1

    def runSimulation(self, NoPlayers=500, FinalTime=0.15):
        assert (NoPlayers>0), "Number of players has to be larger 0"
        assert (FinalTime>0), "Number of steps has to be larger 0"
        if FinalTime>1E3: print ('WARNING number of steps is very large!!')

        self.prepareSimulation(NoPlayers, FinalTime)

        # run the simulation
        self.sim.run()

        # finish simulation
        self.finishSimulation(FinalTime)

if __name__ == '__main__':

    # TODO read this from an XML file
    param = dict(DomainSize=10, DomainN=10,
                 diffusion_coeff=0.5, drift_coeff=0.0,
                 R=1.0, omega_type=1, omega_p=0.42, g_type=1,
                 u0=0.8, bcs='pp', ic_type=1, ic_p=0.1)

    player = Player(param)
    db=player.getDB()

    #player.runSimulation()
