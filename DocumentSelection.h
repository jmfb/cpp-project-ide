////////////////////////////////////////////////////////////////////////////////
// Filename:    DocumentSelection.h
// Description: This file declares the DocumentSelection class.  This class
//              contains all of the information about the selection of a document.
//
// Created:     2012-08-12 10:34:05
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "DocumentPosition.h"

class DocumentSelection
{
public:
	DocumentSelection() = default;
	DocumentSelection(const DocumentSelection& rhs) = default;
	~DocumentSelection() = default;

	DocumentSelection& operator=(const DocumentSelection& rhs) = default;

	void Clear();
	bool IsEmpty() const;

	unsigned long GetStartLine() const;
	unsigned long GetStartColumn() const;
	const DocumentPosition& GetStart() const;
	unsigned long GetEndLine() const;
	unsigned long GetEndColumn() const;
	const DocumentPosition& GetEnd() const;
	bool IsVertical() const;
	void SetStart(const DocumentPosition& value);
	void SetStartLine(unsigned long value);
	void SetStartColumn(unsigned long value);
	void SetEnd(const DocumentPosition& value);
	void SetEndLine(unsigned long value);
	void SetEndColumn(unsigned long value);
	void SetVertical(bool value);

private:
	DocumentPosition start;
	DocumentPosition end;
	bool isVertical = false;
};

