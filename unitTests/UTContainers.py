import os
import csv
from UTFactories import *


# base class for unit testing

# class BaseUnitTest:
# '''base class for unit testing?'''
# def __init__(self):
# # do nothing

# def runTest(self,path):
# # do nothing

# def retrieveResults():
# # do nothing; should be a dictionary of booleans


class TestSet:
    '''For running and capturing test results. all UniRangeInput sets are incremented
		in unison, until all fail to run'''

    # args represent filepaths, overriding arguments, etc depending on need.
    def __init__(self):
        self.name = ""
        self.label = ""
        self.args = ""
        self.simParams = []
        self.csvName = ''
        self.compArgs = None
        self.comparator = None

    # Parsing the testing profiles upon initialization
    def __init__(self, path):
        with open(path, 'rb') as paramFile:
            self.label = self.__nextValidLine(paramFile)
            self.name = self.__nextValidLine(paramFile)
            self.ctrlPath = self.__nextValidLine(paramFile).replace('/', os.path.sep)
            self.compArgs = self.__nextValidLine(paramFile).split(',')
            self.comparator = GenComparator(self.compArgs)

            self.simParams = []

            # iterate through the parameter file
            while True:
                line = self.__nextValidLine(paramFile)
                if line.__len__() <= 0:
                    break
                self.__processParam(line)
        self.__refreshCSVName()

    def __str__(self):
        '''build OMEEngine-friendly argument list'''

        # Alter for options depending on comparator class chosen in profile
        if self.compArgs[0] == "simpleFloat":
            args = ' -c"' + os.getcwd() + os.path.sep + 'temp_files' + os.path.sep + self.csvName + '" '
        else:
            args = ' -f"' + os.getcwd() + os.path.sep + 'temp_files' + os.path.sep + self.csvName + '" '



        for r in self.simParams:
            args += '-o{0}={1} '.format(r.name, r.current)

        return args

    def advanceRanges(self):
        '''Increment each value generator if it has values left. Returns
		true if there are more values to increment, or false otherwise'''
        done = True
        for r in self.simParams:
            try:
                r.next()
                done = False
            except StopIteration:
                pass

        self.__refreshCSVName()
        return done == False

    def __nextValidLine(self, paramFile):
        ''' retrieve next valid line, stripping leading whitespace,
		    ignoring empty lines and comments'''
        line = ''
        while True:
            line = paramFile.readline()
            if line.__len__() == 0:
                break
            line = line.strip()
            if line.__len__() > 0 and line[0] != '#' and line[0] != '\n':
                break;
        return line

    def __processParam(self, info):
        '''build a parameter set based on comma-delimited arguments '''
        args = info.split(',')

        inputGen = GenInputFactory(args)
        if inputGen != None:
            self.simParams.append(inputGen)

    def __refreshCSVName(self):
        ''' build unique identifier for csv name'''
        self.csvName = self.name + '__'
        for r in self.simParams:
            self.csvName += '{0:g}'.format(r.current).replace('.', '_') + '__'
        self.csvName += '.csv'


class RunResults:
    ''' structure for holding result details of run '''

    def __init__(self):
        self.inVals = {}
        self.testResults = {}
        self.error = ''

    def __init__(self, tester):
        self.inVals = {}
        self.error = ''
        # build path to key file using inVals as ordered in simParams

        for r in tester.simParams:
            self.inVals[r.name] = r.current

        self.testResults = {}

        # run through all comparisons here
        # get file key
        omeCsvPath = os.getcwd() + os.path.sep + 'temp_files' + os.path.sep + tester.csvName
        keyCsvPath = os.getcwd() + os.path.sep + 'result_keys' + os.path.sep + tester.name + os.path.sep + tester.csvName

        # load records
        omeVals = {}
        keyVals = {}
        try:
            with open(omeCsvPath, 'rb') as omeFile, open(keyCsvPath, 'rb') as keyFile:
                omeVals = self.__loadCSVRecords(omeFile, 1)
                keyVals = self.__loadCSVRecords(keyFile, 0)
        # run tester's comparator only on values whose name ends with _test
            for k in keyVals.keys():
                if k.endswith('_test'):
                    try:
                        self.testResults[k] = str(tester.comparator.compareRecords(omeVals[k], keyVals[k])) + '\n' + str(omeVals[k]) + '\n' + str(keyVals[k])
                    except KeyError as err:
                        self.testResults[k] = "ERROR: Key not found: " + err.message
        except Exception as failError:
            self.error = "Could not open file: " + tester.csvName + "\n" + failError.message

    def __str__(self):
        outStr = self.error
        if self.error.__len__() == 0:
            outStr = 'Input values:\n'
            inKeys = self.inVals.keys()
            inKeys.sort()
            for k in inKeys:
                outStr += '\t{0}={1}\n'.format(k, self.inVals[k])
            outStr += '\n================================\nTest Results:\n'
            for k, v in self.testResults.iteritems():
                outStr += '\t{0:12} : {1}\n'.format(k[:-5], v if isinstance(v, str) else 'pass' if v else 'FAIL')

        return outStr

    def __loadCSVRecords(self, inFile, nameCol):
        ''' load records from csv '''

        ret = {}
        rdr = csv.reader(inFile)

        # skip header
        rdr.next()


        for row in rdr:
            if row:                             # Deal with blank new lines
                ret[row[nameCol]] = row
            
        return ret
