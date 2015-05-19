import math
import matplotlib.pyplot as plt
import sys
from collections import namedtuple
import numpy as np
import simulator as s
import time

def getValues(l, name):
  return [getattr(r, name) for r in l]    

class Player(object):
    def __init__(self):
        self.steps = 1000
        self.D = 1
        self.A = 1
        
        # domain size, domain is [-L, L]
        self.L = 5 
        # step size 
        self.h = 0.1
        self.Dt = 0.005
        # final time
        self.Ft = 1.0 
        # sensing radius
        self.R = 1 
        self.Rl = 0
        # lattice size
        self.lattice = 0
        self.a = 0
        self.b = 0
        
        self.OmegaConst = 0
      
        # parameters object
        self.param = 0 

        # sim object
        self.sim = 0

    def getDomainSize(self): return self.L
    def getDiffusionCoeff(self): return self.D
    def getAdhesionCoeff(self): return self.A
    def getStepSize(self): return self.h
    def getSensingRadius(self): return self.R
    def getSensingRadiusL(self): return self.Rl
    def getLatticeSize(self): return int(self.lattice)
    def setDomainSize(self, L): self.L=L
    def setDiffusionCoeff(self, D): self.D=D
    def setStepSize(self, h): self.h=h
    def setSensingRadius(self, R): self.R=R

    def getCellPath(self):
        return np.asarray(self.sim.getPath())

    def getCellLocations(self):
        weight = np.asarray(self.sim.getPath())
        domain = np.arange(-self.L, self.L, self.h)
        #print( np.shape(weight) )
        #print( np.shape(domain))
        #print( weight )
        #print( domain )        
        return np.multiply(weight, domain)

    #def getCellCOM(self):
    #    com = np.asarray(self.sim.getCOM())
    #    return (com - self.L)

    # call this somehow after var changes
    def updateValues(self, NoPlayers, NoSteps):
        self.Rl = int(self.R / self.h)
        self.param = s.Parameters(2*self.L, self.h, self.Ft, NoPlayers)
        self.param.SensingRadiusL = self.Rl
        #self.param.NumberOfCells = NoPlayers
        self.param.Diffusion = self.D
        self.param.Adhesion = self.A
        self.param.Check()

    def doHistogram(self, bar_width=None):
        if not bar_width: bar_width = self.h
        x=self.getCellPath()
        bins=np.arange(-self.L, self.L + self.h, bar_width)

        print('shape bins=', np.shape(bins))
        print('shape x=', np.shape(x))

        #hist, bin_edges = np.histogram(x, bins=100, density=True)
        #bins = np.arange(-self.L, self.L +
        #    self.h, bar_width), density=True)
        plt.bar(bins[:-1], x, width=bar_width)
        #plt.bar(x)
        plt.xlim(min(bins), max(bins))
        plt.show()

    def setupSim(self, NoPlayers=100, NoSteps=1000):
        self.updateValues(NoPlayers, NoSteps)
        self.sim = s.Simulator(self.param)

    def printSim(self):
        self.sim._print()

    def runSimulation(self, NoPlayers=1000, NoSteps=1000):
        assert (NoPlayers>0), "Number of players has to be larger 0"
        assert (NoSteps>0), "Number of steps has to be larger 0"
        if NoSteps>1E6: print ('WARNING number of steps is very large!!')

        # make sure parameters are up to date
        self.updateValues(NoPlayers, NoSteps)

        # create sim object
        self.sim = s.Simulator(self.param)
        self.sim._print()
        time.sleep(1)

        #self.doHistogram()

        # run the simulation
        self.sim.run()

if __name__ == '__main__':
    player = Player()
    player.runSimulation()
