#include "CallBack.h"

extern     PDEVICE_OBJECT g_DeviceObject;
extern     PDRIVER_OBJECT g_DriverObject;

extern  PVOID        Ntoskrnl_KLDR_DATA_TABLE_ENTRY;
extern  ULONG_PTR    SYSTEM_ADDRESS_START;
extern  WIN_VERSION  WinVersion;

pfnPsSetLoadImageNotifyRoutine PsSetLoadImageNotifyRoutineAddress = NULL;
ULONG_PTR	PspLoadImageNotifyRoutineAddress = NULL;

pfnCmUnRegisterCallback CmUnRegisterCallbackAddress = NULL;
ULONG_PTR	CallBackList = 0;

pfnKeRegisterBugCheckCallback KeRegisterBugCheckCallbackAddress = NULL;
ULONG_PTR	KeBugCheckCallbackListHead = NULL;

pfnKeRegisterBugCheckReasonCallback KeRegisterBugCheckReasonCallbackAddress = NULL;
ULONG_PTR	KeBugCheckReasonCallbackListHead = NULL;

pfnIoRegisterShutdownNotification   IoRegisterShutdownNotificationAddress = NULL;
ULONG_PTR	IopNotifyShutdownQueueHead = NULL;

pfnPsSetCreateThreadNotifyRoutine   PsSetCreateThreadNotifyRoutineAddress = NULL;
ULONG_PTR PspCreateThreadNotifyRoutineAddress = NULL;

NTSTATUS EnumCallBackList(int InputBuffer, PVOID OutputBuffer)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	Status = GetAllCallbackNotify(OutputBuffer);
	return Status;
}

NTSTATUS GetAllCallbackNotify(PVOID OutBuffer)
{
	NTSTATUS Status = STATUS_SUCCESS;
	BOOLEAN  bRet = FALSE;
	PGET_CALLBACK GetCallback = (PGET_CALLBACK)OutBuffer;

	DbgPrint("GetAllCallbackNotify\r\n");

	bRet = GetLoadImageCallbackNotify(GetCallback);
	bRet = GetRegisterCallbackNotify(GetCallback);
	bRet = GetBugCheckCallbackNotify(GetCallback);
	bRet = GetBugCheckReasonCallbackNotify(GetCallback);
	bRet = GetShutDownCallbackNotify(GetCallback);
	bRet = GetCreateThreadCallbackNotify(GetCallback);

	if (GetCallback->ulRetCnt > GetCallback->ulCnt)
	{
		DbgPrint("STATUS_BUFFER_TOO_SMALL\r\n");
		Status = STATUS_BUFFER_TOO_SMALL;
	}

	return Status;
}

BOOLEAN GetLoadImageCallbackNotify(PGET_CALLBACK GetCallback)
{

	PsSetLoadImageNotifyRoutineAddress =
		(pfnPsSetLoadImageNotifyRoutine)GetFunctionAddressByName(L"PsSetLoadImageNotifyRoutine");
	DbgPrint("%p\r\n", PsSetLoadImageNotifyRoutineAddress);
	PspLoadImageNotifyRoutineAddress =
		FindPspLoadImageNotifyRoutine((ULONG_PTR)PsSetLoadImageNotifyRoutineAddress);
	DbgPrint("%p\r\n", PspLoadImageNotifyRoutineAddress);


	if (!PspLoadImageNotifyRoutineAddress)
	{
		DbgPrint("PspLoadImageNotifyRoutineAddress NULL\r\n");
		return FALSE;
	}

	else
	{
		ULONG i = 0;

		switch (WinVersion)
		{
		case WINDOWS_7:
		{
			SYSTEM_ADDRESS_START = 0x80000000000;

			for (i = 0; i < 64; i++)
			{
				ULONG64 NotifyItem = 0, CallBackAddress = 0;

				DbgPrint("i = %d\r\n", i);

				if (!MmIsAddressValid((PVOID)(PspLoadImageNotifyRoutineAddress + i * sizeof(ULONG64))))
				{
					break;
				}

				NotifyItem = *(PULONG64)(PspLoadImageNotifyRoutineAddress + i * sizeof(ULONG64));

				DbgPrint("NotifyItem: %p\r\n", NotifyItem);

				if (!(NotifyItem > SYSTEM_ADDRESS_START && MmIsAddressValid((PVOID)(NotifyItem & 0xfffffffffffffff8))))
				{
					break;
				}

				CallBackAddress = *((PULONG64)(NotifyItem & 0xfffffffffffffff8));

				DbgPrint("CallBackAddress: %p\r\n", CallBackAddress);

				if (CallBackAddress && MmIsAddressValid((PVOID)CallBackAddress))
				{
					if (GetCallback->ulCnt > GetCallback->ulRetCnt)
					{
						GetCallback->Callbacks[GetCallback->ulRetCnt].Type = NotifyLoadImage;
						GetCallback->Callbacks[GetCallback->ulRetCnt].CallbackAddress = CallBackAddress;
						GetCallback->Callbacks[GetCallback->ulRetCnt].Note = NotifyItem;

						DbgPrint("GetCallback->ulCnt > GetCallback->ulRetCnt\r\n");
					}

					GetCallback->ulRetCnt++;
				}

			}
			break;
		}

		case WINDOWS_XP:
		{
			SYSTEM_ADDRESS_START = 0x80000000;
			for (i = 0; i < 64; i++)
			{
				ULONG32 NotifyItem = 0, CallBackAddress = 0;

				if (!MmIsAddressValid((PVOID)(PspLoadImageNotifyRoutineAddress + i * sizeof(ULONG32))))
				{
					break;
				}

				NotifyItem = *(PULONG32)(PspLoadImageNotifyRoutineAddress + i * sizeof(ULONG32));
				if (!(NotifyItem > SYSTEM_ADDRESS_START && MmIsAddressValid((PVOID)(NotifyItem & 0xFFFFFFF8 + sizeof(ULONG32)))))
				{
					break;
				}

				CallBackAddress = *(PULONG32)(NotifyItem & 0xFFFFFFF8 + sizeof(ULONG));

				if (CallBackAddress && MmIsAddressValid((PVOID)CallBackAddress))
				{
					if (GetCallback->ulCnt > GetCallback->ulRetCnt)
					{
						GetCallback->Callbacks[GetCallback->ulRetCnt].Type = NotifyLoadImage;
						GetCallback->Callbacks[GetCallback->ulRetCnt].CallbackAddress = CallBackAddress;
						GetCallback->Callbacks[GetCallback->ulRetCnt].Note = NotifyItem;
					}

					GetCallback->ulRetCnt++;
				}
			}
			break;
		}
		}
	}

	return TRUE;
}

BOOLEAN GetRegisterCallbackNotify(PGET_CALLBACK GetCallback)
{
	CmUnRegisterCallbackAddress =
		(pfnCmUnRegisterCallback)GetFunctionAddressByName(L"CmUnRegisterCallback");
	DbgPrint("%p\r\n", CmUnRegisterCallbackAddress);
	CallBackList =
		CmpCallBackVector((ULONG_PTR)CmUnRegisterCallbackAddress);
	DbgPrint("%p\r\n", CallBackList);

	//DbgPrint("%02x\r\n",*(char*)((char*)CmUnRegisterCallbackAddress+0xe));

	if (!CallBackList)
	{
		return FALSE;
	}

	else
	{

		ULONG i = 0;
		switch (WinVersion)
		{
		case WINDOWS_7:   //这是个List
		{
			PCM_NOTIFY_ENTRY	Notify = NULL;
			PLIST_ENTRY			NotifyList = (LIST_ENTRY*)(*(ULONG_PTR*)CallBackList);
			SYSTEM_ADDRESS_START = 0x80000000000;

			do
			{
				Notify = (CM_NOTIFY_ENTRY *)NotifyList;
				if (MmIsAddressValid(Notify))
				{
					if (MmIsAddressValid((PVOID)(Notify->Function)) && Notify->Function > SYSTEM_ADDRESS_START)
					{
						if (GetCallback->ulCnt > GetCallback->ulRetCnt)
						{
							GetCallback->Callbacks[GetCallback->ulRetCnt].Type = NotifyCmCallBack;
							GetCallback->Callbacks[GetCallback->ulRetCnt].CallbackAddress = Notify->Function;
							GetCallback->Callbacks[GetCallback->ulRetCnt].Note = Notify->Cookie.QuadPart;
						}

						GetCallback->ulRetCnt++;
					}
				}
				NotifyList = NotifyList->Flink;
			} while (NotifyList != ((LIST_ENTRY*)(*(ULONG_PTR*)CallBackList)));
			break;
		}

		case WINDOWS_XP:
		{

			SYSTEM_ADDRESS_START = 0x80000000;
			for (i = 0; i < 64; i++)
			{
				ULONG32 NotifyItem = 0, CallBackAddress = 0;
				ULONG   Temp = 0;

				if (!MmIsAddressValid((PVOID)(CallBackList + i * sizeof(ULONG32))))
				{
					break;
				}


				NotifyItem = *(PULONG32)(CallBackList + i * sizeof(ULONG32));
				if (!(NotifyItem > SYSTEM_ADDRESS_START && MmIsAddressValid((PVOID)(NotifyItem & 0xFFFFFFF8 + sizeof(ULONG32)))))
				{
					break;
				}

				CallBackAddress = *(PULONG32)(NotifyItem & 0xFFFFFFF8 + sizeof(ULONG));
				if (CallBackAddress && MmIsAddressValid((PVOID)CallBackAddress))
				{
					if (GetCallback->ulCnt > GetCallback->ulRetCnt)
					{
						GetCallback->Callbacks[GetCallback->ulRetCnt].Type = NotifyCmCallBack;
						GetCallback->Callbacks[GetCallback->ulRetCnt].CallbackAddress = CallBackAddress;
						GetCallback->Callbacks[GetCallback->ulRetCnt].Note = NotifyItem;
					}
					GetCallback->ulRetCnt++;
				}
			}
			break;
		}
		}

	}

}

BOOLEAN GetBugCheckCallbackNotify(PGET_CALLBACK GetCallback)
{

	KeRegisterBugCheckCallbackAddress =
		(pfnKeRegisterBugCheckCallback)GetFunctionAddressByName(L"KeRegisterBugCheckCallback");
	DbgPrint("%p\r\n", KeRegisterBugCheckCallbackAddress);
	KeBugCheckCallbackListHead =
		FindKeBugCheckReasonCallbackListHeadNotifyRoutine((ULONG_PTR)KeRegisterBugCheckCallbackAddress);
	DbgPrint("%p\r\n", KeBugCheckCallbackListHead);


	if (!KeBugCheckCallbackListHead)
	{
		return FALSE;
	}

	else
	{

		PLIST_ENTRY Entry = NULL;
		ULONG_PTR   Dispatch = 0;

		switch (WinVersion)
		{
		case WINDOWS_7:
		{
			Entry = ((PLIST_ENTRY)KeBugCheckCallbackListHead)->Flink;
			do
			{
				Dispatch = *(ULONG64*)((ULONG64)Entry + sizeof(LIST_ENTRY));
				if (Dispatch&&MmIsAddressValid((PVOID)Dispatch))
				{
					if (GetCallback->ulCnt > GetCallback->ulRetCnt)
					{
						GetCallback->Callbacks[GetCallback->ulRetCnt].Type = NotifyKeBugCheck;
						GetCallback->Callbacks[GetCallback->ulRetCnt].CallbackAddress = Dispatch;
						GetCallback->Callbacks[GetCallback->ulRetCnt].Note = (PVOID)Entry;
					}

					GetCallback->ulRetCnt++;

				}
				Entry = Entry->Flink;

			} while (Entry != (PLIST_ENTRY)KeBugCheckCallbackListHead);

			break;
		}

		case WINDOWS_XP:
		{
			//在Wrk中搜索 _KBUGCHECK_CALLBACK_RECORD 结构
			Entry = ((PLIST_ENTRY)KeBugCheckCallbackListHead)->Flink;
			do
			{
				Dispatch = *(ULONG32*)((ULONG32)Entry + sizeof(LIST_ENTRY));
				if (Dispatch&&MmIsAddressValid((PVOID)Dispatch))
				{
					if (GetCallback->ulCnt > GetCallback->ulRetCnt)
					{
						GetCallback->Callbacks[GetCallback->ulRetCnt].Type = NotifyKeBugCheck;
						GetCallback->Callbacks[GetCallback->ulRetCnt].CallbackAddress = Dispatch;
						GetCallback->Callbacks[GetCallback->ulRetCnt].Note = (PVOID)Entry;;
					}

					GetCallback->ulRetCnt++;

				}
				Entry = Entry->Flink;

			} while (Entry != (PLIST_ENTRY)KeBugCheckCallbackListHead);
			break;
		}
		}
	}
}

BOOLEAN GetBugCheckReasonCallbackNotify(PGET_CALLBACK GetCallback)
{
	KeRegisterBugCheckReasonCallbackAddress =
		(pfnKeRegisterBugCheckReasonCallback)GetFunctionAddressByName(L"KeRegisterBugCheckReasonCallback");
	DbgPrint("%p\r\n", KeRegisterBugCheckReasonCallbackAddress);
	KeBugCheckReasonCallbackListHead =
		FindKeBugCheckReasonCallbackListHeadNotifyRoutine((ULONG_PTR)KeRegisterBugCheckReasonCallbackAddress);
	DbgPrint("%p\r\n", KeBugCheckReasonCallbackListHead);


	if (!KeBugCheckReasonCallbackListHead)
	{
		return FALSE;
	}

	else
	{

		PLIST_ENTRY Entry = NULL;
		ULONG_PTR   Dispatch = 0;

		switch (WinVersion)
		{
		case WINDOWS_7:
		{
			Entry = ((PLIST_ENTRY)KeBugCheckReasonCallbackListHead)->Flink;
			do
			{
				Dispatch = *(ULONG64*)((ULONG64)Entry + sizeof(LIST_ENTRY));
				if (Dispatch&&MmIsAddressValid((PVOID)Dispatch))
				{
					if (GetCallback->ulCnt > GetCallback->ulRetCnt)
					{
						GetCallback->Callbacks[GetCallback->ulRetCnt].Type = NotifyKeBugCheckReason;
						GetCallback->Callbacks[GetCallback->ulRetCnt].CallbackAddress = Dispatch;
						GetCallback->Callbacks[GetCallback->ulRetCnt].Note = (PVOID)Entry;
					}

					GetCallback->ulRetCnt++;

				}
				Entry = Entry->Flink;

			} while (Entry != (PLIST_ENTRY)KeBugCheckReasonCallbackListHead);

			break;
		}

		case WINDOWS_XP:
		{
			//在Wrk中搜索 _KBUGCHECK_CALLBACK_RECORD 结构
			Entry = ((PLIST_ENTRY)KeBugCheckReasonCallbackListHead)->Flink;
			do
			{
				Dispatch = *(ULONG32*)((ULONG32)Entry + sizeof(LIST_ENTRY));
				if (Dispatch&&MmIsAddressValid((PVOID)Dispatch))
				{
					if (GetCallback->ulCnt > GetCallback->ulRetCnt)
					{
						GetCallback->Callbacks[GetCallback->ulRetCnt].Type = NotifyKeBugCheckReason;
						GetCallback->Callbacks[GetCallback->ulRetCnt].CallbackAddress = Dispatch;
						GetCallback->Callbacks[GetCallback->ulRetCnt].Note = (PVOID)Entry;;
					}

					GetCallback->ulRetCnt++;

				}
				Entry = Entry->Flink;

			} while (Entry != (PLIST_ENTRY)KeBugCheckReasonCallbackListHead);
			break;
		}
		}

	}

}

BOOLEAN GetShutDownCallbackNotify(PGET_CALLBACK GetCallback)
{

	IoRegisterShutdownNotificationAddress =
		(pfnIoRegisterShutdownNotification)GetFunctionAddressByName(L"IoRegisterShutdownNotification");
	DbgPrint("%p\r\n", IoRegisterShutdownNotificationAddress);
	IopNotifyShutdownQueueHead =
		FindIopNotifyShutdownQueueHeadNotifyRoutine((ULONG_PTR)IoRegisterShutdownNotificationAddress);
	DbgPrint("%p\r\n", IopNotifyShutdownQueueHead);


	if (!IopNotifyShutdownQueueHead)
	{
		return FALSE;
	}
	else
	{
		ULONG i = 0;

		switch (WinVersion)
		{
		case WINDOWS_7:
		{

			SYSTEM_ADDRESS_START = 0x80000000000;

			if (IopNotifyShutdownQueueHead && MmIsAddressValid((PVOID)IopNotifyShutdownQueueHead))
			{
				PLIST_ENTRY Entry = ((PLIST_ENTRY)IopNotifyShutdownQueueHead)->Flink;
				while (MmIsAddressValid(Entry) && Entry != (PLIST_ENTRY)IopNotifyShutdownQueueHead)
				{
					ULONG64 ValidAddress = (ULONG64)Entry + sizeof(LIST_ENTRY);

					if (ValidAddress &&
						MmIsAddressValid((PVOID)ValidAddress))
					{
						ULONG64 DeviceObject = *(PULONG64)ValidAddress;

						if (DeviceObject &&
							MmIsAddressValid((PVOID)DeviceObject))
						{
							if (GetCallback->ulCnt > GetCallback->ulRetCnt)
							{
								GetCallback->Callbacks[GetCallback->ulRetCnt].Type = NotifyShutdown;
								GetCallback->Callbacks[GetCallback->ulRetCnt].CallbackAddress = GetShutdownDispatch((PDEVICE_OBJECT)DeviceObject);
								GetCallback->Callbacks[GetCallback->ulRetCnt].Note = DeviceObject;
							}

							GetCallback->ulRetCnt++;
						}
					}

					Entry = Entry->Flink;
				}
			}
			break;
		}

		case WINDOWS_XP:
		{

			SYSTEM_ADDRESS_START = 0x80000000;
			if (IopNotifyShutdownQueueHead && MmIsAddressValid((PVOID)IopNotifyShutdownQueueHead))
			{
				PLIST_ENTRY Entry = ((PLIST_ENTRY)IopNotifyShutdownQueueHead)->Flink;
				while (MmIsAddressValid(Entry) && Entry != (PLIST_ENTRY)IopNotifyShutdownQueueHead)
				{
					ULONG ValidAddress = (ULONG)Entry + sizeof(LIST_ENTRY);

					if (ValidAddress &&
						MmIsAddressValid((PVOID)ValidAddress))
					{
						ULONG DeviceObject = *(PULONG)ValidAddress;

						if (DeviceObject &&
							MmIsAddressValid((PVOID)DeviceObject))
						{
							if (GetCallback->ulCnt > GetCallback->ulRetCnt)
							{
								GetCallback->Callbacks[GetCallback->ulRetCnt].Type = NotifyShutdown;
								GetCallback->Callbacks[GetCallback->ulRetCnt].CallbackAddress = GetShutdownDispatch((PDEVICE_OBJECT)DeviceObject);
								GetCallback->Callbacks[GetCallback->ulRetCnt].Note = DeviceObject;
							}

							GetCallback->ulRetCnt++;
						}
					}

					Entry = Entry->Flink;
				}
			}
			break;
		}
		}

	}

}

BOOLEAN GetCreateThreadCallbackNotify(PGET_CALLBACK GetCallback)
{
	PsSetCreateThreadNotifyRoutineAddress =
		(pfnPsSetCreateThreadNotifyRoutine)GetFunctionAddressByName(L"PsSetCreateThreadNotifyRoutine");
	DbgPrint("%p\r\n", PsSetCreateThreadNotifyRoutineAddress);
	PspCreateThreadNotifyRoutineAddress =
		FindPspCreateThreadNotifyRoutine((ULONG_PTR)PsSetCreateThreadNotifyRoutineAddress);
	DbgPrint("%p\r\n", PspCreateThreadNotifyRoutineAddress);


	if (!PspCreateThreadNotifyRoutineAddress)
	{
		return FALSE;
	}

	else
	{

		ULONG i = 0;


		switch (WinVersion)
		{
		case WINDOWS_7:
		{

			SYSTEM_ADDRESS_START = 0x80000000000;

			for (i = 0; i < 64; i++)
			{
				ULONG64 NotifyItem = 0, CallBackAddress = 0;

				if (!MmIsAddressValid((PVOID)(PspCreateThreadNotifyRoutineAddress + i * sizeof(ULONG64))))
				{
					break;
				}

				NotifyItem = *(PULONG64)(PspCreateThreadNotifyRoutineAddress + i * sizeof(ULONG64));

				if (!(NotifyItem > SYSTEM_ADDRESS_START && MmIsAddressValid((PVOID)(NotifyItem & 0xfffffffffffffff8))))
				{
					break;
				}

				CallBackAddress = *((PULONG64)(NotifyItem & 0xfffffffffffffff8));

				if (CallBackAddress && MmIsAddressValid((PVOID)CallBackAddress))
				{
					if (GetCallback->ulCnt > GetCallback->ulRetCnt)
					{
						GetCallback->Callbacks[GetCallback->ulRetCnt].Type = NotifyCreateThread;
						GetCallback->Callbacks[GetCallback->ulRetCnt].CallbackAddress = CallBackAddress;
						GetCallback->Callbacks[GetCallback->ulRetCnt].Note = NotifyItem;
					}

					GetCallback->ulRetCnt++;
				}



			}
			break;
		}

		case WINDOWS_XP:
		{

			SYSTEM_ADDRESS_START = 0x80000000;
			for (i = 0; i < 64; i++)
			{
				ULONG32 NotifyItem = 0, CallBackAddress = 0;

				if (!MmIsAddressValid((PVOID)(PspCreateThreadNotifyRoutineAddress + i * sizeof(ULONG32))))
				{
					break;
				}

				NotifyItem = *(PULONG32)(PspCreateThreadNotifyRoutineAddress + i * sizeof(ULONG32));
				if (!(NotifyItem > SYSTEM_ADDRESS_START && MmIsAddressValid((PVOID)(NotifyItem & 0xFFFFFFF8 + sizeof(ULONG32)))))
				{
					break;
				}

				CallBackAddress = *(PULONG32)(NotifyItem & 0xFFFFFFF8 + sizeof(ULONG));

				if (CallBackAddress && MmIsAddressValid((PVOID)CallBackAddress))
				{
					if (GetCallback->ulCnt > GetCallback->ulRetCnt)
					{
						GetCallback->Callbacks[GetCallback->ulRetCnt].Type = NotifyCreateThread;
						GetCallback->Callbacks[GetCallback->ulRetCnt].CallbackAddress = CallBackAddress;
						GetCallback->Callbacks[GetCallback->ulRetCnt].Note = NotifyItem;
					}

					GetCallback->ulRetCnt++;
				}
			}
			break;
		}
		}

	}

}


ULONG_PTR FindPspLoadImageNotifyRoutine(ULONG_PTR Address)
{
	ULONG_PTR i = 0;

	/*
	XP:
	kd> u PsSetLoadImageNotifyRoutine l 20
	nt!PsSetLoadImageNotifyRoutine:
	805d0f90 8bff            mov     edi,edi
	805d0f92 55              push    ebp
	805d0f93 8bec            mov     ebp,esp
	805d0f95 53              push    ebx
	805d0f96 57              push    edi
	805d0f97 33ff            xor     edi,edi
	805d0f99 57              push    edi
	805d0f9a ff7508          push    dword ptr [ebp+8]
	805d0f9d e8faca0300      call    nt!ExAllocateCallBack (8060da9c)
	805d0fa2 8bd8            mov     ebx,eax
	805d0fa4 3bdf            cmp     ebx,edi
	805d0fa6 7507            jne     nt!PsSetLoadImageNotifyRoutine+0x1f (805d0faf)
	805d0fa8 b89a0000c0      mov     eax,0C000009Ah
	805d0fad eb2a            jmp     nt!PsSetLoadImageNotifyRoutine+0x49 (805d0fd9)
	805d0faf 56              push    esi
	805d0fb0 bee0495680      mov     esi,offset nt!PspLoadImageNotifyRoutine (805649e0)
	805d0fb5 6a00            push    0
	805d0fb7 53              push    ebx
	805d0fb8 56              push    esi
	805d0fb9 e80ecb0300      call    nt!ExCompareExchangeCallBack (8060dacc)
	805d0fbe 84c0            test    al,al
	805d0fc0 751d            jne     nt!PsSetLoadImageNotifyRoutine+0x4f (805d0fdf)
	805d0fc2 83c704          add     edi,4
	805d0fc5 83c604          add     esi,4
	805d0fc8 83ff20          cmp     edi,20h
	805d0fcb 72e8            jb      nt!PsSetLoadImageNotifyRoutine+0x25 (805d0fb5)
	805d0fcd 53              push    ebx
	805d0fce e83f1a0000      call    nt!RtlpFreeAtom (805d2a12)
	805d0fd3 b89a0000c0      mov     eax,0C000009Ah
	805d0fd8 5e              pop     esi
	805d0fd9 5f              pop     edi
	805d0fda 5b              pop     ebx
	kd> u 805649e0
	nt!PspLoadImageNotifyRoutine:
	805649e0 0000            add     byte ptr [eax],al
	805649e2 0000            add     byte ptr [eax],al
	805649e4 0000            add     byte ptr [eax],al
	805649e6 0000            add     byte ptr [eax],al
	805649e8 0000            add     byte ptr [eax],al
	805649ea 0000            add     byte ptr [eax],al
	805649ec 0000            add     byte ptr [eax],al
	805649ee 0000            add     byte ptr [eax],al

	Win7

	kd> u PsSetLoadImageNotifyRoutine l 20
	nt!PsSetLoadImageNotifyRoutine:
	fffff800`0429db60 48895c2408      mov     qword ptr [rsp+8],rbx
	fffff800`0429db65 57              push    rdi
	fffff800`0429db66 4883ec20        sub     rsp,20h
	fffff800`0429db6a 33d2            xor     edx,edx
	fffff800`0429db6c e8efaffeff      call    nt!ExAllocateCallBack (fffff800`04288b60)
	fffff800`0429db71 488bf8          mov     rdi,rax
	fffff800`0429db74 4885c0          test    rax,rax
	fffff800`0429db77 7507            jne     nt!PsSetLoadImageNotifyRoutine+0x20 (fffff800`0429db80)
	fffff800`0429db79 b89a0000c0      mov     eax,0C000009Ah
	fffff800`0429db7e eb4a            jmp     nt!PsSetLoadImageNotifyRoutine+0x6a (fffff800`0429dbca)
	fffff800`0429db80 33db            xor     ebx,ebx
	fffff800`0429db82 488d0d7799d9ff  lea     rcx,[nt!PspLoadImageNotifyRoutine (fffff800`04037500)]


	kd> u fffff800`04037500
	nt!PspLoadImageNotifyRoutine:
	fffff800`04037500 ff6014          jmp     qword ptr [rax+14h]
	fffff800`04037503 00a0f8ffff00    add     byte ptr [rax+0FFFFF8h],ah
	fffff800`04037509 0000            add     byte ptr [rax],al
	fffff800`0403750b 0000            add     byte ptr [rax],al
	fffff800`0403750d 0000            add     byte ptr [rax],al
	fffff800`0403750f 0000            add     byte ptr [rax],al
	fffff800`04037511 0000            add     byte ptr [rax],al
	fffff800`04037513 0000            add     byte ptr [rax],al



	*/
	switch (WinVersion)
	{
	case WINDOWS_7:
	{
		for (i = Address; i<Address + 0xFF; i++)
		{
			if (*(PUCHAR)i == 0x48 && *(PUCHAR)(i + 1) == 0x8d && *(PUCHAR)(i + 2) == 0x0d)	//lea rcx,xxxx
			{
				LONG OffsetAddr = 0;
				memcpy(&OffsetAddr, (PUCHAR)(i + 3), 4);
				return OffsetAddr + 7 + i;
			}
		}
		break;
	}

	case WINDOWS_XP:
	{
		for (i = Address; i<Address + 0xFF; i++)
		{
			if (*(PUCHAR)i == 0x56 && *(PUCHAR)(i + 1) == 0xbe)	//lea rcx,xxxx
			{
				LONG OffsetAddr = 0;
				memcpy(&OffsetAddr, (PUCHAR)(i + 2), 4);
				return OffsetAddr;
			}
		}
		break;
	}
	}
	return 0;
}

ULONG_PTR CmpCallBackVector(ULONG_PTR Address)
{
	ULONG_PTR i = 0;
	switch (WinVersion)
	{
	case WINDOWS_7:
	{
		for (i = Address; i<Address + 0xFF; i++)
		{
			if (*(PUCHAR)i == 0x48 && *(PUCHAR)(i + 1) == 0x8d && *(PUCHAR)(i + 2) == 0x0d)
			{
				ULONG_PTR j = 0;
				j = i - 5;
				if (*(PUCHAR)j == 0x48 && *(PUCHAR)(j + 1) == 0x8d && *(PUCHAR)(j + 2) == 0x54)
				{
					LONG OffsetAddr = 0;

					memcpy(&OffsetAddr, (PVOID)(i + 3), 4);

					return OffsetAddr + 7 + i;
				}
			}

		}
		break;
	}

	case WINDOWS_XP:
	{
		for (i = Address; i<Address + 0xFF; i++)
		{
			if (*(PUCHAR)i == 0xbb && *(PUCHAR)(i - 1) == 0x57)
			{
				LONG OffsetAddr = 0;
				memcpy(&OffsetAddr, (PUCHAR)(i + 1), 4);
				return OffsetAddr;
			}
		}
		break;
	}
	}
	return 0;
}

ULONG_PTR FindKeBugCheckReasonCallbackListHeadNotifyRoutine(ULONG_PTR Address)
{
	ULONG_PTR i = 0;

	switch (WinVersion)
	{
	case WINDOWS_7:
	{
		for (i = Address; i<Address + 0xFFF; i++)
		{
			if (*(PUCHAR)i == 0x48 && *(PUCHAR)(i + 1) == 0x8b && *(PUCHAR)(i + 2) == 0x05)
			{
				LONG OffsetAddr = 0;
				memcpy(&OffsetAddr, (PUCHAR)(i + 3), 4);
				return OffsetAddr + 7 + i;
			}
		}
		break;
	}

	case WINDOWS_XP:
	{
		for (i = Address; i<Address + 0xFF; i++)
		{
			if (*(PUCHAR)i == 0x8b && *(PUCHAR)(i + 1) == 0x0d)
			{
				LONG OffsetAddr = 0;
				memcpy(&OffsetAddr, (PUCHAR)(i + 2), 4);
				return OffsetAddr;
			}
		}
		break;
	}
	}



	return 0;
}

ULONG_PTR FindIopNotifyShutdownQueueHeadNotifyRoutine(ULONG_PTR Address)
{
	ULONG_PTR i = 0;

	switch (WinVersion)
	{
	case WINDOWS_7:
	{
		for (i = Address; i<Address + 0xFF; i++)
		{
			if (*(PUCHAR)i == 0x48 && *(PUCHAR)(i + 1) == 0x8d && *(PUCHAR)(i + 2) == 0x0d)
			{
				LONG OffsetAddr = 0;
				memcpy(&OffsetAddr, (PUCHAR)(i + 3), 4);
				return OffsetAddr + 7 + i;
			}
		}
		break;
	}

	case WINDOWS_XP:
	{
		for (i = Address; i<Address + 0xFF; i++)
		{
			if (*(PUCHAR)i == 0xb9 && *(PUCHAR)(i - 1) == 0xd7)
			{
				LONG OffsetAddr = 0;
				memcpy(&OffsetAddr, (PUCHAR)(i + 1), 4);
				return OffsetAddr;
			}
		}
		break;
	}
	}

	return 0;
}

ULONG_PTR FindPspCreateThreadNotifyRoutine(ULONG_PTR Address)
{
	ULONG_PTR i = 0;

	switch (WinVersion)
	{
	case WINDOWS_7:
	{
		for (i = Address; i<Address + 0xFF; i++)
		{
			if (*(PUCHAR)i == 0x48 && *(PUCHAR)(i + 1) == 0x8d && *(PUCHAR)(i + 2) == 0x0d)
			{
				LONG OffsetAddr = 0;
				memcpy(&OffsetAddr, (PUCHAR)(i + 3), 4);
				return OffsetAddr + 7 + i;
			}
		}
		break;
	}

	case WINDOWS_XP:
	{
		for (i = Address; i<Address + 0xFF; i++)
		{
			if (*(PUCHAR)i == 0xbe && *(PUCHAR)(i + 1) == 0xa0)
			{
				LONG OffsetAddr = 0;
				memcpy(&OffsetAddr, (PUCHAR)(i + 1), 4);
				return OffsetAddr;
			}
		}
		break;
	}
	}



	return 0;
}

ULONG_PTR GetShutdownDispatch(PDEVICE_OBJECT DeviceObject)
{
	PDRIVER_OBJECT DriverObject = NULL;
	ULONG_PTR ShutdownDispatch = 0;

	if (DeviceObject && MmIsAddressValid((PVOID)DeviceObject))
	{
		DriverObject = DeviceObject->DriverObject;
		if (DriverObject && MmIsAddressValid((PVOID)DriverObject))
		{
			ShutdownDispatch = (ULONG_PTR)DriverObject->MajorFunction[IRP_MJ_SHUTDOWN];
		}
	}
	return ShutdownDispatch;
}

NTSTATUS RemoveCallbackNotify(PVOID InBuffer)
{
	NTSTATUS Status = STATUS_SUCCESS;
	PREMOVE_CALLBACK Temp = (PREMOVE_CALLBACK)InBuffer;

	ULONG_PTR CallbackAddress = Temp->CallbackAddress;
	CALLBACK_TYPE CallBackType = Temp->NotifyType;

	if (!CallbackAddress ||
		!MmIsAddressValid((PVOID)CallbackAddress))
	{
		return STATUS_UNSUCCESSFUL;
	}

	DbgPrint("CallBackType: %d\r\n", CallBackType);
	switch (CallBackType)
	{
	case NotifyLoadImage:
	{
		DbgPrint("Remove NotifyLoadImage\r\n");
		Status = PsRemoveLoadImageNotifyRoutine((PLOAD_IMAGE_NOTIFY_ROUTINE)CallbackAddress);
		break;
	}
	case NotifyCmCallBack:
	{
		LARGE_INTEGER Cookie;
		ULONG_PTR Note = Temp->Note;
		Cookie.QuadPart = 0;

		DbgPrint("Remove NotifyCmCallBack\r\n");

		if (WinVersion == WINDOWS_XP)
		{
			Cookie = XpGetRegisterCallbackCookie(Note);
		}

		if (WinVersion == WINDOWS_7)
		{
			Cookie.QuadPart = Note;
		}

		if (Cookie.LowPart == 0 && Cookie.HighPart == 0)
		{
			return STATUS_UNSUCCESSFUL;
		}

		Status = CmUnRegisterCallback(Cookie);

		break;
	}
	case NotifyKeBugCheckReason:
	{
		PREMOVE_CALLBACK Temp = (PREMOVE_CALLBACK)InBuffer;

		ULONG_PTR Note = Temp->Note;


		if (Note != NULL&&MmIsAddressValid((PVOID)Note))
		{
			KeDeregisterBugCheckReasonCallback((PKBUGCHECK_REASON_CALLBACK_RECORD)Note);
		}

		break;
	}
	case NotifyShutdown:
	{
		LARGE_INTEGER Cookie;

		PREMOVE_CALLBACK Temp = (PREMOVE_CALLBACK)InBuffer;

		ULONG_PTR Note = Temp->Note;


		if (Note != NULL&&MmIsAddressValid((PVOID)Note))
		{
			IoUnregisterShutdownNotification((PDEVICE_OBJECT)Note);
		}

		break;
	}
	case NotifyCreateThread:
	{
		NTSTATUS Status = STATUS_SUCCESS;
		PREMOVE_CALLBACK Temp = (PREMOVE_CALLBACK)InBuffer;

		ULONG_PTR CallbackAddress = Temp->CallbackAddress;

		if (!CallbackAddress ||
			!MmIsAddressValid((PVOID)CallbackAddress) || !PsRemoveCreateThreadNotifyRoutine)
		{
			return STATUS_UNSUCCESSFUL;
		}

		Status = PsRemoveCreateThreadNotifyRoutine((PCREATE_THREAD_NOTIFY_ROUTINE)CallbackAddress);

		break;
	}
	default:
	{
		Status = STATUS_UNSUCCESSFUL;
	}
	}

	return Status;
}

LARGE_INTEGER XpGetRegisterCallbackCookie(ULONG Address)
{

	LARGE_INTEGER Cookie;
	ULONG Temp = 0;
	ULONG Item = 0;

	Cookie.QuadPart = 0;

	if (Address && MmIsAddressValid((PVOID)Address))
	{
		Item = Address & 0xFFFFFFF8;


		if (MmIsAddressValid((PVOID)Item) &&
			MmIsAddressValid((PVOID)(Item + 8)))
		{
			Temp = *(PULONG)(Item + 8);


			if (MmIsAddressValid((PVOID)Temp))
			{
				Cookie.LowPart = *(PULONG)Temp;
				Cookie.HighPart = *(PULONG)(Temp + sizeof(ULONG));
			}
		}
	}

	return Cookie;
}