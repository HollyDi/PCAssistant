#pragma once
#include "D:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\atlmfc\include\afxcmn.h"


// CProcessDlg 对话框

class CProcessDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProcessDlg)

public:
	CProcessDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProcessDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PROCESS };
#endif



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	VOID InitList();
	VOID LoadProcessList();

	LRESULT ProcessDlgSendInsert(WPARAM wParam, LPARAM lParam);

	CListCtrl m_ListCtrl;

	
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnMenuProcessFilelocation();
	afx_msg void OnNMRClickListProcess(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMenuProcessRefresh();
	afx_msg void OnMenuProcessKill();
	afx_msg void OnMenuProcessKillmust();
	afx_msg void OnMenuProcessCopyinfo();
	afx_msg void OnMenuNewRun();
	afx_msg void OnMenuProcessAttribute();
	afx_msg void OnMenuProcessDetail();
	void OpenProcessViewDlg(int nViewType);
	afx_msg void OnMenuProcessThread();
	afx_msg void OnMenuProcessPrivilege();
	afx_msg void OnMenuProcessHandle();
	afx_msg void OnMenuProcessWindow();
	afx_msg void OnMenuProcessModule();
	afx_msg void OnMenuProcessMemory();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
