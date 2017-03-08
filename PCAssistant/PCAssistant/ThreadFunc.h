#pragma once
#include "stdafx.h"

typedef struct _THREAD_INFO_
{
	ULONG_PTR Thread;
	ULONG_PTR Tid;
	ULONG_PTR Teb;
	UCHAR Priority;
	ULONG_PTR Win32StartAddress;
	ULONG ContextSwitches;
	UCHAR State;
}THREAD_INFO, *PTHREAD_INFO;

typedef struct _ALL_THREADS_
{
	ULONG_PTR    nCnt;
	THREAD_INFO Threads[1];
}ALL_THREADS, *PALL_THREADS;

typedef struct _MODULE_INFO_
{
	ULONG_PTR Base;
	ULONG_PTR Size;
	WCHAR Path[MAX_PATH];
}MODULE_INFO, *PMODULE_INFO;

typedef struct _ALL_MODULES_
{
	ULONG_PTR   ulCount;
	MODULE_INFO Modules[1];
}ALL_MODULES, *PALL_MODULES;

typedef enum _KTHREAD_STATE
{
	Initialized,
	Ready,
	Running,
	Standby,
	Terminated,
	Waiting,
	Transition,
	DeferredReady,
	GateWait
} KTHREAD_STATE, *PKTHREAD_STATE;

enum THREAD_COLUMN
{
	THREAD_COLUMN_TID = 0,
	THREAD_COLUMN_ETHREAD,
	THREAD_COLUMN_TEB,
	THREAD_COLUMN_PRIORITY,
	THREAD_COLUMN_ENTRANCE,
	THREAD_COLUMN_MODULE,
	THREAD_COLUMN_COUNTER,
	THREAD_COLUMN_STATUS
};

typedef enum _THREAD_HEADER_INDEX_
{
	ThreadId,
	ThreadObject,
	ThreadTeb,
	ThreadPriority,
	ThreadStartAddress,
	ThreadSwitchTimes,
	ThreadStatus,
	ThreadStartModule,
	ThreadComp
}THREAD_HEADER_INDEX;

void InitThreadList(CListCtrl *m_ListCtrl);
BOOL QueryProcessThread(CListCtrl *m_ListCtrl);
CString GetModulePathByThreadStartAddress(ULONG_PTR ulBase);