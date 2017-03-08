#ifndef CXX_COMMON_H
#define CXX_COMMON_H

#include <ntifs.h>
#include "Common.h"
#endif	
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegisterPath);
NTSTATUS DispatchPass(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS DispatchContorl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
VOID UnloadDriver(PDRIVER_OBJECT DriverObject);



