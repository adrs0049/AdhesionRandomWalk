# RandomWalkData.py
#
# Wrapper for the RandomWalkStatistics c++ class
#
import ctypes as c
from numpy.ctypeslib import ndpointer

class RandomWalkData(object):
	def __init__(self):
		self.lib = c.cdll.LoadLibrary('./build/lib/libStatistics.so')
		self.obj = self.lib.new_RandomWalkData()
		
	def get_size(self):
		return self.lib.RandomWalkData_getSize(self.obj)
		
	def get_data(self):
		self.lib.RandomWalkData_getData.restype = \
			ndpointer(dtype=c.c_long, shape=(self.get_size(),))
		return self.lib.RandomWalkData_getData(self.obj)
	
	def add_data(self, i, j):
		self.lib.RandomWalkData_addData(self.obj, i, j)		
	
if __name__ == '__main__':
	d = RandomWalkData()
	print 'object create at: ', d
	d.add_data(1,2)
	f = d.get_data()
	print 'Data contains=',f
	print 'size is: ',d.get_size()
	print 'end'