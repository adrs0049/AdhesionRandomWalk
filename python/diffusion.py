""" test """

import numpy as np
import math

import matplotlib.pyplot as plt

import numpy.fft as fft

class AD(object):
    """ Simple advection diffusion solver class """


    def __init__(self, shape=None, L=5, D=1.0, c=1.0, refinement = 1):

        N = int(shape[0] / refinement)

        self.N = N
        self.L = L

        # origin
        self.x0 = 0.0

        self.shape = (N,)

        print('N ', type(N), ' =', N)
        # diffusion coefficient
        self.D = D

        # drift coefficient
        self.c = c

        # initial time
        self.t0 = 0.0

        # frequencies
        K = math.pi / L * fft.fftfreq(N) * N

        # operators
        self.sgradient = K*1j
        self.laplacian = -K**2

    def exact(self, t, shift=0.0):
        """ Exact solution to Advection Diffusion Equations """

        L = self.L
        D = self.D
        c = self.c
        t0 = self.t0

        xd = np.linspace(-self.L, self.L, self.N)
        soln = ((4.0 * np.pi * D * (t + t0))**(-0.5)
                    * np.exp(-(xd + shift - ( self.x0 + c * t))**2
                            /(4.0 * D * (t + t0))) )

        return xd, soln

    def convolute(self, t, ic=0.5):

        h = 1.0/self.N
        print('h=',h)

        x1, u1 = self.exact(t)
        x2, u2 = self.exact(t, shift=1.0/self.N)
        x3, u3 = self.exact(t, shift=-1.0/self.N)

        u = 2.0 * h * ic * (u1+u2+u3)

        return x1, u


    def f1_eval(self, u, t, f1):

        z = fft.fftn(u)

        z_sgrad = self.sgradient * z
        u_sgrad = np.real(fft.ifftn(z_sgrad))

        f1[...] = - (self.c * u_sgrad)

    def f2_eval(self, u, t, f2):

        z = fft.fftn(u)
        z = self.D * self.laplacian * z
        u = np.real(fft.ifft(z))

        f2[...] = u

    def eval(self, t):

        N = self.N
        print('N=', N)

        u = np.zeros(N)
        print(np.shape(u))

        u[N/2] = 0.5

        #print 'u=', u

        z = fft.fft(u)
        W = np.exp( ( - self.sgradient * self.c + self.laplacian * self.D ) * t)
        #print 'K2=', self.laplacian
        Ft = z * W
        #print 'Ft=', Ft
        soln = np.real(fft.ifft(Ft))
        #print 'soln=',soln

        return soln

    def error(self, x, y):
        return np.linalg.norm(x-y, ord=2)

if __name__ == '__main__':
    ad = AD(shape=(1024,))
    x, soln = ad.exact(1.00)

    soln2 = ad.eval(1.0)
    error = soln - soln2
    print('Error=', np.linalg.norm(error, ord=2))

    plt.plot(x, soln, label='Exact')
    plt.plot(x, soln2, label='FFT')
    plt.plot(x, error, label='Error')
    plt.legend(loc='upper left')
    plt.show()
