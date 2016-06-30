#include "SimWrapper.h"
#include "stdafx.h"
#include "FieldSelectRep.h"
#include "SimWindow.h"
#include "SimManager.h"
#include "tinyxml2.h"
#include <msclr\marshal_cppstd.h>

using namespace System;
using namespace System::IO;
using namespace System::Text;
using namespace System::Runtime::InteropServices;
using namespace System::Windows::Forms;
using namespace OMESimRunner;


/// <summary>Update Widgets that display progress.</summary>
/// <param name="time">The present simulation time.</param>
/// <param name="pMgr">Pointer to the active Simulation Manager.</param>
/// <param name="extra">Pointer to additional information (in this case, a container of update controls).</param>
void SimWrapper::ProgressUpdate(const OME_SCALAR & time, BaseManager* pMgr,void* extra)
{
	GCHandle h=GCHandle::FromIntPtr(IntPtr(extra));
	UpdateCtrls^ ctrls = (UpdateCtrls^)h.Target;
	if (ctrls != nullptr)
	{
		UpdateProgBar^ d = gcnew UpdateProgBar(SetProgBarValue);
		ctrls->ModelBar->Invoke(d, gcnew array < System::Object^ > {ctrls, safe_cast<System::Object^>(time), safe_cast<System::Object^>(pMgr->GetInterval())});
	}
}

/// <summary>Update the state of the progress bar.</summary>
/// <param name="pObj">Handle to an UpdateCtrls object.</param>
/// <param name="val">Handle to the value to apply.</param>
/// <param name="interval">Handle to the interval to apply to the progress bar.</param>
System::Void SimWrapper::SetProgBarValue(System::Object^ pObj, System::Object^ val, System::Object^ interval)
{ 
	UpdateCtrls^ pCtrls = (UpdateCtrls^)pObj;
	if (pCtrls->ModelBar != nullptr)
		pCtrls->ModelBar->Value = safe_cast<OME_SCALAR>(val)/pCtrls->Interval;
	if (pCtrls->UpdateBox != nullptr)
		pCtrls->UpdateBox->Text = safe_cast<OME_SCALAR>(val).ToString();
}

SimWrapper::SimWrapper()
	:m_pSimManager(NULL), m_startTime(0.0), m_stopTime(1.0), m_hasRun(false), m_pUpCtrls(gcnew UpdateCtrls())
{
	m_pXmlCtrlDoc=new TI2Document();
	m_pResManager=new ResultsDetailsMgr();
	m_uD=gcnew UpdateDelegate(ProgressUpdate);
	delegateGCH=GCHandle::Alloc(m_uD);
}

/// <summary>Detailed controller.</summary>
/// <param name="controlPath">Path to .omec file to load.</param>
SimWrapper::SimWrapper(System::String ^controlPath)
	:m_pSimManager(NULL), m_hasRun(false), m_pUpCtrls(gcnew UpdateCtrls())
{
	m_pXmlCtrlDoc=new TI2Document();
	m_pResManager=new ResultsDetailsMgr();
	m_uD=gcnew UpdateDelegate(ProgressUpdate);
	GCHandle delegateGCH=GCHandle::Alloc(m_uD);

	LoadControlFile(controlPath);
}

SimWrapper::~SimWrapper()
{
	delete m_pSimManager;
	delete m_pResManager;
	delete m_pXmlCtrlDoc;
	delete m_pUpCtrls;

	if(pCtrlsGCH.IsAllocated)
		pCtrlsGCH.Free();
	if(delegateGCH.IsAllocated)
		delegateGCH.Free();
}

/// <summary>Load a .omec file from disk.</summary>
/// <param name="controlPath">Path to control file.</param>
System::Void SimWrapper::LoadControlFile(System::String ^controlPath)
{
	delete m_pSimManager;
	msclr::interop::marshal_context context;
	m_path = controlPath;
	STLString path=context.marshal_as<STLString>(controlPath);

	bool ok = m_pXmlCtrlDoc->LoadFile( path.c_str() )==tinyxml2::XML_SUCCESS;
   
	if ( ok )     
	{
		TI2Element *pXmlRoot = m_pXmlCtrlDoc->RootElement();
		m_pSimManager=new SimManager(true);
		oSTLSStream errStrm;
		unsigned int flags = SimManager::IS_SINGLETON | SimManager::NO_DEBUG;
		m_pSimManager->PopulateFromXMLTree(pXmlRoot,path,flags,errStrm);
		if(!errStrm.str().empty())
		{
			// report loading errors
		}
		m_pSimManager->SetKeepRecords(true);
		m_startTime=m_pSimManager->GetStartTime();
		m_stopTime=m_pSimManager->GetStopTime();
	
		m_currentTime=0.0;

		//prepare views
		m_pResManager->ClearViews();
		m_pResManager->LoadFromXMLTree(pXmlRoot,path);
		m_pResManager->ClearSaveFlags();
	}
	else
	{

		oSTLSStream outStrm;
		outStrm << "Err Loading xml model: " << XMLLoadable::ErrDescription(m_pXmlCtrlDoc->ErrorID()) << ": " << m_pXmlCtrlDoc->GetErrorStr1();
		if (m_pXmlCtrlDoc->GetErrorStr2())
			outStrm << ", " << m_pXmlCtrlDoc->GetErrorStr2();
		msclr::interop::marshal_context context;

		MessageBox::Show(context.marshal_as<String^>(outStrm.str()));
	}
	//set up sim manager
	
}

/// <summary>Save an .omec file from disk.</summary>
/// <param name="controlPath">Path to save location of control file.</param>
/// <returns>True if save was successful; false otherwise.</returns>
bool SimWrapper::SaveControlFile(System::String ^controlPath)
{
	static const STLString xmlheader="<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	bool ret=false;

	TI2Element* pXmlRoot=m_pXmlCtrlDoc->RootElement();
	if(pXmlRoot)
	{
		TI2Element* pXmlViews=pXmlRoot->FirstChildElement(m_pResManager->GetXMLTag());
		if(pXmlViews)
			pXmlRoot->DeleteChild(pXmlViews);

		TI2Document tempDoc;
		if(tempDoc.Parse((xmlheader+m_pResManager->GetXMLRepresentation(0,4)).c_str())==tinyxml2::XML_SUCCESS)
		{
			DeepXMLCopy((pXmlRoot->InsertEndChild(tempDoc.RootElement()->ShallowClone(m_pXmlCtrlDoc)))->ToElement(),m_pXmlCtrlDoc,tempDoc.RootElement());
			
			//should check for error.
			const char* cPath=(const char*)(Marshal::StringToHGlobalAnsi(controlPath)).ToPointer();
			if(m_pXmlCtrlDoc->SaveFile(cPath)==tinyxml2::XML_SUCCESS)
				ret=true;
			Marshal::FreeHGlobal(IntPtr((void*)cPath));

			m_pResManager->ClearSaveFlags();
			m_path = controlPath;
		}
		//else err;
		
	}
	return ret;
}

/// <summary>Perform a recursive deep copy of all xml objects.</summary>
/// <param name="pCopyTo">Pointer to target element of copy operation.</param>
/// <param name="pToDoc">Pointer to target document of copy operation.</param>
/// <param name="pCopyFrom">Pointer to source element of copy operation.</param>
System::Void SimWrapper::DeepXMLCopy(TI2Element* pCopyTo, TI2Document* pToDoc, TI2Element* pCopyFrom)
{
	TI2Element* pCurrElem=pCopyFrom->FirstChildElement();
	while(pCurrElem)
	{
		DeepXMLCopy((pCopyTo->InsertEndChild(pCurrElem->ShallowClone(pToDoc)))->ToElement(),pToDoc,pCurrElem);

		pCurrElem=pCurrElem->NextSiblingElement();
	}
}

/// <summary>Carry out a simulation run in a seperate process/thread.</summary>
/// <param name="frm">A copy of the SimWindow.</param>
System::Void SimWrapper::SpawnSim(Object^ frm)
{
	msclr::interop::marshal_context context;
	m_pSimManager->EnableFileCache(context.marshal_as<STLString>(m_path));
	
	Int32 time = Environment::TickCount;

	m_pSimManager->RunSimulation();
	m_lastRunTime = (Environment::TickCount - time)/1000;

	//alternate run method (spawns instance of OMEEngine)
	//system((STLString("C:\\ome\\src\\x64\\Release\\OMEEngine.exe ") + "\""+context.marshal_as<STLString>(m_path)+"\""+STLString(" quiet")).c_str());
	
	m_hasRun=true;
	m_currentTime=m_stopTime;
	EndRun^ d=gcnew EndRun(UpdateForm);
	((SimWindow^)frm)->Invoke(d,gcnew array<System::Object^>{frm});

}

/// <summary>Update widgets for the conclusion of a run.</summary>
/// <param name="frm">A copy of the SimWindow.</param>
System::Void SimWrapper::UpdateForm(SimWindow^ frm)
{
	((SimWindow^)frm)->FinishRun();
}

/// <summary>Start a new simulation run in a seperate process/thread.</summary>
/// <param name="frm">A copy of the SimWindow.</param>
System::Void SimWrapper::RunSimulation(SimWindow^ frm)
{
	if(m_pSimManager)
	{
		if (pCtrlsGCH.IsAllocated)
			pCtrlsGCH.Free();

		m_pUpCtrls->ModelBar=frm->RunProgressBar;
		m_pUpCtrls->UpdateBox = frm->RunTextBox;

		pCtrlsGCH = GCHandle::Alloc(m_pUpCtrls);
		m_pUpCtrls->Interval = m_pSimManager->GetInterval();

		if (m_pUpCtrls->ModelBar)
		{
			m_pUpCtrls->ModelBar->Minimum = m_startTime;
			m_pUpCtrls->ModelBar->Maximum = m_stopTime/m_pUpCtrls->Interval;
			m_pUpCtrls->ModelBar->Value = m_startTime;
		}
		frm->PrepRun();
		m_pSimManager->SetManagerReportFunction(static_cast<ManagerReportFunction>(Marshal::GetFunctionPointerForDelegate(m_uD).ToPointer()),
			GCHandle::ToIntPtr(pCtrlsGCH).ToPointer());

		m_pSimManager->SetStartTime(m_startTime);
		m_pSimManager->SetStopTime(m_stopTime);
		m_pSimManager->PreloadSubmodels();
		//run model in thread seperate from the UI thread
		System::Threading::ParameterizedThreadStart^ threadDelegate=gcnew System::Threading::ParameterizedThreadStart(this,&SimWrapper::SpawnSim);
		System::Threading::Thread^ simThread=gcnew System::Threading::Thread(threadDelegate);
		simThread->Start(frm);

		
	}
}

/// <summary>Generate a text-based description of the loaded model.</summary>
/// <returns>A text summary of the loaded model.</returns>
String^ SimWrapper::GetModelDescription()
{
	String^ out=gcnew String("No Model Loaded");
		
	if(m_pSimManager)
	{
		StringWriter^ sw=gcnew StringWriter;
		sw->WriteLine(String::Concat("Start Time: ",m_startTime));
		sw->WriteLine(String::Concat("Stop Time: ",m_stopTime));
		sw->WriteLine(String::Concat("Report Interval: ",m_pSimManager->GetInterval()));
		sw->WriteLine(String::Concat("Integration Step: ",m_pSimManager->GetStepSize()));
		sw->WriteLine(String::Concat("Integration Method: ",m_pSimManager->GetIntMethod()==1 ? "Euler" : "RK4"));
		/*if(m_hasRun)
			sw->WriteLine(String::Concat("Current Step: ",m_currentTime));
		else
			sw->WriteLine(String::Concat("Current Step: ","N/A"));*/
		if (m_hasRun)
			sw->WriteLine(String::Concat("Last Run Duration (s): ", m_lastRunTime));

		out=sw->ToString();
	}

	return out;
}

SimWrapper::!SimWrapper()
{
	delete m_pSimManager;
	delete m_pResManager;
};

/// <summary> Load requested field selection sources.</summary>
/// <param name="available">Handle to a BindingSource containing all available fields.</param>
/// <param name="selected">Handle to BindingSource that holds just the selected fields.</param>
/// <param name="removeSelected">If true any field values in selected are removed from available.</param>
System::Void SimWrapper::LoadFieldData(BindingSource^ available, BindingSource^ selected, const bool & removeSelected)
{
	selected->Clear();
	available->Clear();
	EvalRepArray reps=m_pSimManager->GetReadableObjectList(DT_FULL | DT_ALIAS | DT_NO_AUTOGEN);
	EvalRepArray sels=m_pResManager->GetActiveDetails()->GetPartialReps();

	EvalRepArray toRemove;

	for (auto rItr = reps.begin(); rItr != reps.end(); ++rItr)
	{
		for (auto sItr = sels.begin(); sItr != sels.end(); ++sItr)
		{
			if((*rItr).id==(*sItr).id)
			{
				toRemove.push_back(*rItr);

				(*sItr).name=(*rItr).name;
				(*sItr).type=(*rItr).type;
				(*sItr).isConstant=(*rItr).isConstant;
				(*sItr).isArray=(*rItr).isArray;
				(*sItr).count=(*rItr).count;
				(*sItr).parentModel=(*rItr).parentModel;
				(*sItr).modelPath=(*rItr).modelPath;

				break;
			}
		}
	}

	if (removeSelected)
		reps.erase(toRemove.begin(),toRemove.end());

	for(auto itr=reps.begin(); itr!=reps.end(); ++itr)
		available->Add(gcnew FieldSelectRep(&(*itr),false));

	//populate selected.
	for(auto itr=sels.begin(); itr!=sels.end(); ++itr)
		selected->Add(gcnew FieldSelectRep(&(*itr),true));
}

/// <summary>Apply selected field source to ResultDetails settings.</summary>
/// <param name="selected">Handle to BindingSource that contains the selected fields.</param>
System::Void SimWrapper::ApplyFieldData(BindingSource^ selected)
{
	//save selected to m_pResManager
	EvalRepArray toSave;
	for(int i=0; i<selected->Count; i++)
	{
		EvalRep rep;
		((FieldSelectRep^)selected[i])->PopulatePartialRep(&rep);
		toSave.push_back(rep);
	}

	m_pResManager->GetActiveDetails()->SetPartialReps(toSave);
}

/// <summary>Export currently loaded timestep values to a csv.</summary>
/// <param name="path">Where to save exported values.</param>
/// <returns>True if save was successful; false otherwise.</returns>
System::Boolean SimWrapper::ExportValues(String^ path)
{
	bool ret = false;
	msclr::interop::marshal_context context;

	if (m_pSimManager && this->HasRun)
		ret=m_pSimManager->ExportLoadedValuesToCSV(context.marshal_as<STLString>(path));

	return ret;
}

/// <summary>Export all timestep values to a csv.</summary>
/// <param name="path">Where to save exported values.</param>
/// <returns>True if save was successful; false otherwise.</returns>
System::Boolean SimWrapper::ExportAllValues(String^ path)
{
	bool ret = false;
	msclr::interop::marshal_context context;

	if (m_pSimManager && this->HasRun)
		ret = m_pSimManager->ExportAllValuesToCSV(context.marshal_as<STLString>(path));

	return ret;
}