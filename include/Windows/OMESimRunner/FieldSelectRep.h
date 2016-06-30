#pragma once
#include "SimWrapper.h"
#include "OMEDefines.h"
#include "omeobjects/Evaluable.h"

class Evaluable;

namespace OMESimRunner {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

		/// <summary>
	/// Representation of entry used in field selection.
	/// </summary>
	public ref class FieldSelectRep
	{
	public: 
		/// <summary>The name of the represented field.</summary>
		property String^ Name
		{
			String^ get()
			{
				return m_name;
			}
		};

		/// <summary>The ID of the represented field.</summary>
		property String^ ID
		{
			String^ get()
			{
				return m_id;
			}
		};

		/// <summary>The type of the represented field.</summary>
		property String^ Type
		{
			String^ get()
			{
				return m_type;
			}
		};

		/// <summary>The name of the parent model of the represented field.</summary>
		property String^ ParentModel
		{
			String^ get()
			{
				return m_parentModel;
			}
		};

		/// <summary>The model path to the represented field.</summary>
		property String^ Path
		{
			String^ get()
			{
				return m_modelPath;
			}
		};

		/// <summary>Pointer to the color struct of the represented field.</summary>
		property OMEColor* ColorRef
		{
			OMEColor* get()
			{
				return m_pColor;
			}

			System::Void set(OMEColor* c)
			{
				m_pColor=c;
			}
		};

		/// <summary>The Color object used when drawing the represented field.</summary>
		property System::Drawing::Color Color
		{
			System::Drawing::Color get()
			{
				return System::Drawing::Color::FromArgb(255, m_pColor->r * 255, m_pColor->g * 255, m_pColor->b * 255);
			}
		};

		/// <summary>The active/inactive state of the representation.</summary>
		property bool Enabled
		{
			bool get()
			{
				return m_enabled;
			}
		
			System::Void set(bool e)
			{
				m_enabled=e;
			}
		};

		/// <summary>String representation of internally stored coordinates.</summary>
		property String^ CoordStr
		{
			String^ get()
			{
				if (m_coords)
				{
					StringBuilder^ sb = gcnew StringBuilder("{");
					for (unsigned int i = 0; i < m_coords->Count; ++i)
					{
						sb->AppendFormat(" {0}", m_coords[i]);
					}
					sb->Append(" }");
					return sb->ToString();
				}
				
				return "";
			}
		};

		/// <summary>Access to a specific ordinate.</summary>
		property unsigned int Coord[unsigned int]
		{
			unsigned int get(unsigned int index)
			{
				if (!m_coords)
					throw gcnew IndexOutOfRangeException("No coords defined");
				return m_coords[index];
			}

			void set(unsigned int index, unsigned int coord)
			{
				if (!m_coords)
					throw gcnew IndexOutOfRangeException("No coords defined");
				m_coords[index] = coord;
			}
		}

		/// <summary>Handle to the list of coordinates.</summary>
		property Generic::List<size_t>^ CoordList
		{
			Generic::List<size_t>^ get()
			{
				return m_coords;
			}

			System::Void set(Generic::List<size_t>^ cl)
			{
				m_coords = cl;
			}
		}

		System::Void PopulatePartialRep(EvalRep* pRep);

		FieldSelectRep(Evaluable* pEval);
		FieldSelectRep(Evaluable* pEval,EvalRep* pRep);
		FieldSelectRep(EvalRep* pRep,bool isEnabled);

		//copy constructor
		FieldSelectRep(const FieldSelectRep^ toCopy);

		System::Void AddCoord(unsigned int coord);
		System::Void ClearCoords();
	protected:
		~FieldSelectRep();

		/// <summary>The name storage.</summary>
		String^ m_name;
		/// <summary>The ID storage.</summary>
		String^ m_id;
		/// <summary>The Type storage.</summary>
		String^ m_type;
		/// <summary>The parent model name storage.</summary>
		String^ m_parentModel;
		/// <summary>The model path of the represented object.</summary>
		String^ m_modelPath;
		/// <summary>Pointer to OMEColor struct.</summary>
		OMEColor* m_pColor;
		/// <summary>List of coordinates.</summary>
		Generic::List<size_t>^ m_coords;

		/// <summary>Enabled flag storage.</summary>
		bool m_enabled;

		void LoadFromEval(Evaluable* pEval);
		
	};
};