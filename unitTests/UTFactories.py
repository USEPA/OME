import sys
import os

###
#  Factory methods
###

def GenInputFactory(args):
	'''generate a specific input factory based on first argument'''
	if args.__len__() and isinstance(args[0],str):
		#return value based on first args
		if args[0]=='uniRange':
			#create a uniform range
			return UniRangeInput(args[1],float(args[2]),float(args[3]),float(args[4]))
		elif args[0]=='single':
			return SingleInput(args[1],float(args[2]))
		elif args[0]=='simpleSet':
			return SimpleSet(args[1],[float(i) for i in args[2:]])
		#more options to come!


	#no match; return none
	return None

def GenComparator(args):
	'''generate a specific input factory based on first argument'''
	if args.__len__() and isinstance(args[0],str):
		#return value based on first args

		if args[0]=='simpleFloat':
			#create a uniform range, return threshold
			return SimpleFloatCompare(float(args[1]))

		if args[0]=='stepFloat':
			return StepFloatCompare(float(args[1]))
		#more options to come!


	#no match; return none
	return None


#=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~
#=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~

###
# Input classes
#	Required attributes:
#		current (for SimpleFloatCompare)
#		name
#	Required methods
#		__iter__()
#		next()
###

class UniRangeInput:
	''' Generates a range of inputs for a set of values '''

	EPSILON=1.0e-8
	def __init__(self):
		self.name=""
		self.current=0.0
		self.end=0.0
		self.inc=0.0

	def __init__(self,name,start,end,inc):
		self.name=name
		self.current=start
		self.end=end
		self.inc=inc

	def __iter__(self):
		return self

	def next(self):
		if self.current == self.end:
			raise StopIteration
		else:
			self.current+=self.inc
			if (self.inc > 0 and self.current>self.end) or (self.inc < 0 and self.current<self.end):
				self.current=self.end
			if abs(self.current)<UniRangeInput.EPSILON:
				self.current=0

			return self.current

class SingleInput:
	''' store and return a single value '''
	def __init__(self):
		self.current=0
		self.name=''

	def __init__(self,name,inVal):
		self.name=name
		self.current=inVal

	def __iter__(self):
		return self

	def next(self):
		raise StopIteration

class SimpleSet:
	''' store and return values from a set '''
	def __init__(self):
		self.current=0
		self.name=''
		self.vals=[]
		self.index=0

	def __init__(self,name,inVals):
		self.name=name
		self.current=0
		self.index=0
		self.vals=inVals
		if inVals.__len__()>0:
			self.current=inVals[0]

	def __iter__(self):
		return self

	def next(self):
		self.index+=1
		if self.index<self.vals.__len__():
			self.current=self.vals[self.index]
		else:
			raise StopIteration
		return self.current

###
# Comparator classes
#
#	Required methods:
#		compareRecords(self,omeRecord_as_list,keyRecord_as_list)
###

class SimpleFloatCompare:
	'''Compare two float values with an acceptable deviation value'''

	def __init__(self):
		self.threshold=0.0
									# WHAT DOES THIS MEAN?
	def __init__(self,threshold):
		self.threshold=threshold

	def compareRecords(self,omeRec,keyRec):
		'''take list of values and do compare. Return true if difference between values is less than or equal todeviation threshold'''

		if str(omeRec[3]) == "1.#INF":					# Not sure why appearing in temp_files misc_signals
			omeRec[3] = 1
		omeVal=float(omeRec[3])
		keyVal=float(keyRec[1])

		rawDev=omeVal-keyVal
		if keyVal!=0:
			rawDev/=keyVal
		deviation=abs(rawDev)

		return deviation <= self.threshold

class StepFloatCompare:
	'''Compare two float values with an acceptable deviation value. Repeat for every step'''

	def __init__(self):
		self.threshold=0.0
									# WHAT DOES THIS MEAN?
	def __init__(self,threshold):
		self.threshold=threshold

	def compareRecords(self,omeRec,keyRec):
		'''take list of values and do compare. Return true if difference between values is less than or equal todeviation threshold'''

		if str(omeRec[3]) == "1.#INF":					# Not sure why appearing in temp_files misc_signals
			omeRec[3] = float('Inf')

		for i in xrange(1,len(keyRec)):			
			omeVal=float(omeRec[i+2])
			keyVal=float(keyRec[i])

			rawDev=omeVal-keyVal
			if keyVal!=0:
				rawDev/=keyVal
			deviation=abs(rawDev)

			if deviation > self.threshold:
				return False

		return True
