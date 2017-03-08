#pragma once
#include "stdafx.h"


typedef struct _IO_TIMERS_
{
	ULONG_PTR TimerObject;
	ULONG_PTR DeviceObject;
	ULONG_PTR TimeDispatch;
	ULONG_PTR TimerEntry;
	ULONG     Status;
}IO_TIMERS, *PIO_TIMERS;

typedef struct _IO_TIMER_INFOR_
{
	ULONG ulCnt;
	ULONG ulRetCnt;
	IO_TIMERS IoTimer[1];
}IO_TIMER_INFOR, *PIO_TIMER_INFOR;

typedef struct _PCOMMUNICATE_IO_TIMER_
{
	PLIST_ENTRY     TimerEntry;
	ULONG_PTR       DeviceObject;
	BOOLEAN         bStart;
}COMMUNICATE_IO_TIMER, *PCOMMUNICATE_IO_TIMER;


VOID InitIOTimerList(CListCtrl *m_ListCtrl);

VOID LoadIOTimerList(CListCtrl *m_ListCtrl);

VOID QueryIOTimerList(CListCtrl *m_ListCtrl);

VOID RemoveIOTimerItem(CListCtrl* m_ListCtrl);

VOID OperIOTimer(CListCtrl* m_ListCtrl);
