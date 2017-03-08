
// PCAssistantDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PCAssistant.h"
#include "PCAssistantDlg.h"
#include "afxdialogex.h"
#include "Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern HANDLE g_hDevice;

CWnd* g_wParent = NULL;
BOOL bIsChecking = FALSE; //当前的检查状态

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPCAssistantDlg 对话框



CPCAssistantDlg::CPCAssistantDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PCASSISTANT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_ProcessDlg = NULL;
	m_ModuleDlg	= NULL;
	m_SystemDlg = NULL;
	m_TimeDlg = NULL;
}

void CPCAssistantDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_MAIN, m_MainTab);
}



BEGIN_MESSAGE_MAP(CPCAssistantDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(MESSAGE_STATUSDETAIL, SetStatusDetail)
	ON_MESSAGE(MESSAGE_STATUSTIP, SetStatusTip)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAIN, &CPCAssistantDlg::OnTcnSelchangeTabMain)
END_MESSAGE_MAP()


// CPCAssistantDlg 消息处理程序

BOOL CPCAssistantDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	g_wParent = this;

	//添加状态栏
	m_StatusBar = new CStatusBarCtrl;

	m_StatusBar->Create(WS_CHILD | WS_VISIBLE | SBT_OWNERDRAW, CRect(0, 0, 0, 0), this, 0);

	int strPartDim[3] = { 745,845,0 };
	m_StatusBar->SetParts(3, strPartDim);

	m_StatusBar->SetText(L"准备就绪。", 0, 0);
	m_StatusBar->SetText(L"", 1, 0);

	InitTab();

	if (m_ProcessDlg == NULL)
	{
		m_ProcessDlg = new CProcessDlg(this);
		m_ProcessDlg->Create(IDD_DIALOG_PROCESS, GetDlgItem(IDC_TAB_MAIN));

		CRect Rect;
		m_MainTab.GetClientRect(&Rect);
		Rect.top += 21;
		Rect.left += 1;
		Rect.right -= 1;
		Rect.bottom -= 3;
		m_ProcessDlg->MoveWindow(&Rect);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPCAssistantDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPCAssistantDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPCAssistantDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

VOID CPCAssistantDlg::InitTab()
{
	m_MainTab.InsertItem(0, L"进程");
	m_MainTab.InsertItem(1, L"驱动模块");
	m_MainTab.InsertItem(2, L"内核");
	m_MainTab.InsertItem(3, L"定时器");
	return;
}




void CPCAssistantDlg::OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	int iSelect = m_MainTab.GetCurSel();

	switch (iSelect)
	{
	case DIALOG_PROCESS:
	{
		if (m_ProcessDlg == NULL)
		{
			m_ProcessDlg = new CProcessDlg(this);
			m_ProcessDlg->Create(IDD_DIALOG_PROCESS, GetDlgItem(IDC_TAB_MAIN));

			CRect Rect;
			m_MainTab.GetClientRect(&Rect);
			Rect.top += 21;
			Rect.left += 1;
			Rect.right -= 1;
			Rect.bottom -= 3;
			m_ProcessDlg->MoveWindow(&Rect);
		}
		if (m_ProcessDlg)
			m_ProcessDlg->ShowWindow(TRUE);
		if (m_ModuleDlg)
			m_ModuleDlg->ShowWindow(FALSE);
		if (m_SystemDlg)
			m_SystemDlg->ShowWindow(FALSE);

		break;
	}
	case DIALOG_MOUDLE:
	{
		if (m_ModuleDlg == NULL)
		{
			m_ModuleDlg = new CModuleDlg(this);
			m_ModuleDlg->Create(IDD_DIALOG_MODULE, GetDlgItem(IDC_TAB_MAIN));

			CRect Rect;
			m_MainTab.GetClientRect(&Rect);
			Rect.top += 21;
			Rect.left += 1;
			Rect.right -= 1;
			Rect.bottom -= 3;
			m_ModuleDlg->MoveWindow(&Rect);
		}
		if (m_ModuleDlg)
			m_ModuleDlg->ShowWindow(TRUE);
		if (m_ProcessDlg)
			m_ProcessDlg->ShowWindow(FALSE);
		if (m_SystemDlg)
			m_SystemDlg->ShowWindow(FALSE);
		if (m_TimeDlg)
			m_TimeDlg->ShowWindow(FALSE);
		break;
		
	}
	case DIALOG_SYSTEM:
	{
		if (m_SystemDlg == NULL)
		{
			m_SystemDlg = new CSystemDlg(this);
			m_SystemDlg->Create(IDD_DIALOG_SYSTEM, GetDlgItem(IDC_TAB_MAIN));

			CRect Rect;
			m_MainTab.GetClientRect(&Rect);
			Rect.top += 21;
			Rect.left += 1;
			Rect.right -= 1;
			Rect.bottom -= 3;
			m_SystemDlg->MoveWindow(&Rect);
		}
		if (m_SystemDlg)
			m_SystemDlg->ShowWindow(TRUE);
		if (m_ModuleDlg)
			m_ModuleDlg->ShowWindow(FALSE);
		if (m_ProcessDlg)
			m_ProcessDlg->ShowWindow(FALSE);
		if (m_TimeDlg)
			m_TimeDlg->ShowWindow(FALSE);

		break;
	}
	case DIALOG_TIME:
	{
		if (m_TimeDlg == NULL)
		{
			m_TimeDlg = new CTimeDlg(this);
			m_TimeDlg->Create(IDD_DIALOG_TIME, GetDlgItem(IDC_TAB_MAIN));

			CRect Rect;
			m_MainTab.GetClientRect(&Rect);
			Rect.top += 21;
			Rect.left += 1;
			Rect.right -= 1;
			Rect.bottom -= 3;
			m_TimeDlg->MoveWindow(&Rect);
		}
		if (m_TimeDlg)
			m_TimeDlg->ShowWindow(TRUE);
		if (m_SystemDlg)
			m_SystemDlg->ShowWindow(FALSE);
		if (m_ModuleDlg)
			m_ModuleDlg->ShowWindow(FALSE);
		if (m_ProcessDlg)
			m_ProcessDlg->ShowWindow(FALSE);
		

		break;
	}
	default:
		break;
	}

	*pResult = 0;
}


LRESULT CPCAssistantDlg::SetStatusDetail(WPARAM wParam, LPARAM lParam)
{
	CString recvstr = (LPCTSTR)lParam;

	m_StatusBar->SetText(recvstr.GetBuffer(), 0, 0);
	return TRUE;
}

LRESULT CPCAssistantDlg::SetStatusTip(WPARAM wParam, LPARAM lParam)
{
	CString recvstr = (LPCTSTR)lParam;

	m_StatusBar->SetText(recvstr.GetBuffer(), 1, 0);
	return TRUE;
}
