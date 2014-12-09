import random
import math
import matplotlib.pyplot as p
import sys
from collections import namedtuple

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

# current concentration of A
Point = namedtuple('Point', ['x', 'y', 'z'])
#A = []
#A.append(Point(0,0,0))

def getValues(l, name):
  return [getattr(r, name) for r in l]

fig = p.figure()
ax = p.subplot(111)
for real in xrange(1,12):
  A = []
  A.append(Point(0,0,0))
  for step in xrange(0, steps):
    nx = random.gauss(0,1)
    ny = random.gauss(0,1)
    nz = random.gauss(0,1)
  
    NewPoint = Point(A[-1].x + math.sqrt(2*D*Dt)*nx,
		    A[-1].y + math.sqrt(2*D*Dt)*ny,
		    A[-1].z + math.sqrt(2*D*Dt)*nz)
  
    A.append(NewPoint)
  
  #t = p.arange(0.0, (steps+1)*Dt, Dt)
  ax.plot(getValues(A, 'x'), getValues(A, 'y'))

p.xlabel('time (s)')
p.ylabel('Number of molecules')
p.title('Decay Realizations')
ax.set_xlim(-0.1, 0.1)
ax.set_ylim(-0.1, 0.1)
p.grid(True)
#p.savefig("test.png")
p.show()
