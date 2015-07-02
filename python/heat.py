import numpy as np
import matplotlib.pyplot as plt

class FFTHeat1D_test(object):

        def __init__(self, u, dt, L):
            self.ux = u
            self.L = L
            self.uk = self.compute_to_k(u)
            self.n = len(self.uk)
            self.dt = dt
            self.e = self.get_derivConst()

            print('dt=', dt, ' n=', self.n, ' L=', self.L)

        def get_derivConst(self):
            kVec = np.arange(-self.n/2, self.n/2)
            derivConst = -(2.0*np.pi*kVec / self.L)**2 * self.dt
            return np.exp(derivConst)

        def compute_to_k(self, u):
            " converts vector from spatial to frequency domain "
            u = np.fft.fft(u)
            return np.fft.fftshift(u)

        def compute_to_x(self, uk):
            uk = np.fft.ifftshift(uk)
            return np.fft.ifft(uk)

        def get_x(self):
            return self.ux

        def time_step(self):
            self.uk = self.e * self.uk
            self.ux = np.real(self.compute_to_x(self.uk))

def map(a, b, x):
    return (x + a)/(a + b)

def test():

    D = 10.0
    t = 0.1
    N = 2**10
    domainSize = 3.0

    # actual domain
    x = np.arange(0, domainSize, domainSize/N)
    u_exact = np.exp(-(2.0*np.pi / domainSize)**2 * D * t) \
            * np.sin(2.0 * np.pi * x / domainSize)

    # map everything to [0, 1]
    x_solver = np.arange(0.0, 1.0, 1.0/N)

    # initial condition
    u = np.sin(2.0 * np.pi * x / domainSize)

    solver = FFTHeat1D_test(u, D * t, domainSize)
    solver.time_step()

    u = solver.get_x()

    plt.plot(x, u, color='k')
    plt.plot(x, u_exact, color='g')

    print('ERROR=', np.linalg.norm(u-u_exact))

    plt.show()

def g(x):
    h = 0.1
    value = 1.0 / 3.0
    mid = 5.0
    if np.abs(x - mid) < h:
        return value
    else:
        return 0.0

vg = np.vectorize(g)

def delta():

    D = 0.5
    t = 0.1
    N = 2**11
    domainSize = 10.0

    # actual domain
    x = np.arange(0, domainSize, domainSize/N)

    # map everything to [0, 1]
    x_solver = np.arange(0.0, 1.0, 1.0/N)

    mid = N/2
    #u = np.zeros(N)
    #u[mid-2:mid] = 0.5
    u = vg(x)

    solver = FFTHeat1D_test(u, D * t, domainSize)
    solver.time_step()

    u = solver.get_x()
    print('max=', np.max(u))

    plt.plot(x, u, color='k')

    plt.show()

