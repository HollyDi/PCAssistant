#include "Privilege.h"

extern WIN_VERSION  WinVersion;

extern ULONG_PTR    ObjectHeaderSize;
extern ULONG_PTR    ObjectTypeOffsetOf_Object_Header;
extern ULONG_PTR    ObjectTableOffsetOf_EPROCESS;
extern ULONG_PTR    PreviousModeOffsetOf_KTHREAD;

ULONG_PTR KeGetObjectType(PVOID Object)
{
	ULONG_PTR ObjectType = NULL;
	pfnObGetObjectType ObGetObjectType = NULL;


	if (!MmIsAddressValid || !Object || !MmIsAddressValid(Object))
	{
		return NULL;
	}

	if (WinVersion < WINDOWS_7)
	{
		ULONG SizeOfObjectHeader = 0, ObjectTypeOffset = 0, ObjectTypeAddress = 0;

		ObjectTypeAddress = (ULONG_PTR)Object - ObjectHeaderSize + ObjectTypeOffsetOf_Object_Header;

		if (MmIsAddressValid((PVOID)ObjectTypeAddress))
		{
			ObjectType = *(ULONG_PTR*)ObjectTypeAddress;
		}
	}
	else
	{
		//高版本使用函数

		ObGetObjectType = (pfnObGetObjectType)GetFunctionAddressByName(L"ObGetObjectType");


		if (ObGetObjectType)
		{
			ObjectType = ObGetObjectType(Object);
		}
	}

	return ObjectType;
}


NTSTATUS EnumProcessPrivilege(WCHAR* szOutBuffer, ULONG_PTR ProcessID, ULONG_PTR ulOutSize)
{
	NTSTATUS  Status = STATUS_UNSUCCESSFUL;
	PEPROCESS EProcess = NULL;
	HANDLE    hProcess = NULL;
	HANDLE    hToken = NULL;

	ULONG_PTR ulRet = 0;

	Status = PsLookupProcessByProcessId((HANDLE)ProcessID, &EProcess);


	if (!NT_SUCCESS(Status))
	{
		return Status;
	}

	if (!IsRealProcess(EProcess))
	{
		ObfDereferenceObject(EProcess);
		return Status;
	}


	Status = ObOpenObjectByPointer(EProcess,
		OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
		NULL,
		GENERIC_ALL,
		*PsProcessType,
		KernelMode,
		&hProcess
	);

	if (NT_SUCCESS(Status))
	{
		PETHREAD EThread = NULL;
		CHAR     PreMode = 0;

		EThread = PsGetCurrentThread();
		PreMode = ChangePreMode(EThread);

		Status = NtOpenProcessToken(hProcess, SACL_SECURITY_INFORMATION, &hToken);

		if (NT_SUCCESS(Status))
		{
			Status = NtQueryInformationToken(hToken, TokenPrivileges, szOutBuffer, ulOutSize, &ulRet);

			if (NT_SUCCESS(Status))
			{
				DbgPrint("NtQueryInformationToken Success\r\n");

			}
			else if (Status == STATUS_BUFFER_TOO_SMALL)
			{
				DbgPrint("Memory Too Small\r\n");
			}
		}

		if (hToken)
		{
			NtClose(hToken);
		}

		if (hProcess)
		{
			NtClose(hProcess);
		}

		RecoverPreMode(EThread, PreMode);
		ObfDereferenceObject(EProcess);

		Status = STATUS_SUCCESS;
	}
	return Status;
}