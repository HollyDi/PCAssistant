#pragma once
#include <ntifs.h>
#include "Common.h"

typedef struct _REMOVE_DPCTIMER
{
	ULONG_PTR     TimerObject;
}REMOVE_DPCTIMER, *PREMOVE_DPCTIMER;

typedef struct _DPC_TIMER_
{
	ULONG_PTR TimerObject;
	ULONG_PTR Period;			// ÖÜÆÚ
	ULONG_PTR TimeDispatch;
	ULONG_PTR Dpc;
}DPC_TIMER, *PDPC_TIMER;

typedef struct _DPC_TIMER_INFOR_
{
	ULONG ulCnt;
	ULONG ulRetCnt;
	DPC_TIMER DpcTimer[1];
}DPC_TIMER_INFOR, *PDPC_TIMER_INFOR;

NTSTATUS EnumDPCTimer(PVOID OutBuffer);
ULONG GetKiTimerTableListHeadXP();
NTSTATUS GetDpcTimerInformationWinXP(PLIST_ENTRY KiTimerTableListHead, PDPC_TIMER_INFOR DpcTimerInfor);
VOID FindKiWaitFunc(PULONG64 *KiWaitNeverAddr, PULONG64 *KiWaitAlwaysAddr);
KDPC* TransTimerDpcEx(
	IN PKTIMER InTimer,
	IN ULONGLONG InKiWaitNever,
	IN ULONGLONG InKiWaitAlways);
NTSTATUS RemoveDPCTimer(PVOID InBuffer);