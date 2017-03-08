#pragma once
#include "stdafx.h"

typedef struct _WND_INFO_
{
	HWND  hWnd;
	ULONG uPid;
	ULONG uTid;
}WND_INFO, *PWND_INFO;

typedef struct _ALL_WNDS_
{
	ULONG nCnt;
	WND_INFO WndInfo[1];
}ALL_WNDS, *PALL_WNDS;

void InitWindowList(CListCtrl *m_ListCtrl);
VOID QueryProcessWindow(CListCtrl *m_ListCtrl);
void AddWndItem(WND_INFO WndInfor, BOOL bAll, CListCtrl *m_ListCtrl);
