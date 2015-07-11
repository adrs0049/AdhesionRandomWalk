import simulator as s
import numpy as np

EventCalled = False

def f(*args, **kwargs):
    print('HELLO CALLBACK')
    #print(args[0])
    data = kwargs
    print('kwargs_type=',type(data))
    print('time=', np.around(data["time"], decimals=3),
          ' steps=', data["steps"], ' states=', data["states"])

    #print('type=', type(args[0]))

if __name__ == '__main__':
    times = s.DVector([0.1,0.2,0.3])
    print('times=', times)
    domain = s.DVector([-5.0, 5.0])
    p = s.Parameters(domain,0.1,times,1)
    p.setSensingRadius(1.0)
    p.setDiffusion(1.0)
    p.setDrift(1.0)

    sim = s.Simulator(p)
    sim.registerPyListener(f)

    sim.run()

