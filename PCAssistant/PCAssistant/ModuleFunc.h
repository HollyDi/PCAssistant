#pragma once
#include "stdafx.h"
#include "ProcessFunc.h"

typedef struct _PROCESSMODULE_INFO
{
	ULONG_PTR	ulBase;
	ULONG_PTR	ulSize;
	ULONG_PTR	ulEntryPoint;
	CHAR	    szPath[264];
} PROCESSMODULE_INFO, *PPROCESSMODULE_INFO;


VOID InitPModuleDetailList(CListCtrl *m_ListCtrl);
VOID LoadPModuleDetailList(CListCtrl *m_ListCtrl);
ULONG_PTR EnumProcessModule(_PROCESSMODULE_INFO* mi);
BOOL EnableDebugPri32();
BOOL EnableDebugPri64();