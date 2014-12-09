
import random
import math
import matplotlib.pyplot as plt
import sys
from collections import namedtuple
import numpy as np

from DataModel import *

def getParametersFromDatabase(self):    
    # tries to fetch current Parameter Entry from the database 
    # if it doesnt exist it will create the record
    try:
        currentParameters = Parameters.select().where(Parameters.DiffusionP==self.getDiffusionCoeff(), 
                                       Parameters.AdhesionP==self.getAdhesionCoeff(),
                                       Parameters.SensingR==self.getSensingRadius(),
                                       Parameters.DomainL==self.getDomainSize(),
                                       Parameters.StepSize==self.getStepSize(),
                                       Parameters.TimeStep==self.Dt).get()
    except:
        print 'Parameters did not exist. Creating...'
        assert False


def getRunsWithDiffP(diffusionCoeff):
    query = (Runs
             .select(Runs, Parameters)
             .join(Parameters)
             .where(Parameters.DiffusionP == diffusionCoeff)
             .order_by(Runs.runId.desc())
             .limit(10))
    
    assert query.exists(), 'Query doesnt return anything!!!'
    return query

def getMostRecentRun():
    query = (Runs
             .select(Runs)
             .order_by(Runs.runId.desc())
             .limit(1))
    
    assert query.exists(), 'Query doesnt return anything!!!'
    return query[0]

def getCellLocations(run):
    p=run.parameterId
    x=[(y.pos * p.StepSize - p.DomainL) for y in run.FinalPos]
    return x      
        
def doHistogram(bar_width=None):
    r=getMostRecentRun()
    p=r.parameterId
    if bar_width is None: bar_width=p.StepSize
    x=getCellLocations(r)
    hist, bin_edges = np.histogram(x, bins = np.arange(-p.DomainL, p.DomainL, bar_width))
    plt.bar(bin_edges[:-1], hist, width=bar_width)
    plt.xlim(min(bin_edges), max(bin_edges))
    plt.show()
    
    