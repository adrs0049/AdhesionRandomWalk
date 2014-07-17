# RandomWalkStatistics.py
#
# Wrapper for the RandomWalkStatistics c++ class
#
import ctypes as c
from numpy.ctypeslib import ndpointer
from ctypes import *


lib = c.cdll.LoadLibrary('./build/lib/libStatistics.so')

class DataBase(c.c_void_p):
	
	def dim(self):
		lib.dim.restype = c.c_int
		lib.dim.argtypes = [Cell]
		return lib.dim(self)

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
		#print 'self=',self
		#print 'Data get elements'
		lib.RandomWalkData_getSize.restype = c_long
		#print 'get size'
		size = lib.RandomWalkData_getSize(self)
		#print 'Data sz=', size
		
		lib.RandomWalkData_getData.restype = ndpointer(dtype=c.c_long, shape=(2*size,))
		#print 'call get RandomWalkData data'
		l = lib.RandomWalkData_getData(self)
		#print 'end call'
		self.t = [[l[2*i], l[2*i+1]] for i in range(size) ]
		#print 't=',self.t
		
	#	print 'end get_elem in Data'
		
	
#
# TODO
# define an iterator for this class instead of copying all the data!!!
#
class RandomWalkStatisticsp(c.c_void_p):
	def __init__(self):
		#pair = c.c_double * 2
		#point = (pair * len(p))(*(pair(*q[:2]) for q in p))
		#print 'pt=',point
		#self.value = lib.new_RandomWalkStatistics(point, len(p)).value
		self.value = lib.new_RandomWalkStatistics().value

	@property
	def dimension(self):
		return lib.RandomWalkStatistics_getSize(self)

	@property
	def size(self):
		return (lib.RandomWalkStatistics_getSize(self), 
				lib.RandomWalkStatistics_getElementSizeMax(self))

	def get_elements(self):
		#print 'get em'
		el = []
		#print 'dim=',self.dimension
		for i in range(self.dimension):
			size = lib.RandomWalkStatistics_getSize(self)
			#print 'sz=',size
			lib.RandomWalkStatistics_getData.restype = Data
			
			cells = lib.RandomWalkStatistics_getData(self, i)
			cells.get_elements()
			#print 'cells=', cells
			#print 'cells=', self.print_res()
			#print' test'
			#for cell in cells:
			#	print 'cell=', cell
			
			
			#print 'cells[0]=', cells[0]
			el.append(cells)
		return el
	
	def print_res(self):
		lib.RandomWalkStatistics_printData(self)
		#print 'Done'
	
	def get_cobj(self):
		return self.value

class RandomWalkStatistics(object):
	def __init__(self):
		self.lib = c.cdll.LoadLibrary('./build/lib/libStatistics.so')
		self.obj = self.lib.new2_RandomWalkStatistics()
		
	def get_size(self):
		rows = self.lib.RandomWalkStatistics_getSize(self.obj)
		cols = self.lib.RandomWalkStatistics_getElementSize(self.obj)
		return (rows, cols)
		
	def get_data(self):
		size = self.get_size()
		#print 'size=',size
		self.lib.RandomWalkStatistics_getData = ndpointer(dtype = c.c_long, shape=size)
		return self.lib.RandomWalkStatistics_getData(self.obj)
	
	def get_cobj(self):
		return self.obj

lib.new_RandomWalkStatistics.restype = RandomWalkStatisticsp
lib.new_RandomWalkStatistics.argtype = None
	
lib.RandomWalkStatistics_getSize.restype = c.c_long
lib.RandomWalkStatistics_getSize.argtype = None

lib.RandomWalkStatistics_getElementSize.restype = c.c_long
lib.RandomWalkStatistics_getElementSize.argtype = None
#
#lib.RandomWalkStatistics_getData.restype = c.POINTER(Data)
lib.RandomWalkStatistics_getData.argtype = [RandomWalkStatisticsp, c.c_long]

#d = RandomWalkStatisticsp([[1,2],[3,4]])
#res = d.get_elements()
#print 'res=',res
#print 'res[0]=',res[0]
#print 'res[0][0]=',res[0][0], ' dim=', res[0][0].dimension


#p = res[0][0]
#t = p.get_elements()

if __name__ == '__main__':
	#d = RandomWalkStatistics()
	#print 'object create at: ', d
	#print 'object contains: '
	#d.get_data()
	#print 'size is: ',d.get_size()
	#print 'end'
	
	lib.new_RandomWalkStatistics.restype = RandomWalkStatisticsp
	lib.new_RandomWalkStatistics.argtype = None
	
	lib.RandomWalkStatistics_getSize.restype = c.c_long
	lib.RandomWalkStatistics_getSize.argtype = None
	
	lib.RandomWalkStatistics_getElementSize.restype = c.c_long
	lib.RandomWalkStatistics_getElementSize.argtype = None
	
	lib.RandomWalkStatistics_getData.restype = c.POINTER(Data)
	lib.RandomWalkStatistics_getData.argtype = [RandomWalkStatisticsp, c.c_long]
	
	#d = RandomWalkStatisticsp([[1,2],[3,4]])
	#print d.dimension
	#print d.size
	#res = d.get_elements()
