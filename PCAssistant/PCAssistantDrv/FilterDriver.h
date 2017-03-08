#pragma once
#include <ntifs.h>
#include "common.h"

#define MAX_PATH 260

typedef enum _FILTER_TYPE_
{
	Unkonw,
	File,
	Disk,
	Volume,
	Keyboard,
	Mouse,
	I8042prt,
	Tcpip,
	NDIS,
	PnpManager,
	Tdx,
	RAW
}FILTER_TYPE;

typedef struct _FILTER_INFO_
{
	FILTER_TYPE Type;
	ULONG_PTR FileterDeviceObject;
	WCHAR wzFilterDriverName[100];
	WCHAR wzAttachedDriverName[100];
	WCHAR wzPath[MAX_PATH];
}FILTER_INFO, *PFILTER_INFO;

typedef struct _FILTER_DRIVER_
{
	ULONG_PTR ulCnt;
	ULONG_PTR ulRetCnt;
	FILTER_INFO Filter[1];
}FILTER_DRIVER, *PFILTER_DRIVER;

typedef struct _UNLOAD_FILTER_
{
	FILTER_TYPE Type;
	ULONG_PTR   DeviceObject;
}UNLOAD_FILTER, *PUNLOAD_FILTER;

extern
POBJECT_TYPE* IoDriverObjectType;


NTSTATUS EnumFilterDriver(PFILTER_DRIVER FilterDriverInfor);
NTSTATUS GetFilterDriverByDriverName(WCHAR *wzDriverName, PFILTER_DRIVER FilterDriverInfor, FILTER_TYPE Type);
NTSTATUS AddFilterInfo(PDEVICE_OBJECT AttachDeviceObject, PDRIVER_OBJECT AttachedDriverObject, PFILTER_DRIVER FilterDriverInfor, FILTER_TYPE Type);
NTSTATUS UnloadFilterDriver(PUNLOAD_FILTER UnloadFilter);
NTSTATUS ClearFilters(WCHAR* wzDriverName, ULONG_PTR DeviceObject);
