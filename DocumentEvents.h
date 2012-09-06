////////////////////////////////////////////////////////////////////////////////
// Filename:    DocumentEvents.h
// Description: This file declares the DocumentEvents class.  This class contains
//              all events that the Document class can generate for a listener
//              to observe.
//
// Created:     2012-08-12 10:33:07
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "DocumentPosition.h"

class DocumentEvents
{
public:
	virtual void OnDocumentSizeChanged() = 0;
	virtual void OnDocumentEditRegion(const DocumentPosition& start, const DocumentPosition& end) = 0;
	virtual void OnDocumentSelectionChanged() = 0;
};

