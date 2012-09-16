////////////////////////////////////////////////////////////////////////////////
// Filename:    DocumentWindow.h
// Description: This file declares the DocumentWindow class.  This is the main
//              area of the application window that contains the document tab
//              strip and the code editor window.
//
// Created:     2012-08-05 00:20:57
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <map>
#include "DocumentView.h"
#include "Document.h"
#include "DocumentWindowEvents.h"
#include "FindInDocumentEvents.h"
#include "OutputTarget.h"
#include "Project.h"
#include <vector>

class DocumentWindow :
	public WIN::CWindowImpl<DocumentWindow>,
	public WIN::IMDITabEvents
{
public:
	DocumentWindow() = default;
	DocumentWindow(const DocumentWindow& rhs) = delete;
	~DocumentWindow() = default;

	DocumentWindow& operator=(const DocumentWindow& rhs) = delete;

	static void SetupClass(WNDCLASSEX& cls);

	bool OnCreate(CREATESTRUCT* cs) override;
	void OnSize(unsigned long flag, unsigned short w, unsigned short h) override;
	void OnPaint() override;
	void OnCommand(WORD code, WORD id, HWND hwnd) override;
	void OnSetFocus(HWND prev) override;

	unsigned long OnMDITabUnselectItem(int id, unsigned long item) override;
	void OnMDITabSelectItem(int id, unsigned long item) override;
	void OnMDITabNoSelection(int id) override;
	void OnMDITabCloseSelection(int id, unsigned long item) override;

	void SetEvents(DocumentWindowEvents* value);
	void SetProject(Project* project);
	void OpenDocument(const std::string& fileName);
	void CloseSelectedDocument();
	void CloseAllDocuments();
	void CloseAllButThis();
	bool IsAnyDocumentDirty();
	void SaveAllDirtyDocuments();
	void RenameDocument(const std::string& oldFileName, const std::string& newFileName);
	std::pair<std::string, bool> GetFileNameAtCursor();
	void SetCursorPosition(unsigned long line, unsigned long column);
	FindInDocumentEvents* GetFindInDocumentEvents();
	void SetOutputTarget(OutputTarget* outputTarget);
	bool IsDocumentOpen();
	unsigned long GetDocumentLineCount();
	void SetViewFocus();
	std::string GetDocumentFileName();

private:
	WIN::CMDITab tabs;
	DocumentView view;
	unsigned long nextDocumentId = 1;
	std::map<unsigned long, Document> documents;
	std::map<unsigned long, POINT> scrollPositions;
	DocumentWindowEvents* events = nullptr;
	Project* project = nullptr;
	std::vector<unsigned long> documentOrder;
};

