#include "System.h"
#include "CallBack.h"
#include "IOTimer.h"
#include "FilterDriver.h"
#include "GetFuncAddress.h"
#include "SysThread.h"

extern
WIN_VERSION WinVersion;
extern
ULONG_PTR PreviousModeOffsetOf_KTHREAD;
extern
ULONG_PTR ObjectTableOffsetOf_EPROCESS;
extern
ULONG_PTR SYSTEM_ADDRESS_START;
extern
ULONG_PTR ObjectHeaderSize;
extern
ULONG_PTR ObjectTypeOffsetOf_Object_Header;

NTSTATUS DispatchControlForSystem(PIO_STACK_LOCATION  IrpSp, PVOID OutputBuffer, ULONG_PTR* ulRet)
{

	WCHAR* szOutputBuffer = (WCHAR*)OutputBuffer;
	ULONG				ulIoControlCode = 0;
	NTSTATUS			Status = STATUS_UNSUCCESSFUL;
	PVOID               pvInputBuffer = NULL;
	ULONG               ulInputLen = 0;
	ULONG				ulOutputLen = 0;

	pvInputBuffer = IrpSp->Parameters.DeviceIoControl.Type3InputBuffer;
	ulInputLen = IrpSp->Parameters.DeviceIoControl.InputBufferLength;
	ProbeForRead(pvInputBuffer, ulInputLen, sizeof(CHAR));

	ulOutputLen = IrpSp->Parameters.DeviceIoControl.OutputBufferLength;

	ProbeForWrite(OutputBuffer, ulOutputLen, sizeof(CHAR));

	ulIoControlCode = IrpSp->Parameters.DeviceIoControl.IoControlCode;
	ulIoControlCode = (ulIoControlCode >> 2) & 0x00000FFF;

	DbgPrint("%x\r\n", ulIoControlCode);

	InitSystemGlobalVariable();

	switch (ulIoControlCode)
	{

	case IOCTL_SYSK_SYSTHREAD:
	{
		DbgPrint("IOCTL_SYSK_SYSTHREAD\r\n");

		Status = EnumSysThread(OutputBuffer, ulOutputLen);

		*ulRet = sizeof(ULONG_PTR);
		break;
	}

	case IOCTL_SYSK_IOTIMER:		//枚举IOTIMER
	{
		DbgPrint("IOCTL_SYSK_IOTIMER\r\n");

		Status = EnumIOTimer(OutputBuffer);

		if (Status)
		{
			*ulRet = sizeof(ULONG_PTR);
		}
		break;
	}
	
	case IOCTL_SYSK_OPERIOTIMER:		//切换IOTIMER运行状态
	{
		DbgPrint("IOCTL_SYSK_OPERIOTIMER\r\n");

		Status = OperIOTimer(pvInputBuffer);

		if (Status)
		{
			*ulRet = sizeof(ULONG_PTR);
		}
		break;
	}

	case IOCTL_SYSK_REMOVEIOTIMER:
	{
		DbgPrint("IOCTL_SYSK_REMOVEIOTIMER\r\n");

		Status = RemoveIOTimer(pvInputBuffer);

		if (Status)
		{
			*ulRet = sizeof(ULONG_PTR);
		}
		break;
	}

	
	case IOCTL_SYSK_DPCTIMER:		//枚举IOTIMER
	{
		DbgPrint("IOCTL_SYSK_DPCTIMER\r\n");

		Status = EnumDPCTimer(OutputBuffer);

		if (Status)
		{
			*ulRet = sizeof(ULONG_PTR);
		}
		break;
	}
	case IOCTL_SYSK_REMOVEDPCTIMER:
	{
		DbgPrint("IOCTL_SYSK_REMOVEDPCTIMER\r\n");

		Status = RemoveDPCTimer(pvInputBuffer);

		if (Status)
		{
			*ulRet = sizeof(ULONG_PTR);
		}
		break;
	}

	case IOCTL_SYSK_CALLBACKLIST:
	{
		DbgPrint("IOCTL_SYSK_CALLBACKLIST\r\n");

		Status = EnumCallBackList(*(int*)pvInputBuffer, OutputBuffer);

		if (Status)
		{
			*ulRet = sizeof(ULONG_PTR);
		}
		break;
	}
	
	case IOCTL_SYSK_REMOVECALLBACK:
	{
		DbgPrint("HS_IOCTL_SYSK_REMOVECALLBACK\r\n");

		Status = RemoveCallbackNotify(pvInputBuffer);

		if (Status)
		{
			*ulRet = sizeof(ULONG_PTR);
		}
		break;
	}

	case IOCTL_SYSK_FILTERDRIVER:
	{
		DbgPrint("IOCTL_SYSK_FILTERDRIVER\r\n");

		Status = EnumFilterDriver((PFILTER_DRIVER)OutputBuffer);

		if (Status)
		{
			*ulRet = sizeof(ULONG_PTR);
		}
		break;
	}
	case IOCTL_SYSK_FILTERUNLOAD:
	{
		DbgPrint("IOCTL_SYSK_FILTERUNLOAD\r\n");

		Status = UnloadFilterDriver((PUNLOAD_FILTER)pvInputBuffer);

		if (Status)
		{
			*ulRet = sizeof(ULONG_PTR);
		}
		break;
	}
	
	default:
	{
		Status = STATUS_UNSUCCESSFUL;
	}
	}

	return Status;


}


VOID InitSystemGlobalVariable()
{
	switch (WinVersion)
	{
	case WINDOWS_XP:
	{
		PreviousModeOffsetOf_KTHREAD = 0x140;
		ObjectHeaderSize = 0x18;
		ObjectTypeOffsetOf_Object_Header = 0x8;
		ObjectTableOffsetOf_EPROCESS = 0x0c4;
		SYSTEM_ADDRESS_START = 0x80000000;
		break;
	}

	case WINDOWS_7:
	{
		PreviousModeOffsetOf_KTHREAD = 0x1f6;
		ObjectTableOffsetOf_EPROCESS = 0x200;
		ObjectHeaderSize = 0x30;
		SYSTEM_ADDRESS_START = 0x80000000000;
		break;
	}
	}
}



