////////////////////////////////////////////////////////////////////////////////
// Filename:    CompileThreadEvents.h
// Description: This file declares the CompileThreadEvents interface.
//
// Created:     2012-08-26 23:00:24
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>

class CompileThreadEvents
{
public:
	virtual bool IsStopping() const = 0;
	virtual void ProcessMessage(unsigned long id, const std::string& message) = 0;
};

