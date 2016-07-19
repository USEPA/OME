#! /bin/python

import sys
import os
import subprocess
import datetime
from UTContainers import *

def runSingleTest(tester,enginePath,outputOpt):
	'''Run a test using testSet and generate a results container'''
	fullControlPath=os.getcwd()+os.path.sep+'test_models'+os.path.sep+tester.ctrlPath+'.omec'
	sys.stdout.write(".")
	#solution for suppressing output from: http://stackoverflow.com/questions/11269575/how-to-hide-output-of-subprocess-in-python-2-7
	FNULL = open(os.devnull, 'w')
	print str(tester)
	#subprocess.call(enginePath+str(tester)+' -q '+fullControlPath,stdout=FNULL)
	os.system(enginePath+str(tester)+' -q '+fullControlPath)
	
	return RunResults(tester,outputOpt)

def runTests(tester,enginePath,outputOpt):
	'''Run all permutations of a test set '''
	sys.stdout.write('Running tests for "'+tester.label+'"')
	results=[]
	while True:
		results.append(str(runSingleTest(tester,enginePath,outputOpt)))
		if tester.advanceRanges()==False:
			break
	
	sys.stdout.write(" Done.\n")
	return '>>>>> '+tester.label+' <<<<<\n\n'+('\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n'.join(results))


if __name__=='__main__':
	'''This is the equivalent of main() in c-like languages when the script is 
	executed from the command line'''
	
	if sys.argv.__len__() > 1:
		#grab path to engine
		omeEngine=sys.argv[1]
		outFile=""
		outOptions=[]
		
		if sys.argv.__len__()>2:
			for x in sys.argv[2:]:
				if x.startswith('-'):
					outOptions.append(x)
				else:
					outFile=x
				
				
		#assume script is being run from unitTests base directory
		paramDir=os.getcwd()+os.path.sep+'testing_profiles'
		
		#this parsing method will only work if we want to read all files listed
		# and there are no directories
		paramFiles=os.listdir(paramDir)
		
		runResults=[]
		for f in paramFiles:
			#run through all tests for config...(feeding in param files in testing_profiles)
			#									param file path         engine path
			runResults.append(runTests(TestSet(paramDir+os.path.sep+f),omeEngine,outOptions))
		
		outStr='Unit Testing begun on '+datetime.datetime.now().strftime("%B %d, %Y at %I:%M%p")+'\n\n'
		outStr+='\n\n################################\n################################\n\n'.join(runResults)
		if outFile.__len__()>0:
			with open(outFile,'w') as outf:
				outf.write(outStr)
			print "results written to: {0}".format(outFile)
		else:	
			print outStr
			
			#run through each grouping of variables and generate results
	else:
		#print help
		print '''Usage:
'''+__file__+''' <exec_path> [<result_file>]
	run unit tests using the OMEEngine executable found at <exec_path>.
	If a <result_file> path is provided, then the report is 
	written out to it; otherwise, the report is written to screen.'''