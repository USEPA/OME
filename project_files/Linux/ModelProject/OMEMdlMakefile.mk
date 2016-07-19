#################################################################################
#                                                                               #
# make file for model project under Linux                                       #
#                                                                               #
# Required Arguments:                                                           #
#    lib_headers   Location of header files for other OME libraries.            #
#    mdl_headers   Location of header files for model project to be compiled.   #
#    omeLibs       Path to location of required OME libraries.                  #
#    outdir        Location of where the newly compiled library will be placed. #
#    MDLNAME       The name of the class representing the model logic.          #
#                                                                               #
#                                                                               #
# Notes:                                                                        #
#    Built and tested under clang, but the latest version of g++ should work    #
#    as well.                                                                   #
#                                                                               #
#################################################################################

# example flags for running with additional hooks for nonstandard build setups
#MY_CXXFLAGS=-stdlib=libc++
#MY_INCLUDES=-I"/home/pwingo/build_local/libc/llvm/projects/libcxx/include"
#MY_LIBPATHS=-L../../../build_local/myLibs/
#MY_LFLAGS=-lcxxrt

#general compile settings
CXX = clang++
CXXFLAGS = -O3 -shared -fPIC -Wall -std=c++11 $(MY_CXXFLAGS)

#include paths
INCPATH =  -I"../../../extern_support/tinyxml2" -I"../../../extern_support/Shiny/include" -I"../../../include" -I"../../../include/OMERuntime" $(MY_INCLUDES)

#library path
OME_LIBPATH = -Llib $(MY_LIBPATHS)

#libs to link against
LFLAGS = -lOMERuntime $(MY_LFLAGS)

#user args
OUTPATH="$(OUTDIR)/lib$(MDLNAME).so"

#Necessary Preprocessor macros
DEFINES = -D__EXPORT__=  -DOME_MDL_BUILD -D__MDL_EXPORT__= -DPASCAL= -DMAXPATHLEN=4096 $(MY_DEFINES)

#suppressed warnings
NO_WARNS= -Wno-unknown-pragmas -Wno-reorder

#weak linking info for OS X; will need to be changed for Linux
WKLNK= -Wl,-rpath,@rpath/ -install_name @rpath/$(MDLNAME).so

#build configs
#buildclean:
all:
	$(CXX) $(CXXFLAGS) -v -o $(OUTPATH) $(WKLNK) $(MDLNAME).cpp ModelProject.cpp $(DEFINES) $(INCPATH) $(OME_LIBPATH) $(LFLAGS) $(NO_WARNS)
#	rm *.o
