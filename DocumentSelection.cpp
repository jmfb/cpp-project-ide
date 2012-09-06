////////////////////////////////////////////////////////////////////////////////
// Filename:    DocumentSelection.cpp
// Description: This file implements all DocumentSelection member functions.
//
// Created:     2012-08-12 10:37:06
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "DocumentSelection.h"

void DocumentSelection::Clear()
{
	start.Set(0, 0);
	end.Set(0, 0);
	isVertical = false;
}

bool DocumentSelection::IsEmpty() const
{
	return (start.GetColumn() == end.GetColumn()) &&
		((start.GetLine() == end.GetLine()) || isVertical);
}

unsigned long DocumentSelection::GetStartLine() const
{
	return start.GetLine();
}

unsigned long DocumentSelection::GetStartColumn() const
{
	return start.GetColumn();
}

const DocumentPosition& DocumentSelection::GetStart() const
{
	return start;
}

unsigned long DocumentSelection::GetEndLine() const
{
	return end.GetLine();
}

unsigned long DocumentSelection::GetEndColumn() const
{
	return end.GetColumn();
}

const DocumentPosition& DocumentSelection::GetEnd() const
{
	return end;
}

bool DocumentSelection::IsVertical() const
{
	return isVertical;
}

void DocumentSelection::SetStart(const DocumentPosition& value)
{
	start = value;
}

void DocumentSelection::SetStartLine(unsigned long value)
{
	start.SetLine(value);
}

void DocumentSelection::SetStartColumn(unsigned long value)
{
	start.SetColumn(value);
}

void DocumentSelection::SetEnd(const DocumentPosition& value)
{
	end = value;
}

void DocumentSelection::SetEndLine(unsigned long value)
{
	end.SetLine(value);
}

void DocumentSelection::SetEndColumn(unsigned long value)
{
	end.SetColumn(value);
}

void DocumentSelection::SetVertical(bool value)
{
	isVertical = value;
}

