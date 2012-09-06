////////////////////////////////////////////////////////////////////////////////
// Filename:    DocumentPosition.h
// Description: This file declares the DocumentPosition class.  This class
//              represents a line and column position within a document.  Both
//              line and column are zero based.
//
// Created:     2012-08-14 00:04:57
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once

class DocumentPosition
{
public:
	DocumentPosition() = default;
	DocumentPosition(unsigned long line, unsigned long column);
	DocumentPosition(const DocumentPosition& rhs) = default;
	~DocumentPosition() = default;

	DocumentPosition& operator=(const DocumentPosition& rhs) = default;

	void Set(unsigned long line, unsigned long column);
	void SetLine(unsigned long value);
	void SetColumn(unsigned long value);
	unsigned long GetLine() const;
	unsigned long GetColumn() const;

	POINT ToPoint() const;

	bool operator<(const DocumentPosition& rhs) const;

private:
	unsigned long line = 0;
	unsigned long column = 0;
};

