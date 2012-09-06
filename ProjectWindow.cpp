////////////////////////////////////////////////////////////////////////////////
// Filename:    ProjectWindow.cpp
// Description: This file implements all ProjectWindow member functions.
//
// Created:     2012-08-04 23:36:19
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ProjectWindow.h"
#include "resource.h"

void ProjectWindow::SetupClass(WNDCLASSEX& cls)
{
	cls.lpszClassName = "ProjectWindow";
}

bool ProjectWindow::OnCreate(CREATESTRUCT* cs)
{
	tree.Create(
		GetHWND(),
		treeId,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|
		TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_SHOWSELALWAYS|TVS_EDITLABELS,
		WS_EX_CLIENTEDGE);
	//TVM_SETEXTENDEDSTYLE, TVS_EX_DOUBLEBUFFER
	tree.Send(0x1100 + 44, 0x004, 0x004);

	images.CreateSmallIconList();
	tree.SetImageList(images);

	tree.Expand(tree.GetRoot());

	return true;
}

void ProjectWindow::OnSize(unsigned long flag, unsigned short w, unsigned short h)
{
	tree.Move(GetClientRect());
}

void ProjectWindow::OnTreeViewDblClk(int id)
{
	auto item = tree.GetSelection();
	if (id != treeId || events == nullptr || project == nullptr || item == nullptr)
		return;
	auto param = tree.GetParam(item);
	if (param == 0 || tree.HitTest(GetCursorPos()) != item)
		return;
	auto projectItem = reinterpret_cast<ProjectItem*>(param);
	if (projectItem->GetType() != ProjectItemType::File)
		return;
	auto fileName = FSYS::FormatPath(FSYS::GetFilePath(project->GetFileName()), projectItem->GetName());
	events->OnProjectOpenFile(fileName);
}

void ProjectWindow::OnTreeViewReturn(int id)
{
	auto item = tree.GetSelection();
	if (id != treeId || events == nullptr || project == nullptr || item == nullptr)
		return;
	auto param = tree.GetParam(item);
	if (param == 0)
		return;
	auto projectItem = reinterpret_cast<ProjectItem*>(param);
	if (projectItem->GetType() != ProjectItemType::File)
		return;
	auto fileName = FSYS::FormatPath(FSYS::GetFilePath(project->GetFileName()), projectItem->GetName());
	events->OnProjectOpenFile(fileName);
}

void ProjectWindow::OnTreeViewRClick(int id)
{
	tree.Select(tree.HitTest(GetCursorPos()));
	auto item = tree.GetSelection();
	if (id != treeId || item == nullptr || project == nullptr || events == nullptr)
		return;
	auto param = tree.GetParam(item);
	WIN::CPopupMenu menu;
	if (param == 0)
	{
		//Right click on project
		menu.InsertCommand(0, "Open Containing Folder\tCtrl+Shift+O", ID_FILE_OPEN_CONTAINING_FOLDER);
		menu.InsertCommand(1, "Add Existing Documents\tCtrl+Shift+E", ID_FILE_ADD_EXISTING_FILE);
		menu.InsertCommand(2, "Rename\tF2", ID_FILE_RENAME);
		menu.InsertCommand(3, "Settings\tAlt+F10", ID_FILE_PROJECT_SETTINGS);
	}
	else
	{
		auto projectItem = reinterpret_cast<ProjectItem*>(param);
		switch(projectItem->GetType())
		{
		case ProjectItemType::Folder:
			//Right click on folder
			menu.InsertCommand(0, "Add Existing Documents\tCtrl+Shift+E", ID_FILE_ADD_EXISTING_FILE);
			menu.InsertCommand(1, "Rename\tF2", ID_FILE_RENAME);
			break;
		case ProjectItemType::File:
			//Right click on file
			menu.InsertCommand(0, "Compile\tF6", ID_BUILD_COMPILE);
			menu.InsertCommand(1, "Rename\tF2", ID_FILE_RENAME);
			break;
		}
	}

	POINT pt;
	::GetCursorPos(&pt);
	menu.TrackPopupMenu(0, pt, ::GetActiveWindow());
}

void ProjectWindow::OnTreeViewEndLabelEdit(int id, NMTVDISPINFO* hdr)
{
	auto item = hdr->item.hItem;
	if (project == nullptr || item == nullptr || events == nullptr || hdr->item.pszText == nullptr)
		return;
	auto value = STRING::trim(hdr->item.pszText);
	if (value.empty())
		return;
	auto param = tree.GetParam(item);
	//Rename project
	if (param == 0)
	{
		//TODO: Make sure project name does not contain invalid characters.
		//TODO: Remove message box and support the operation
		MsgBox("Project rename is not implemented.", "Not Implemented", MB_OK|MB_ICONINFORMATION);
	}
	else
	{
		auto projectItem = reinterpret_cast<ProjectItem*>(param);
		switch(projectItem->GetType())
		{
		case ProjectItemType::Folder:
			project->SetDirty();
			tree.SetText(item, value);
			projectItem->SetName(value);
			break;
		case ProjectItemType::File:
			//TODO: Make sure file name does not contain invalid characters
			project->SetDirty();
			tree.SetText(item, value);
			{
				auto projectDirectory = FSYS::GetFilePath(project->GetFileName());
				auto oldFileName = FSYS::FormatPath(projectDirectory, projectItem->GetName());
				auto directory = FSYS::GetFilePath(projectItem->GetName());
				if (directory.empty())
					projectItem->SetName(value);
				else
					projectItem->SetName(FSYS::FormatPath(directory, value));
				auto newFileName = FSYS::FormatPath(projectDirectory, projectItem->GetName());
				::MoveFile(oldFileName.c_str(), newFileName.c_str());
				events->OnProjectRenameFile(oldFileName, newFileName);
			}
			break;
		}
	}
}

class DragDropHelper : public IDropSource, public IDropTarget, public IDataObject, public IEnumFORMATETC
{
public:
	WIN::CTreeView& tree;

	DragDropHelper(WIN::CTreeView& tree)
		: tree(tree)
	{
	}

	//IUnknown
	HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) override
	{
		if (ppvObject == nullptr)
			return E_POINTER;
		if (riid == IID_IUnknown)
			*ppvObject = this;
		else if (riid == IID_IDropSource)
			*ppvObject = static_cast<IDropSource*>(this);
		else if (riid == IID_IDropTarget)
			*ppvObject = static_cast<IDropTarget*>(this);
		else if (riid == IID_IDataObject)
			*ppvObject = static_cast<IDataObject*>(this);
		else
			return E_NOINTERFACE;
		return S_OK;
	}
	ULONG __stdcall AddRef() override
	{
		return 1;
	}
	ULONG __stdcall Release() override
	{
		return 1;
	}

	//IDropSource
	HRESULT __stdcall QueryContinueDrag(WINBOOL fEscapePressed,DWORD grfKeyState) override
	{
		if (fEscapePressed)
			return DRAGDROP_S_CANCEL;
		if (!MATH::TestFlag(grfKeyState, MK_LBUTTON))
			return DRAGDROP_S_DROP;
		return S_OK;
	}
	HRESULT __stdcall GiveFeedback(DWORD dwEffect) override
	{
		return DRAGDROP_S_USEDEFAULTCURSORS;
	}

	HTREEITEM lastSource = nullptr;
	HTREEITEM lastTarget = nullptr;

	//IDropTarget
	HRESULT __stdcall DragEnter(IDataObject *pDataObj,DWORD grfKeyState,POINTL pt,DWORD *pdwEffect) override
	{
		if (pdwEffect == nullptr)
			return E_POINTER;
		*pdwEffect = DROPEFFECT_NONE;
		if (pDataObj != static_cast<IDataObject*>(this))
			return S_OK;
		lastSource = tree.GetSelection();
		tree.SetDropHilite(nullptr);

		POINT point = {0};
		point.x = pt.x;
		point.y = pt.y;
		::ScreenToClient(tree.GetHWND(), &point);
		auto item = tree.HitTest(point);
		if (item != nullptr && IsValidTarget(lastSource, item))
		{
			*pdwEffect = DROPEFFECT_MOVE;
			tree.SetDropHilite(item);
			lastTarget = item;
		}

		return S_OK;
	}
	HRESULT __stdcall DragOver(DWORD grfKeyState,POINTL pt,DWORD *pdwEffect) override
	{
		if (pdwEffect == nullptr)
			return E_POINTER;
		*pdwEffect = DROPEFFECT_NONE;
		tree.SetDropHilite(nullptr);

		POINT point = {0};
		point.x = pt.x;
		point.y = pt.y;
		::ScreenToClient(tree.GetHWND(), &point);
		auto item = tree.HitTest(point);
		if (item != nullptr && IsValidTarget(lastSource, item))
		{
			*pdwEffect = DROPEFFECT_MOVE;
			tree.SetDropHilite(item);
			lastTarget = item;
		}

		return S_OK;
	}
	HRESULT __stdcall DragLeave() override
	{
		lastTarget = nullptr;
		tree.SetDropHilite(nullptr);
		if (lastSource != nullptr)
		{
			tree.Select(lastSource);
			lastSource = nullptr;
		}
		return S_OK;
	}

	HTREEITEM dropTarget = nullptr;

	HRESULT __stdcall Drop(IDataObject *pDataObj,DWORD grfKeyState,POINTL pt,DWORD *pdwEffect) override
	{
		if (pdwEffect == nullptr)
			return E_POINTER;
		tree.SetDropHilite(nullptr);
		*pdwEffect = DROPEFFECT_NONE;
		if (pDataObj != static_cast<IDataObject*>(this))
			return S_OK;

		POINT point = {0};
		point.x = pt.x;
		point.y = pt.y;
		::ScreenToClient(tree.GetHWND(), &point);
		auto item = tree.HitTest(point);
		if (item != nullptr && IsValidTarget(lastSource, item))
		{
			*pdwEffect = DROPEFFECT_MOVE;
			dropTarget = item;
		}

		return S_OK;
	}

	//IDataObject
	HRESULT __stdcall GetData(
		FORMATETC *pformatetcIn,
		STGMEDIUM *pmedium) override
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall GetDataHere(
		FORMATETC *pformatetc,
		STGMEDIUM *pmedium) override
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall QueryGetData(
		FORMATETC *pformatetc) override
	{
		if (pformatetc == nullptr)
			return DV_E_FORMATETC;
		if (pformatetc->lindex != -1)
			return DV_E_LINDEX;
		if (pformatetc->tymed != TYMED_HGLOBAL)
			return DV_E_TYMED;
		if (pformatetc->dwAspect != DVASPECT_CONTENT)
			return DV_E_DVASPECT;
		return S_OK;
	}

	HRESULT __stdcall GetCanonicalFormatEtc(
		FORMATETC *pformatectIn,
		FORMATETC *pformatetcOut) override
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall SetData(
		FORMATETC *pformatetc,
		STGMEDIUM *pmedium,
		WINBOOL fRelease) override
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall EnumFormatEtc(
		DWORD dwDirection,
		IEnumFORMATETC **ppenumFormatEtc) override
	{
		if (ppenumFormatEtc == nullptr)
			return E_POINTER;
		*ppenumFormatEtc = static_cast<IEnumFORMATETC*>(this);
		return S_OK;
	}

	HRESULT __stdcall DAdvise(
		FORMATETC *pformatetc,
		DWORD advf,
		IAdviseSink *pAdvSink,
		DWORD *pdwConnection) override
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall DUnadvise(
		DWORD dwConnection) override
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall EnumDAdvise(
		IEnumSTATDATA **ppenumAdvise) override
	{
		return E_NOTIMPL;
	}

	bool nextCalled = false;

	//IEnumFORMATETC
	HRESULT __stdcall Next(
		ULONG celt,
		FORMATETC *rgelt,
		ULONG *pceltFetched) override
	{
		if (celt == 0)
			return E_INVALIDARG;
		if (rgelt == nullptr)
			return E_POINTER;
		if (celt != 1 && pceltFetched == nullptr)
			return E_POINTER;
		if (nextCalled)
		{
			if (pceltFetched != nullptr)
				*pceltFetched = 0;
			return S_FALSE;
		}
		nextCalled = true;
		FORMATETC format = {0};
		format.cfFormat = CF_TEXT;
		format.ptd = nullptr;
		format.dwAspect = DVASPECT_CONTENT;
		format.lindex = -1;
		format.tymed = TYMED_HGLOBAL;
		rgelt[0] = format;
		if (pceltFetched != nullptr)
			*pceltFetched = 1;
		return S_OK;
	}

	HRESULT __stdcall Skip(
		ULONG celt) override
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Reset(
		) override
	{
		nextCalled = false;
		return S_OK;
	}

	HRESULT __stdcall Clone(
		IEnumFORMATETC **ppenum) override
	{
		return E_NOTIMPL;
	}

	bool IsValidTarget(HTREEITEM source, HTREEITEM target)
	{
		//Ignore attempts to move the project item
		if (source == tree.GetRoot())
			return false;
		//Ignore attempts to drop an item on one of its children
		for (auto parent = tree.GetParent(target); parent != nullptr; parent = tree.GetParent(parent))
			if (parent == source)
				return false;
		return true;
	}
};

void ProjectWindow::OnTreeViewBeginDrag(int id, NMTREEVIEW* hdr)
{
	auto item = hdr->itemNew.hItem;
	if (project == nullptr || item == nullptr)
		return;

	DragDropHelper helper(tree);
	IDataObject* dataObject = &helper;
	IDropSource* dropSource = &helper;
	IDropTarget* dropTarget = &helper;
	auto effect = 0ul;
	auto hr = S_OK;

	tree.Select(item);

	hr = ::RegisterDragDrop(GetHWND(), dropTarget);
	if (FAILED(hr))
		return;

	hr = ::DoDragDrop(dataObject, dropSource, DROPEFFECT_MOVE, &effect);
	if (hr == DRAGDROP_S_DROP &&
		effect == DROPEFFECT_MOVE &&
		item != nullptr &&
		helper.dropTarget != nullptr &&
		item != helper.dropTarget &&
		helper.IsValidTarget(item, helper.dropTarget))
	{
		auto param = tree.GetParam(item);
		auto projectItem = reinterpret_cast<ProjectItem*>(param);
		ProjectItemPtr projectItemPtr;

		auto parent = tree.GetParent(item);
		auto parentParam = tree.GetParam(parent);
		if (parentParam == 0)
			projectItemPtr = project->GetRootFolder().RemoveChild(projectItem);
		else
			projectItemPtr = reinterpret_cast<ProjectItemFolder*>(parentParam)->RemoveChild(projectItem);
		project->SetDirty();

		auto targetParam = tree.GetParam(helper.dropTarget);
		if (targetParam == 0)
		{
			project->GetRootFolder().AddChild(projectItemPtr);
			AddChild(projectItemPtr, helper.dropTarget);
		}
		else
		{
			auto targetProjectItem = reinterpret_cast<ProjectItem*>(targetParam);
			switch(targetProjectItem->GetType())
			{
			case ProjectItemType::Folder:
				dynamic_cast<ProjectItemFolder*>(targetProjectItem)->AddChild(projectItemPtr);
				AddChild(projectItemPtr, helper.dropTarget);
				break;
			case ProjectItemType::File:
				{
					auto targetParent = tree.GetParent(helper.dropTarget);
					auto targetParentParam = tree.GetParam(targetParent);
					if (targetParentParam == 0)
					{
						project->GetRootFolder().AddChild(projectItemPtr);
						AddChild(projectItemPtr, targetParent);
					}
					else
					{
						auto targetParentProjectItem = reinterpret_cast<ProjectItem*>(targetParentParam);
						switch(targetParentProjectItem->GetType())
						{
						case ProjectItemType::Folder:
							dynamic_cast<ProjectItemFolder*>(targetParentProjectItem)->AddChild(projectItemPtr);
							AddChild(projectItemPtr, targetParent);
							break;
						case ProjectItemType::File:
							//WTF? How did a file have children.  Well we just deleted the source...
							break;
						}
					}
				}
				break;
			}
		}
		tree.Delete(item);
	}

	::RevokeDragDrop(GetHWND());
}

void ProjectWindow::SetEvents(ProjectEvents* value)
{
	events = value;
}

void ProjectWindow::SetProject(Project& value)
{
	project = &value;
	tree.Clear();
	auto item = tree.Insert(TVI_ROOT, TVI_LAST, project->GetName(), images[IDI_APPLICATION_SMALL], 0);
	for (auto index = 0ul; index < project->GetRootFolder().GetChildrenCount(); ++index)
		AddChild(project->GetRootFolder().GetChild(index), item);
	tree.Expand(item);
}

void ProjectWindow::EnsureFileIsVisible(const std::string& fileName)
{
	if (project == nullptr)
		return;
	auto projectDirectory = FSYS::GetFilePath(project->GetFileName());
	auto callback = [&](HTREEITEM item) -> bool
	{
		auto param = tree.GetParam(item);
		if (param != 0)
		{
			auto projectItem = reinterpret_cast<ProjectItem*>(param);
			if (projectItem->GetType() == ProjectItemType::File)
			{
				auto fullPath = FSYS::FormatPath(projectDirectory, projectItem->GetName());
				if (fileName == fullPath)
				{
					tree.Select(item);
					tree.EnsureVisible(item);
					return false;
				}
			}
		}
		return true;
	};
	tree.Enumerate(callback);
}

void ProjectWindow::RenameSelection()
{
	auto item = tree.GetSelection();
	if (item == nullptr)
		return;
	tree.EditLabel(item);
}

void ProjectWindow::CreateNewFolder(const std::string& value)
{
	if (project == nullptr || value.empty())
		return;
	ProjectItemPtr folderToAdd(new ProjectItemFolder(value));
	CreateProjectItem(folderToAdd);
}

void ProjectWindow::CreateNewFile(const std::string& value)
{
	if (project == nullptr || value.empty())
		return;
	ProjectItemPtr fileToAdd(new ProjectItemFile(value));
	CreateProjectItem(fileToAdd);
}

void ProjectWindow::DeleteSelection()
{
	auto item = tree.GetSelection();
	if (project == nullptr || item == nullptr)
		return;
	auto param = tree.GetParam(item);
	if (param == 0)
		return;	//You cannot delete the project by hitting delete
	auto projectItem = reinterpret_cast<ProjectItem*>(param);
	auto parent = tree.GetParent(item);
	auto parentParam = tree.GetParam(parent);
	if (parentParam == 0)
	{
		project->GetRootFolder().RemoveChild(projectItem);
		project->SetDirty();
		tree.Delete(item);
	}
	else
	{
		auto parentProjectItem = reinterpret_cast<ProjectItem*>(parentParam);
		switch(parentProjectItem->GetType())
		{
		case ProjectItemType::Folder:
			dynamic_cast<ProjectItemFolder*>(parentProjectItem)->RemoveChild(projectItem);
			project->SetDirty();
			tree.Delete(item);
			break;
		case ProjectItemType::File:
			//WTF? Files should not have children.
			return;
		}
	}
}

ProjectItemFile* ProjectWindow::GetSelectedFile()
{
	auto item = tree.GetSelection();
	if (project == nullptr || item == nullptr)
		return nullptr;
	auto param = tree.GetParam(item);
	if (param == 0)
		return nullptr;
	auto projectItem = reinterpret_cast<ProjectItem*>(param);
	if (projectItem->GetType() != ProjectItemType::File)
		return nullptr;
	return dynamic_cast<ProjectItemFile*>(projectItem);
}

void ProjectWindow::UpdateProjectName()
{
	tree.SetText(tree.GetRoot(), project->GetName());
}

void ProjectWindow::Clear()
{
	project = nullptr;
	tree.Clear();
}

HTREEITEM ProjectWindow::AddChild(ProjectItemPtr item, HTREEITEM parent)
{
	switch(item->GetType())
	{
	case ProjectItemType::File:
		return tree.Insert(parent, TVI_LAST, FSYS::GetFileName(item->GetName()), images[IDI_FILE], reinterpret_cast<LPARAM>(item.get()));

	case ProjectItemType::Folder:
		{
			auto result = tree.Insert(parent, TVI_LAST, item->GetName(), images[IDI_FOLDER_CLOSED], reinterpret_cast<LPARAM>(item.get()));
			auto folder = dynamic_cast<const ProjectItemFolder*>(item.get());
			for (auto index = 0ul; index < folder->GetChildrenCount(); ++index)
				AddChild(folder->GetChild(index), result);
			return result;
		}
	}
	return nullptr;
}

void ProjectWindow::CreateProjectItem(ProjectItemPtr itemToAdd)
{
	if (project == nullptr)
		return;
	auto item = tree.GetSelection();
	if (item == nullptr)
		item = tree.GetRoot();
	auto param = tree.GetParam(item);
	HTREEITEM child = nullptr;
	if (param == 0)
	{
		project->GetRootFolder().AddChild(itemToAdd);
		child = AddChild(itemToAdd, item);
	}
	else
	{
		auto projectItem = reinterpret_cast<ProjectItem*>(param);
		switch(projectItem->GetType())
		{
		case ProjectItemType::Folder:
			dynamic_cast<ProjectItemFolder*>(projectItem)->AddChild(itemToAdd);
			child = AddChild(itemToAdd, item);
			break;
		case ProjectItemType::File:
			{
				auto parent = tree.GetParent(item);
				auto parentParam = tree.GetParam(parent);
				if (parentParam == 0)
				{
					project->GetRootFolder().AddChild(itemToAdd);
					child = AddChild(itemToAdd, parent);
				}
				else
				{
					auto parentProjectItem = reinterpret_cast<ProjectItem*>(parentParam);
					switch(parentProjectItem->GetType())
					{
					case ProjectItemType::Folder:
						dynamic_cast<ProjectItemFolder*>(parentProjectItem)->AddChild(itemToAdd);
						child = AddChild(itemToAdd, parent);
						break;
					case ProjectItemType::File:
						//WTF?  Files cannot be nested beneath other files...
						break;
					}
				}
			}
			break;
		}
	}
	if (child != nullptr)
	{
		project->SetDirty();
		tree.Select(child);
		tree.EnsureVisible(child);
	}
}

