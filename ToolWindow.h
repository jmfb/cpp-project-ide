////////////////////////////////////////////////////////////////////////////////
// Filename:    ToolWindow.h
// Description: This file declares the ToolWindow class.
//
// Created:     2012-09-12 18:43:39
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <CRL/WinUtility.h>
#include "OutputWindow.h"
#include "TestResultsWindow.h"

class ToolWindow :
	public WIN::CWindowImpl<ToolWindow>,
	public WIN::IMDITabEvents
{
public:
	ToolWindow() = default;
	ToolWindow(const ToolWindow& rhs) = delete;
	~ToolWindow() = default;

	ToolWindow& operator=(const ToolWindow& rhs) = delete;

	static void SetupClass(WNDCLASSEX& cls);

	bool OnCreate(CREATESTRUCT* cs) override;
	void OnSize(unsigned long flag, unsigned short w, unsigned short h) override;

	unsigned long OnMDITabUnselectItem(int id, unsigned long item) override;
	void OnMDITabSelectItem(int id, unsigned long item) override;
	void OnMDITabNoSelection(int id) override;
	void OnMDITabCloseSelection(int id, unsigned long item) override;

	void ShowOutputWindow();
	void ShowTestResultsWindow();

	FindInDocumentWindow& GetFindInDocumentWindow();
	OutputWindow& GetOutputWindow();
	TestResultsWindow& GetTestResultsWindow();

private:
	WIN::CMDITab tabs;
	OutputWindow outputWindow;
	TestResultsWindow testResultsWindow;
};

