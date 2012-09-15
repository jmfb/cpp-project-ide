////////////////////////////////////////////////////////////////////////////////
// Filename:    MainFrame.cpp
// Description: This file implements all MainFrame member functions.
//
// Created:     2012-08-04 10:39:43
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "MainFrame.h"
#include "NewProjectDialog.h"
#include "NewFolderDialog.h"
#include "NewFileDialog.h"
#include "NewClassDialog.h"
#include "GotoLineDialog.h"
#include "FindInFilesDialog.h"
#include "ProjectSettingsDialog.h"
#include "EditOptionsDialog.h"
#include "DocumentUtility.h"
#include "OutputWindow.h"
#include "FindInDocumentWindow.h"
#include "TestResultsWindow.h"
#include "BuildThread.h"
#include "Process.h"
#include "Settings.h"
#include "resource.h"
#include <cstring>
#include <future>

const UINT_PTR buildTimer = 1;

MainFrame::MainFrame()
	: stoppingBuild(false)
{
}

void MainFrame::SetupClass(WNDCLASSEX& cls)
{
	cls.lpszClassName = "MainFrame";
	cls.lpszMenuName = MAKEINTRESOURCE(IDM_MAINFRAME);
	cls.hbrBackground = nullptr;
	cls.hIcon = WIN::LoadLargeIcon(IDI_APPLICATION_LARGE);
	cls.hIconSm = WIN::LoadSmallIcon(IDI_APPLICATION_SMALL);
	cls.style = CS_HREDRAW|CS_VREDRAW;
}

bool MainFrame::OnCreate(CREATESTRUCT* cs)
{
	statusBar.Create(GetHWND(), 1, CCS_BOTTOM|SBARS_SIZEGRIP);
	statusBar.SetText(0, "Ready");

	projectSplitter.Create(GetHWND(), GetClientRect());
	documentSplitter.Create(
		projectSplitter,
		WIN::RECT_DEFAULT,
		false,
		-1,
		WIN::SPLIT_BOTH,
		true);

	projectWindow.Create(
		projectSplitter,
		nullptr,
		nullptr,
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
		WS_EX_CONTROLPARENT);
	documentWindow.Create(
		documentSplitter,
		nullptr,
		nullptr,
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN);
	toolWindow.Create(
		documentSplitter,
		nullptr,
		nullptr,
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
		WS_EX_CONTROLPARENT);

	projectSplitter.SetPanes(projectWindow, documentSplitter);
	projectSplitter.SetPos(250);
	documentSplitter.SetPanes(documentWindow, toolWindow);
	documentSplitter.SetPos(200);

	outputWindow = &toolWindow.GetOutputWindow();
	findWindow = &toolWindow.GetFindInDocumentWindow();
	testResultsWindow = &toolWindow.GetTestResultsWindow();

	projectWindow.SetEvents(this);
	documentWindow.SetEvents(this);
	documentWindow.SetProject(&project);
	findWindow->SetEvents(documentWindow.GetFindInDocumentEvents());
	testResultsWindow->SetTopLevelEvents(this);
	documentWindow.SetOutputTarget(outputWindow);
	outputWindow->Append("Ready\r\n");

	return true;
}

void MainFrame::OnDestroy()
{
	::PostQuitMessage(0);
}

void MainFrame::OnClose()
{
	if (CloseProject())
		Destroy();
}

void MainFrame::OnCommand(WORD code, WORD id, HWND hwnd)
{
	switch(id)
	{
	case ID_FILE_NEW_PROJECT:
		OnFileNewProject();
		break;
	case ID_FILE_OPEN_PROJECT:
		OnFileOpenProject();
		break;
	case ID_FILE_SAVE_PROJECT:
		OnFileSaveProject();
		break;
	case ID_FILE_CLOSE_PROJECT:
		OnFileCloseProject();
		break;
	case ID_FILE_RENAME:
		OnFileRename();
		break;
	case ID_FILE_CLOSE_DOCUMENT:
		OnFileCloseDocument();
		break;
	case ID_FILE_CLOSE_ALL_DOCUMENTS:
		OnFileCloseAllDocuments();
		break;
	case ID_FILE_CLOSE_ALL_BUT_THIS:
		OnFileCloseAllButThis();
		break;
	case ID_FILE_OPEN_CONTAINING_FOLDER:
		OnFileOpenContainingFolder();
		break;
	case ID_FILE_EXIT:
		Post(WM_CLOSE);
		break;
	case ID_EDIT_OPEN_FILE:
		OnEditOpenFile();
		break;
	case ID_FILE_CREATE_FOLDER:
		OnFileCreateFolder();
		break;
	case ID_FILE_CREATE_FILE:
		OnFileCreateFile();
		break;
	case ID_FILE_ADD_EXISTING_FILE:
		OnFileAddExistingFile();
		break;
	case ID_EDIT_DELETE:
		if (projectWindow.HasFocus())
			projectWindow.DeleteSelection();
		else
			documentWindow.OnCommand(code, id, hwnd);
		break;
	case ID_FILE_CREATE_CLASS:
		OnFileCreateClass();
		break;
	case ID_BUILD_COMPILE:
		OnBuildCompile();
		break;
	case ID_BUILD_CANCEL:
		OnBuildCancel();
		break;
	case ID_BUILD_BUILD:
		OnBuildBuild();
		break;
	case ID_BUILD_EXECUTE:
		OnBuildExecute();
		break;
	case ID_BUILD_REBUILD:
		OnBuildRebuild();
		break;
	case ID_BUILD_CLEAN:
		OnBuildClean();
		break;
	case ID_FILE_PROJECT_SETTINGS:
		OnFileProjectSettings();
		break;
	case ID_BUILD_EXECUTE_UNIT_TEST:
		OnBuildExecuteUnitTest();
		break;
	case ID_BUILD_GOTO_ERROR:
		OnBuildGotoError();
		break;
	case ID_EDIT_FIND:
		OnEditFind();
		break;
	case ID_EDIT_GOTO_LINE:
		OnEditGotoLine();
		break;
	case ID_EDIT_FIND_IN_FILES:
		OnEditFindInFiles();
		break;
	case ID_TOOLS_EDIT_OPTIONS:
		OnToolsEditOptions();
		break;

	default:
		if (id >= ID_FIRST_DOCUMENT_COMMAND && id <= ID_LAST_DOCUMENT_COMMAND)
			documentWindow.OnCommand(code, id, hwnd);
		break;
	}
}

void MainFrame::OnSize(unsigned long flag, unsigned short w, unsigned short h)
{
	statusBar.UpdateSize();
	auto client = GetClientRect();
	auto bar = statusBar.GetWindowRect();
	client.bottom -= bar.bottom - bar.top;
	projectSplitter.Move(client);
}

void MainFrame::OnTimer(UINT_PTR id)
{
	switch(id)
	{
	case buildTimer:
		if (!buildThread)
		{
			KillTimer(id);
		}
		else if (buildThread->IsDone())
		{
			KillTimer(id);
			buildThread.reset();
		}
		break;
	}
}

void MainFrame::OnFileNewProject()
{
	if (CloseProject())
	{
		NewProjectDialog dialog;
		if (dialog.DoModal(GetHWND()) == IDOK)
		{
			project.New(dialog.GetDirectory(), dialog.GetName());
			projectWindow.SetProject(project);
			//TODO: update UI display of shit
		}
	}
}

void MainFrame::OnFileOpenProject()
{
	if (CloseProject())
	{
		static const auto SIZE = 1024ul;
		char buffer[SIZE] = "";

		OPENFILENAME ofn;
		std::memset(&ofn, 0, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = GetHWND();
		ofn.lpstrFilter = "C++ Project Files\0*.cpp-project\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFile = buffer;
		ofn.nMaxFile = SIZE;
		ofn.lpstrInitialDir = "c:\\save\\code";
		ofn.lpstrTitle = "Open Project File";
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		if (::GetOpenFileName(&ofn))
		{
			project.Open(buffer);
			projectWindow.SetProject(project);
			//TODO: change title bar, populate project window
		}
	}
}

void MainFrame::OnFileSaveProject()
{
	if (project.IsOpen())
	{
		if (project.IsDirty())
			project.Save(project.GetFileName());
		documentWindow.SaveAllDirtyDocuments();
	}
}

void MainFrame::OnFileCloseProject()
{
	CloseProject();
}

void MainFrame::OnFileRename()
{
	projectWindow.RenameSelection();
}

void MainFrame::OnFileCloseDocument()
{
	documentWindow.CloseSelectedDocument();
}

void MainFrame::OnFileCloseAllDocuments()
{
	if (documentWindow.IsAnyDocumentDirty())
	{
		auto result = MsgBox("Save changes to documents?", "Save Changes?", MB_YESNOCANCEL);
		if (result == IDCANCEL)
		{
			documentWindow.SetFocus();
			return;
		}
		if (result == IDYES)
			documentWindow.SaveAllDirtyDocuments();
	}
	documentWindow.CloseAllDocuments();
}

void MainFrame::OnFileCloseAllButThis()
{
	documentWindow.CloseAllButThis();
}

void MainFrame::OnFileOpenContainingFolder()
{
	if (project.IsOpen())
	{
		auto folder = STRING::to_wstring(project.GetFileName());
		PIDLIST_ABSOLUTE idlist = nullptr;
		::SHParseDisplayName(folder.c_str(), nullptr, &idlist, 0, nullptr);
		::SHOpenFolderAndSelectItems(idlist, 0, nullptr, 0);
	}
}

void MainFrame::OnProjectOpenFile(const std::string& fileName)
{
	documentWindow.OpenDocument(fileName);
}

void MainFrame::OnProjectRenameFile(const std::string& oldFileName, const std::string& newFileName)
{
	documentWindow.RenameDocument(oldFileName, newFileName);
}

void MainFrame::OnEditOpenFile()
{
	Settings settings;
	auto systemIncludeDirectories = settings.GetSystemIncludeDirectories();
	auto fileNamePair = documentWindow.GetFileNameAtCursor();
	if (!fileNamePair.first.empty())
	{
		auto directories = systemIncludeDirectories;
		directories.insert(fileNamePair.second ? directories.end() : directories.begin(),
			project.GetIncludeDirectories().begin(),
			project.GetIncludeDirectories().end());
		directories.insert(fileNamePair.second ? directories.end() : directories.begin(),
			FSYS::GetFilePath(project.GetFileName()));
		directories.push_back(FSYS::GetFilePath(documentWindow.GetDocumentFileName()));
		for (const auto& directory: directories)
		{
			auto fullPath = FSYS::FormatPath(directory, fileNamePair.first);
			if (FSYS::FileExists(fullPath))
			{
				documentWindow.OpenDocument(fullPath);
				return;
			}
		}
		MsgBox("Could not locate '" + fileNamePair.first + "' in the include paths.",
			"Could Not Find File",
			MB_OK|MB_ICONINFORMATION);
	}
}

void MainFrame::OnFileCreateFolder()
{
	if (!project.IsOpen())
		return;
	NewFolderDialog dlg;
	if (dlg.DoModal(GetHWND()) == IDOK)
		projectWindow.CreateNewFolder(dlg.GetName());
}

void MainFrame::OnFileCreateFile()
{
	if (!project.IsOpen())
		return;
	NewFileDialog dlg;
	if (dlg.DoModal(GetHWND()) == IDOK)
	{
		auto fileName = FSYS::FormatPath(FSYS::GetFilePath(project.GetFileName()), dlg.GetName());
		if (FSYS::FileExists(fileName))
		{
			auto result = MsgBox(
				"That file already exists, are you sure you want to overwrite it with a blank file?\r\n" + fileName,
				"Confirm File Overwrite",
				MB_YESNO|MB_ICONQUESTION);
			if (result == IDNO)
				return;
		}

		projectWindow.CreateNewFile(dlg.GetName());

		std::ofstream out(fileName.c_str());
		out << std::endl;
		out.close();

		documentWindow.OpenDocument(fileName);
	}
}

void MainFrame::OnFileAddExistingFile()
{
	if (!project.IsOpen())
		return;

	const auto bufferSize = 4096ul;
	char buffer[bufferSize] = "";

	auto defaultdir = FSYS::GetFilePath(project.GetFileName());

	OPENFILENAME ofn = {0};
	std::memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetHWND();
	ofn.lpstrFilter =
		"Code Files\0*.cpp;*.h;*.inl;*.rc\0"
		"Resource Files\0*.ico;*.bmp;*.xml;*.txt\0"
		"All Files\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = buffer;
	ofn.nMaxFile = bufferSize;
	ofn.lpstrInitialDir = defaultdir.c_str();
	ofn.lpstrTitle = "Add Documents(s) to Project";
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

	auto b = ::GetOpenFileName(&ofn);
	if (!b)
		return;
	if (buffer[ofn.nFileOffset - 1] == '\0')
	{
		auto dir = buffer;
		std::string file;
		for (unsigned long i = ofn.nFileOffset; i < bufferSize; ++i)
		{
			if (buffer[i] == '\0')
			{
				AddFile(FSYS::FormatPath(dir, file));
				file.erase();
				if (buffer[i + 1] == '\0')
				{
					break;
				}
			}
			else
			{
				file += buffer[i];
			}
		}
	}
	else
	{
		AddFile(buffer);
	}
}

void MainFrame::AddFile(const std::string& fileName)
{
	char buffer[MAX_PATH] = "";
	::PathRelativePathTo(buffer, project.GetFileName().c_str(), 0, fileName.c_str(), 0);
	auto relativePath = STRING::trim(buffer);
	if (relativePath.find("..\\") == 0)
	{
		MsgBox("You cannot add files above the root directory of the project.",
			"Cannot Add Document",
			MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	if (relativePath.find(".\\") == 0)
	{
		relativePath.erase(relativePath.begin(), relativePath.begin() + 2);
		relativePath = STRING::trim(relativePath);
	}

	if (relativePath.empty())
		return;

	projectWindow.CreateNewFile(relativePath);
}

void MainFrame::OnFileCreateClass()
{
	if (!project.IsOpen())
		return;
	NewClassDialog dlg;
	if (dlg.DoModal(GetHWND()) == IDOK)
	{
		//Determine the directory for the new class, make sure it exists
		auto directory = FSYS::GetFilePath(project.GetFileName());
		if (!dlg.GetFolder().empty())
			directory = FSYS::FormatPath(directory, dlg.GetFolder());
		if (!FSYS::PathExists(directory))
			FSYS::CreatePath(directory);

		//Generate the header (there will always be one)
		auto headerFileName = FSYS::FormatPath(directory, dlg.GetClassName() + ".h");
		std::ofstream outHeader(headerFileName.c_str());
		outHeader << DocumentUtility::GenerateClassHeader(
			headerFileName,
			dlg.GetClassName(),
			dlg.GetClassNamespace(),
			dlg.GetBigFourOption(),
			dlg.GetUseDefaultBigFour(),
			dlg.GetFilesOption(),
			dlg.GetUnitTest());
		outHeader.close();

		//Create the folder in the project window for holding the new class
		projectWindow.CreateNewFolder(dlg.GetClassName());

		//Add the header file to the folder
		AddFile(headerFileName);

		//If we are not generating only a header file, generate the source file (may be cpp or inl at this point)
		if (dlg.GetFilesOption() != NewClassDialog::FilesOption::Header)
		{
			auto suffix = dlg.GetFilesOption() == NewClassDialog::FilesOption::Source ? ".cpp" : ".inl";
			auto sourceFileName = FSYS::FormatPath(directory, dlg.GetClassName() + suffix);
			std::ofstream outSource(sourceFileName.c_str());
			outSource << DocumentUtility::GenerateClassSource(
				sourceFileName,
				dlg.GetClassName(),
				dlg.GetClassNamespace(),
				dlg.GetBigFourOption(),
				dlg.GetUseDefaultBigFour(),
				dlg.GetFilesOption());
			outSource.close();

			//Add the source file to the project window
			AddFile(sourceFileName);

			//Open the source document
			documentWindow.OpenDocument(sourceFileName);
		}

		if (dlg.GetUnitTest())
		{
			auto unitTestFileName = FSYS::FormatPath(directory, dlg.GetClassName() + ".Test.cpp");
			std::ofstream outUnitTest(unitTestFileName.c_str());
			outUnitTest << DocumentUtility::GenerateClassUnitTest(
				unitTestFileName,
				dlg.GetClassName(),
				dlg.GetClassNamespace());
			outUnitTest.close();

			AddFile(unitTestFileName);
			documentWindow.OpenDocument(unitTestFileName);
		}

		//Open the header document (do this last because it is more likely to be modified first).
		documentWindow.OpenDocument(headerFileName);
	}
}

void MainFrame::OnBuildCompile()
{
	//Do not build if we are currently building or do not have a project open
	if (!project.IsOpen() || buildThread)
		return;

	auto projectItem = projectWindow.GetSelectedFile();
	if (projectItem == nullptr)
		return;
	FileCompileSettings setting;
	setting.SetProjectItemFile(&project, projectItem);
	if (!setting.CanCompile())
		return;

	//Save changes to disk (compile will be unstable otherwise)
	if (project.IsDirty())
		project.Save(project.GetFileName());
	documentWindow.SaveAllDirtyDocuments();

	//Clear the output window
	outputWindow->Clear();

	stoppingBuild = false;
	buildThread.reset(new BuildThread());
	buildThread->AddFileCompileSettings(setting);
	buildThread->Build(this, 1, FSYS::GetFilePath(project.GetFileName()));
	SetTimer(buildTimer, 10);
}

void MainFrame::OnBuildCancel()
{
	stoppingBuild = true;
}

void MainFrame::OnBuildBuild()
{
	if (!project.IsOpen() || buildThread)
		return;

	//Save changes to disk (compile will be unstable otherwise)
	if (project.IsDirty())
		project.Save(project.GetFileName());
	documentWindow.SaveAllDirtyDocuments();

	//Clear the output window
	outputWindow->Clear();
	toolWindow.ShowOutputWindow();

	stoppingBuild = false;
	buildThread.reset(new BuildThread());

	class BuildVisitor : public ProjectItemVisitor
	{
	public:
		BuildVisitor(Project* project, BuildThreadPtr buildThread)
			: project(project), buildThread(buildThread)
		{
		}
		void VisitFile(ProjectItemFile& file) override
		{
			FileCompileSettings setting;
			setting.SetProjectItemFile(project, &file);
			if (setting.CanCompile())
			{
				buildThread->AddFileCompileSettings(setting);
				auto outputFile = setting.GetOutputFile("o");
				auto isObject = true;
				auto isTestObject = true;
				if (STRING::EndsWith(outputFile, "/main.o"))
					isTestObject = false;
				else if (STRING::EndsWith(outputFile, ".Test.o"))
				{
					buildUnitTest = true;
					isObject = false;
				}
				if (isObject)
					objects << "./" << outputFile << " ";
				if (isTestObject)
					testObjects << "./" << outputFile << " ";
			}
		}
		void VisitFolder(ProjectItemFolder& folder) override
		{
			//nothing
		}
		std::string GetObjectList() const
		{
			return objects.str();
		}
		std::string GetTestObjectList() const
		{
			return testObjects.str();
		}
		bool GetBuildUnitTest() const
		{
			return buildUnitTest;
		}
	private:
		Project* project = nullptr;
		BuildThreadPtr buildThread;
		std::ostringstream objects;
		std::ostringstream testObjects;
		bool buildUnitTest = false;
	};
	BuildVisitor buildVisitor(&project, buildThread);
	project.GetRootFolder().Visit(&buildVisitor);

	auto objects = buildVisitor.GetObjectList();
	auto testObjects = buildVisitor.GetTestObjectList();
	if (!objects.empty())
		buildThread->MakeProjectTarget(&project, objects);
	if (!testObjects.empty() && buildVisitor.GetBuildUnitTest())
		buildThread->MakeProjectUnitTest(&project, testObjects);
	buildThread->Build(this, 1, FSYS::GetFilePath(project.GetFileName()));
	SetTimer(buildTimer, 10);
}

void MainFrame::OnBuildExecute()
{
	if (!project.IsOpen() || buildThread)
		return;
	auto targetFile = project.GetTargetFile();
	if (!FSYS::FileExists(targetFile))
	{
		MsgBox("Please build the project before executing.",
			"Target file does not exist.", MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	STARTUPINFO startupInfo = {0};
	std::memset(&startupInfo, 0, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);

	PROCESS_INFORMATION processInfo = {0};
	std::memset(&processInfo, 0, sizeof(processInfo));

	//We need to make a copy of the string for the command line (non-const)
	STRING::CStringPtr commandCopy(new char[targetFile.size() + 1]);
	std::strcpy(commandCopy.Get(), targetFile.c_str());

	//Create the process (do not show the associated console)
	auto result = ::CreateProcess(
		nullptr,
		commandCopy.Get(),
		nullptr,
		nullptr,
		FALSE,
		CREATE_NEW_PROCESS_GROUP,
		nullptr,
		FSYS::GetFilePath(targetFile).c_str(),
		&startupInfo,
		&processInfo);
	if (!result)
	{
		MsgBox("Failed to create the process.", "Could not execute.", MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	//Attach resultant process and thread handles to scoped containers.
	WIN::CHandle processThread(processInfo.hThread);
	WIN::CHandle process(processInfo.hProcess);
}

void MainFrame::OnBuildClean()
{
	if (!project.IsOpen() || buildThread)
		return;

	auto directory = FSYS::FormatPath(FSYS::GetFilePath(project.GetFileName()), project.GetOutputFolder());
	if (!FSYS::PathExists(directory))
		return;

	STRING::CStringPtr buffer(new char[directory.size() + 2]);
	std::strcpy(buffer.Get(), directory.c_str());
	buffer.Get()[directory.size() + 1] = '\0';

	SHFILEOPSTRUCT fileOperation = {0};
	std::memset(&fileOperation, 0, sizeof(fileOperation));
	fileOperation.hwnd = nullptr;
	fileOperation.wFunc = FO_DELETE;
	fileOperation.pFrom = buffer.Get();
	fileOperation.pTo = nullptr;
	fileOperation.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;
	fileOperation.fAnyOperationsAborted = FALSE;
	fileOperation.hNameMappings = nullptr;
	fileOperation.lpszProgressTitle = nullptr;
	auto result = ::SHFileOperation(&fileOperation);
	if (fileOperation.hNameMappings != nullptr)
		::SHFreeNameMappings(fileOperation.hNameMappings);
	if (result != 0)
		MsgBox(ERR::FormatWindowsError(result), "Could not delete output directory.", MB_OK|MB_ICONERROR);
}

void MainFrame::OnBuildRebuild()
{
	if (!project.IsOpen() || buildThread)
		return;
	OnBuildClean();
	OnBuildBuild();
}

void MainFrame::OnFileProjectSettings()
{
	if (!project.IsOpen() || buildThread)
		return;

	ProjectSettingsDialog dlg;
	dlg.SetProject(&project);
	if (dlg.DoModal(GetHWND()) == IDOK)
	{
		OnBuildClean();
		projectWindow.UpdateProjectName();
	}
}

void MainFrame::OnBuildExecuteUnitTest()
{
	if (!project.IsOpen() || buildThread)
		return;
	auto targetFile = project.GetTargetUnitTestFile();
	if (!FSYS::FileExists(targetFile))
	{
		MsgBox("Please build the project before executing.",
			"Target file does not exist.", MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	toolWindow.ShowTestResultsWindow();

	try
	{
		Process process;
		process.Start(targetFile + " PrintTests", FSYS::GetFilePath(targetFile));
		process.SoftWaitForExit();
		auto testList = process.ReadOutputPipe();
		testResultsWindow->RunTests(testList, targetFile);
	}
	catch (const ERR::CError& error)
	{
		MsgBox(error.Format(), "Error", MB_OK|MB_ICONERROR);
	}
}

void MainFrame::OnBuildGotoError()
{
	GotoFileLocation(outputWindow->GetSelectedFileLocation());
}

void MainFrame::GotoFileLocation(const FileLocation& fileLocation)
{
	if (fileLocation.IsValid())
	{
		auto fileName = FSYS::FormatPath(FSYS::GetFilePath(project.GetFileName()), fileLocation.GetFileName());
		if (FSYS::FileExists(fileName))
		{
			documentWindow.OpenDocument(fileName);
			documentWindow.SetCursorPosition(fileLocation.GetLine(), fileLocation.GetColumn());
			documentWindow.SetViewFocus();
		}
	}
}

void MainFrame::OnEditFind()
{
	toolWindow.ShowOutputWindow();
	findWindow->OnEditFind();
}

void MainFrame::OnEditGotoLine()
{
	if (!documentWindow.IsDocumentOpen())
		return;
	GotoLineDialog dlg;
	dlg.SetLineCount(documentWindow.GetDocumentLineCount());
	if (dlg.DoModal(GetHWND()) == IDOK)
	{
		documentWindow.SetCursorPosition(dlg.GetLine(), 0);
		documentWindow.SetViewFocus();
	}
}

void MainFrame::OnEditFindInFiles()
{
	if (!project.IsOpen())
		return;
	FindInFilesDialog dlg;
	if (dlg.DoModal(GetHWND()) == IDOK)
	{
		auto findText = dlg.GetFindText();

		class FindVisitor : public ProjectItemVisitor
		{
		public:
			FindVisitor(Project* project, std::string findText)
				: project(project), findText(findText)
			{
			}
			void VisitFile(ProjectItemFile& file) override
			{
				auto relativeFileName = file.GetName();
				auto fileName = FSYS::FormatPath(FSYS::GetFilePath(project->GetFileName()), relativeFileName);
				results.emplace_back(std::async(&FindVisitor::FindInFile, fileName, relativeFileName, findText));
			}
			void VisitFolder(ProjectItemFolder& folder) override
			{
				//nothing
			}

			static std::string FindInFile(std::string fileName, std::string relativeFileName, std::string findText)
			{
				std::ostringstream out;
				std::ifstream in(fileName.c_str());
				auto lineNumber = 1ul;
				for (std::string line; std::getline(in, line); ++lineNumber)
				{
					auto iter = std::search(line.begin(), line.end(), findText.begin(), findText.end(), STRING::iequal_char());
					if (iter != line.end())
						out << "0> " << relativeFileName << ":" << lineNumber << ":" << ((iter - line.begin()) + 1) << ": " << line << std::endl;
				}
				return STRING::replace(out.str(), "\n", "\r\n");
			}

			void Finish(OutputTarget* outputTarget)
			{
				outputTarget->Clear();
				outputTarget->Append("Find in files results for '" + findText + "'.\r\n");
				for (auto& result: results)
					outputTarget->Append(result.get());
			}

		private:
			Project* project = nullptr;
			std::string findText;
			std::vector<std::future<std::string>> results;
		};

		FindVisitor visitor(&project, findText);
		project.GetRootFolder().Visit(&visitor);
		visitor.Finish(outputWindow);
	}
}

void MainFrame::OnToolsEditOptions()
{
	EditOptionsDialog dlg;
	dlg.DoModal(GetHWND());
}

void MainFrame::OnDocumentWindowSelectionChanged(const std::string& fileName)
{
	projectWindow.EnsureFileIsVisible(fileName);
}

bool MainFrame::IsStopping() const
{
	return stoppingBuild;
}

void MainFrame::ProcessMessage(unsigned long id, const std::string& message)
{
	outputWindow->ProcessBuildMessage(id, message);
}

bool MainFrame::CloseProject()
{
	if (project.IsOpen() && (project.IsDirty() || documentWindow.IsAnyDocumentDirty()))
	{
		auto result = MsgBox("Save changes to project?", "Save Changes?", MB_YESNOCANCEL);
		if (result == IDCANCEL)
			return false;
		if (result == IDYES)
		{
			if (project.IsDirty())
				project.Save(project.GetFileName());
			documentWindow.SaveAllDirtyDocuments();
		}
	}

	project.Close();
	projectWindow.Clear();
	documentWindow.CloseAllDocuments();
	return true;
}

