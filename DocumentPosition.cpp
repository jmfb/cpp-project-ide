////////////////////////////////////////////////////////////////////////////////
// Filename:    DocumentPosition.cpp
// Description: This file implements all DocumentPosition member functions.
//
// Created:     2012-08-14 00:07:54
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "DocumentPosition.h"

DocumentPosition::DocumentPosition(unsigned long line, unsigned long column)
	: line(line), column(column)
{
}

void DocumentPosition::Set(unsigned long line, unsigned long column)
{
	this->line = line;
	this->column = column;
}

void DocumentPosition::SetLine(unsigned long value)
{
	line = value;
}

void DocumentPosition::SetColumn(unsigned long value)
{
	column = value;
}

unsigned long DocumentPosition::GetLine() const
{
	return line;
}

unsigned long DocumentPosition::GetColumn() const
{
	return column;
}

POINT DocumentPosition::ToPoint() const
{
	return { static_cast<long>(column), static_cast<long>(line) };
}

bool DocumentPosition::operator<(const DocumentPosition& rhs) const
{
	return (line < rhs.line) || ((line == rhs.line) && (column < rhs.column));
}

