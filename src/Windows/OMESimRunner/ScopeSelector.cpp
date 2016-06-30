#include "stdafx.h"
#include "ScopeSelector.h"

using namespace OMESimRunner;

/// <summary>Add an index node to the list of indices. </summary>
/// <param name="level"> The string representing the depth level.</param>
/// <param name="index"> The active index for the entry.</param>
void ScopeSelector::AddEntry(String^ level, const size_t & index)
{
	m_entries->Add(gcnew SSNode(level, index));
}

/// <summary>Add an index node level to the list of indices. </summary>
/// <param name="level"> The string representing the depth level.</param>
void ScopeSelector::AddLevel(String^ level)
{
	m_entries->Add(gcnew SSNode(level, 1));
}



