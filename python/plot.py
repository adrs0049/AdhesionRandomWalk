
import matplotlib.pyplot as plt
import simulator as s
from models import Simulation, PathMetaData, Parameters

import os

import heat as diffusion_solver
import numpy as np

import randomWalk_db as rw
import time


class Plotter(object):
    def __init__(self, simId, Compare=False):
        self.db = rw.RandomWalkDB()
        self.Compare = Compare
        self.simId = simId
        self.data = None
        self.sim = None
        self.param = None

    def __call__(self):
        self.plot()

    def getDomainSize(self):
        return float(self.param.DomainSize)

    def getDomainLeftBoundary(self):
        return 0.0

    def getDomainRightBoundary(self):
        return self.getDomainSize()

    def verify_data(self, data):
        assert isinstance(data, dict), "df is expected to be a dict"

        # TODO raise exception
        if not len(data) > 0:
            print('Data returned from simulation %d is empty!' % self.sim.id)
            return False
        else:
            return True

    def get_sim(self):
        self.sim = self.db.getSimulationFromId(self.simId)

    def get_param(self):
        assert self.sim is not None, ''
        self.param = self.sim.Parameters

    def get_data_from_db(self, simId=None):
        data = self.db.returnPathsForSim(simId)
        self.verify_data(data)
        return data

    def get_max_xlim(self):

        max_values = []
        for key, path_data in iter(sorted(self.data.items())):
            try:
                df = path_data.dataFrame
                x = df['avg']
                total = np.sum(x)
                #print('key=', key, ' total=', total, ' max=', np.max(x))
                max_values.append(np.max(x))
            except KeyError:
                print('Data returned from simulation %d is empty!' % self.runId)
                return df
            except:
                raise

        return max(max_values)

    def getStepSize(self):
        assert self.param is not None, ''
        return float(1.0 / self.param.DomainN)

    def data_keys(self):
        for key in sorted(self.data.keys()):
            yield key

    def get_related_sim(self):
        # get the numerical solns
        # FIXME
        print('GETTING REALTED DATA')
        session = self.db.getSession()
        q = session.query(Simulation, Parameters, PathMetaData).\
                filter(Simulation.parameter_id==Parameters.id).\
                filter(Parameters.R==self.param.R).\
                filter(Parameters.diffusion_coeff==self.param.diffusion_coeff).\
                filter(Parameters.drift_coeff==self.param.drift_coeff).\
                filter(PathMetaData.simulation_id==Simulation.id).\
                filter(PathMetaData.stochastic==False)

        # FIXME
        print(len(q.all()))
        print('Found ', len(q.all()), ' paths.')
        simId = q.all()[0][0].id

        print('found simulation twin at id %d.' % simId)

        data = self.get_data_from_db(simId)

        return data

    def plot(self):

        self.get_sim()
        self.get_param()

        if self.simId is None:
            self.simId = self.sim.id

        self.data = self.get_data_from_db(self.simId)
        bar_width = self.getStepSize()

        print('Plotting for times ', end='')
        print(" ".join(str(x) for x in self.data_keys()))
        print('.')

        # FIXME use time from database or sth?
        timestr = time.strftime("%Y%m%d-%H%M%S")

        # first find max so we can plot without moving xlims
        max_states = self.get_max_xlim()
        print('max_states =', max_states)

        rw_type = self.param.rw_type
        assert rw_type is not None, ''

        rw_type_name = 'Unknown'
        simulation_name = 'Unknown'
        adata = None
        if rw_type == s.RANDOMWALK_TYPE_DIFFUSION:
            simulation_name = 'Results of a diffusion space-jump process'
            rw_type_name = 'Diffusion'
            label_cont = 'FFT solution'

        elif rw_type == s.RANDOMWALK_TYPE_DIFFUSION_AND_DRIFT:
            simulation_name = 'Results of an advection-diffusion' \
               ' space-jump process'
            rw_type_name = 'Drift'
            label_cont = 'FFT solution'

        elif rw_type == s.RANDOMWALK_TYPE_ADHESION:
            simulation_name = 'Results of a simple adhesion space-jump' \
                    ' process'
            rw_type_name = 'Adhesion'
            label_cont = 'Adhesion cont soln'
            if self.Compare:
                adata = self.get_related_sim()
                print('adata keys=', adata.keys())

        else:
            print('WARNING Unknown random walk type!')
            assert False

        for key, path_data in iter(sorted(self.data.items())):
            df = path_data.dataFrame
            steps = path_data.steps
            try:
                avg = df['avg']
                avg = avg / self.getStepSize()
            except KeyError:
                print('Data returned from simulation %d is empty!' % self.runId)
                return df
            except:
                raise

            # maybe do the total over a unit interval?
            total = np.sum(avg) * self.getStepSize()

            bins=np.arange(-self.getDomainLeftBoundary(),
                            self.getDomainRightBoundary(),
                            bar_width)

            # should we check all?
            stochastic = self.sim.paths[0].stochastic

            if plt is not None: plt.clf()

            label_ssa = 'Density of Gillespie SSA simulation'

            labels = []

            colors = ['c', 'm', 'y', 'k', 'r', 'b', 'g', 'w']
            idx = 0
            for column in df:
                if idx==8:
                    idx=0
                if column == 'avg':
                    continue

                x = df[column]
                x = x / self.getStepSize()
                print('idx=', idx, ' len=', len(colors))
                l = plt.plot(bins, x, 'ro', color=colors[idx])
                idx += 1
                labels.append(l)

            #states, = plt.plot(bins, x, 'ro', color='r', label=label_ssa)
            states, = plt.plot(bins, avg, color='k', linewidth=2.5)

            if self.Compare and \
               rw_type == s.RANDOMWALK_TYPE_ADHESION:
                try:
                    # FIXME
                    akey = np.round(key, decimals=1)
                    data = adata[key].dataFrame['avg']
                    u2 = np.asarray(data)
                    print('Key=', key, ' SSA total=', total, ' continuum_total=', \
                          np.sum(u2) * 1.0 / 16.0)
                    # FIXME dont hard code values
                    x2 = np.arange(0.0, 10, 1.0 / 16.0)

                    density, = plt.plot(x2, u2, color='g', linewidth=2.0,
                                    label=label_cont)

                except KeyError as e:
                    print('KeyError on key %f.' % key)

                except:
                    raise

            if self.Compare and (\
                rw_type == s.RANDOMWALK_TYPE_DIFFUSION_AND_DRIFT or\
                rw_type == s.RANDOMWALK_TYPE_DIFFUSION):

                density, = plt.plot(x2, u2, color='g', linewidth=2.0,
                                    label=label_cont)
            if self.Compare and (\
                rw_type == s.RANDOMWALK_TYPE_DIFFUSION_AND_DRIFT or\
                rw_type == s.RANDOMWALK_TYPE_DIFFUSION):

                x2, u2 = self.compute_fft(key)

                y_max = max(np.max(x), np.max(u2))
                print('y_max=', y_max, ' u2=', np.max(u2))
            elif self.Compare and \
                rw_type == s.RANDOMWALK_TYPE_ADHESION:

                y_max = max(np.max(x), np.max(u2))
                print('y_max=', y_max, ' u2=', np.max(u2))

            else:
                y_max = np.max(x)

            plt.ylim(0, 1.5 * max(y_max, max_states))

            plot_title = simulation_name + \
                    '\n with %d players at time %2.2f' \
                    '\n or %2.2e simulations steps'
            plt.title(plot_title % (total, key, steps), fontsize=20)

            plt.xlabel('Spacial domain', fontsize=18)
            plt.ylabel('Density [UNITS]', fontsize=18)
            plt.tight_layout()
            plt.tick_params(axis='x', labelsize=15)
            plt.tick_params(axis='y', labelsize=15)

            #try:
            #    plt.legend(handles=[states, density])
            #except:
            #    plt.legend(handles=[states])

            result_dir = 'Results'
            path = None
            try:
                cdir = os.getcwd()
                path = os.path.join(cdir, result_dir)
                path = os.path.join(path, timestr)
                if not os.path.exists(path):
                    os.makedirs(path)
            except:
                raise
            finally:
                fname = 'plot_'+rw_type_name+'_'+str(self.simId)+\
                '_'+str(key)+'.png'
                plt.savefig(os.path.join(path, fname))

            #if show:
            #    plt.show()

    # Compute norm between vectors x, y
    def norm1(self, x, y):
        return np.linalg.norm(x-y, ord=1)

    # compute l2 between vectors x, y
    def norm2(self, x, y):
        return np.linalg.norm(x-y, ord=2)

    def print_error(self, x, y):
        print('The L1 error=', self.norm1(x, y))
        print('The L2 error=', self.norm2(x, y))

    def compute_fft(self, time):
        # try yet another diffusion solver
        N = self.getDomainSize() * self.param.DomainN

        if N<2000:
            print('Using a larger N')
            N = 2**12

        x2 = np.arange(-self.getDomainLeftBoundary(),
                       self.getDomainRightBoundary(),
                       self.getDomainSize()/N)
        assert len(x2)==N, ""
        u_ic = np.zeros(N)

        # index is zero based -> midpoint one lower
        mid = int(N/2)-1

        #u_ic[mid-1:mid+2]=0.33
        u_ic = diffusion_solver.vg(x2, 1.0 / self.param.DomainN)
        #print('u_ic total=', np.sum(u_ic))
        assert len(u_ic)==N, ""

        solver = diffusion_solver.FFTHeat1D_test(u_ic, float(time), \
                                                 self.getDomainSize(), \
                                float(self.param.diffusion_coeff), \
                                float(self.param.drift_coeff))
        solver.time_step()
        u2 = solver.get_x()

        return x2, u2

if __name__ == '__main__':
    plotter = Plotter(150, Compare=True)
