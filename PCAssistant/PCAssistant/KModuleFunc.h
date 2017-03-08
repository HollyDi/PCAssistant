#pragma once
#include "stdafx.h"
#include <vector>

using namespace std;

typedef struct _DRIVER_INFO_
{
	ULONG_PTR LodeOrder;
	ULONG_PTR Base;
	ULONG_PTR Size;
	ULONG_PTR DriverObject;
	ULONG_PTR DirverStartAddress;
	WCHAR wzDriverPath[MAX_PATH];
	WCHAR wzKeyName[MAX_PATH];
}DRIVER_INFO, *PDRIVER_INFO;

typedef struct _ALL_DRIVERS_
{
	ULONG_PTR ulCount;
	DRIVER_INFO Drivers[1];
}ALL_DRIVERS, *PALL_DRIVERS;

DWORD WINAPI QueryModuleFunction(CListCtrl *m_ListCtrl);
void QueryModuleList(CListCtrl *m_ListCtrl);
void InitModuleList(CListCtrl *m_ListCtrl);
void FixDriverPath(PDRIVER_INFO DriverInfor);
void RemoveDriverModule(CListCtrl* m_ListCtrl);