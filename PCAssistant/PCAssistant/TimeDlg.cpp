// TimeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PCAssistant.h"
#include "TimeDlg.h"
#include "afxdialogex.h"
#include "IOTimerFunc.h"
#include "DPCTimerFunc.h"

// CTimeDlg 对话框

enum TIME_LIST
{
	TIME_IOTIMER,
	TIME_DPCTIMER,
};

extern BOOL bIsChecking;

BOOL bNowTimeSel = 255;

extern int ResizeX;
extern int ResizeY;

IMPLEMENT_DYNAMIC(CTimeDlg, CDialogEx)

CTimeDlg::CTimeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_TIME, pParent)
{

}

CTimeDlg::~CTimeDlg()
{
}

void CTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TIME, m_ListTime);
	DDX_Control(pDX, IDC_LIST_TIME_CTRL, m_ListTimeCtrl);
}


BEGIN_MESSAGE_MAP(CTimeDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_LBN_SELCHANGE(IDC_LIST_TIME, &CTimeDlg::OnLbnSelchangeListTime)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_TIME_CTRL, &CTimeDlg::OnNMRClickListTimeCtrl)
	ON_COMMAND(ID_MENU_IOTIMER_REFRESH, &CTimeDlg::OnMenuIotimerRefresh)
	ON_COMMAND(ID_MENU_IOTIMER_OPER, &CTimeDlg::OnMenuIotimerOper)
	ON_COMMAND(ID_MENU_IOTIMER_DELETE, &CTimeDlg::OnMenuIotimerDelete)
	ON_WM_SIZE()
	ON_COMMAND(ID_MENU_DPCTIMER_REFRESH, &CTimeDlg::OnMenuDpctimerRefresh)
	ON_COMMAND(ID_MENU_DPCTIMER_DELETE, &CTimeDlg::OnMenuDpctimerDelete)
END_MESSAGE_MAP()


// CTimeDlg 消息处理程序


BOOL CTimeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitTimeList();
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

VOID CTimeDlg::InitTimeList()
{
	m_ListTime.InsertString(TIME_IOTIMER, L"IOPTimer");
	m_ListTime.InsertString(TIME_DPCTIMER, L"DPCTimer");
}


void CTimeDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow == TRUE)
	{
		SendStatusDetail(L"显示定时器相关信息。");
		SendStatusTip(L"定时器");

		m_ListTime.SetCurSel(0);

		bNowTimeSel = 255;

		m_ListTime.MoveWindow(0, 5, 99, 180);
		m_ListTimeCtrl.MoveWindow(100, 0, ResizeX, ResizeY);

		OnLbnSelchangeListTime();

		m_ListTime.SetFocus();
	}
}


void CTimeDlg::OnLbnSelchangeListTime()
{
	int nCurSel = m_ListTime.GetCurSel();

	switch (nCurSel)
	{
	case TIME_IOTIMER:
	{
		if (bIsChecking == TRUE || bNowTimeSel == TIME_IOTIMER)	//
		{
			m_ListTime.SetCurSel(bNowTimeSel);
			m_ListTimeCtrl.SetFocus();
			return;
		}

		bNowTimeSel = nCurSel;

		InitIOTimerList(&m_ListTimeCtrl);

		CloseHandle(
			CreateThread(NULL, 0,
			(LPTHREAD_START_ROUTINE)LoadIOTimerList, &m_ListTimeCtrl, 0, NULL)
		);

		break;
	}
	case TIME_DPCTIMER:
	{
		if (bIsChecking == TRUE || bNowTimeSel == TIME_DPCTIMER)	//
		{
			m_ListTime.SetCurSel(bNowTimeSel);
			m_ListTimeCtrl.SetFocus();
			return;
		}

		bNowTimeSel = nCurSel;

		InitDPCTimerList(&m_ListTimeCtrl);

		CloseHandle(
			CreateThread(NULL, 0,
			(LPTHREAD_START_ROUTINE)LoadDPCTimerList, &m_ListTimeCtrl, 0, NULL)
		);

		break;
	}
	}
}


void CTimeDlg::OnNMRClickListTimeCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CMenu	popup;
	CPoint	p;
	switch (bNowTimeSel)
	{
	case TIME_IOTIMER:
	{
		popup.LoadMenu(IDR_MENU_IOTIMER);			//加载菜单资源
		CMenu*	pM = popup.GetSubMenu(0);				//获得菜单的子项

		GetCursorPos(&p);
		int	count = pM->GetMenuItemCount();
		if (m_ListTimeCtrl.GetSelectedCount() == 0)		//如果没有选中
		{
			for (int i = 1; i<count; i++)
			{
				pM->EnableMenuItem(i, MF_BYPOSITION | MF_DISABLED | MF_GRAYED); //菜单全部变灰
			}
		}

		POSITION pos = m_ListTimeCtrl.GetFirstSelectedItemPosition();

		while (pos)
		{
			int nItem = m_ListTimeCtrl.GetNextSelectedItem(pos);

			if (_wcsnicmp(L"运行", m_ListTimeCtrl.GetItemText(nItem, 2), wcslen(L"运行")) == 0)
			{
				pM->ModifyMenuW(ID_MENU_IOTIMER_OPER, MF_BYCOMMAND, ID_MENU_IOTIMER_OPER, L"停止");
			}
			else if (_wcsnicmp(L"停止", m_ListTimeCtrl.GetItemText(nItem, 2), wcslen(L"停止")) == 0)
			{
				pM->ModifyMenuW(ID_MENU_IOTIMER_OPER, MF_BYCOMMAND, ID_MENU_IOTIMER_OPER, L"运行");
			}
		}

		pM->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);
		break;
		break;
	}
	case TIME_DPCTIMER:
	{
		break;
	}
	}


	*pResult = 0;
}


void CTimeDlg::OnMenuIotimerRefresh()
{
	m_ListTime.SetCurSel(TIME_IOTIMER);
	bNowTimeSel = 255;
	OnLbnSelchangeListTime();
}


void CTimeDlg::OnMenuIotimerOper()
{
	if (bIsChecking)
	{
		return;
	}

	bIsChecking = TRUE;

	CloseHandle(
		CreateThread(NULL, 0,
		(LPTHREAD_START_ROUTINE)OperIOTimer, &m_ListTimeCtrl, 0, NULL)
	);
}


void CTimeDlg::OnMenuIotimerDelete()
{
	// TODO: 在此添加命令处理程序代码
}


void CTimeDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	ResizeX = cx;
	ResizeY = cy;
}


void CTimeDlg::OnMenuDpctimerRefresh()
{
	m_ListTime.SetCurSel(TIME_DPCTIMER);
	bNowTimeSel = 255;
	OnLbnSelchangeListTime();
}


void CTimeDlg::OnMenuDpctimerDelete()
{
	BOOL bRet = MessageBox(L"删除DPCTimer操作可能会造成系统异常或崩溃，\r\n请在确认后继续。", L"PCAssistent", MB_ICONWARNING | MB_OKCANCEL);

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
		(LPTHREAD_START_ROUTINE)RemoveDPCTimerItem, &m_ListTimeCtrl, 0, NULL)
	);
}
