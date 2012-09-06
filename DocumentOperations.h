////////////////////////////////////////////////////////////////////////////////
// Filename:    DocumentOperations.h
// Description: This file declares the DocumentOperation class.  This class defines
//              all interface functions required for document operations to execute.
//
// Created:     2012-08-17 23:53:09
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "DocumentSelection.h"
#include "DocumentPosition.h"
#include <string>

class DocumentOperations
{
public:
	virtual const DocumentSelection& GetSelection() const = 0;
	virtual std::string GetSelectedText() const = 0;
	virtual void SelectPosition(const DocumentPosition& position, bool extend, bool isVertical) = 0;
	virtual void AtomicDeleteSelection() = 0;
	virtual void AtomicInsertText(const std::string& value) = 0;
};

