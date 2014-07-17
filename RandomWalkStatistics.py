# RandomWalkStatistics.py
#
# Wrapper for the RandomWalkStatistics c++ class
#
import ctypes as c
from numpy.ctypeslib import ndpointer
from ctypes import *

lib = c.cdll.LoadLibrary('./build/lib/libStatistics.so')

class Data(c.c_void_p):
	
	def __init__(self):
		self.t = []
	
	def __new__(cls, *args, **kwds):
		self.t = []
		raise TypeError("cannot create %r instance" % cls.__name__)
	
	def __str__(self):
		return str(self.t)
	
	@property
	def dimension(self):
		return lib.RandomWalkData_getSize(self)
	
	def xaverage(self, time):
		return self.t[time][0]
	
	def xaverage(self):
		return [x[0] for x in self.t]
	
	def x2average(self, time):
		return self.t[time][1]
	
	def x2average(self):
		return [x[1] for x in self.t]
	
	def print_res(self):
		lib.RandomWalkData_print(self)
		
	def get_elements(self):
		lib.RandomWalkData_getSize.restype = c_long
		size = lib.RandomWalkData_getSize(self)
		
		lib.RandomWalkData_getData.restype = ndpointer(dtype=c.c_long, shape=(2*size,))
		l = lib.RandomWalkData_getData(self)
		self.t = [[l[2*i], l[2*i+1]] for i in range(size) ]
#
# TODO
# define an iterator for this class instead of copying all the data!!!
#
class RandomWalkStatistics(c.c_void_p):
	def __init__(self):
		self.value = lib.new_RandomWalkStatistics().value

	@property
	def dimension(self):
		return lib.RandomWalkStatistics_getSize(self)

	@property
	def size(self):
		return (lib.RandomWalkStatistics_getSize(self), 
				lib.RandomWalkStatistics_getElementSizeMax(self))

	def get_elements(self):
		el = []
		for i in range(self.dimension):
			size = lib.RandomWalkStatistics_getSize(self)
			lib.RandomWalkStatistics_getData.restype = Data
			
			cells = lib.RandomWalkStatistics_getData(self, i)
			cells.get_elements()
			el.append(cells)
		return el
	
	def print_res(self):
		lib.RandomWalkStatistics_printData(self)
	
	def get_cobj(self):
		return self.value

lib.new_RandomWalkStatistics.restype = RandomWalkStatistics
lib.new_RandomWalkStatistics.argtype = None
	
lib.RandomWalkStatistics_getSize.restype = c.c_long
lib.RandomWalkStatistics_getSize.argtype = None

lib.RandomWalkStatistics_getElementSize.restype = c.c_long
lib.RandomWalkStatistics_getElementSize.argtype = None

lib.RandomWalkStatistics_getData.argtype = [RandomWalkStatistics, c.c_long]