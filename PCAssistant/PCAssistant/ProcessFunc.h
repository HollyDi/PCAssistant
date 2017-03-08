#pragma once
#include "stdafx.h"
#include "Common.h"

enum ENUM_PROC_COLUMN	//进程列表列枚举
{
	PROCESS_COLUMN_NAME = 0,
	PROCESS_COLUMN_PID,
	PROCESS_COLUMN_PPID,
	PROCESS_COLUMN_PATH,
	PROCESS_COLUMN_EPROCESS,
	PROCESS_COLUMN_USERACCESS,
	PROCESS_COLUMN_COMPANY
};

enum HS_PROCESSDIG_SEND_TYPE
{
	PROCESSDIG_SEND_INSERT = 555,	//插入条目
	PROCESSDIG_SEND_DELETE			//删除条目
};

typedef struct PROCESSINFO
{

	WCHAR     Name[100];
	WCHAR     Path[260];
	WCHAR     CompanyName[100];
	BOOL      UserAccess;
	ULONG_PTR Pid;
	ULONG_PTR PPid;
	ULONG_PTR Eprocess;
	LONGLONG  CreateTime;

}PROCESSINFO, *PPROCESSINFO;

DWORD WINAPI QueryProcessFunction(CListCtrl* pListCtrl);
VOID QueryProcessList(CListCtrl* pListCtrl);
CString GetFileCompanyName(CString strPath);
BOOL R3CanOpenProcess(DWORD dwPid);
BOOL DebugPrivilege(const WCHAR *pName, BOOL bEnable);
BOOL __stdcall Is32BitFile(const WCHAR * pwszFullPath);
VOID LocationExplorer(CString strPath);
BOOL KillProcessByForce(CListCtrl* m_ListCtrl);
VOID CheckAttribute(CString strPath);