#pragma once
#include "Resource.h"
#include "SimManager.h"

// OMEPrefDLog dialog

class OMEProcess;

/** Dialog for preferences for OME in Envision.*/
class OMEPrefDLog : public CDialogEx
{
	DECLARE_DYNAMIC(OMEPrefDLog)

public:
   OMEPrefDLog(SimManager* pSim, OMEProcess* pAdapter,CWnd* pParent = NULL);   // standard constructor
   virtual BOOL OnInitDialog();
	virtual ~OMEPrefDLog();

// Dialog Data
	enum { IDD = ID_OMEA_PREFS };

   virtual void OnOK();

protected:

   SimManager* m_pSim;		///< Pointer to active SimManager.
   OMEProcess* m_pAdapter;	///< Pointer to active OMEAdapter instance.
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
