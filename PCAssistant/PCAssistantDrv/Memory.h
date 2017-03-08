#pragma once


#include <ntifs.h>
#include "Common.h"

#include "Process.h"

typedef struct _MEMORY_INFO_
{
	ULONG_PTR ulBase;
	ULONG_PTR ulSize;
	ULONG ulProtect;
	ULONG ulState;
	ULONG ulType;
}MEMORY_INFO, *PMEMORY_INFO;

typedef struct _ALL_MEMORYS_
{
	ULONG_PTR ulCount;
	MEMORY_INFO Memorys[1];
}ALL_MEMORYS, *PALL_MEMORYS;

typedef enum _MEMORY_INFORMATION_CLASS
{
	MemoryBasicInformation,
	MemoryWorkingSetList,
	MemorySectionName
}MEMORY_INFORMATION_CLASS;

typedef
NTSTATUS
(*pfnNtQueryVirtualMemory)(HANDLE ProcessHandle, PVOID BaseAddress, MEMORY_INFORMATION_CLASS MemoryInformationClass,
	PVOID MemoryInformation,
	SIZE_T MemoryInformationLength,
	PSIZE_T ReturnLength);

typedef struct _MEMORY_BASIC_INFORMATION {
	PVOID BaseAddress;
	PVOID AllocationBase;
	ULONG AllocationProtect;
	SIZE_T RegionSize;
	ULONG State;
	ULONG Protect;
	ULONG Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;

extern
PPEB
PsGetProcessPeb(PEPROCESS Process);

NTSTATUS EnumProcessesModule(ULONG ulProcessID, PVOID OutBuffer, ULONG_PTR ulOutSize);
NTSTATUS EnumProcessesMemory(ULONG ulProcessID, PVOID OutBuffer, ULONG_PTR ulOutSize);
VOID InitMemoryVariable();
NTSTATUS GetMemorys(PEPROCESS EProcess, PALL_MEMORYS Memorys, ULONG_PTR ulCount);

