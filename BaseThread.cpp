////////////////////////////////////////////////////////////////////////////////
// Filename:    BaseThread.cpp
// Description: This file implements all BaseThread member functions.
//
// Created:     2012-08-26 22:54:30
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "BaseThread.h"

BaseThread::~BaseThread()
{
	Stop();
}

void BaseThread::Start()
{
	Stop();
	Create();	
}

void BaseThread::Stop()
{
	if (!IsNull())
		Wait();
}

unsigned int BaseThread::OnRun()
{
	Run();
	return 0;
}

