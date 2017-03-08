#include "stdafx.h"
#include "IoTimerFunc.h"
#include "Common.h"
#include "SSDTFunc.h"
#include "KModuleFunc.h"
#include <vector>

#include "ProcessFunc.h"

using namespace std;
COLUMN_STRUCT g_Column_IOTimer[] =
{
	{ L"定时器对象",			125 },
	{ L"设备对象",				125 },
	{ L"状态",					45 },
	{ L"函数入口",				125 },
	{ L"模块路径",				180 },
	{ L"出品厂商",				125 }
};

ULONG_PTR m_ulIOTimerCount = 0;

UINT g_Column_IOTimer_Count = 6;

extern HANDLE g_hDevice;
extern WIN_VERSION WinVersion;
extern BOOL bIsChecking;

extern vector<DRIVER_INFO> m_DriverList;
vector<IO_TIMERS> m_IOTimerVector;

VOID InitIOTimerList(CListCtrl *m_ListCtrl)
{
	while (m_ListCtrl->DeleteColumn(0));
	m_ListCtrl->DeleteAllItems();

	m_ListCtrl->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);

	UINT i;
	for (i = 0; i<g_Column_IOTimer_Count; i++)
	{
		m_ListCtrl->InsertColumn(i, g_Column_IOTimer[i].szTitle, LVCFMT_LEFT, (int)(g_Column_IOTimer[i].nWidth));
	}
}


VOID LoadIOTimerList(CListCtrl *m_ListCtrl)
{
	if (bIsChecking == TRUE)
	{
		return;
	}

	bIsChecking = TRUE;

	SendStatusDetail(L"IOTimer正在加载...");
	SendStatusTip(L"IOTimer");

	QueryIOTimerList(m_ListCtrl);

	bIsChecking = FALSE;
}

VOID QueryIOTimerList(CListCtrl *m_ListCtrl)
{
	
	BOOL bRet = FALSE;
	
	bRet = EnumDriver();
	if (bRet == FALSE)
	{
		SendStatusDetail(L"驱动模块初始化失败...");
		return;
	}
	
	ULONG_PTR ulCnt = 100;
	PIO_TIMER_INFOR IOTimerInfor = NULL;
	DWORD ulReturnSize = 0;

	m_ListCtrl->DeleteAllItems();
	m_IOTimerVector.clear();

	do
	{
		ULONG_PTR ulSize = sizeof(IO_TIMER_INFOR) + ulCnt * sizeof(IO_TIMERS);

		if (IOTimerInfor)
		{
			free(IOTimerInfor);
			IOTimerInfor = NULL;
		}

		IOTimerInfor = (PIO_TIMER_INFOR)malloc(ulSize);

		if (IOTimerInfor)
		{
			memset(IOTimerInfor, 0, ulSize);
			IOTimerInfor->ulCnt = (ULONG)ulCnt;
			bRet = DeviceIoControl(g_hDevice, IOCTL(IOCTL_SYSK_IOTIMER),
				NULL,
				0,
				IOTimerInfor,
				(DWORD)ulSize,
				&ulReturnSize,
				NULL);
		}

		ulCnt = IOTimerInfor->ulCnt + 10;


	} while (!bRet && IOTimerInfor->ulRetCnt > IOTimerInfor->ulCnt);

	if (!bRet)
	{
		SendStatusDetail(L"IOTimer加载失败。");
		return;
	}

	if (bRet &&
		IOTimerInfor->ulCnt >= IOTimerInfor->ulRetCnt)
	{
		for (ULONG i = 0; i < IOTimerInfor->ulRetCnt; i++)
		{
			m_IOTimerVector.push_back(IOTimerInfor->IoTimer[i]);
		}
	}

	if (IOTimerInfor)
	{
		free(IOTimerInfor);
		IOTimerInfor = NULL;
	}

	//////////////////////////////////////////////////////////////////////////

	m_ulIOTimerCount = 0;

	for (vector<IO_TIMERS>::iterator itor = m_IOTimerVector.begin(); itor != m_IOTimerVector.end(); itor++)
	{
		CString strTimer, strDeviceObject, strPath, strStatus, strDispatch;

		strTimer.Format(L"0x%p", itor->TimerObject);
		strDeviceObject.Format(L"0x%p", itor->DeviceObject);
		strDispatch.Format(L"0x%p", itor->TimeDispatch);
		strPath = GetDriverPath(itor->TimeDispatch);

		if (itor->Status)
		{
			strStatus = L"运行";
		}
		else
		{
			strStatus = L"停止";
		}


		int n = m_ListCtrl->InsertItem(m_ListCtrl->GetItemCount(), strTimer);
		m_ListCtrl->SetItemText(n, 1, strDeviceObject);
		m_ListCtrl->SetItemText(n, 2, strStatus);
		m_ListCtrl->SetItemText(n, 3, strDispatch);
		m_ListCtrl->SetItemText(n, 4, strPath);
		m_ListCtrl->SetItemText(n, 5, GetFileCompanyName(strPath));

		m_ListCtrl->SetItemData(n, itor->TimerEntry);

		m_ulIOTimerCount++;

		CString StatusBarContext;
		StatusBarContext.Format(L"IOTimer正在加载。 定时器数：%d", m_ulIOTimerCount);
		SendStatusDetail(StatusBarContext);
	}

	CString StatusBarContext;
	StatusBarContext.Format(L"IOTimer加载完成。 定时器数：%d", m_ulIOTimerCount);
	SendStatusDetail(StatusBarContext);

}


VOID RemoveIOTimerItem(CListCtrl* m_ListCtrl)
{
	BOOL bRet = FALSE;
	DWORD ulReturnSize = 0;
	int Index = m_ListCtrl->GetSelectionMark();

	if (Index<0)
	{
		return;
	}

	CString Temp = m_ListCtrl->GetItemText(Index, 3);

	COMMUNICATE_IO_TIMER  IOTimer;

	for (vector <IO_TIMERS>::iterator Iter = m_IOTimerVector.begin(); Iter != m_IOTimerVector.end(); Iter++)
	{

		if (m_ListCtrl->GetItemData(Index) == Iter->TimerEntry)
		{

			IOTimer.TimerEntry = (PLIST_ENTRY)Iter->TimerEntry;

			bRet = DeviceIoControl(g_hDevice, IOCTL(IOCTL_SYSK_REMOVEIOTIMER),
				&IOTimer,
				sizeof(COMMUNICATE_IO_TIMER),
				NULL,
				0,
				&ulReturnSize,
				NULL);

			break;
		}
	}
	if (bRet)
	{
		QueryIOTimerList(m_ListCtrl);
	}

	bIsChecking = FALSE;
}

VOID OperIOTimer(CListCtrl* m_ListCtrl)
{
	BOOL bRet = FALSE;
	DWORD ulReturnSize = 0;
	int Index = m_ListCtrl->GetSelectionMark();

	if (Index<0)
	{
		return;
	}

	CString Temp = m_ListCtrl->GetItemText(Index, 1);

	COMMUNICATE_IO_TIMER  IOTimer;

	for (vector <IO_TIMERS>::iterator Iter = m_IOTimerVector.begin(); Iter != m_IOTimerVector.end(); Iter++)
	{
		CString strObject;
		strObject.Format(L"0x%p", Iter->DeviceObject);
		if (!strObject.CompareNoCase(Temp))
		{
			IOTimer.DeviceObject = Iter->DeviceObject;

			if (Iter->Status)
			{

				IOTimer.bStart = 0;
			}
			else
			{

				IOTimer.bStart = 1;
			}

			bRet = DeviceIoControl(g_hDevice, IOCTL(IOCTL_SYSK_OPERIOTIMER),
				&IOTimer,
				sizeof(COMMUNICATE_IO_TIMER),
				NULL,
				0,
				&ulReturnSize,
				NULL);

			break;
		}
	}

	if (bRet == TRUE)
	{
		QueryIOTimerList(m_ListCtrl);
	}

	bIsChecking = FALSE;
}