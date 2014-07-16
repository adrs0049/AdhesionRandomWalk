# RandomWalkStatistics.py
#
# Wrapper for the RandomWalkStatistics c++ class
#
import ctypes as c

class RandomWalkStatistics(object):
	def __init__(self):
		self.lib = c.cdll.LoadLibrary('./build/lib/libStatistics.so')
		self.obj = self.lib.new_RandomWalkStatistics()
		
	def get_size(self):
		rows = self.lib.RandomWalkStatistics_getSize(self.obj)
		cols = self.lib.RandomWalkStatistics_getElementSize(self.obj)
		return (rows, cols)
		
	def get_data(self):
		return self.lib.RandomWalkStatistics_getData(self.obj)
	
if __name__ == '__main__':
	d = RandomWalkStatistics()
	print 'object create at: ', d
	print 'object contains: '
	d.get_data()
	print 'size is: ',d.get_size()
	print 'end'