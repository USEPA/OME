# Open Modeling Environment (OME)

The Open Modeling Environment (OME) is a cross-platform, open source System Dynamics modeling engine that emphasizes modularity and explicit spatial representation as core design features.

##Project availability

| Project Name           | Type       | Description                                                                                   | Windows?   | Mac?       | Linux?     | 
| :--------------------- | :--------- | :-------------------------------------------------------------------------------------------- | :---------:| :--------: | :--------: |
| OMERuntime             | Library    | Base Library for OME functionality.                                                           | **Yes**    | **Yes**    | **Yes**    |
| OMEDraw                | Library    | Auxiliary handlers for OMERuntime that are not always needed.                                 | **Yes**    | **Yes**    | **Yes**    |
| CSVSpatialDataProvider | Library    | SDP Backend for reading spatial coverage data from CSVs.                                      | **Yes**    | **Yes**    | **Yes**    |
| OMEEngine              | Executable | Command-line tool for running OME simulations.                                                | **Yes**    | **Yes**    | **Yes**    |
| ModelClassBuilder      | Executable | Command-line tool for generating C++ code from model logic.                                   | **Yes**    | **Yes**    | **Yes**    |
| UniversalConverter     | Executable | Command-line tool for producing OME-compatible models from various sources.                   | **Yes**    | **Yes**    | **Yes**    |
| OMEWizard              | Executable | UI tool for launching the various CLI tools (UniversalConverter,ModelClassBuilder,OMEEngine). | **Yes**    | **Yes**    | No         |
| OMESimRunner           | Executable | UI tool for running the simulation engine and viewing results.                                | **Yes**    | **Yes**    | No         |
| OMEAdapter             | Library    | OME plugin for Envision [Envision homepage](https://envision.bioe.orst.edu).                  | **Yes**    | No         | No         |
| OMEMultiProc           | Executable | Tool used to run instances of OMEEngine in parallel.          
| **Yes**  | No         | No         |

##3rd Party Libraries and Tools Used:

* [Lua](https://www.lua.org/) -- Scripting language and runtime. Used to customize aspects of user input.
* [TinyXML-2](http://www.grinninglizard.com/tinyxml2/) -- Lightweight XML reader and writer.
* [Doxygen](http://www.stack.nl/~dimitri/doxygen/index.html) -- Utility for producing source code documentation.
* [Shiny profiler](https://code.google.com/archive/p/shinyprofiler/) -- Preprocessor-based profiling tool.
* [CMake](https://cmake.org/) -- Meta build system presently used to deploy OME's Linux build.

##Source Installation:

1. Check out the source code.

2. Retrieve required source code dependencies.
	* Download Lua >= 5.3 from [here](https://www.lua.org/download.html).
		- Copy the contents of the root archive into extern_support/lua/.
	* Download the source for tinyxml2 from [here](https://github.com/leethomason/tinyxml2).
		- Copy tinyxml2.h and tinyxml2.cpp into extern_support/tinyxml2/.
		
3. Build static lua library. Instructions will be dependent on platform (see below).

##Build Under Windows

Open the project_files/Windows/OME.sln file in Visual Studio. Select BUILD --> Build Solution.

##Build Under OS X / macOS

Open the project_files/OS_X/OME_OSX.xcworkspace in XCode. Select the "Build All" option from the scheme/target dropdown, and Select Project --> Build.

##Build Linux

**NOTE:** Clang or g++ >= 4.7 with either libstdc++ or libc++ required.

Install [CMake](https://cmake.org/) if it isn't already. `cd` to the desired build directory and run cmake on project_files/Linux/.
To build lua: `cd` to exern_support/lua which should contain the files from step 2 of the _Source Installation_ section above; run `make linux`.
