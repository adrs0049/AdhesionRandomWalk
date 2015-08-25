
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import simulator as s
from models import Simulation, PathMetaData, Parameters

import os

import heat as diffusion_solver
import numpy as np
import scipy.signal as sp

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
        self.bins = None
        self.x_prediction = None
        self.timestr = time.strftime("%Y%m%d-%H%M%S")

        # counter to catch errors in recursion
        # TODO there must be a better way to do this
        self.counter = 0

        # Compare threshold
        self.CompareThreshold = 5.0

    def __call__(self):
        self.plot()

    def getDomainSize(self):
        return float(self.param.DomainSize)

    def getR(self):
        return float(self.param.R)

    def getW(self):
        w = self.getR()
        return w * self.param.DomainN

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
                filter(Parameters.DomainN==self.param.DomainN).\
                filter(Parameters.diffusion_coeff==self.param.diffusion_coeff).\
                filter(Parameters.drift_coeff==self.param.drift_coeff).\
                filter(PathMetaData.simulation_id==Simulation.id).\
                filter(PathMetaData.stochastic==False)

        if len(q.all())==0:
            print('WARNING no related continuum simulation found!')
            return

        # FIXME
        print(len(q.all()))
        print('Found ', len(q.all()), ' paths.')
        simId = q.all()[0][0].id

        print('found simulation twin at id %d.' % simId)

        data = self.get_data_from_db(simId)

        return data


    def get_plot_names(self, rw_type):

        rw_type_name = 'Unknown'
        simulation_name = 'Unknown'
        label_cont = 'Unknown'
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

        else:
            print('WARNING Unknown random walk type!')
            assert False

        return (rw_type_name, simulation_name, label_cont, )

    def get_data(self):
        self.get_sim()
        self.get_param()

        if self.simId is None:
            self.simId = self.sim.id

        self.data = self.get_data_from_db(self.simId)

    """ get extrema of path """
    def compute_path_extrema(self, path):
        step = 0.25
        widths = np.arange(step, int(3.0 * self.getR()), step)
        print('widths=', widths)
        print('other extrema=', sp.argrelextrema(path, np.greater))
        print('extrema=', sp.find_peaks_cwt(path, widths))
        extrema = sp.argrelextrema(path, np.greater)
        extrema = np.append(extrema, sp.find_peaks_cwt(path, widths))
        return np.unique(extrema)

    """ get x and u for Container at key """
    def getStateAtKey(self, dataContainer, key):
        stepSize = 1.0 / self.param.DomainN
        try:
            key = np.round(key, decimals=2)
            data = dataContainer[key].dataFrame['avg']
            u = np.asarray(data)

            x = np.arange(self.getDomainLeftBoundary(),
                          self.getDomainRightBoundary(),
                          stepSize)

            return x, u
        except KeyError as e:
            print('KeyError on key %f.' % key)
        except:
            raise

    """ get peak prediction from highest time available cont result """
    def compute_prediction(self, dataContainer):
        assert isinstance(dataContainer, dict), "dataContainer has to be a dict"

        try:
            # get max key
            key = np.round(max(dataContainer.keys()))
            x, u = self.getStateAtKey(dataContainer, key)
            return x, u
        except:
            raise

    """ elim too small extrema """
    def elim_small_extrema(self, extrema_path, path, threshold_percentage=0.8):

        max_state = np.max(path)
        print('PathMax=%5.2f, Threshold is %2.0f, Threshold values is %5.2f' \
              % (max_state, 100.0 * threshold_percentage, \
                 max_state * threshold_percentage))

        indeces = []
        idx=0
        for point in extrema_path:
            print('path[', point, ']=', path[point], ' considering')
            if path[point] < threshold_percentage * max_state:
                print('path[', point, ']=', path[point], ' eliminating')
                indeces.append(idx)
            idx+=1

        #print('extrema_before_elim=', extrema_path)
        #print('indeces=', indeces, ' len=', len(extrema_path))
        extrema_path = np.delete(extrema_path, indeces)

        return extrema_path

    def find_duplicate_peaks(self, extrema_path, number_predicted_peaks):
        epts = []
        width_threshold = 10
        # detect extreme points that may be double
        idx_del = []
        print('extrema_path=', extrema_path, ' predicted peaks=', \
              number_predicted_peaks)
        for i in range(0, len(extrema_path)-1):
            if np.abs(extrema_path[i] - extrema_path[i+1]) < width_threshold:
                # found two points that are large and close together
                # merge them
                print('i=', i, 'extrema[i]=', extrema_path[i], \
                      ' extrema[i+1]=', extrema_path[i+1])
                idx_del.append(i)
                idx_del.append(i+1)
                epts.append(extrema_path[i] +
                    int(np.abs(extrema_path[i] - extrema_path[i+1])))

        idx_del = list(set(idx_del))
        print('idx_del=', idx_del, ' epts=', epts)
        extrema_path = np.delete(extrema_path, idx_del)
        extrema_path = np.append(extrema_path, epts)
        if len(extrema_path)>number_predicted_peaks and number_predicted_peaks>1:
            extrema_path = self.find_duplicate_peaks(extrema_path, \
                                                     number_predicted_peaks)

        return extrema_path

    """ path is a numpy array """
    def compute_path_correction(self, prediction, path):
        # roughly one extreme per 3 * R
        number_of_extrema = self.getDomainSize() / (3.0 * self.getR())
        threshold_percentage = 0.95

        print('Looking for %d extrema with an initial threshold of %f' \
              % (number_of_extrema, threshold_percentage))

        # find max in continuum data first
        extrema_predicted = self.compute_path_extrema(prediction)
        print('extrema_pred=', extrema_predicted)
        extrema_predicted = self.elim_small_extrema(extrema_predicted,
                                                    prediction,
                                                    threshold_percentage)

        if np.max(extrema_predicted) - np.min(extrema_predicted) < self.getW():
            extrema_predicted = np.array([int(np.average(extrema_predicted))])
            print('extrema_predicted=', extrema_predicted)

        extrema_predicted = np.sort(extrema_predicted)
        if len(extrema_predicted)!=number_of_extrema:
            print('%d Extrema predicted!.' % len(extrema_predicted))
            print('extrema_prediction=', extrema_predicted)
            print('The path is ', prediction)
            assert False
        if len(prediction) != len(path):
            print('WARNING careful length of prediction and length of ' \
                  'path are different!')
        #print('prediction=', prediction)
        print('extrema prediction=', extrema_predicted)
        extrema_path = self.compute_path_extrema(path.values)

        assert len(extrema_path)>=len(extrema_predicted), \
               'not enough peaks found in the data'
        print('path=', path.values)
        print('extrema_path_before=', extrema_path)

        while True:
            extrema_path_tmp = self.elim_small_extrema(extrema_path,
                                                   path.values,
                                                   threshold_percentage)

            print('extrema_path_tmp=', extrema_path_tmp)
            if len(extrema_path_tmp)>=len(extrema_predicted):
                print('Found enough peaks with at a threshold of %1.2f' \
                      % threshold_percentage)
                extrema_path = extrema_path_tmp
                break

            threshold_percentage -= 0.1
            if threshold_percentage <= 0.5:
                assert False, 'couldn\'t find enough peaks in path data'

        extrema_path = np.sort(extrema_path)
        print('Find duplicates.')
        extrema_path = self.find_duplicate_peaks(extrema_path, \
                                                 len(extrema_predicted))
        extrema_path = np.sort(extrema_path)

        if np.max(extrema_path) - np.min(extrema_path) < self.getW():
            extrema_path = np.array([int(np.average(extrema_path))])
            print('extrema_path=', extrema_path)

        print('path extrema=', extrema_path, ' len(ext_path)=', \
              len(extrema_path), ' len(ext_pred)=', len(extrema_predicted))
        # what to do in this case=
        if len(extrema_path) != len(extrema_predicted):
            print('WARNING a different number of extrema found!')
            print('len_path=', len(extrema_path), ' len_pred=', \
                  len(extrema_predicted))

                        #extrema_path = np.array(int(np.average(extrema_path)))
            #print(' after filter=', extrema_path)
            #correction = extrema_predicted  - extrema_path
            extrema_path = np.array(int(np.average(extrema_path)))
            #correction = np.zeros(len(extrema_predicted))
            correction = extrema_predicted - extrema_path

            print('prediction=', extrema_predicted, \
                  'extrema_path=', extrema_path, \
                  'correction=', correction)

            assert False, 'DEBUG FIX ME'

        else:
            #print('prediction[', extrema_predicted[0], ', ',\
            #      extrema_predicted[1],']=(', prediction[extrema_predicted[0]], \
            #      ', ', prediction[extrema_predicted[1]], '). The path=',\
            #      '. It has a length of ', len(prediction), \
            #      ' The path extrema are=(', extrema_path[0], ', ', \
            #        extrema_path[1], ').')

            correction = extrema_predicted - extrema_path

        print('correction=', correction, ' extrema_predicted=',\
              extrema_predicted, ' extrema_path=', extrema_path)
        #correction = extrema_predicted
        return correction

    def correct_paths(self, df, prediction):
        print('self correct paths')
        for column in df:
            if column == 'avg':
                continue

            print('computing for column ', column)
            x = df[column]
            correction = self.compute_path_correction(prediction, x)
            correction = np.int(np.round(np.average(correction), decimals=0))
            print('correction %d' % correction)
            x = np.roll(x, correction)

            df[column] = x

        return df

    def isAdhesionCompare(self, rw_type):
        return (self.Compare and \
                rw_type == s.RANDOMWALK_TYPE_ADHESION)

    def isFFTCompare(self, rw_type):
        return (self.Compare and \
                (rw_type == s.RANDOMWALK_TYPE_DIFFUSION or \
                 rw_type == s.RANDOMWALK_TYPE_DIFFUSION_AND_DRIFT))

    def get_output_dir(self, subdir=None):
        result_dir = 'Results'
        path = None
        try:
            cdir = os.getcwd()
            path = os.path.join(cdir, result_dir)
            path = os.path.join(path, self.timestr)
            if subdir is not None:
                path = os.path.join(path, subdir)
            if not os.path.exists(path):
                print('Creating=', path)
                os.makedirs(path)
        except:
            raise

        return path

    def create_figure(self):
        return plt.figure()

    def finalize_figure(self, fig, ax, values, subdir):
        assert isinstance(values, dict), 'values is expected to be dict'
        path = self.get_output_dir(subdir)
        rw_type = self.param.rw_type
        assert rw_type is not None, ''
        rw_type_name, simulation_name, label_cont, = self.get_plot_names(rw_type)

        print('SAVING')
        total = values["total"]
        key = values["time"]
        steps = values["steps"]
        plot_title = simulation_name + \
                '\n with %d players at time %2.2f' \
                '\n or %2.2e simulations steps'
        ax.set_title(plot_title % (total, key, steps), fontsize=20)

        ax.set_ylim(0, 1.5 * values["ylim_max"])
        ax.set_xlabel('Spacial domain', fontsize=18)
        ax.set_ylabel('Density [CELLS / UNIT LENGTH]', fontsize=18)
        #ax.tight_layout()
        ax.tick_params(axis='x', labelsize=15)
        ax.tick_params(axis='y', labelsize=15)

        # get the legends
        try:
            extra = mpatches.Rectangle((0,0), 1, 1, fc="w", fill=False, \
                                       edgecolor='none', linewidth=0)
            handles, labels = ax.get_legend_handles_labels()

            handles.append(extra)
            labels.append('The relative L2 Error is %2.2f%%' \
                          % values["error"])
        except:
            handles, labels = ax.get_legend_handles_labels()

        ax.legend(handles, labels)

        try:
            fname = 'plot_'+rw_type_name+'_'+str(values["name"])+'_'\
                +str(self.simId)+\
                '_'+str(key).replace('.', '_')+'.eps'
        except KeyError:
            fname = 'plot_'+rw_type_name+'_'+str(self.simId)+\
                '_'+str(key).replace('.', '_')+'.eps'

        print('path=', os.path.join(path, fname))
        plt.tight_layout()
        fig.savefig(os.path.join(path, fname), format='eps', dpi=900)

    def plot_individual_paths(self, path_data, prediction, time, tag=None):

        """ Colors: c = cyan
                    m = magenta
                    y = yellow
                    k = black
                    r = red
                    b = blue
                    g = green
                    w = white
        """
        colors = ['c', 'm', 'y', 'k', 'r', 'b', 'g', 'w']

        if tag is None:
            name = 'individual'
        else:
            name = 'individual_' + tag

        df = path_data.dataFrame
        idx=0
        num=0
        total=0
        ylim_max=0.0
        # recompute average
        df['avg']=df.mean(axis=1)

        print('len=', len(df.columns))
        for column in df:
            #print('column=', column)
            if column == 'avg':
                continue

            if idx == 8:
                values = dict(total=total, steps=path_data.steps, time=time,\
                              ylim_max=max(ylim_max, np.max(prediction)), \
                              name=num)
                avg = df['avg']
                avg = avg / self.getStepSize()
                ax.plot(self.bins, avg, color='k', linewidth=2.0, \
                        label='Gillespie average over %d runs' % len(df.columns))
                ax.plot(self.bins, prediction, color='r', linewidth=2.0, \
                        label='Continuum solution')
                self.finalize_figure(fig, ax, values, name)
                num+=1
                idx=0

            if idx == 0:
                print('create new figure')
                fig = plt.figure()
                ax = plt.subplot(111)

            x = df[column]

            if total==0: total = np.sum(x)
            total_tmp = np.sum(x)

            # rescale after total calculation
            x = x / self.getStepSize()

            if ylim_max <= np.max(x):
                ylim_max = np.max(x)

            assert total_tmp == total, 'Total values do not fit!'

            print('idx=', idx, ' len=', len(colors), ' color=', colors[idx])
            print('bins_length=', len(self.bins), ' len(x)=', len(x))
            ax.plot(self.bins, x, 'ro', color=colors[idx])
            idx+=1

        if idx is not 0:
            values = dict(total=total, steps=path_data.steps, time=time,\
                          ylim_max=max(ylim_max, np.max(prediction)),\
                          name=num)

            avg = df['avg']
            avg = avg / self.getStepSize()
            ax.plot(self.bins, avg, color='k', linewidth=2.0,  \
                    label='Gillespie average over %d runs' % len(df.columns))
            ax.plot(self.bins, prediction, color='r', linewidth=2.0, \
                    label='Continuum solution')
            self.finalize_figure(fig, ax, values, name)

    def plot_avg(self, path_data, prediction, time, tag='avg'):
        df = path_data.dataFrame
        idx=0
        total=0
        ylim_max=0.0
        # recompute average
        df['avg']=df.mean(axis=1)
        x = df['avg']

        # sum before rescaling to a density
        total = np.sum(x)

        # rescale to a density
        x = x / self.getStepSize()
        print('stepSize=', self.getStepSize())

        fig = plt.figure()
        ax = plt.subplot(111)

        try:
            values = dict(total=total, steps=path_data.steps, time=time,\
                      ylim_max=max(np.max(x), np.max(prediction)), \
                      error=self.rel_norm2(prediction,x) * 100.0)
        except ValueError:
            values = dict(total=total, steps=path_data.steps, time=time,\
                      ylim_max=max(np.max(x), np.max(prediction)))
        except:
            raise

        ax.plot(self.bins, x, 'ro', color='k', linewidth=2.0, \
                label='Gillespie average over %d runs' % len(df.columns))

        ax.plot(self.x_prediction, prediction, color='r', linewidth=2.0, \
                label='Continuum solution')

        self.finalize_figure(fig, ax, values, tag)

    """ do a plot for adhesion simulation data
        the list keys is there to restrict keys to be plotted
        this is useful for debugging """
    def plot(self, keys=[]):

        self.get_data()
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

        rw_type_name, simulation_name, label_cont, = self.get_plot_names(rw_type)
        if self.isAdhesionCompare(rw_type):
            adata = self.get_related_sim()
            if adata is None:
                return
            print('adata keys=', adata.keys())

        for key, path_data in iter(sorted(self.data.items())):

            if keys and key not in keys:
                continue

            if key < 25.0:
                continue

            df = path_data.dataFrame
            steps = path_data.steps

            print('step_width=', bar_width, end='')
            self.bins=np.arange(-self.getDomainLeftBoundary(),
                            self.getDomainRightBoundary(),
                            bar_width)

            print(' Length_bins=', len(self.bins), ' Domain=[', \
                  -self.getDomainLeftBoundary(), ', ', \
                  self.getDomainRightBoundary(), '].')

            if plt is not None: plt.clf()

            label_ssa = 'Density of Gillespie SSA simulation'
            labels = []

            if self.isAdhesionCompare(rw_type):
                self.x_prediction, u_prediction = self.getStateAtKey(adata, key)
                print('prediction=', u_prediction)

                self.plot_individual_paths(path_data, u_prediction, key)
                # don't compute correction if peaks haven't formed
                # maybe just make the computation silently fail
                if key > self.CompareThreshold:
                    print('computing correction for %f.' % key)
                    self.counter = 0
                    df = self.correct_paths(df, u_prediction)
                    path_data2 = rw.PathDataContainer(steps, df)
                    self.plot_individual_paths(path_data2, u_prediction,
                                               key, tag='corrected')

                    self.plot_avg(path_data2, u_prediction, key)
                else:
                    self.plot_avg(path_data, u_prediction, key)

            if self.isFFTCompare(rw_type):

                self.x_prediction, u_prediction = self.compute_fft(key)
                self.plot_avg(path_data, u_prediction, key)

    # Compute norm between vectors x, y
    def norm1(self, x):
        return np.linalg.norm(x, ord=1)

    # compute l2 between vectors x, y
    def norm2(self, x):
        return np.linalg.norm(x, ord=2)

    """ computs ||x - y||^2 / ||x||^2 """
    def rel_norm2(self, x, y):
        return (self.norm2(x-y) / self.norm2(x))

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
    plotter = Plotter(193, Compare=True)
    #plotter.plot()
