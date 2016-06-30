#################################################################################
#                                                                               #
# make file for model project under mac OS-X                                    #
#                                                                               #
# Required Arguments:                                                           #
#    lib_headers   Location of header files for other OME libraries.            #
#    mdl_headers   Location of header files for model project to be compiled.   #
#    omeLibs       Path to location of required OME libraries.                  #
#    outdir        Location of where the newly compiled library will be placed. #
#    mdlname       The name of the class representing the model logic.          #
#                                                                               #
#                                                                               #
# Notes:                                                                        #
#    The OS X build requires Clang to be installed, and expects to use the      #
#    XCode alias ("Clang" with an upper case 'C'). The easiest way to get XCode #
#    is to retrieve the free distribution from the Mac App Store. See the Apple #
#    developer site for more details.                                           #
#                                                                               #
#################################################################################

#general compile settings
CC = Clang
CFLAGS = -dynamiclib -g -Wall -std=c++11 

#include paths
INCPATH = -I"$(lib_headers)" -I"$(mdl_headers)"

#library path
OME_LIBPATH = -L$(omeLibs)

#libs to link against
LFLAGS = -llibLuaLib -llibOMRERuntime

#Necessary Preprocessor macros
DEFINES = -D __EXPORT__=  -D LUA_COMPAT_5_1 -D OME_MDL_BUILD -D __MDL_EXPORT__= -D PASCAL=

#suppressed warnings
NO_WARNS= -Wno-unknown-pragmas -Wno-reorder

#user args
OUTPATH=$(outdir)/$(mdlname)
MDLNAME=$(mdlname)

#weak linking info for OS X; will need to be changed for Linux
WKLNK= -Wl,-rpath,@rpath/ -install_name @rpath/$(MDLNAME).dylib

#build configs
#buildclean:
all:
	$(CC) $(CFLAGS) -o "$(OUTPATH).dylib" $(WKLNK) $(MDLNAME).cpp ModelProject.cpp $(DEFINES) $(INCPATH) $(OME_LIBPATH) $(LFLAGS) $(NO_WARNS)
#	rm *.o
