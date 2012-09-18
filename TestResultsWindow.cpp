////////////////////////////////////////////////////////////////////////////////
// Filename:    TestResultsWindow.cpp
// Description: This file implements all TestResultsWindow member functions.
//
// Created:     2012-09-12 19:29:35
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "TestResultsWindow.h"
#include "resource.h"

const auto testManagerTimerId = 1;
const auto labelStatusId = 1001;
const auto checkFilterId = 1002;
const auto listViewId = 1003;
const auto checkFilterWidth = 220;
const auto headerHeight = 16;

void TestResultsWindow::SetupClass(WNDCLASSEX& cls)
{
	cls.lpszClassName = "TestResultsWindow";
}

bool TestResultsWindow::OnCreate(CREATESTRUCT* cs)
{
	auto dc = ::GetDC(GetHWND());
	font.Create("Courier New", WIN::CFont::CalcHeight(dc, 8));
	::ReleaseDC(GetHWND(), dc);

	imageList.CreateSmallIconList();
	imageList[IDI_TEST_PENDING];
	imageList[IDI_TEST_RUNNING];
	imageList[IDI_TEST_SUCCESS];
	imageList[IDI_TEST_FAILED];

	labelStatus.Attach(WIN::CWindow::Create(WC_STATIC, GetHWND(), reinterpret_cast<HMENU>(labelStatusId),
		"", WS_CHILD|WS_VISIBLE|SS_CENTERIMAGE, 0, 0, 0, 1, 1, nullptr));
	checkFilter.Attach(WIN::CWindow::Create(WC_BUTTON, GetHWND(), reinterpret_cast<HMENU>(checkFilterId),
		"Only Show Failed Unit Tests", WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX, 0, 0, 0, 1, 1, nullptr));
	labelStatus.SetFont(font.Get());
	checkFilter.SetFont(font.Get());

	listView.Create(
		GetHWND(),
		listViewId,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_HSCROLL|WS_VSCROLL|
		LVS_REPORT|LVS_NOSORTHEADER|LVS_SHAREIMAGELISTS|LVS_SINGLESEL,
		WS_EX_CLIENTEDGE);
	listView.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

	listView.InsertColumn(0, "", 24, LVCFMT_IMAGE);
	listView.InsertColumn(1, "Test Class", 150);
	listView.InsertColumn(2, "Test Method", 200);
	listView.InsertColumn(3, "Description", 600);

	listView.SetImageList(imageList, LVSIL_SMALL);

	return true;
}

void TestResultsWindow::OnSize(unsigned long flag, unsigned short w, unsigned short h)
{
	auto client = GetClientRect();

	auto labelRect = client;
	labelRect.bottom = labelRect.top + headerHeight;
	labelRect.right -= checkFilterWidth;
	labelStatus.Move(labelRect);

	auto checkRect = client;
	checkRect.left = labelRect.right;
	checkRect.bottom = labelRect.bottom;
	checkFilter.Move(checkRect);

	auto listRect = client;
	listRect.top = labelRect.bottom;
	listView.Move(listRect);
}

void TestResultsWindow::OnCommand(WORD code, WORD id, HWND hwnd)
{
	switch(id)
	{
	case checkFilterId:
		if (code == BN_CLICKED)
			FilterResults();
		break;
	}
}

void TestResultsWindow::OnTimer(UINT_PTR id)
{
	switch(id)
	{
	case testManagerTimerId:
		if (testManager)
		{
			if (testManager->IsDone())
			{
				testManager.reset();
				KillTimer(testManagerTimerId);
			}

			std::lock_guard<std::mutex> lock(updatesLock);
			for (const auto& update: updates)
			{
				auto listIndex = listView.Find(update.index);
				if (listIndex != -1)
				{
					listView.SetItemImage(listIndex, TestStatusToImage(update.status));
					listView.SetItemText(listIndex, 3, update.description);
				}
				auto& test = tests[update.index];
				test.status = update.status;
				test.description = update.description;
				if (update.status == TestStatus::Success)
					++successCount;
				else if (update.status == TestStatus::Failed)
					++failedCount;
			}
			updates.clear();

			std::ostringstream out;
			out << successCount << "/" << tests.size() << " passed. " << failedCount << " failed.";
			labelStatus.SetText(out.str());

			//Show failed tests on completion if there were any
			if (!testManager && failedCount > 0)
			{
				SetDlgItemChecked(checkFilterId, true);
				FilterResults();
			}
		}
		break;
	}
}

void TestResultsWindow::OnNotify(NMHDR* hdr)
{
	if (static_cast<int>(hdr->idFrom) == listViewId)
	{
		switch(hdr->code)
		{
		case NM_DBLCLK:
			{
				NMITEMACTIVATE* item = reinterpret_cast<NMITEMACTIVATE*>(hdr);
				unsigned long index = listView.GetItemParam(item->iItem);
				if (index < tests.size() && events != nullptr)
					events->GotoFileLocation({ "0> " + tests[index].location });
			}
			break;
		}
	}
}

void TestResultsWindow::RunTests(const std::string& testList, const std::string& targetFile)
{
	if (testManager)
		return;

	listView.Clear();
	tests.clear();
	successCount = 0;
	failedCount = 0;
	SetDlgItemChecked(checkFilterId, false);
	labelStatus.SetText("");
	testManager = std::make_shared<TestManagerThread>();

	std::istringstream in(testList);
	for (std::string line; std::getline(in, line); )
	{
		std::istringstream parts(line);
		std::string location, className, methodName;
		parts >> location >> className >> methodName;
		tests.push_back({ TestStatus::Pending, location, className, methodName, "" });
		auto index = listView.GetItemCount();
		listView.InsertItem(index, "", index);
		listView.SetItemImage(index, imageList[IDI_TEST_PENDING]);
		listView.SetItemText(index, 1, className);
		listView.SetItemText(index, 2, methodName);
		testManager->AddUnitTest(index, targetFile + " RunSingleTest " + className + " " + methodName);
	}

	testManager->RunTests(FSYS::GetFilePath(targetFile), this);
	SetTimer(testManagerTimerId, 10);
}

void TestResultsWindow::TestRunning(unsigned long index)
{
	std::lock_guard<std::mutex> lock(updatesLock);
	updates.push_back({ index, TestStatus::Running, "" });
}

void TestResultsWindow::TestPassed(unsigned long index)
{
	std::lock_guard<std::mutex> lock(updatesLock);
	updates.push_back({ index, TestStatus::Success, "" });
}

void TestResultsWindow::TestFailed(unsigned long index, const std::string& description)
{
	std::lock_guard<std::mutex> lock(updatesLock);
	updates.push_back({ index, TestStatus::Failed, description });
}

void TestResultsWindow::FilterResults()
{
	listView.Clear();
	auto filtered = IsDlgItemChecked(checkFilterId);
	for (auto index = 0ul; index < tests.size(); ++index)
	{
		const auto& test = tests[index];
		if (test.status == TestStatus::Failed || !filtered)
		{
			auto listIndex = listView.GetItemCount();
			listView.InsertItem(listIndex, "", index);
			listView.SetItemImage(listIndex, TestStatusToImage(test.status));
			listView.SetItemText(listIndex, 1, test.className);
			listView.SetItemText(listIndex, 2, test.methodName);
			listView.SetItemText(listIndex, 3, test.description);
		}
	}
}

void TestResultsWindow::SetTopLevelEvents(TopLevelEvents* events)
{
	this->events = events;
}

int TestResultsWindow::TestStatusToImage(TestStatus status)
{
	switch(status)
	{
	case TestStatus::Running:
		return imageList[IDI_TEST_RUNNING];
	case TestStatus::Success:
		return imageList[IDI_TEST_SUCCESS];
	case TestStatus::Failed:
		return imageList[IDI_TEST_FAILED];
	default:
		return imageList[IDI_TEST_PENDING];
	}
}

