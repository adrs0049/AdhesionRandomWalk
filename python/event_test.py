import simulator as s

EventCalled = False

def f(*args, **kwargs):
    print(args[0])
    print("HELLO CALLBACK")

if __name__ == '__main__':
    times = s.DVector([0.1,0.2,0.3])
    p = s.Parameters(10,0.1,times,1)
    p.setSensingRadius(1.0)
    p.setDiffusion(1.0)
    p.setDrift(1.0)

    sim = s.Simulator(p)
    sim.registerPyListener(f)

    sim.run()

