////////////////////////////////////////////////////////////////////////////////
// Filename:    ToolWindow.cpp
// Description: This file implements all ToolWindow member functions.
//
// Created:     2012-09-12 18:43:39
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "ToolWindow.h"

const auto outputTabId = 1;
const auto testResultsTabId = 2;
const auto tabHeight = 27;

void ToolWindow::SetupClass(WNDCLASSEX& cls)
{
	cls.lpszClassName = "ToolWindow";
}

bool ToolWindow::OnCreate(CREATESTRUCT* cs)
{
	tabs.Create(GetHWND(), 1001, WIN::RECT_DEFAULT, this);

	outputWindow.Create(
		GetHWND(),
		nullptr,
		nullptr,
		WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
		WS_EX_CONTROLPARENT);
	testResultsWindow.Create(
		GetHWND(),
		nullptr,
		nullptr,
		WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
		WS_EX_CONTROLPARENT);

	tabs.AddItem("Output", 0, outputTabId);
	tabs.AddItem("Test Results", 0, testResultsTabId);
	return true;
}

void ToolWindow::OnSize(unsigned long flag, unsigned short w, unsigned short h)
{
	auto client = GetClientRect();

	auto tabRect = client;
	tabRect.bottom = tabRect.top + tabHeight;
	tabs.Move(tabRect);

	auto viewRect = client;
	viewRect.top = tabRect.bottom;
	outputWindow.Move(viewRect);
	testResultsWindow.Move(viewRect);
}

void ToolWindow::OnMDITabUnselectItem(int id, unsigned long item)
{
	switch(item)
	{
	case outputTabId:
		outputWindow.Show(SW_HIDE);
		break;
	case testResultsTabId:
		testResultsWindow.Show(SW_HIDE);
		break;
	}
}

void ToolWindow::OnMDITabSelectItem(int id, unsigned long item)
{
	switch(item)
	{
	case outputTabId:
		outputWindow.Show(SW_NORMAL);
		break;
	case testResultsTabId:
		testResultsWindow.Show(SW_NORMAL);
		break;
	}
}

void ToolWindow::OnMDITabNoSelection(int id)
{
}

void ToolWindow::OnMDITabCloseSelection(int id, unsigned long item)
{
	MsgBox("Closing tool windows is not supported.");
}

void ToolWindow::ShowOutputWindow()
{
	tabs.Select(outputTabId);
}

void ToolWindow::ShowTestResultsWindow()
{
	tabs.Select(testResultsTabId);
}

FindInDocumentWindow& ToolWindow::GetFindInDocumentWindow()
{
	return outputWindow.GetFindInDocumentWindow();
}

OutputWindow& ToolWindow::GetOutputWindow()
{
	return outputWindow;
}

TestResultsWindow& ToolWindow::GetTestResultsWindow()
{
	return testResultsWindow;
}

