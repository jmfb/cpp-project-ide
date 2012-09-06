////////////////////////////////////////////////////////////////////////////////
// Filename:    FindInDocumentEvents.h
// Description: This file declares the FindInDocumentEvents interface.
//
// Created:     2012-09-03 20:47:27
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>

class FindInDocumentEvents
{
public:
	virtual void FindTextInDocument(const std::string& text) = 0;
};

