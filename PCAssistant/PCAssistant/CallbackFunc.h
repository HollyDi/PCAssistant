#pragma once
#include "stdafx.h"

typedef enum _CALLBACK_TYPE_
{
	NotifyCreateProcess,
	NotifyCreateThread,
	NotifyLoadImage,
	NotifyShutdown,
	NotifyCmCallBack,
	NotifyKeBugCheckReason,
	NotifyKeBugCheck
}CALLBACK_TYPE;

typedef struct _CALLBACK_INFO_
{
	CALLBACK_TYPE Type;
	ULONG_PTR     CallbackAddress;
	ULONG_PTR     Note;
}CALLBACK_INFO, *PCALLBACK_INFO;

typedef struct _GET_CALLBACK_
{
	ULONG_PTR ulCnt;
	ULONG_PTR ulRetCnt;
	CALLBACK_INFO Callbacks[1];
}GET_CALLBACK, *PGET_CALLBACK;

typedef struct _REMOVE_CALLBACK
{
	CALLBACK_TYPE NotifyType;
	ULONG_PTR     CallbackAddress;
	ULONG_PTR     Note;
}REMOVE_CALLBACK, *PREMOVE_CALLBACK;


VOID InitCallBackList(CListCtrl *m_ListCtrl);
VOID LoadCallBackList(CListCtrl *m_ListCtrl);
VOID QueryCallBackList(CListCtrl *m_ListCtrl);
BOOL QueryCallBackList();
VOID InsertCallbackItem(CListCtrl* m_ListCtrl);
VOID RemoveCallBackItem(CListCtrl* m_ListCtrl);