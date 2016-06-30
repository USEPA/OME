// OMEPrefDLog.cpp : implementation file
//

#include "stdafx.h"
#include "OMEAdapter.h"
#include "OMEPrefDLog.h"
#include "afxdialogex.h"

// OMEPrefDLog dialog

IMPLEMENT_DYNAMIC(OMEPrefDLog, CDialogEx)

/**  Detailed constructor.
	@param pSim Pointer to the active SimManager.
	@param pAdapter Pointer to the active OMEAdapter.
	@param pParent Not used.
*/
OMEPrefDLog::OMEPrefDLog(SimManager* pSim, OMEProcess* pAdapter, CWnd* pParent /*=NULL*/)
: CDialogEx(OMEPrefDLog::IDD, pParent)
   {
   m_pSim = pSim;
   m_pAdapter = pAdapter;
   }

/** Initialization event handler.
	@return True if successful; false otherwise.
*/
BOOL OMEPrefDLog::OnInitDialog()
   {
   CDialog::OnInitDialog();
   //set control values
   CComboBox* drop = (CComboBox*)GetDlgItem(IDC_SNYCCOMBO);
   drop->SetCurSel(m_pSim->GetIncToBounds() ? 1 : 0);

   drop = (CComboBox*)GetDlgItem(IDC_SOLVECOMBO);

   drop->SetCurSel(0);
   
   drop = (CComboBox*)GetDlgItem(IDC_REPORTCOMBO);
   drop->SetCurSel(m_pAdapter->GetReportLevel());
   return TRUE;
   }

OMEPrefDLog::~OMEPrefDLog()
   {
   }

/** DataExchange event handler.
@param pDX Pointer to the Data Exchange object.
*/
void OMEPrefDLog::DoDataExchange(CDataExchange* pDX)
   {
   CDialogEx::DoDataExchange(pDX);
   }

/** OK button event handler.
*/
void OMEPrefDLog::OnOK()
   {
   CComboBox* drop = (CComboBox*)GetDlgItem(IDC_SNYCCOMBO);
   m_pSim->SetIncToBounds(drop->GetCurSel()==0 ? false : true);
   /// @todo add set for Solver type on merge
   drop = (CComboBox*)GetDlgItem(IDC_REPORTCOMBO);
   m_pAdapter->SetReportLevel((OMEProcess::RPRT_LEVEL)drop->GetCurSel());
   CDialog::OnOK();
   }

BEGIN_MESSAGE_MAP(OMEPrefDLog, CDialogEx)
END_MESSAGE_MAP()


// OMEPrefDLog message handlers
