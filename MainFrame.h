////////////////////////////////////////////////////////////////////////////////
// Filename:    MainFrame
// Description: This file declares the MainFrame window class.  This is the main
//              frame of the application window.
//
// Created:     2012-08-04 10:37:36
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ProjectWindow.h"
#include "DocumentWindow.h"
#include "OutputWindow.h"
#include "FindInDocumentWindow.h"
#include "Project.h"
#include "ProjectEvents.h"
#include "DocumentWindowEvents.h"
#include "CompileThreadEvents.h"
#include "BuildThread.h"
#include <atomic>

class MainFrame :
	public WIN::CWindowImpl<MainFrame>,
	public ProjectEvents,
	public DocumentWindowEvents,
	public CompileThreadEvents
{
public:
	MainFrame();
	MainFrame(const MainFrame& rhs) = delete;
	~MainFrame() = default;

	MainFrame& operator=(const MainFrame& rhs) = delete;

	static void SetupClass(WNDCLASSEX& cls);

	bool OnCreate(CREATESTRUCT* cs) override;
	void OnDestroy() override;
	void OnClose() override;
	void OnCommand(WORD code, WORD id, HWND hwnd) override;
	void OnSize(unsigned long flag, unsigned short w, unsigned short h) override;
	void OnTimer(UINT_PTR id) override;

	void OnFileNewProject();
	void OnFileOpenProject();
	void OnFileSaveProject();
	void OnFileRename();
	void OnFileCloseProject();
	void OnFileCloseDocument();
	void OnFileCloseAllDocuments();
	void OnFileCloseAllButThis();
	void OnFileOpenContainingFolder();
	void OnEditOpenFile();
	void OnFileCreateFolder();
	void OnFileCreateFile();
	void OnFileAddExistingFile();
	void AddFile(const std::string& fileName);
	void OnFileCreateClass();
	void OnBuildCompile();
	void OnBuildCancel();
	void OnBuildBuild();
	void OnBuildExecute();
	void OnBuildClean();
	void OnBuildRebuild();
	void OnFileProjectSettings();
	void OnBuildExecuteUnitTest();
	void OnBuildGotoError();
	void OnEditFind();

	void OnProjectOpenFile(const std::string& fileName) override;
	void OnProjectRenameFile(const std::string& oldFileName, const std::string& newFileName) override;

	void OnDocumentWindowSelectionChanged(const std::string& fileName) override;

	bool IsStopping() const override;
	void ProcessMessage(unsigned long id, const std::string& message) override;

	bool CloseProject();

private:
	WIN::CStatusBar statusBar;
	WIN::CSplitter projectSplitter;
	WIN::CSplitter documentSplitter;
	WIN::CSplitter findSplitter;
	ProjectWindow projectWindow;
	DocumentWindow documentWindow;
	OutputWindow outputWindow;
	FindInDocumentWindow findWindow;
	Project project;
	BuildThreadPtr buildThread;
	std::atomic<bool> stoppingBuild;
};

