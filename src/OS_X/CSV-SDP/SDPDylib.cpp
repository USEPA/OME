// SDPDLL.cpp : Defines the exported functions for the DLL application.
//

#include "SDPDylib.h"
#include "CSVSpatialDataProvider.h"
#include "LuaOMEParser.h"
#include <set>

std::set<CSVSpatialDataProvider*> s_catalog;

extern "C" ISpatialDataProvider* GenSDP(SimManager* pSim,Model* pRoot, TI2Element* pSDPElem, const char* modelPath)
{
    //return a new instance of the target model
    //pick and choose arguments for specified
    STLString spPath = pSDPElem->Attribute("filepath");
    STLString xLbl = pSDPElem->Attribute("x_label");
    STLString yLbl = pSDPElem->Attribute("y_label");
    char delim = ',';
    
    STLString covStr = pSDPElem->Attribute("coverage_format");
    int covType = 0;
    if (covStr.find("grid") != STLString::npos)
        covType |= GRID_SUPPORT;
    
    if (covStr.find("hex") != STLString::npos)
        covType |= GRID_SUPPORT | HEX_SUPPORT;
    
    if (covStr.find("poly") != STLString::npos)
        covType |= POLY_SUPPORT;
    
    
    const char* delimCheck = pSDPElem->Attribute("delim");
    
    if (delimCheck)
        delim = delimCheck[0];
    
    CSVSpatialDataProvider* pProvider = NULL;
    try
    {
        pProvider = new CSVSpatialDataProvider(PrepPath(spPath, modelPath).c_str(),delim, xLbl, yLbl);
    }
    catch (exception* pE)
    {
        pSim->ReportError(pE->what());
        return NULL;
    }
    
    pProvider->SetCoverageType(covType);
    
    LuaOMEParser csvParser(pRoot);
    csvParser.LoadSDPFuncs(pProvider);
    
    
    //set expression after sdp is registered with manager, so as to
    TI2Element *pXmlExpr = pSDPElem->FirstChildElement("neighbor_expr");
    if (pXmlExpr)
    {
        CSVSpatialDataProvider::NEIGHMODE nMode = CSVSpatialDataProvider::NM_NONE;
        const char* scriptPath = pXmlExpr->Attribute("script");
        STLString ne;
        
        if (scriptPath)
        {
            std::ifstream inFile(PrepPath(scriptPath, modelPath));
            if (inFile.good())
            {
                oSTLSStream strmTemp;
                strmTemp << inFile.rdbuf();
                ne = strmTemp.str();
            }
            else
            {
                pSim->ReportError(STLString("File ") + scriptPath + " failed to open.");
                delete pProvider;
                return NULL;
            }
        }
        else
            ne = STLString(pXmlExpr->GetText());
        
        const char* mode = pXmlExpr->Attribute("mode");
        const char* modeArg = pXmlExpr->Attribute("extra");
        const char* lang = pXmlExpr->Attribute("language");
        if (mode)
        {
            if (!strcmp(mode, "compare_all"))
                nMode = CSVSpatialDataProvider::NM_COMPARE_ALL;
            else if (!strcmp(mode, "compare_xy"))
                nMode = CSVSpatialDataProvider::NM_COMPARE_XY;
            else if (!strcmp(mode, "use_column"))
                nMode = CSVSpatialDataProvider::NM_USE_COLUMN;
        }
        
        
        if (!ne.empty())
        {
            DBG_PRINT(STLString("Determining Neighbor relationships for '") + mode + '\'');
            pProvider->SetNeighborExpr(ne, lang, &csvParser, nMode, (void*)modeArg);
        }
    }
    
    pXmlExpr = pSDPElem->FirstChildElement("poly_expr");
    if (pXmlExpr)
    {
        STLString polyCol = pXmlExpr->Attribute("poly_label");
        STLString partCol = pXmlExpr->Attribute("part_label");
        pProvider->SetPolyCols(polyCol, partCol);
        
        /// \todo add support for script override
        
        pProvider->RefreshPolys();
        
    }
    
    s_catalog.insert(pProvider);
    return pProvider;
}

extern "C" void DisposeSDP(ISpatialDataProvider* pSDP)
{
    //cleanup sdp passed out from the dll.
    //default handler
    
    //ensure that we are deallocating a pointer that originated from this lib.
    auto fItr=s_catalog.find((CSVSpatialDataProvider*)pSDP);
    if(fItr!=s_catalog.end())
    {
        s_catalog.erase(fItr);
        delete pSDP;
    }
}

extern "C" const char* GetSDPLabel()
{
    //label identifier for SDP module
    return "CSV";
}