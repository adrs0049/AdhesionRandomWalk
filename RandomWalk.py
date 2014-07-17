# RandomWalk.py
#
# Wrapper for the RandomWalkBase + RandomWalkSimple c++ class
#
import ctypes as c

class ListPOINTER(object):
	def __init__(self, etype):
		self.etype = etype
	
	def from_param(self, param):
		if isinstance(param, (list, tuple)):
			return (self.etype * len(param))(*param)

class RandomWalk(object):
	def __init__(self, LookAtTimes, pathLength):
		self.lib = c.cdll.LoadLibrary('./build/lib/libRandomWalk.so')
		
		self.lib.new_RandomWalk.argtypes = [ ListPOINTER(c.c_long), c.c_long, c.c_long]

		self.obj = self.lib.new_RandomWalk(LookAtTimes, len(LookAtTimes), pathLength)

	def get_size(self):
		return self.lib.RandomWalk_getLookAtTimeSize(self.obj)

	def get_NumberOfSteps(self):
		return self.lib.RandomWalk_GetNumberOfSteps(self.obj)

	def get_cobj(self):
		return self.obj

if __name__ == '__main__':
	d = RandomWalk([1,2,3],4)
	print 'object create at: ', d
	print 'size=',d.get_size()
	print 'steps=',d.get_NumberOfSteps()