////////////////////////////////////////////////////////////////////////////////
// Filename:    FindInDocumentWindow.h
// Description: This file declares the FindInDocumentWindow class.
//
// Created:     2012-09-03 20:15:32
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <CRL/CWindowImpl.h>
#include <CRL/CEdit.h>
#include <CRL/CFont.h>
#include "FindInDocumentEvents.h"

class FindInDocumentWindow : public WIN::CWindowImpl<FindInDocumentWindow>
{
public:
	FindInDocumentWindow() = default;
	FindInDocumentWindow(const FindInDocumentWindow& rhs) = delete;
	~FindInDocumentWindow() = default;

	FindInDocumentWindow& operator=(const FindInDocumentWindow& rhs) = delete;

	static void SetupClass(WNDCLASSEX& cls);

	bool OnCreate(CREATESTRUCT* cs) override;
	void OnSize(unsigned long flag, unsigned short w, unsigned short h) override;
	void OnCommand(WORD code, WORD id, HWND hwnd) override;

	void SetEvents(FindInDocumentEvents* events);
	void OnEditFind();

private:
	friend class FindInDocumentWindowTest;
	WIN::CWindow labelFindText;
	WIN::CEdit editFindText;
	WIN::CFont font;
	FindInDocumentEvents* events = nullptr;
};

