////////////////////////////////////////////////////////////////////////////////
// Filename:    TestResultsWindow.h
// Description: This file declares the TestResultsWindow class.
//
// Created:     2012-09-12 19:29:35
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <CRL/WinUtility.h>
#include "TestManagerThread.h"
#include "TestResultsTarget.h"
#include "TopLevelEvents.h"
#include <mutex>

class TestResultsWindow :
	public WIN::CWindowImpl<TestResultsWindow>,
	public TestResultsTarget
{
public:
	TestResultsWindow() = default;
	TestResultsWindow(const TestResultsWindow& rhs) = delete;
	~TestResultsWindow() = default;

	TestResultsWindow& operator=(const TestResultsWindow& rhs) = delete;

	static void SetupClass(WNDCLASSEX& cls);

	bool OnCreate(CREATESTRUCT* cs) override;
	void OnSize(unsigned long flag, unsigned short w, unsigned short h) override;
	void OnCommand(WORD code, WORD id, HWND hwnd) override;
	void OnTimer(UINT_PTR id) override;
	void OnNotify(NMHDR* hdr) override;

	void RunTests(const std::string& testList, const std::string& targetFile);

	void TestRunning(unsigned long index) final;
	void TestPassed(unsigned long index) final;
	void TestFailed(unsigned long index, const std::string& description) final;

	void FilterResults();
	void SetTopLevelEvents(TopLevelEvents* events);
	
private:
	enum class TestStatus
	{
		Pending,
		Running,
		Success,
		Failed
	};

	int TestStatusToImage(TestStatus status);

private:
	struct TestData
	{
		TestStatus status;
		std::string location;
		std::string className;
		std::string methodName;
		std::string description;
	};
	struct TestUpdate
	{
		unsigned long index;
		TestStatus status;
		std::string description;
	};

	TopLevelEvents* events = nullptr;
	WIN::CFont font;
	WIN::CWindow labelStatus;
	WIN::CWindow checkFilter;
	WIN::CListView listView;
	WIN::CImageList imageList;
	std::vector<TestData> tests;
	std::mutex updatesLock;
	std::vector<TestUpdate> updates;
	TestManagerThreadPtr testManager;
	unsigned long successCount = 0;
	unsigned long failedCount = 0;
};

