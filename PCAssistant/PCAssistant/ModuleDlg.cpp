// ModuleDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PCAssistant.h"
#include "ModuleDlg.h"
#include "afxdialogex.h"

#include "PCAssistantDlg.h"
#include "Common.h"
#include "KModuleFunc.h"

// CModuleDlg 对话框
extern int ResizeX;
extern int ResizeY;

extern BOOL bIsChecking;

IMPLEMENT_DYNAMIC(CModuleDlg, CDialogEx)

CModuleDlg::CModuleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MODULE, pParent)
{
	m_wParent = pParent;
}

CModuleDlg::~CModuleDlg()
{
}

void CModuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MODULE, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CModuleDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_MODULE, &CModuleDlg::OnNMRClickListModule)
	ON_COMMAND(ID_MENU_MODULE_REMOVE, &CModuleDlg::OnMenuModuleRemove)
	ON_COMMAND(ID_MENU_MODULE_REFRESH, &CModuleDlg::OnMenuModuleRefresh)
END_MESSAGE_MAP()


// CModuleDlg 消息处理程序


BOOL CModuleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitModuleList(&m_ListCtrl);
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CModuleDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow == TRUE)
	{
		m_ListCtrl.MoveWindow(0, 0, ResizeX, ResizeY);

		LoadModuleList();

		SendStatusTip(L"驱动模块");

		m_ListCtrl.SetFocus();
	}
}


void CModuleDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	ResizeX = cx;
	ResizeY = cy;
}

VOID CModuleDlg::LoadModuleList()
{
	if (bIsChecking == TRUE)
	{
		return;
	}

	bIsChecking = TRUE;

	m_ListCtrl.DeleteAllItems();

	m_ListCtrl.SetSelectedColumn(-1);

	SendStatusDetail(L"模块列表正在加载");

	CloseHandle(
		CreateThread(NULL, 0,
		(LPTHREAD_START_ROUTINE)QueryModuleFunction, &m_ListCtrl, 0, NULL)
	);

	return ;
}




void CModuleDlg::OnNMRClickListModule(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	CMenu	popup;
	CPoint	p;

	popup.LoadMenu(IDR_MENU_MODULE);				//加载菜单资源
	CMenu*	pM = popup.GetSubMenu(0);				//获得菜单的子项

	GetCursorPos(&p);
	int	count = pM->GetMenuItemCount();
	if (m_ListCtrl.GetSelectedCount() == 0)		//如果没有选中
	{
		for (int i = 0; i<count; i++)
		{
			pM->EnableMenuItem(i, MF_BYPOSITION | MF_DISABLED | MF_GRAYED); //菜单全部变灰
		}
	}

	int Index = m_ListCtrl.GetSelectionMark();

	if (Index >= 0)
	{
		if (_wcsicmp(L"-", m_ListCtrl.GetItemText(Index, 3)) == 0)
		{
			pM->EnableMenuItem(ID_MENU_MODULE_REMOVE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
	}

	pM->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);

	*pResult = 0;
}


void CModuleDlg::OnMenuModuleRemove()
{
	BOOL bRet = MessageBox(L"卸载模块操作极可能会造成系统异常或崩溃，\r\n请在确认后继续。", L"天影卫士", MB_ICONWARNING | MB_OKCANCEL);

	if (bRet == IDCANCEL)
	{
		return;
	}

	if (bIsChecking == TRUE)
	{
		return;
	}

	bIsChecking = TRUE;

	CloseHandle(
		CreateThread(NULL, 0,
		(LPTHREAD_START_ROUTINE)RemoveDriverModule, &m_ListCtrl, 0, NULL)
	);
}


void CModuleDlg::OnMenuModuleRefresh()
{
	LoadModuleList();
}
