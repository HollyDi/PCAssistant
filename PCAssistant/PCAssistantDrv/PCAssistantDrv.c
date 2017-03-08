#ifndef CXX_COMMON_H
#	include "PCAssistantDrv.h"
#endif

#include "Process.h"
#include "Module.h"
#include "System.h"

PDEVICE_OBJECT g_DeviceObject;
PDRIVER_OBJECT g_DriverObject;

WIN_VERSION  WinVersion = WINDOWS_UNKNOW;
PEPROCESS    SystemEProcess = NULL;
// 链接器可能需要 /SAFESEH:NO 
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegisterPath)
{
	
	NTSTATUS       Status;
	UNICODE_STRING uniDeviceName;
	UNICODE_STRING uniLinkName;
	PDEVICE_OBJECT DeviceObject = NULL;
	int            i = 0;

	WinVersion = GetWindowsVersion();

	SystemEProcess = PsGetCurrentProcess();

	RtlInitUnicodeString(&uniDeviceName, DEVICE_NAME);
	Status = IoCreateDevice(DriverObject, 0, &uniDeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);

	if (!NT_SUCCESS(Status))
	{
		return STATUS_UNSUCCESSFUL;
	}


	//创建一个LinkName  让Ring3层可以通过此打开设备访问句柄
	RtlInitUnicodeString(&uniLinkName, LINK_NAME);

	Status = IoCreateSymbolicLink(&uniLinkName, &uniDeviceName);

	if (!NT_SUCCESS(Status))
	{
		IoDeleteDevice(DeviceObject);
		DeviceObject = NULL;
		return STATUS_UNSUCCESSFUL;
	}

	DbgPrint("Load Successfully");

	DriverObject->DriverUnload = UnloadDriver;
	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		DriverObject->MajorFunction[i] = DispatchPass;
	}
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchContorl;
	
	g_DeviceObject = DeviceObject;
	g_DriverObject = DriverObject;

	return STATUS_SUCCESS;
}


NTSTATUS DispatchContorl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	PIO_STACK_LOCATION  IrpSp;
	ULONG				ulIoControlCode = 0;
	NTSTATUS			Status = STATUS_UNSUCCESSFUL;
	ULONG_PTR			ulRet = 0;
	PVOID               pInputBuffer = NULL;
	PVOID               pOutputBuffer = NULL;
	ULONG_PTR			ulOutputLen = 0;

	ULONG  ulIoControlFunc = 0;

	IrpSp = IoGetCurrentIrpStackLocation(Irp);

	pOutputBuffer = Irp->UserBuffer;	//UserBuffer
	ulOutputLen = IrpSp->Parameters.DeviceIoControl.OutputBufferLength;
	ProbeForWrite(pOutputBuffer, ulOutputLen, sizeof(CHAR));

	ulRet = ulOutputLen;

	DbgPrint("Hello PCAssistant!\r\n");

	ulIoControlCode = IrpSp->Parameters.DeviceIoControl.IoControlCode;

	ulIoControlFunc = (ulIoControlCode >> 2) & 0x00000FFF & 0xF80;
	DbgPrint("Major Func Code: %x\r\n", ulIoControlFunc);

	switch (ulIoControlFunc)
	{
	case IOCTL_PROC:
	{
		Status = DispatchControlForProcess(IrpSp, pOutputBuffer, &ulRet);
		break;
	}
	case IOCTL_MODU:
	{
		Status = DispatchControlForModule(IrpSp, pOutputBuffer, &ulRet);
		break;
	}

	case IOCTL_SYSK:		//内核大功能
	{
		Status = DispatchControlForSystem(IrpSp, pOutputBuffer, &ulRet);
		break;
	}
	default:				//错误请求
	{
		Status = STATUS_UNSUCCESSFUL;
	}
	}

	Irp->IoStatus.Status = Status;
	Irp->IoStatus.Information = ulRet;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return Status;
}


NTSTATUS DispatchPass(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;

}


VOID UnloadDriver(PDRIVER_OBJECT DriverObject)
{

	UNICODE_STRING  uniLinkName;

	RtlInitUnicodeString(&uniLinkName, LINK_NAME);

	IoDeleteSymbolicLink(&uniLinkName);

	if (DriverObject->DeviceObject != NULL)
	{
		IoDeleteDevice(DriverObject->DeviceObject);
	}
	DbgPrint("Unload Successfully");
	return;
}