#pragma once
#include "SimWrapper.h"
#include <msclr\marshal_cppstd.h>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace OMESimRunner
{
	
	ref class RNControl;

	/// <summary> Node used to represent OME model components.</summary>
	public ref class RepNode : IComparable
	{
	public:
		
		RepNode(OMEObject* pObj);

		/// <summary> The OMEType identifier for the represented object.</summary>
		property OMETYPE ObjectType
		{
			OMETYPE get()
			{
				if(m_pObj)
					return m_pObj->GetOMEType();
				else
					return OME_NULL;
			}
		}

		/// <summary> The value of the represented object.</summary>
		property OME_SCALAR Value
		{
			OME_SCALAR get()
			{
				if (m_pObj->IsOMEType(OME_EVAL) && ((Evaluable*)m_pObj)->IsInitialized())
				{
					
					if (m_pObj->GetOMEType() != OME_VARARRAY)
					{
						OME_SCALAR temp = ((Evaluable*)m_pObj)->GetValue();
						return temp;
					}
					else
						return (*(VarArray*)m_pObj)[m_index];
				}
				else
					return 0.0;
			}
		}

		/// <summary> The name of the represented object.</summary>
		property String^ Name
		{
			String^ get()
			{
				return m_name;
			}
		}
		
		/// <summary> The name of the parent model of the represented object.</summary>
		property String^ ParentModelName
		{
			String^ get()
			{
				msclr::interop::marshal_context context;
				return context.marshal_as<String^>(m_pObj->GetParentModel() ? m_pObj->GetParentModel()->GetName() : STLString("None"));
			}
        }

		/// <summary> The represented instance.</summary>
		property size_t Instance
		{
			size_t get()
			{
				if(m_pObj->GetOMEType()==OME_MODEL)
					return ((Model*)m_pObj)->GetCurrentInstance()+1;
				else if(m_pObj->GetOMEType()==OME_VARARRAY)
					return m_index+1;
				else return 0;
			}

			System::Void set(size_t i)
			{
				i--;
				if(m_pObj->GetOMEType()==OME_MODEL)
				{
					Model* pMdl=(Model*)m_pObj;
					if(i>=pMdl->GetNumInstances())
						i=pMdl->GetNumInstances()-1;
					pMdl->SetCurrentInstance(i);
				}
				else if(m_pObj->GetOMEType()==OME_VARARRAY)
				{
					VarArray* pVa=(VarArray*)m_pObj;
					if(i>=pVa->GetSize())
						i=pVa->GetSize()-1;
					m_index=i;
				}
			}
		}

		/// <summary> The number of values contained within the represented object.</summary>
		property size_t Count
		{
			size_t get()
			{
				size_t ret=0;
				if(m_pObj->GetOMEType()==OME_MODEL)
					ret=((Model*)m_pObj)->GetNumInstances();
				else if(m_pObj->GetOMEType()==OME_VARARRAY && ((VarArray*)m_pObj)->IsInitialized())
					ret=((VarArray*)m_pObj)->GetSize();

				return ret;
			}
		}

		/// <summary> Pointer to the represented object.</summary>
		property OMEObject* Object
		{
			OMEObject* get() {return m_pObj; }
		}

		/// <summary> Color to apply to represented object.</summary>
		property System::Drawing::Color Color
		{
			System::Drawing::Color get(){ return m_txtColor; }
			void set(System::Drawing::Color inColor) { m_txtColor = inColor; }
		}

		virtual Int32 CompareTo(System::Object^ obj);

	protected:
		
		/// <summary> Pointer to the represented object.</summary>
		OMEObject* m_pObj;
		/// <summary> Name of the represented object.</summary>
		String^ m_name;
		/// <summary> The index of the represented instance.</summary>
		size_t m_index;

		/// <summary> Color to apply to text.</summary>
		System::Drawing::Color m_txtColor;
	};

};