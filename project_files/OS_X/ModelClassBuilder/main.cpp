//
//  main.cpp
//  ModelClassBuilder
//
//  Created by Patrick  Wingo on 10/19/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#include <iostream>
#include "tinyxml2.h"
#include "DebugOutput.h"
#include "SimManager.h"
#include "SimpleCSVSpatialDataProvider.h"
#include "CPPSerializer.h"
#include "FileExts.h"

enum {ARG_CALL=0,ARG_INFILE,ARG_OUTFILE,ARG_OUTDIR, ARG_TEMPLDIR, ARG_GENMAIN};
int main(int argc, const char * argv[])
{
    using namespace std;
    
   	int nRetCode = 0;
    //
    //    CString buildScript(DEFAULT_BUILD_SCRIPT);
    //    if (argc >= 2)
    //        buildScript = argv[ARG_SCRIPT];
    
    
    
    if(argc>=ARG_INFILE)
    {
        TI2Document doc;
        STLString filename(argv[ARG_INFILE]);
        
        if (doc.LoadFile(filename.c_str()) == tinyxml2::XML_NO_ERROR)
        {
            //CString currDir = filename;;
            //PathRemoveFileSpec(currDir.GetBuffer());
            
            STLString fileExt=filename.substr(filename.rfind("."));
            
            
            const float start = 0.0;
            const float stop = 5.0;
            const float interval = 1.0;
            const float stepsize = 1.0;
            unsigned short flags = SimManager::NO_DEBUG | SimManager::NO_PARSING |
            SimManager::NO_SERIALIZING | SimManager::NO_COMPILED;
            if (fileExt != STLString(OME_EXT_CTL))
                flags |= SimManager::NO_SDP | SimManager::NO_COVERAGE;
            SimManager sMngr(filename.c_str(), start, stop, interval, stepsize, flags);
            //try
            //{
            DBG_PRINTBIGDIV();
            DBG_LOGFILE("OME Test log.txt");
            DBG_TIMESTAMP();
            
            //grab model here
            //produce cpp file using serializer
            CPPSerializer serializer;
            
            if(argc>=ARG_OUTDIR)
            {
                
                clock_t start = clock();
                if(argc>=ARG_TEMPLDIR)
                    serializer.SetTemplateDir(argv[ARG_TEMPLDIR]);
                STLString outPath(argv[ARG_OUTDIR]);
                serializer.WriteToCPPFile(sMngr.GetRootModel(), argv[ARG_OUTFILE], outPath,sMngr.GetSpatialDataProvider());
                clock_t stop = clock();
                //exit
                
                cout << "Total time (s): " << (stop - start) / CLOCKS_PER_SEC << endl;
                
            }
            if (fileExt == STLString(OME_EXT_CTL) && argc>ARG_GENMAIN && STLString(argv[ARG_GENMAIN])=="true")
            {
                //serializer.WriteMainFile(argv[ARG_OUTFILE], argv[ARG_OUTDIR], &sMngr, dynamic_cast<SimpleCSVSpatialDataProvider*>(sMngr.GetSpatialDataProvider()));
            }
            //optionally run build script
            //if (AfxMessageBox("Would you like to run the build script?", MB_YESNO) == IDYES)
            //{
            //	//...
            //}
            //}
            //catch (std::exception & e)
            //{
            //cout << "Exception! " << e.what() << endl;
            //}
            
        }
        else
        {
            //CString msg= doc.GetErrorStr1();
            //Report::ErrorMsg( msg );
            doc.PrintError();
            //					return false;
        }
    }
    else
        cout << "No Path provided" << endl;
    
    
    return nRetCode;
}
