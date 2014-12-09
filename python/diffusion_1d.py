import random
import math
import matplotlib.pyplot as p
import sys
from collections import namedtuple
import numpy as np

# initial number of molecules
n0 = 20

# rate of reaction
# k st. k dt gives probability that randomly chosen molecule reacts
k = 0.1

# time step
Dt = 0.005
steps = 1000

# diffusion coefficient
D = 1e-4

# domain of interest
L = 1

# current concentration of A
Point = namedtuple('Point', ['x'])
#A = []
#A.append(Point(0,0,0))

def getValues(l, name):
  return [getattr(r, name) for r in l]

fig = p.figure()
ax = p.subplot(111)

xf = []

for real in xrange(1,1000):
  A = []
  A.append(Point(0))
  for step in xrange(0, steps):
    nx = random.gauss(0,1)
   
    NewPoint = Point(A[-1].x + math.sqrt(2*D*Dt)*nx)
  
    A.append(NewPoint)
  
  xf.append(A[-1].x)
  t = np.arange(0.0, (len(A))*Dt, Dt)
  #ax.plot(getValues(A, 'x'), t)

#p.xlabel('Position')
#p.ylabel('Time (s)')
#p.title('Decay Realizations')
#ax.set_xlim(-0.1, 0.1)
#ax.set_ylim(0, (len(A))*Dt)
#p.grid(True)
#p.savefig("test.png")
#p.show()
