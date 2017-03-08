#pragma once
#include "stdafx.h"

typedef struct _SSDT_INFO
{
	ULONG_PTR	Id;
	ULONG_PTR	CurAddr;
	ULONG_PTR	OriAddr;
	CHAR	    szFuncName[80];
}SSDT_INFO, *PSSDT_INFO;


VOID InitSSDTList(CListCtrl *m_ListCtrl);
VOID LoadSSDTList(CListCtrl *m_ListCtrl);
VOID QuerySSDTList(CListCtrl *m_ListCtrl);
BOOL InitDataOfSSDT();
BOOL GetNtosNameAndBase();
BOOL GetNtosImageBase();
BOOL GetTempNtosName();
BOOL GetKiServiceTable();
BOOL EnumSSDT(SSDT_INFO* si);
DWORD GetSSDTFunctionIndex(char *FunctionName);
ULONG_PTR GetFunctionOriginalAddress(ULONG_PTR dwIndex);
BOOL SendIoCodeSSDT(DWORD dwFuncIndex, PULONG_PTR SSDTCurAddr);
DWORD GetSpecialIndex(char *FunctionName);
BOOL EnumDriver();
CString GetDriverPath(ULONG_PTR Address);
