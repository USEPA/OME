#pragma once

#include "SimManager.h"
#include "ResultsDetailsMgr.h"
//#include "OMEDefines.h"
#include <msclr\marshal_cppstd.h>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::IO;
using namespace System::Text;
using namespace System::Runtime::InteropServices;

namespace OMESimRunner {

	ref class SimWindow;

	public delegate void UpdateDelegate(const OME_SCALAR &,BaseManager*,void*);
	public delegate void UpdateProgBar(System::Object^, System::Object^, System::Object^);
	ref class SimWrapper;
	public delegate void EndRun(SimWindow^);

	/// <summary>Wrapper for ResultsDetails objects.</summary>
	public ref class ViewWrapper
	{
	public:
		/// <summary>Detailed Contructor.</summary>
		/// <param name="pRD">Pointer to the ResultsDetails object.</param>
		ViewWrapper(BaseResultsDetails* pRD)
			:m_pRD(pRD)
		{

		}

		~ViewWrapper()
		{}

		/// <summary>Name of represented view object.</summary>
		property String^ ViewName
		{
			String^ get()
			{
				msclr::interop::marshal_context context;

				return context.marshal_as<String^>(m_pRD->GetViewName());
			}
		}

		/// <summary>String representation of view type.</summary>
		property String^ ViewTypeStr
		{
			String^ get()
			{
				String^ ret;
				switch(m_pRD->GetRVType())
				{
				case RVT_TABLE:
					ret="Table";
					break;
				case RVT_TREE:
					ret="Tree";
					break;
				case RVT_MULTVAL:
					ret = "Multi Value";
					break;
				default:
					ret="None";
				}
				return ret;
			}
		}

		/// <summary>Pointer to the underlying ResultsDetails object.</summary>
		property BaseResultsDetails* RawPtr
		{
			BaseResultsDetails* get()
			{
				return m_pRD;
			}
		}

		
	protected:
		!ViewWrapper()
		{}
	private:
		/// <summary>Pointer to represented ResultsDetails object.</summary>
		BaseResultsDetails* m_pRD;
	};

	/// <summary>Wrapper for OME's SimManager object.</summary>
	public ref class SimWrapper
	{
	public:
		SimWrapper();
		SimWrapper(System::String ^controlPath);
		~SimWrapper();

		/// <summary>Start time of simulation run.</summary>
		property OME_SCALAR StartTime
		{
			OME_SCALAR get()
			{
				return m_startTime;
			}
			void set(OME_SCALAR start)
			{
				m_startTime=start;
			}
		}

		/// <summary>Stop time of simulation run.</summary>
		property OME_SCALAR StopTime
		{
			OME_SCALAR get()
			{
				return m_stopTime;
			}
			void set(OME_SCALAR stop)
			{
				m_stopTime=stop;
			}
		}

		/// <summary>Time step report interval of simulation run.</summary>
		property OME_SCALAR Interval
		{
			OME_SCALAR get()
			{
				return m_pSimManager ? m_pSimManager->GetInterval() : 1.0;
			}
			System::Void set(OME_SCALAR val)
			{
				if (m_pSimManager)
					m_pSimManager->SetInterval(val);
			}
		}

		/// <summary>Integration step size of simulation run.</summary>
		property OME_SCALAR StepSize
		{
			OME_SCALAR get()
			{
				return m_pSimManager ? m_pSimManager->GetStepSize() : 1.0;
			}
			System::Void set(OME_SCALAR val)
			{
				if (m_pSimManager)
					m_pSimManager->SetStepSize(val);
			}
		}

		/// <summary>Pointer to ResultsDetailsMgr used to manage results views.</summary>
		property ResultsDetailsMgr* ViewMgr
		{
			ResultsDetailsMgr* get()
			{
				return m_pResManager;
			}
		}

		/// <summary>Pointer to root Model.</summary>
		property Model* RootModel
		{
			Model* get()
			{
				return m_pSimManager ? m_pSimManager->GetRootModel() : NULL;
			}
		}

		/// <summary>Flag indicating whether or not the model has already run.</summary>
		property bool HasRun
		{
			bool get()
			{
				return m_hasRun;
			}
		}

		/// <summary>The currently selected time for reviewing results.</summary>
		property OME_SCALAR SelectedTime
		{
			OME_SCALAR get()
			{
				return m_currentTime;
			}
			System::Void set(OME_SCALAR time)
			{
				m_currentTime=time;
				if (m_pSimManager->GetRootModel())
				{
					//AfxMessageBox(m_pSimManager->GetRootModel()->GetName().c_str(), MB_OK);
					m_pSimManager->LoadValuesForTime(time);
				}
			}
		}

		/// <summary>The integration method presently in use.</summary>
		property OMEIntegrationBlock::INT_METHOD IntMethod
		{
			OMEIntegrationBlock::INT_METHOD get()
			{
				return m_pSimManager->GetIntMethod();
			}

			System::Void set(OMEIntegrationBlock::INT_METHOD im)
			{
				m_pSimManager->SetIntMethod(im);
			}
		}

		/// <summary>The integration solver presently in use.</summary>
		property SimManager::SOLVER_TYPE SolverType
		{
			SimManager::SOLVER_TYPE get()
			{
				return m_pSimManager->GetSolverType();
			}

			System::Void set(SimManager::SOLVER_TYPE t)
			{
				m_pSimManager->SetSolverType(t);
			}
		}

		/// <summary>Flag indicating whether or not minimal record keeping is active.</summary>
		property bool UseMinRecord
		{
			bool get()
			{
				return m_pSimManager->GetMinimalRecords();
			}

			void set(bool ufr)
			{
				m_pSimManager->SetMinimalRecords(ufr);
			}
		}

		System::Void LoadControlFile(System::String ^controlPath);
		bool SaveControlFile(System::String ^controlPath);
		System::Void RunSimulation(SimWindow^ frm);
		String^ GetModelDescription();

		System::Void LoadFieldData(BindingSource^ available,BindingSource^ selected,const bool & removeSelected);
		System::Void ApplyFieldData(BindingSource^ selected);
		
		System::Boolean ExportValues(String^ path);
		System::Boolean ExportAllValues(String^ path);
	protected:
		!SimWrapper();


	private:

		/// <summary>Collection of controls that are updated during a simulation run.</summary>
		ref class UpdateCtrls
		{
		public:
			UpdateCtrls()
				:m_modelBar(nullptr), m_currBox(nullptr) {}

			/// <summary>Handle to progress bar to be updated.</summary>
			property ProgressBar^ ModelBar
			{
				ProgressBar^ get() { return m_modelBar; }
				System::Void set(ProgressBar^ pb) { m_modelBar = pb; }
			}
			/// <summary>Handle to TextBox to be updated.</summary>
			property TextBox^ UpdateBox
			{
				TextBox^ get() { return m_currBox; }
				System::Void set(TextBox^ tb) { m_currBox = tb; }
			}
			/// <summary>Interval used in updates.</summary>
			property OME_SCALAR Interval
			{
				OME_SCALAR get() { return m_interval; }
				void set(OME_SCALAR i){ m_interval = i; }
			}
		private:
			/// <summary>Handle to progress bar to be updated.</summary>
			ProgressBar^ m_modelBar;
			/// <summary>Handle to TextBox to be updated.</summary>
			TextBox^ m_currBox;
			/// <summary>Interval used in updates.</summary>
			OME_SCALAR m_interval;

		};

		System::Void SpawnSim(Object^ frm);
		
		/// <summary> Pointer to the wrapped SimManager. </summary>
		SimManager* m_pSimManager;
		/// <summary>Pointer to the ResultsDetailsMgr used for loading views.</summary>
		ResultsDetailsMgr* m_pResManager;
		/// <summary>The start time of the simulation.</summary>
		OME_SCALAR m_startTime;
		/// <summary>The presently loaded time of the simulation.</summary>
		OME_SCALAR m_currentTime;
		/// <summary>The stop time of the simulation.</summary>
		OME_SCALAR m_stopTime;
		
		/// <summary>Flag indicating whether or not the model has run.</summary>
		bool m_hasRun;
		/// <summary>The duration of the last simulation run.</summary>
		unsigned int m_lastRunTime;

		/// <summary>Handle to controls that need to be updated at every time step.</summary>
		UpdateCtrls^ m_pUpCtrls;
		/// <summary>Pointer to XML document containing information loaded from .omec file.</summary>
		TI2Document* m_pXmlCtrlDoc;
		/// <summary>Path to loaded .omec file.</summary>
		String^ m_path;

		//for moving objects through unmanaged code
		/// <summary>Delegate for unmanaged code call.</summary>
		GCHandle delegateGCH;
		/// <summary>Pointer for UpdateCtrls in unmanaged code.</summary>
		GCHandle pCtrlsGCH;
		/// <summary>Delegate to call when widgets need updating during simulation run.</summary>
		UpdateDelegate^ m_uD;

		static void ProgressUpdate(const OME_SCALAR & time, BaseManager* pMgr,void* extra);
		static System::Void SetProgBarValue(System::Object^ pBar, System::Object^ val, System::Object^ interval);
		static System::Void UpdateForm(SimWindow^ frm);
		static System::Void DeepXMLCopy(TI2Element* pCopyTo, TI2Document* pToDoc, TI2Element* pCopyFrom);
	};

};