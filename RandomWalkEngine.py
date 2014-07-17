# RandomWalkEngine.py
#
# Wrapper for the RandomWalkBase + RandomWalkSimple c++ class
#
import ctypes as c
#import libRandom as random
import RandomWalk as randomwalk
import RandomWalkStatistics as stat
import numpy as np
import matplotlib.pyplot as plt

class SimpleRandomWalk(object):
	def __init__(self, pathLength):
		self.lib = c.cdll.LoadLibrary('./build/lib/libRandomWalk.so')
		self.random = c.cdll.LoadLibrary('./build/lib/libRandom.so')
		
		self.random_engine = self.random.new_RandomParkMiller(pathLength)
		self.rw = randomwalk.RandomWalk([1], pathLength)
		
		self.obj = self.lib.new_SimpleRandomWalk(self.random_engine, self.rw.get_cobj())

	def DoSim(self, paths):
		gatherer = stat.RandomWalkStatistics()
		self.lib.RandomWalk_DoSimulation(self.obj, gatherer.get_cobj(), paths)
		return gatherer.get_elements()

if __name__ == '__main__':
	NumberOfPaths = 10000
	LengthOfEachPath = 2000
	d = SimpleRandomWalk(LengthOfEachPath)
	l = d.DoSim(NumberOfPaths)
	
	# storage for the results
	hist_x=[]
	hist_x2=[]
	
	for elem in l:
		hist_x.append(elem.xaverage()[-1])
		hist_x2.append(elem.x2average()[-1])
		
	if len(hist_x) != len(hist_x2):
		print 'ERROR: lengths of results are different shouldn\'t be'
	print 'len=',len(hist_x)
	
	max_val = max([abs(x) for x in hist_x]) + 1
	hist, bins = np.histogram(hist_x, bins=[x for x in xrange(-max_val, max_val+1, 2)])
	width = 0.7 * (bins[1] - bins[0])
	center = (bins[:-1] + bins[1:]) / 2
	#print 'center=', center, ' width=', width
	
	plt.bar(center, hist, align='center', width=width)
	plt.xlabel("Position in steps")
	plt.ylabel("# of cells")
	plt.title("Simulation of %s paths each of length %s" % (NumberOfPaths, LengthOfEachPath))
	plt.xlim((-max_val, max_val))
	plt.show()