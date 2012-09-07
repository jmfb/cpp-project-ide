////////////////////////////////////////////////////////////////////////////////
// Filename:    DocumentWindow.cpp
// Description: This file implements all DocumentWindow member functions.
//
// Created:     2012-08-05 00:22:38
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "DocumentWindow.h"
#include "resource.h"

static const int tabHeight = 27;

void DocumentWindow::SetupClass(WNDCLASSEX& cls)
{
	cls.lpszClassName = "DocumentWindow";
	cls.style = CS_HREDRAW|CS_VREDRAW;
}

bool DocumentWindow::OnCreate(CREATESTRUCT* cs)
{
	tabs.Create(GetHWND(), 1001, WIN::RECT_DEFAULT, this);
	view.Create(
		GetHWND(),
		reinterpret_cast<HMENU>(1002),
		nullptr,
		WS_CHILD|WS_VISIBLE|WS_TABSTOP,
		WS_EX_CLIENTEDGE|WS_EX_ACCEPTFILES);
	view.Show(SW_HIDE);
	return true;
}

void DocumentWindow::OnSize(unsigned long flag, unsigned short w, unsigned short h)
{
	auto client = GetClientRect();

	auto tabRect = client;
	tabRect.bottom = tabRect.top + tabHeight;
	tabs.Move(tabRect);

	auto viewRect = client;
	viewRect.top = tabRect.bottom;
	view.Move(viewRect);
}

void DocumentWindow::OnPaint()
{
	PAINTSTRUCT ps;
	auto dc = ::BeginPaint(GetHWND(), &ps);
	if (!view.IsVisible())
	{
		auto rect = GetClientRect();
		rect.top += tabHeight;
		::DrawEdge(dc, &rect, EDGE_SUNKEN, BF_ADJUST|BF_RECT);
		::FillRect(dc, &rect, ::GetSysColorBrush(COLOR_APPWORKSPACE));
	}
	::EndPaint(GetHWND(), &ps);
}

void DocumentWindow::OnCommand(WORD code, WORD id, HWND hwnd)
{
	if (!view.IsWindow() || !view.IsVisible())
		return;
	switch(id)
	{
	default:
		view.OnCommand(code, id, hwnd);
		break;
	}
}

void DocumentWindow::OnSetFocus(HWND prev)
{
	if (view.IsWindow() && view.IsVisible())
		view.SetFocus();
}

void DocumentWindow::OnMDITabUnselectItem(int id, unsigned long item)
{
	scrollPositions[item] = view.GetScrollPos();
}

void DocumentWindow::OnMDITabSelectItem(int id, unsigned long item)
{
	view.SetDocument(&documents[item]);
	view.SetScrollPos(scrollPositions[item]);
	view.Show(SW_SHOWNORMAL);
	view.SetFocus();
	if (events)
		events->OnDocumentWindowSelectionChanged(documents[item].GetFileName());
}

void DocumentWindow::OnMDITabNoSelection(int id)
{
	view.Show(SW_HIDE);
	Invalidate();
}

void DocumentWindow::OnMDITabCloseSelection(int id, unsigned long item)
{
	if (documents[item].IsDirty())
	{
		auto result = MsgBox("Save changes to document?", "Save Changes?", MB_YESNOCANCEL);
		view.SetFocus();
		if (result == IDCANCEL)
			return;
		if (result == IDYES)
			documents[item].Save();
	}

	tabs.RemoveItem(item);
	documents.erase(item);
	scrollPositions.erase(item);
}

void DocumentWindow::SetEvents(DocumentWindowEvents* value)
{
	events = value;
}

void DocumentWindow::SetProject(Project* project)
{
	this->project = project;
}

void DocumentWindow::OpenDocument(const std::string& fileName)
{
	static const std::set<std::string> supportedExtensions = {
		"H", "CPP", "INL", "XML", "RC", "", "HPP", "CPP-PROJECT", "C"
	};
	auto extension = FSYS::GetFileExt(fileName);
	if (supportedExtensions.find(STRING::upper(extension)) == supportedExtensions.end())
	{
		MsgBox("This application does not currently support editing " + extension + " files.",
			"Unable to open document.",
			MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	for (const auto& item: documents)
	{
		if (item.second.GetFileName() == fileName)
		{
			tabs.Select(item.first);
			return;
		}
	}

	auto documentId = nextDocumentId++;
	documents[documentId].Open(fileName, project->GetRelativeFileName(fileName));

	POINT scrollPosition = {0, 0};
	scrollPositions[documentId] = scrollPosition;

	tabs.AddItem(FSYS::GetFileName(fileName), 0, documentId);
	tabs.Select(documentId);
	SetViewFocus();
}

void DocumentWindow::CloseSelectedDocument()
{
	auto documentId = tabs.GetSelectedItem();
	if (documentId == std::numeric_limits<decltype(documentId)>::max())
		return;
	OnMDITabCloseSelection(0, documentId);
}

void DocumentWindow::CloseAllDocuments()
{
	for (auto& item: documents)
		tabs.RemoveItem(item.first);
	documents.clear();
	scrollPositions.clear();
}

void DocumentWindow::CloseAllButThis()
{
	auto documentId = tabs.GetSelectedItem();
	if (documentId == std::numeric_limits<decltype(documentId)>::max())
		return;

	auto dirty = false;
	for (const auto& item: documents)
	{
		if (item.first != documentId && item.second.IsDirty())
		{
			dirty = true;
			break;
		}
	}

	if (dirty)
	{
		auto result = MsgBox("Save changes to other documents?", "Save Changes?", MB_YESNOCANCEL);
		view.SetFocus();
		if (result == IDCANCEL)
			return;
		if (result == IDYES)
			for (auto& item: documents)
				if (item.first != documentId && item.second.IsDirty())
					item.second.Save();
	}

	std::set<unsigned long> documentsToClose;
	for (const auto& item: documents)
		if (item.first != documentId)
			documentsToClose.insert(item.first);
	for (auto item: documentsToClose)
	{
		tabs.RemoveItem(item);
		documents.erase(item);
		scrollPositions.erase(item);
	}
}

bool DocumentWindow::IsAnyDocumentDirty()
{
	for (const auto& item: documents)
		if (item.second.IsDirty())
			return true;
	return false;
}

void DocumentWindow::SaveAllDirtyDocuments()
{
	for (auto& item: documents)
		if (item.second.IsDirty())
			item.second.Save();
}

void DocumentWindow::RenameDocument(const std::string& oldFileName, const std::string& newFileName)
{
	for (auto& item: documents)
	{
		if (item.second.GetFileName() == oldFileName)
		{
			item.second.SetFileName(newFileName);
			tabs.SetItemText(item.first, FSYS::GetFileName(newFileName));
			break;
		}
	}
}

std::pair<std::string, bool> DocumentWindow::GetFileNameAtCursor()
{
	return view.GetFileNameAtCursor();
}

void DocumentWindow::SetCursorPosition(unsigned long line, unsigned long column)
{
	view.SetCursorPosition(line, column);
}

FindInDocumentEvents* DocumentWindow::GetFindInDocumentEvents()
{
	return &view;
}

void DocumentWindow::SetOutputTarget(OutputTarget* outputTarget)
{
	view.SetOutputTarget(outputTarget);
}

bool DocumentWindow::IsDocumentOpen()
{
	return view.IsVisible();
}

unsigned long DocumentWindow::GetDocumentLineCount()
{
	return view.GetDocumentLineCount();
}

void DocumentWindow::SetViewFocus()
{
	view.Post(WM_COMMAND, MAKEWPARAM(ID_SELECT_SET_FOCUS, 0));
}

std::string DocumentWindow::GetDocumentFileName()
{
	return view.GetDocumentFileName();
}

