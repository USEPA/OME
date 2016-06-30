#pragma once

#include "RepNode.h"
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace OMESimRunner
{

	ref class RNControl;

	/// <summary>Subclass of RepNode that represents values
	/// in a table view.</summary>
	public ref class TblRepNode : RepNode
	{
	public:

		/// <summary>The number of coordinates for a given entry.</summary>
		property size_t CoordCount
		{
			size_t get()
			{
				return m_coords->Count;
			}
		}

		/// <summary>Retrieve a specific coordinate.</summary>
		property size_t Coord[unsigned int]
		{
			size_t get(unsigned int ind)
			{
				return m_coords[ind];
			}

			System::Void set(unsigned int ind, size_t val)
			{
				m_coords[ind] = val;
			}
		}

		/// <summary>List of TblRepNode's coordinates.</summary>
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

		/// <summary>String representation of coordinates.</summary>
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

		/// <summary>Find value string for current coords..</summary>
		property String^ LookupValStr
		{
			String^ get()
			{
				if (m_coords)
					return FindValStr();
					
				return "";
			}
		};
		TblRepNode(OMEObject* pObj);
		virtual Int32 CompareTo(System::Object^ obj) override;

		void AppendCoordinate(const size_t & coord);
		void ClearCoordinates();
	protected:

		String^ FindValStr();
		/// <summary>List of TblRepNode's coordinates.</summary>
		Generic::List<size_t>^ m_coords;
	};
}
