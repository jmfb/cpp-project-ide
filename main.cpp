////////////////////////////////////////////////////////////////////////////////
// Filename:    main.cpp
// Description: Main entry point for the application.
//
// Created:     2012-08-04 10:16:09
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "MainFrame.h"
#include "resource.h"

int __stdcall WinMain(HINSTANCE instance, HINSTANCE previous, char* command, int show)
{
	auto hr = ::OleInitialize(nullptr);
	if (FAILED(hr))
		return hr;

	WIN::CWinInstance::Get().SetInstance(instance);

	MainFrame mainFrame;
	mainFrame.Create(nullptr, nullptr, "The New IDE Jake Built", WS_OVERLAPPEDWINDOW);
	mainFrame.Show(show);

	MSG msg;
	auto accelerator = ::LoadAccelerators(instance, MAKEINTRESOURCE(IDA_MAINFRAME));
	for (;;)
	{
		switch(::GetMessage(&msg, nullptr, 0, 0))
		{
		case -1:
			//TODO: Handle/Log error
			continue;
		case 0:
			::OleUninitialize();
			return msg.wParam;
		default:
			if (!::TranslateAccelerator(mainFrame.GetHWND(), accelerator, &msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
	}
}

