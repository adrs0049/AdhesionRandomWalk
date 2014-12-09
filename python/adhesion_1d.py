import random
import math
import matplotlib.pyplot as plt
import sys
from collections import namedtuple
import numpy as np

from data import DataObject

class Cell(DataObject):
            default_value = 'no data'
            __slots__ = (
                'com',
            )

# current concentration of A
#Cell = namedtuple('Cell', ['com'])
    
def getValues(l, name):
  return [getattr(r, name) for r in l]    

class Simulator(object):
    def __init__(self):
        self.steps = 1000
        self.D = 1
        self.A = 40
        
        # domain size, domain is [-L, L]
        self.L = 10
        # step size 
        self.h = 0.1
        self.Dt = 0.005
        # sensing radius
        self.R = 1 
        self.Rl = 0
        # lattice size
        self.lattice = 0
        self.a = 0
        self.b = 0
        
        self.OmegaConst = 0
        
        # create cells
        self.Cells = []
        # want that key is the spatial location where something is to be found
        self.AdhesionEnvironment = {}
    
        # set a whole bunch of values
        self.updateValues()
    
    def getDomainSize(self): return self.L
    def getDiffusionCoeff(self): return self.D
    def getAdhesionCoeff(self): return self.A
    def getStepSize(self): return self.h
    def getSensingRadius(self): return self.R
    def getSensingRadiusL(self): return self.Rl
    def getLatticeSize(self): return int(self.lattice)
    def getRandomLatticePos(self): return random.randint(0, self.getLatticeSize())
    def setDomainSize(self, L): self.L=L
    def setDiffusionCoeff(self, D): self.D=D
    def setStepSize(self, h): self.h=h
    def setSensingRadius(self, R): self.R=R
    
    # call this somehow after var changes
    def updateValues(self):
        self.lattice = 2 * self.L / self.h + 1
        self.Rl = int(self.R / self.h)
        self.OmegaConst = 1.0 / ( 2 * self.Rl )
        
        self.a = self.getDiffusionCoeff() * self.Dt / self.getStepSize()**2
        self.b = self.getAdhesionCoeff() * self.Dt / (2 * self.getStepSize()**2) 
        
        print 'a=',self.a,' b=',self.b

    def updateEnvironment(self):
        for cell in self.Cells:
            try:
                self.AdhesionEnvironment[cell.com] += 1
            except:
                self.AdhesionEnvironment[cell.com] = 1
    
    def initCells(self, NumberOfCells):
        # initial collection of cells
        #for i in xrange(0, NumberOfCells):
        #    self.Cells.append(Cell( self.getRandomLatticePos() ))
        for i in xrange(0, self.getLatticeSize() ):
            for j in xrange(0, 5):
                self.Cells.append(Cell ( i ) )
    
    def step(self, StepNo):
        
        for cell in self.Cells:
            r = random.random()
            lp, rp = self.getTransitionProb(cell.com)            

            if not StepNo % 100:
                print StepNo, lp, rp

            if r < lp: # move left
                
                if self.AdhesionEnvironment[cell.com]==1:
                    del self.AdhesionEnvironment[cell.com]
                else:
                    self.AdhesionEnvironment[cell.com]-=1
                    
                cell.com=cell.com-1
                                
                try:
                    self.AdhesionEnvironment[cell.com]+=1
                except:
                    self.AdhesionEnvironment[cell.com]=1
                
                #print StepNo, 'MOVE LEFT', lp, rp
            elif r < lp + rp: # move right
                if self.AdhesionEnvironment[cell.com]==1:
                    del self.AdhesionEnvironment[cell.com]
                else:
                    self.AdhesionEnvironment[cell.com]-=1
                
                cell.com=cell.com+1
                
                try:
                    self.AdhesionEnvironment[cell.com]+=1
                except:
                    self.AdhesionEnvironment[cell.com]=1
                    
                #print StepNo, 'MOVE RIGHT', lp, rp
            #else: # no movement
            #    print StepNo, 'DONT MOVE', lp, rp
        
        #nx = random.gauss(0,1)
        #NewPoint = Point(A[-1].x + math.sqrt(2*D*Dt)*nx)
        #A.append(NewPoint)

    def getCellLocations(self):
        x=[]
        for cell in self.Cells:
            x.append(cell.com*self.getStepSize()-self.getDomainSize())
        return x

    def doHistogram(self, bar_width=0.125):
        x=self.getCellLocations()
        hist, bin_edges = np.histogram(x, bins = np.arange(-self.L, self.L, bar_width))
        plt.bar(bin_edges[:-1], hist, width=bar_width)
        plt.xlim(min(bin_edges), max(bin_edges))
        plt.show()

    def cleanUp(self):
        self.Cells = []
        self.AdhesionEnvironment = {}

    def runSimulation(self, NoPlayers, NoSteps):
        assert (NoPlayers>0), "Number of players has to be larger 0"
        assert (NoSteps>0), "Number of steps has to be larger 0"
        if NoSteps>1E6: print 'WARNING number of steps is very large!!'

        self.cleanUp()
        self.initCells(NoPlayers)
        self.updateEnvironment()

        for step in xrange(0, NoSteps):
            self.step(step)

    def getTransitionProb(self, x):
        adhesion = self.Adhesion(x)
        lp = self.a - self.b * adhesion
        rp = self.a + self.b * adhesion
        
        assert (0<=lp<=1), 'Probability is required to be within [0,1]'
        assert (0<=rp<=1), 'Probability is required to be within [0,1]'

        return lp, rp

    def omega(self, r):
        # simple uniform distribution for now
        #if abs(r)>self.getSensingRadiusL():
        #    return 0.0
        #else:
        return self.OmegaConst

    def freeSpace(self, x):
        # TODO there always is free space atm
        try:
            val = self.AdhesionEnvironment[x]
            return max(0.0, 1.0 - val / 20.0)
        except:
            return 1.0

    def adhesivity(self, x):
        # TODO number of cells affect adhesivity at a location
        try:
            val = self.AdhesionEnvironment[x]
            return val * 1.0
        except:
            return 0.0
        #if any(a.com==x for a in self.Cells):
        #    return 1.0
        #else:
        #    return 0.0
    
    # compute adhesion integral about the point x
    def Adhesion(self, x):
        # adhesion is given by integral -R to R about x
        total=0
        for j in xrange(1, self.Rl):
            #print x+j, j, self.omega(j), self.freeSpace(x+j), self.adhesivity(x+j)
            total+=(self.freeSpace(x+j) * self.omega(j)  * self.adhesivity(x+j) -
                    self.freeSpace(x-j) * self.omega(-j) * self.adhesivity(x-j)) 

        return self.getStepSize()**2 * total
    
if __name__ == '__main__':
    s = Simulator()
