import random
import math
import pylab as p
import sys
from collections import namedtuple

# initial number of molecules
n0 = 20

# rate of reaction
# k st. k dt gives probability that randomly chosen molecule reacts
k = 0.1

# time step
Dt = 0.005
steps = 10000

# current concentration of A
DataPoint = namedtuple('Data', ['time', 'mol'])
A = []
A.append(DataPoint(time=0, mol=n0))

for step in xrange(0, steps):
  if A[-1].mol<=0:
    print 'Reaction done. Exiting...'
    sys.exit()
 
  r = random.random()
  # time of next reaction
  tau =  1.0 / ( A[-1].mol * k ) * math.log(1.0 / r)
  
  A.append(DataPoint(time=A[-1].time + tau, mol=A[-1].mol - 1))
   
#t = p.arange(0.0, (steps+1)*Dt, Dt)
#p.plot(t, A)

#p.xlabel('time (s)')
#p.ylabel('Number of molecules')
#p.title('Decay Realizations')
#p.grid(True)
#p.savefig("test.png")
#p.show()
