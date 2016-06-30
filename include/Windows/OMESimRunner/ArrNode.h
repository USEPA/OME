#pragma once
#include "omeobjects/VarArray.h"

namespace OMESimRunner {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>Representative node used for Array collections.</summary>
	public ref class ArrNode
	{
	public:
		/// <summary>Value constructor.</summary>
		/// <param name="val">The value to assign to the ArrNode</param>
		ArrNode(OME_SCALAR val)
		{
			m_val = val;
			m_coordStr = nullptr;
		}

		/// <summary>Value & coordinate constructor.</summary>
		/// <param name="val">The value to assign to the ArrNode</param>
		/// <param name="coords">Handle to the coordinates to associate with val.</param>
		ArrNode(OME_SCALAR val,String^ coords)
		{
			m_val = val;
			m_coordStr = coords;
		}

		/// <summary>The value of the node.</summary>
		property OME_SCALAR Value
		{
			OME_SCALAR get()
			{
				return m_val;
			}
		}

		/// <summary>The coordinates of the node.</summary>
		property String^ Coords
		{
			String^ get()
			{
				return m_coordStr;
			}

			System::Void set(String^ str)
			{
				m_coordStr = str;
			}
		}

	protected:
		~ArrNode() {}

	private:
		/// <summary>The value of the node.</summary>
		OME_SCALAR m_val;
		/// <summary>The coordinates of the node.</summary>
		String^ m_coordStr;
	};
};