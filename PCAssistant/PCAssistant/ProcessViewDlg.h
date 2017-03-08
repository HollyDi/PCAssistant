#pragma once
#include "stdafx.h"
#include "ProcessFunc.h"

enum HS_ENUM_PROCVIEW_TYPE
{
	PROCVIEW_TYPE_DETAIL,
	PROCVIEW_TYPE_THREAD,
	PROCVIEW_TYPE_PRIVILEGE,
	PROCVIEW_TYPE_HANDLE,
	PROCVIEW_TYPE_WINDOW,
	PROCVIEW_TYPE_MODULE,
	PROCVIEW_TYPE_MEMORY
};
// CProcessViewDlg 对话框

class CProcessViewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProcessViewDlg)

public:
	CProcessViewDlg(ULONG_PTR ulViewType, PPROCESSINFO piProcInfo, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProcessViewDlg();

	enum { IDD = IDD_DIALOG_PROCESSVIEW };

	PPROCESSINFO m_piProcInfo;		// 所属进程信息
	CWnd* m_wParent;				// 父窗口指针
	ULONG_PTR m_ulViewType;			// 窗口显示类型

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListBox m_ListType;
	CListCtrl m_ListCtrl;
	afx_msg void OnLbnSelchangeListProcessviewtype();
	afx_msg void OnMenuProcessThreadRefresh();
	afx_msg void OnMenuProcessThreadClose();
};
