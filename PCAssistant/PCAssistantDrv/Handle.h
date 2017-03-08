#pragma once
#include <ntifs.h>
#include "common.h"
#include "Process.h"

#define MAX_OBJECT_NAME  50
#define MAX_PATH         500

typedef struct _HANDLE_INFO_
{
	ULONG_PTR Handle;
	ULONG_PTR Object;
	ULONG_PTR ReferenceCount;
	WCHAR ObjectName[MAX_OBJECT_NAME];
	WCHAR HandleName[MAX_PATH];
}HANDLE_INFO, *PHANDLE_INFO;

typedef struct _PROCESS_HANDLES_
{
	ULONG_PTR ulCount;
	HANDLE_INFO Handles[1];
}ALL_HANDLES, *PALL_HANDLES;


typedef
NTSTATUS(NTAPI *pfnNtQueryObject)(HANDLE Handle, OBJECT_INFORMATION_CLASS ObjectInformationClass,
	PVOID ObjectInformation,
	ULONG ObjectInformationLength,
	PULONG ReturnLength);


extern
NTSTATUS
NtQuerySystemInformation(
	IN ULONG SystemInformationClass,
	IN PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength);

typedef enum _OBJECT_INFO_CLASS {
	ObjectBasicInfo,
	ObjectNameInfo,
	ObjectTypeInfo,
	ObjectAllInfo,
	ObjectDataInfo
} OBJECT_INFO_CLASS, *POBJECT_INFO_CLASS;


VOID InsertHandleToList(PEPROCESS EProcess, HANDLE Handle, ULONG_PTR Object, PALL_HANDLES Handles);
NTSTATUS GetHandles(ULONG_PTR ulPid, ULONG_PTR EProcess, PALL_HANDLES OutHandles, ULONG_PTR ulCount);
NTSTATUS EnumProcessHandle(PVOID InBuffer, ULONG_PTR InSize, PVOID OutBuffer, ULONG_PTR OutSize);
VOID InitHandleVariable();
VOID GetHandleTypeName(HANDLE hHandle, ULONG_PTR Object, WCHAR* wzTypeName);
VOID GetHandleObjectName(HANDLE hHandle, WCHAR* wzObjectName);
