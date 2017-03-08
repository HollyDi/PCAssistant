
// PCAssistantDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "ProcessDlg.h"
#include "ModuleDlg.h"
#include "SystemDlg.h"
#include "TimeDlg.h"

// CPCAssistantDlg 对话框
class CPCAssistantDlg : public CDialogEx
{
// 构造
public:
	CPCAssistantDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PCASSISTANT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	CProcessDlg*	m_ProcessDlg;
	CModuleDlg*		m_ModuleDlg;
	CSystemDlg*		m_SystemDlg;
	CTimeDlg*		m_TimeDlg;
	LRESULT SetStatusDetail(WPARAM wParam, LPARAM lParam);
	LRESULT SetStatusTip(WPARAM wParam, LPARAM lParam);
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_MainTab;
	CStatusBarCtrl* m_StatusBar;

	VOID InitTab();
	afx_msg void OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult);
};
