# OME

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
