*****************************
*                           *
* OME Unit Testing overview *
*                           *
*****************************

Quick start (run command):

     runUnitTests.py <path to OME Engine> [<output file>]
	 
	 where:
		<path to OME Engine> is the path to OMEEngine.exe.
		<output file> is an optional path to where the test results should be written;
		              if omitted, the result is printed directly to stdout.
	 example:
		python runUnitTests.py ..\project_files\Windows\x64\Debug\OMEEngine.exe testResults.txt

=============================

Python script breakdown:
	runUnitTests.py: The script to be executed on the command line. Contains entry point for 
	                 running tests. Inserts a list of all testing profiles 'tester'. Also
	                 outputs results. This file will rarely be edited.
					 
	UTContainers.py: Contains the classes for storing/parsing test conditions and results derived 
					 from the testing profiles. Calls functionality from UTfactories.py to 
					 compare results. This file will be occasionally edited.
					 
	UTFactories.py:  Contains the classes used to generate inputs and compare results. This 
	                 file will be modified often.

=============================
					 
Directory breakdown:
	result_keys:      Contains .csv files that hold the correct answers for unit tests.
	
	source_models:    Contains models used to generate keys from Vensim or Simile. Not used 
	                  in unit testing.
					  
	temp_files:       Contains output .csv files generated from OMEEngine runs. Contents can
	                  be deleted between Unit Test runs.
					  
	test_models:      Contains the OME .omem models used for Unit testing, and any associated
	                  dynamic libraries. There are also two subfolders: compiled_tests and 
					  interpreted_tests. Each of these folders will hold the .omec files used
					  to describe the simulation configuration for each model used for unit 
					  testing. Tests that involve using a compiled dynamic library should be
					  placed in compiled_tests, while .omec files used with testing lua-based
					  simulations should be placed in interpreted_tests.
					  
	testing_profiles: Contains text files that outline the structure of each set of unit tests.
	                  When runUnitTests.py is executed, each text file is processed in sequence
					  and used to structure a new set of unit tests. For details on the structure,
					  see TestProfileDescription.txt








***********************************
*                                 *
* Test Profile Format Description *
*                                 *
***********************************

The text files found in the testing_profiles directory are known as test profiles. Each
test profile contains information to structure a set of unit tests for a specific model.
Each entry is interpreted in the order that it is found. Note that any line beginning with
a '#' character or is empty will be skipped during processing.

The file format is thus:

<Test Label>
<solution prefix>
<control file>
<result comparator>
<input generator>
<input generator>
    .
	.
	.
<input generator>


Key:
	<Test Label>:        This is the label used in the final report for the profile's tests.
	<solution prefix>:   The name of the directory in result_keys where the solution files
	                     are found, and the prefix used for each solution file.
	<control file>:      The path to the control file with test_models as root. The .omec
	                     extension of the control file should be omitted from this entry.
	<result comparator>: Entry for the comparator used; presently there is only one entry per
	                     test profile. See the section "Result Comparators", for a list of 
						 acceptable entries.
	<input generator>:   Identifier for input generator to be applied to the specified model 
	                     component. One or more entries are acceptable. See the section  
						 "Input Generators" for a list of acceptable entries.
						 
						 
Result Generators:
	simpleFloat,<% deviation>: 
		compare two float values, and pass the test if the relative difference between the two 
		values is less than or equal to <% deviation>. Set <% deviation> to 0 to require 
		absolute equality.

	stepFloat,<% deviation>: 
		Very similar to simpleFloat, except that the values are compared for the every step including the initial state of the variables.

Input Generators:
	single,<field label>,<value>
		A single constant value that does not change throughout the unit testing.
		
	simpleSet,<field_label>,<val 1>,<val 2>,...,<val n>
		A set of values to be iterated through; every argument after <field_label> will be
		considered part of the set to be evaluated.
		
	uniRange,<field label>,<start value>,<end value>,<increment>
		Iterate through a uniform range, starting at <start value>, ending with <end value>,
		and incrementing at each step with <increment>







***********************************
*                                 *
* Creating test model DLLs in VS  *
*                                 *
***********************************



1.  Build & run ModelClassBuilder. 

	Open OME in Visual studio and build the ModelClassBuilder. Through either command line input or 
	using the wizard enter the model file's (omem) path within the unitTests. The other values are hardcoded
	as shown in the example. The class name has to be TestModel as this is used in the source:
	
	note*: IF in the wizard remember to click on the ModelClassBuilder tab
	example: 
		Input Model File: trunk\unitTests\test_models\conditionals.omem

		Output Directory: trunk\project_files\Windows\ModelProject
		
		Class Name: TestModel   (Always set the class name as TestModel)




2. Build the ModelProject. This will use the files TestModel.cpp/.h to in the Output Directory to generate the dll. 
	After building ModelProject, the path for the dll necessary for testing should appear as follows.

	trunk\project_files\Windows\x64\Debug\ModelProject.dll

	Put the file in the following subdirectory and change the name to that of the test being ran, ending with _dbg. In this case
	it is conditionals_dbg.dll

	trunk\unitTests\test_models\conditionals_dbg.dll

3. runTests.cmd