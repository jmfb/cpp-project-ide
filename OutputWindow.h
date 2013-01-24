////////////////////////////////////////////////////////////////////////////////
// Filename:    OutputWindow.h
// Description: This file declares the OutputWindow class.  This is the lower
//              read-only edit pane that contains the build output.
//
// Created:     2012-08-04 23:59:30
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <mutex>
#include <string>
#include <list>
#include "FileLocation.h"
#include "OutputTarget.h"
#include "FindInDocumentWindow.h"

class OutputWindow :
	public WIN::CWindowImpl<OutputWindow>,
	public OutputTarget
{
public:
	OutputWindow();
	OutputWindow(const OutputWindow& rhs) = delete;
	~OutputWindow();

	OutputWindow& operator=(const OutputWindow& rhs) = delete;

	static void SetupClass(WNDCLASSEX& cls);

	bool OnCreate(CREATESTRUCT* cs) override;
	void OnSize(unsigned long flag, unsigned short w, unsigned short h) override;
	HBRUSH OnCtlColorStatic(HWND control, HDC dc) override;
	void OnCommand(WORD code, WORD id, HWND hwnd) override;

	void Clear() override;
	void Append(const std::string& message) override;
	void ProcessBuildMessage(unsigned long id, const std::string& message) override;

	FileLocation GetSelectedFileLocation();

	FindInDocumentWindow& GetFindInDocumentWindow();
	
public:
	WIN::CEdit output;
	WIN::CFont font;
	WIN::CBrush background;
	std::mutex messageQueueLock;
	std::list<std::string> messageQueue;
	FindInDocumentWindow findWindow;
};

