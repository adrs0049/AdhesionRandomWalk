# RandomWalkEngine.py
#
# Wrapper for the RandomWalkBase + RandomWalkSimple c++ class
#
import ctypes as c
#import libRandom as random
import RandomWalk as randomwalk
import RandomWalkStatistics as stat

class SimpleRandomWalk(object):
	def __init__(self, pathLength):
		self.lib = c.cdll.LoadLibrary('./build/lib/libRandomWalk.so')
		self.random = c.cdll.LoadLibrary('./build/lib/libRandom.so')
		
		# TODO set seed from time or sth like that
		#self.random_engine = random.RandomParkMiller(pathLength)
		self.random_engine = self.random.new_RandomParkMiller(pathLength, 2)
		self.random.RandomParkMiller_SetSeet(self.random_engine, 2)
		
		self.rw = randomwalk.RandomWalk([1,2,3], pathLength)
		
		#print self.random_engine
		self.obj = self.lib.new_SimpleRandomWalk(self.random_engine, self.rw.get_cobj())

	def DoSim(self, paths):
		gatherer = stat.RandomWalkStatistics()
		self.lib.RandomWalk_DoSimulation(self.obj, gatherer.get_cobj(), paths)
		gatherer.get_data()

if __name__ == '__main__':
	d = SimpleRandomWalk(4)
	print 'object create at: ', d
	d.DoSim(10)