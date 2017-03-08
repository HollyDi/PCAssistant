#pragma once
#include "D:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\atlmfc\include\afxwin.h"
#include "D:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\atlmfc\include\afxcmn.h"

#include "Common.h"
// CTimeDlg 对话框

class CTimeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTimeDlg)

public:
	CTimeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTimeDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TIME };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_ListTime;
	CListCtrl m_ListTimeCtrl;
	virtual BOOL OnInitDialog();
	VOID InitTimeList();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLbnSelchangeListTime();
	afx_msg void OnNMRClickListTimeCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMenuIotimerRefresh();
	afx_msg void OnMenuIotimerOper();
	afx_msg void OnMenuIotimerDelete();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMenuDpctimerRefresh();
	afx_msg void OnMenuDpctimerDelete();
};
