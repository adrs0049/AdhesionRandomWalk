import random
import pylab as p

# initial number of molecules
n0 = 20

# rate of reaction
# k st. k dt gives probability that randomly chosen molecule reacts
k = 0.1

# time step
Dt = 0.005
steps = 10000

# current concentration of A
A = []
A.append(n0)

for step in xrange(0, steps):
  r = random.random()
  
  if r < A[-1] * k * Dt:
    A.append(A[-1]-1)
  else:
    A.append(A[-1])
   
t = p.arange(0.0, (steps+1)*Dt, Dt)
p.plot(t, A)

p.xlabel('time (s)')
p.ylabel('Number of molecules')
p.title('Decay Realizations')
p.grid(True)
#p.savefig("test.png")
p.show()
