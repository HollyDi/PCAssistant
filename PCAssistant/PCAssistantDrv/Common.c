#include "Common.h"

ULONG_PTR  ulBuildNumber = 0;	//GetWindowsVersion
ULONG_PTR    SYSTEM_ADDRESS_START = 0;

WIN_VERSION GetWindowsVersion()
{
	RTL_OSVERSIONINFOEXW osverInfo = { sizeof(osverInfo) };
	pfnRtlGetVersion RtlGetVersion = NULL;
	WIN_VERSION WinVersion;
	WCHAR szRtlGetVersion[] = L"RtlGetVersion";


	RtlGetVersion = (pfnRtlGetVersion)GetFunctionAddressByName(szRtlGetVersion);

	if (RtlGetVersion)
	{
		RtlGetVersion((PRTL_OSVERSIONINFOW)&osverInfo);
	}
	else
	{
		PsGetVersion(&osverInfo.dwMajorVersion, &osverInfo.dwMinorVersion, &osverInfo.dwBuildNumber, NULL);
	}

	ulBuildNumber = osverInfo.dwBuildNumber;

	if (osverInfo.dwMajorVersion == 5 && osverInfo.dwMinorVersion == 1)
	{
		DbgPrint("WINDOWS_XP\r\n");
		WinVersion = WINDOWS_XP;
	}
	else if (osverInfo.dwMajorVersion == 6 && osverInfo.dwMinorVersion == 1)
	{
		DbgPrint("WINDOWS 7\r\n");
		WinVersion = WINDOWS_7;
	}
	else if (osverInfo.dwMajorVersion == 6 &&
		osverInfo.dwMinorVersion == 2 &&
		osverInfo.dwBuildNumber == 9200)
	{
		DbgPrint("WINDOWS 8\r\n");
		WinVersion = WINDOWS_8;
	}
	else if (osverInfo.dwMajorVersion == 6 &&
		osverInfo.dwMinorVersion == 3 &&
		osverInfo.dwBuildNumber == 9600)
	{
		DbgPrint("WINDOWS 8.1\r\n");
		WinVersion = WINDOWS_8_1;
	}
	else
	{
		DbgPrint("WINDOWS_UNKNOW\r\n");
		WinVersion = WINDOWS_UNKNOW;
	}

	return WinVersion;
}

PVOID GetFunctionAddressByName(WCHAR *szFunction)
{
	UNICODE_STRING uniFunction;
	PVOID AddrBase = NULL;

	if (szFunction && wcslen(szFunction) > 0)
	{
		RtlInitUnicodeString(&uniFunction, szFunction);   //³£Á¿Ö¸Õë
		AddrBase = MmGetSystemRoutineAddress(&uniFunction);
	}

	return AddrBase;
}

BOOLEAN IsUnicodeStringValid(PUNICODE_STRING uniString)
{
	BOOLEAN bRet = FALSE;

	__try
	{
		if (uniString->Length > 0 &&
			uniString->Buffer		&&
			MmIsAddressValid(uniString->Buffer) &&
			MmIsAddressValid(&uniString->Buffer[uniString->Length / sizeof(WCHAR) - 1]))
		{
			bRet = TRUE;
		}

	}
	__except (1)
	{
		bRet = FALSE;
	}

	return bRet;
}