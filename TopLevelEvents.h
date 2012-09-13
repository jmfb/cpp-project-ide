////////////////////////////////////////////////////////////////////////////////
// Filename:    TopLevelEvents.h
// Description: This file declares the TopLevelEvents interface.
//
// Created:     2012-09-13 02:30:13
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "FileLocation.h"

class TopLevelEvents
{
public:
	virtual void GotoFileLocation(const FileLocation& fileLocation) = 0;
};

