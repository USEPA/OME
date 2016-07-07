**Instructions for building OMEAdapter as of 7/5/2016.**

OMEAdapter is a plugin for Envision that uses OME as its core. In order to build the plugin, use the following steps:

1. Download the Envision SDK from here: http://envision.bioe.orst.edu/Downloads.aspx. NOTE: the Envision source code can be used as well, just make sure to build Libs.dll.
2. Open the OME solution file in Visual Studio, and navigate to _External Plugins Project-->OMEAdapter_ in the Solutions explorer.
3. Replace the EnvExtension.cpp entry in the Project with the EnvExtension.cpp included with the SDK.
4. Right click on the OMEAdapter project file and select Properties.
5. Add the SDK Include directory to _Configuration Properties --> C/C++ --> General --> Additional Include Directories_.
6. Add the SDK base directory to _Configuration Properties --> Linker --> General --> Additional Library Directories_.
7. Ensure that Libs.lib is in _Configuration Properties --> Linker --> Input --> Additional Dependencies_. If not, add it.
8. Close the Properties dialog by pressing OK.
9. Right-click on the OMEAdapter Project and select _Build_. With any luck everything should build successfully.
