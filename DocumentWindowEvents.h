////////////////////////////////////////////////////////////////////////////////
// Filename:    DocumentWindowEvents.h
// Description: This file declares the DocumentWindowEvents interface.
//
// Created:     2012-08-25 14:57:01
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>

class DocumentWindowEvents
{
public:
	virtual void OnDocumentWindowSelectionChanged(const std::string& fileName) = 0;
};

