OMEMultiProc was built to minimize the time to build a set of models for a given project. 
The need initially came from the Durham-Orange Light Rail project. 

The model dll and omem files are to be placed in the test folder.
The control omec file is to be placed in the compiled_tests subfolder of the test folder. 

Old files are to be archived(subfolder in test). When archiving a profile change it's name 
from profile to a more descriptive one, such as LRP_Profile. 

Notes on File Structure:

The default working directory is x64/release as opposed to OMEMultiproc/bin/release.
This was done to follow the directory structure used within the OME project





The profile schema is as follows (<I am a variable>):

#This is a comment . Each new base model begins with ** followed by the base run name
#IF A FLAG IN THE PROFILE, THE MIN, MAX, and ITER have TO BE SET
#Next lines are all variables that vary in format shown
**Run_Name
<var name in OME Engine>,<Var descriptor>,<var default value>,<min>,<max>,<iterations>,<iter>





#Comments are used to clarify variables/segments etc
#If the iterations(iter) is set to 0, that variable is always set to it's default

