// ProcessDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PCAssistant.h"
#include "ProcessDlg.h"
#include "afxdialogex.h"
#include "ProcessFunc.h"
#include "Common.h"

#include "ProcessViewDlg.h"

// CProcessDlg 对话框
int ResizeX;
int ResizeY;

IMPLEMENT_DYNAMIC(CProcessDlg, CDialogEx)

extern BOOL bIsChecking;
extern CWnd* g_wParent;
CWnd* g_process = NULL;

COLUMN_STRUCT g_Column_Process[] =
{
	{ L"映像名称",		160 },
	{ L"进程ID",		65 },
	{ L"父进程ID",		65 },
	{ L"映像路径",		230 },
	{ L"EPROCESS",		125 },
	{ L"应用层访问",	75 },
	{ L"文件厂商",		122 }
};

UINT g_Column_Process_Count = 7;	  //进程列表列数

CProcessDlg::CProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_PROCESS, pParent)
{

}

CProcessDlg::~CProcessDlg()
{
}

void CProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CProcessDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(PROCESSDIG_SEND_INSERT, ProcessDlgSendInsert)
	ON_COMMAND(ID_MENU_PROCESS_FILELOCATION, &CProcessDlg::OnMenuProcessFilelocation)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PROCESS, &CProcessDlg::OnNMRClickListProcess)
	ON_COMMAND(ID_MENU_PROCESS_REFRESH, &CProcessDlg::OnMenuProcessRefresh)
	ON_COMMAND(ID_MENU_PROCESS_KILL, &CProcessDlg::OnMenuProcessKill)
	ON_COMMAND(ID_MENU_PROCESS_KILLMUST, &CProcessDlg::OnMenuProcessKillmust)
	ON_COMMAND(ID_MENU_PROCESS_COPYINFO, &CProcessDlg::OnMenuProcessCopyinfo)
	ON_COMMAND(ID_MENU_NEW_RUN, &CProcessDlg::OnMenuNewRun)
	ON_COMMAND(ID_MENU_PROCESS_ATTRIBUTE, &CProcessDlg::OnMenuProcessAttribute)
	ON_COMMAND(ID_MENU_PROCESS_DETAIL, &CProcessDlg::OnMenuProcessDetail)
	ON_COMMAND(ID_MENU_PROCESS_THREAD, &CProcessDlg::OnMenuProcessThread)
	ON_COMMAND(ID_MENU_PROCESS_PRIVILEGE, &CProcessDlg::OnMenuProcessPrivilege)
	ON_COMMAND(ID_MENU_PROCESS_HANDLE, &CProcessDlg::OnMenuProcessHandle)
	ON_COMMAND(ID_MENU_PROCESS_WINDOW, &CProcessDlg::OnMenuProcessWindow)
	ON_COMMAND(ID_MENU_PROCESS_MODULE, &CProcessDlg::OnMenuProcessModule)
	ON_COMMAND(ID_MENU_PROCESS_MEMORY, &CProcessDlg::OnMenuProcessMemory)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CProcessDlg 消息处理程序


BOOL CProcessDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	g_process = this;
	InitList();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

VOID CProcessDlg::InitList()
{
	CRect Rect;
	this->GetClientRect(&Rect);
	m_ListCtrl.MoveWindow(Rect);
	m_ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);

	UINT i;
	for (i = 0; i<g_Column_Process_Count; i++)
	{
		m_ListCtrl.InsertColumn(i, g_Column_Process[i].szTitle, LVCFMT_LEFT, g_Column_Process[i].nWidth);
	}

}

void CProcessDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow == TRUE)
	{
		m_ListCtrl.MoveWindow(0, 0, ResizeX, ResizeY);

		LoadProcessList();

		SendStatusTip(L"进程");
	}
}

VOID CProcessDlg::LoadProcessList()
{
	if (bIsChecking == FALSE)
	{
		bIsChecking = TRUE;

		m_ListCtrl.DeleteAllItems();
		m_ListCtrl.SetSelectedColumn(-1);

		SendStatusDetail(L"进程列表正在加载。");

		CreateThread(NULL, 0,
			(LPTHREAD_START_ROUTINE)QueryProcessFunction, this, 0, NULL);
	}
}

LRESULT CProcessDlg::ProcessDlgSendInsert(WPARAM wParam, LPARAM lParam)
{
	PPROCESSINFO* hsProcItem = (PPROCESSINFO*)lParam;

	CString Name = NULL;
	CString Pid = NULL;
	CString PPid = NULL;
	CString Path = NULL;
	CString EProcess = NULL;
	CString UserAccess = NULL;
	CString CompanyName = NULL;

	ULONG ulItem = m_ListCtrl.GetItemCount();

	WCHAR tempdir[100];
	GetEnvironmentVariableW(L"windir", tempdir, 100);


	Name = (*hsProcItem)->Name;
	Pid.Format(L"%d", (*hsProcItem)->Pid);

	if ((*hsProcItem)->PPid == 0xffffffff)
	{
		PPid = L"-";
	}
	else
	{
		PPid.Format(L"%d", (*hsProcItem)->PPid);
	}

	Path = (*hsProcItem)->Path;

	EProcess.Format(L"0x%p", (*hsProcItem)->Eprocess);

	if ((*hsProcItem)->UserAccess == TRUE)
	{
		UserAccess = L"允许";
	}
	else
	{
		UserAccess = L"拒绝";
	}

	CompanyName = (*hsProcItem)->CompanyName;

	//AddProcessFileIcon(Path.GetBuffer());

	m_ListCtrl.InsertItem(ulItem, Name, ulItem);
	m_ListCtrl.SetItemText(ulItem, PROCESS_COLUMN_PID, Pid);
	m_ListCtrl.SetItemText(ulItem, PROCESS_COLUMN_PPID, PPid);
	m_ListCtrl.SetItemText(ulItem, PROCESS_COLUMN_PATH, Path);
	m_ListCtrl.SetItemText(ulItem, PROCESS_COLUMN_EPROCESS, EProcess);
	m_ListCtrl.SetItemText(ulItem, PROCESS_COLUMN_USERACCESS, UserAccess);
	m_ListCtrl.SetItemText(ulItem, PROCESS_COLUMN_COMPANY, CompanyName);

	/*
	CToolTipCtrl ToolTipCtrl;
	ToolTipCtrl.Create(this);
	m_ListCtrl.SetToolTips(&ToolTipCtrl);
	*/

	return TRUE;
}



void CProcessDlg::OnMenuProcessFilelocation()
{
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();

	while (pos)
	{
		int iItem = m_ListCtrl.GetNextSelectedItem(pos);

		CString csFilePath = m_ListCtrl.GetItemText(iItem, PROCESS_COLUMN_PATH);

		LocationExplorer(csFilePath);
	}
}


void CProcessDlg::OnNMRClickListProcess(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CMenu	popup;
	popup.LoadMenu(IDR_MENU_PROCESS);				//加载菜单资源
	CMenu*	pM = popup.GetSubMenu(0);				//获得菜单的子项
	CPoint	p;
	GetCursorPos(&p);
	int	count = pM->GetMenuItemCount();
	if (m_ListCtrl.GetSelectedCount() == 0)		//如果没有选中
	{
		for (int i = 0; i<count; i++)
		{
			pM->EnableMenuItem(i, MF_BYPOSITION | MF_DISABLED | MF_GRAYED); //菜单全部变灰
		}

		pM->EnableMenuItem(ID_MENU_PROCESS_REFRESH, MF_BYCOMMAND | MF_ENABLED);
	}

	//pM->SetDefaultItem(ID_MENU_PROCESS_DETAIL, FALSE);

	pM->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);

	*pResult = 0;
}


void CProcessDlg::OnMenuProcessRefresh()
{
	LoadProcessList();
}


void CProcessDlg::OnMenuProcessKill()
{
	BOOL bIsOK = MessageBox(L"确定要结束该进程吗？", L"天影卫士", MB_YESNO);

	if (bIsOK == IDYES)
	{
		POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();

		while (pos)
		{
			int nItem = m_ListCtrl.GetNextSelectedItem(pos);

			DWORD_PTR ulPid = _ttoi(m_ListCtrl.GetItemText(nItem, PROCESS_COLUMN_PID).GetBuffer());

			if (ulPid == 0 || ulPid == 4)
			{
				MessageBox(L"进程关闭失败。", L"天影卫士", 0);
				return;
			}

			DebugPrivilege(SE_DEBUG_NAME, TRUE);

			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | PROCESS_VM_OPERATION, TRUE, (DWORD)ulPid);

			BOOL bIsSuccess = TerminateProcess(hProcess, 0);

			DebugPrivilege(SE_DEBUG_NAME, FALSE);

			if (bIsSuccess == FALSE)
			{
				MessageBox(L"关闭进程失败。", L"天影卫士", 0);
			}
			else
			{
				m_ListCtrl.DeleteItem(nItem);
			}

			CloseHandle(hProcess);
		}

	}
}


void CProcessDlg::OnMenuProcessKillmust()
{
	if (bIsChecking)
	{
		return;
	}

	if (MessageBox(L"强制关闭进程的操作有风险，请谨慎操作。\r\n确定要结束该进程吗？", L"PCAssistant", MB_YESNO) == IDNO)
	{
		return;
	}

	// 	CreateThread(NULL,0, 
	// 		(LPTHREAD_START_ROUTINE)HsKillProcessByForce,(CMyList*)&m_ProcessList, 0,NULL);

	bIsChecking = TRUE;

	KillProcessByForce((CListCtrl*)&m_ListCtrl);

	bIsChecking = FALSE;
}


void CProcessDlg::OnMenuProcessCopyinfo()
{
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();

	while (pos)
	{
		int iItem = m_ListCtrl.GetNextSelectedItem(pos);

		CStringA(csProcInfo);
		csProcInfo = "映像名称: ";
		csProcInfo += m_ListCtrl.GetItemText(iItem, PROCESS_COLUMN_NAME);
		csProcInfo += "  进程ID: ";
		csProcInfo += m_ListCtrl.GetItemText(iItem, PROCESS_COLUMN_PID);
		csProcInfo += "  父进程ID: ";
		csProcInfo += m_ListCtrl.GetItemText(iItem, PROCESS_COLUMN_PPID);
		csProcInfo += "  映像路径: ";
		csProcInfo += m_ListCtrl.GetItemText(iItem, PROCESS_COLUMN_PATH);
		csProcInfo += "  EPROCESS: ";
		csProcInfo += m_ListCtrl.GetItemText(iItem, PROCESS_COLUMN_EPROCESS);
		csProcInfo += "  应用层访问: ";
		csProcInfo += m_ListCtrl.GetItemText(iItem, PROCESS_COLUMN_USERACCESS);
		csProcInfo += "  文件厂商: ";
		if (m_ListCtrl.GetItemText(iItem, PROCESS_COLUMN_COMPANY).GetLength()>0)
		{
			csProcInfo += m_ListCtrl.GetItemText(iItem, PROCESS_COLUMN_COMPANY);
		}

		if (::OpenClipboard(NULL))
		{
			HGLOBAL hmem = GlobalAlloc(GHND, csProcInfo.GetLength() + 1);
			char *pmem = (char*)GlobalLock(hmem);

			EmptyClipboard();
			memcpy(pmem, csProcInfo.GetBuffer(), csProcInfo.GetLength() + 1);
			SetClipboardData(CF_TEXT, hmem);
			CloseClipboard();
			GlobalFree(hmem);
		}
	}
}


void CProcessDlg::OnMenuNewRun()
{

	WCHAR wzFileFilter[] = L"应用程序 (*.exe)\0*.exe\0";
	WCHAR wzFileChoose[] = L"打开文件";


	CFileDialog FileDlg(TRUE);
	FileDlg.m_ofn.lpstrTitle = wzFileChoose;
	FileDlg.m_ofn.lpstrFilter = wzFileFilter;

	if (IDOK != FileDlg.DoModal())
	{
		return;
	}

	CString ExePath = FileDlg.GetPathName();

	ShellExecuteW(NULL, L"open", ExePath, L"", L"", SW_SHOW);
}


void CProcessDlg::OnMenuProcessAttribute()
{
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();

	while (pos)
	{
		int iItem = m_ListCtrl.GetNextSelectedItem(pos);

		CString csFilePath = m_ListCtrl.GetItemText(iItem, PROCESS_COLUMN_PATH);

		CheckAttribute(csFilePath);
	}
}


void CProcessDlg::OnMenuProcessDetail()
{
	OpenProcessViewDlg(PROCVIEW_TYPE_DETAIL);
}


void CProcessDlg::OnMenuProcessThread()
{
	OpenProcessViewDlg(PROCVIEW_TYPE_THREAD);
}


void CProcessDlg::OnMenuProcessPrivilege()
{
	OpenProcessViewDlg(PROCVIEW_TYPE_PRIVILEGE);
}


void CProcessDlg::OnMenuProcessHandle()
{
	OpenProcessViewDlg(PROCVIEW_TYPE_HANDLE);
}


void CProcessDlg::OnMenuProcessWindow()
{
	OpenProcessViewDlg(PROCVIEW_TYPE_HANDLE);
}


void CProcessDlg::OnMenuProcessModule()
{
	OpenProcessViewDlg(PROCVIEW_TYPE_MODULE);
}


void CProcessDlg::OnMenuProcessMemory()
{
	OpenProcessViewDlg(PROCVIEW_TYPE_MEMORY);
}

void CProcessDlg::OpenProcessViewDlg(int nViewType)
{
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();

	while (pos)
	{
		
		int nItem = m_ListCtrl.GetNextSelectedItem(pos);

		ULONG_PTR ulPid = _ttoi(m_ListCtrl.GetItemText(nItem, PROCESS_COLUMN_PID).GetBuffer());
		ULONG_PTR ulPPid = _ttoi(m_ListCtrl.GetItemText(nItem, PROCESS_COLUMN_PPID).GetBuffer());
		
		CString ProcessEProcess = m_ListCtrl.GetItemText(nItem, PROCESS_COLUMN_EPROCESS);

		ProcessEProcess = ProcessEProcess.GetBuffer() + 2;

		ULONG_PTR ulEProcess = 0;
		
		//MessageBox(ProcessEProcess, NULL, 0);
		//swscanf_s(ProcessEProcess.GetBuffer(), L"%P", &ulEProcess);

		
		PROCESSINFO hsProcInfo = { 0 };

		hsProcInfo.Pid = ulPid;
		hsProcInfo.PPid = ulPPid;
		hsProcInfo.Eprocess = ulEProcess;

		if (_wcsnicmp(m_ListCtrl.GetItemText(nItem, PROCESS_COLUMN_COMPANY).GetBuffer(), L"拒绝", wcslen(L"拒绝")) == 0)
		{
			hsProcInfo.UserAccess = FALSE;
		}
		else
		{
			hsProcInfo.UserAccess = TRUE;
		}
		
		StringCchCopyW(hsProcInfo.CompanyName,
			m_ListCtrl.GetItemText(nItem, PROCESS_COLUMN_COMPANY).GetLength() + 1,
			m_ListCtrl.GetItemText(nItem, PROCESS_COLUMN_COMPANY).GetBuffer());
		StringCchCopyW(hsProcInfo.Name,
			m_ListCtrl.GetItemText(nItem, PROCESS_COLUMN_NAME).GetLength() + 1,
			m_ListCtrl.GetItemText(nItem, PROCESS_COLUMN_NAME).GetBuffer());
		StringCchCopyW(hsProcInfo.Path,
			m_ListCtrl.GetItemText(nItem, PROCESS_COLUMN_PATH).GetLength() + 1,
			m_ListCtrl.GetItemText(nItem, PROCESS_COLUMN_PATH).GetBuffer());

		CProcessViewDlg* dlg = new CProcessViewDlg(nViewType, &hsProcInfo, this);
		dlg->DoModal();
	}
}

void CProcessDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	ResizeX = cx;
	ResizeY = cy;
}
