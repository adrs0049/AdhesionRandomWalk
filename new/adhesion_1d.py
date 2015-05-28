import math
import matplotlib.pyplot as plt
import sys
from collections import namedtuple
import numpy as np
import simulator as s
import time

from diffusion import *

def getValues(l, name):
  return [getattr(r, name) for r in l]    

class Player(object):
    def __init__(self):
        self.steps = 1000
        self.D = 1
        self.A = 0.0
        
        # domain size, domain is [-L, L]
        self.L = 5 
        # step size 
        self.h = 0.05
        self.Dt = 0.005
        # final time
        self.Ft = 0.1
        # sensing radius
        self.R = 1 
        self.Rl = 0
        # lattice size
        self.lattice = 0
        self.a = 0
        self.b = 0

        # number of NumberOfCells
        self.NoPlayers = 0
        
        self.OmegaConst = 0
      
        # parameters object
        self.param = 0 

        # sim object
        self.sim = 0

    def getDomainSize(self): return 2*self.L
    def getDomainSizeL(self): return self.getDomainSize() / self.getStepSize()
    def getDomainLeftBoundary(self): return -self.L
    def getDomainRightBoundary(self): return self.L
    def getDiffusionCoeff(self): return self.D
    def getAdhesionCoeff(self): return self.A
    def getStepSize(self): return self.h
    def getSensingRadius(self): return self.R
    def getSensingRadiusL(self): return self.Rl
    def getLatticeSize(self): return int(self.lattice)
    def getFinalTime(self): return self.Ft
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
        self.param.setDiffusion(self.D)
        self.param.setDrift(self.A)
        self.param.update()

    def doHistogram(self, bar_width=None):
        if not bar_width: bar_width = self.h
        x=self.getCellPath()
        bins=np.arange(-self.L, self.L + self.h, bar_width)

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

    def setupSim(self, NoPlayers=100, NoSteps=1000):
        self.updateValues(NoPlayers, NoSteps)
        self.sim = s.Simulator(self.param)

    def computeDiffusionSoln(self, FinalTime=None):
        N=self.getDomainSizeL()
        Nhalf = N/2

        if not FinalTime==None:
            T = FinalTime
        else:
            T=self.getFinalTime()
        
        print('Solving heat equation using the FFT')
        print('N=',N,' T=',T)

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

    def printSim(self):
        self.sim._print()

    def prepareSimulation(self, NoPlayers, NoSteps):
 
        self.NoPlayers = NoPlayers

        # make sure parameters are up to date
        self.updateValues(NoPlayers, NoSteps)

        # create sim object
        self.sim = s.Simulator(self.param)
        self.sim._print()
        time.sleep(1)

    def runSimulation(self, NoPlayers=500, NoSteps=1000):
        assert (NoPlayers>0), "Number of players has to be larger 0"
        assert (NoSteps>0), "Number of steps has to be larger 0"
        if NoSteps>1E6: print ('WARNING number of steps is very large!!')

        self.prepareSimulation(NoPlayers, NoSteps)

        # run the simulation
        self.sim.run()

if __name__ == '__main__':
    player = Player()
    #player.runSimulation()
