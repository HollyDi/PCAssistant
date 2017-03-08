#pragma once
#include <ntifs.h>
#include "Common.h"

#define MAX_PATH 260

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

typedef struct _KLDR_DATA_TABLE_ENTRY64 {
	LIST_ENTRY64 InLoadOrderLinks;
	ULONG64 __Undefined1;
	ULONG64 __Undefined2;
	ULONG64 __Undefined3;
	ULONG64 NonPagedDebugInfo;
	ULONG64 DllBase;
	ULONG64 EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING64 FullDllName;
	UNICODE_STRING64 BaseDllName;
	ULONG   Flags;
	USHORT  LoadCount;
	USHORT  __Undefined5;
	ULONG64 __Undefined6;
	ULONG   CheckSum;
	ULONG   __padding1;
	ULONG   TimeDateStamp;
	ULONG   __padding2;
} KLDR_DATA_TABLE_ENTRY64, *PKLDR_DATA_TABLE_ENTRY64;

typedef struct _KLDR_DATA_TABLE_ENTRY32 {
	LIST_ENTRY32 InLoadOrderLinks;
	ULONG __Undefined1;
	ULONG __Undefined2;
	ULONG __Undefined3;
	ULONG NonPagedDebugInfo;
	ULONG DllBase;
	ULONG EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING32 FullDllName;
	UNICODE_STRING32 BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT __Undefined5;
	ULONG  __Undefined6;
	ULONG  CheckSum;
	ULONG  TimeDateStamp;
} KLDR_DATA_TABLE_ENTRY32, *PKLDR_DATA_TABLE_ENTRY32;


#ifdef _WIN64
#define PKLDR_DATA_TABLE_ENTRY  PKLDR_DATA_TABLE_ENTRY64
#else
#define PKLDR_DATA_TABLE_ENTRY  PKLDR_DATA_TABLE_ENTRY32
#endif

#define NUMBER_HASH_BUCKETS 37
typedef struct _OBJECT_DIRECTORY_ENTRY
{
	struct _OBJECT_DIRECTORY_ENTRY *ChainLink;
	PVOID Object;
	ULONG HashValue;
} OBJECT_DIRECTORY_ENTRY, *POBJECT_DIRECTORY_ENTRY;

typedef struct _OBJECT_DIRECTORY
{
	struct _OBJECT_DIRECTORY_ENTRY *HashBuckets[NUMBER_HASH_BUCKETS];
} OBJECT_DIRECTORY, *POBJECT_DIRECTORY;

typedef
ULONG_PTR
(*pfnObGetObjectType)(PVOID Object);

typedef
NTSTATUS
(*pfnNtOpenDirectoryObject)(PHANDLE  DirectoryHandle, ACCESS_MASK  DesiredAccess, POBJECT_ATTRIBUTES  ObjectAttributes);

VOID InitModuleGlobalVariable();
NTSTATUS DispatchControlForModule(PIO_STACK_LOCATION  IrpSp, PVOID OutputBuffer, ULONG_PTR* ulRet);
VOID EnumDriverByLdrDataTableEntry(PALL_DRIVERS DriversInfor, ULONG_PTR ulCount);
VOID WalkerDirectoryObject(PALL_DRIVERS DriversInfor, PVOID DirectoryObject, ULONG_PTR ulCount);
NTSTATUS EnumSystemModuleList(PVOID OutBuffer, ULONG OutSize);
BOOLEAN GetKernelLdrDataTableEntry(PDRIVER_OBJECT DriverObject);
VOID EnumDriversByWalkerDirectoryObject(PALL_DRIVERS DriversInfor, ULONG_PTR ulCount);
POBJECT_TYPE KeGetObjectTypeInModule(PVOID Object);
BOOLEAN IsDriverInList(PALL_DRIVERS DriversInfor, PDRIVER_OBJECT DriverObject, ULONG_PTR ulCount);
VOID InsertDriver(PALL_DRIVERS DriversInfor, PDRIVER_OBJECT DriverObject, ULONG_PTR ulCount);
NTSTATUS UnloadDriverModule(ULONG_PTR InBuffer, ULONG_PTR InSize);
BOOLEAN IsRealDriverObject(PDRIVER_OBJECT DriverObject);
NTSTATUS PspUnloadDriver(PDRIVER_OBJECT DriverObject);
VOID NotHaveDriverUnloadThread(IN PVOID lParam);
VOID HaveDriverUnloadThread(PVOID lParam);