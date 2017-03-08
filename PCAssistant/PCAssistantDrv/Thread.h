#pragma once

#include <ntifs.h>
#include "Common.h"
#include "Process.h"

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

extern
PPEB
PsGetProcessPeb(PEPROCESS Process);

NTSTATUS EnumProcessThread(PVOID InBuffer, ULONG InSize, PVOID OutBuffer, ULONG_PTR OutSize);
VOID EnumThreads(PEPROCESS EProcess, PALL_THREADS ProcessThreads, ULONG_PTR ulCount);
NTSTATUS EnumProcessThreadModule(ULONG ulProcessID, PVOID OutBuffer, ULONG_PTR ulOutSize);
VOID EnumProcessThreadByList(PEPROCESS EProcess, PALL_THREADS ProcessThreads, ULONG_PTR ulCount);
VOID InsertThread(PETHREAD EThread, PEPROCESS EProcess, PALL_THREADS ProcessThreads, ULONG ulCount);
VOID InitGlobalVariable();
ULONG_PTR GetThreadStartAddress(PETHREAD EThread);
BOOLEAN IsModuleInList(ULONG_PTR Base, ULONG_PTR Size, PALL_MODULES AllModules, ULONG_PTR ulCount);
VOID WalkerModuleList32(PLIST_ENTRY32 ListEntry, ULONG nType, PALL_MODULES AllModules, ULONG ulCount);
VOID WalkerModuleList64(PLIST_ENTRY64 ListEntry, ULONG nType, PALL_MODULES AllModules, ULONG ulCount);
NTSTATUS EnumDllModuleByPeb(PEPROCESS EProcess, PALL_MODULES AllModules, ULONG_PTR ulCount);
BOOLEAN IsThreadInList(PETHREAD EThread, PALL_THREADS ProcessThreads, ULONG ulCount);
