#include "stdafx.h"
#include "PrivilegeFunc.h"
#include "Common.h"

#include "resource.h"

extern HANDLE g_hDevice;

extern ULONG_PTR g_ulProcessId;

COLUMN_STRUCT g_Column_Privilege[] =
{
	{ L"权限名称",			170 },
	{ L"权限描述",			200 },
	{ L"权限状态",			150 }
};

UINT g_Column_Privilege_Count = 3;	  //进程列表列数

void InitPrivilegeList(CListCtrl *m_ListCtrl)
{

	while (m_ListCtrl->DeleteColumn(0));
	m_ListCtrl->DeleteAllItems();

	m_ListCtrl->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);

	UINT i;
	for (i = 0; i<g_Column_Privilege_Count; i++)
	{
		m_ListCtrl->InsertColumn(i, g_Column_Privilege[i].szTitle, LVCFMT_LEFT, (int)(g_Column_Privilege[i].nWidth));
	}
}


VOID QueryProcessPrivilege(CListCtrl *m_ListCtrl)
{

	ULONG_PTR  ProcessID = g_ulProcessId;

	ULONG dwReturnSize = 0;
	ULONG dwRet = 0;

	PVOID Temp = NULL;
	ULONG nSize = 1000;

	if (ProcessID == 0)
	{
		return;
	}

	m_ListCtrl->DeleteAllItems();

	Temp = (PTOKEN_PRIVILEGES)malloc(sizeof(char)*nSize);
	if (!Temp)
	{
		return;
	}

	dwRet = DeviceIoControl(g_hDevice, IOCTL(IOCTL_PROC_PROCESSPRIVILEGE),
		&ProcessID,
		sizeof(ULONG_PTR),
		Temp,
		nSize,
		&dwReturnSize,
		NULL);

	if (dwRet && ((PTOKEN_PRIVILEGES)Temp)->PrivilegeCount > 0)
	{
		for (ULONG i = 0; i <((PTOKEN_PRIVILEGES)Temp)->PrivilegeCount; i++)
		{
			WCHAR PrivilegeName[MAX_PATH] = { 0 };
			WCHAR DisplayName[MAX_PATH] = { 0 };
			DWORD LanguageId = 0;
			DWORD dwRet1 = MAX_PATH;
			DWORD dwRet2 = MAX_PATH;

			LookupPrivilegeName(NULL, &((PTOKEN_PRIVILEGES)Temp)->Privileges[i].Luid, PrivilegeName, &dwRet1);
			LookupPrivilegeDisplayName(NULL, PrivilegeName, DisplayName, &dwRet2, &LanguageId);


			//printf("%S\r\n", PrivilegeName);
			//wprintf(L"%s", DisplayName);

			if (wcslen(PrivilegeName) == 0)
			{
				break;
			}

			m_ListCtrl->InsertItem(i, PrivilegeName);

			m_ListCtrl->SetItemText(i, 1, DisplayName);

			if (((PTOKEN_PRIVILEGES)Temp)->Privileges[i].Attributes & 1)
			{
				//printf("\t\tDefault Enabled");
				m_ListCtrl->SetItemText(i, 2, L"Default Enabled");
			}
			else if (((PTOKEN_PRIVILEGES)Temp)->Privileges[i].Attributes & 2)
			{
				//printf("\t\tEnabled");
				m_ListCtrl->SetItemText(i, 2, L"Enabled");
			}
			else
			{
				//printf("\t\tDisabled");
				m_ListCtrl->SetItemText(i, 2, L"Disabled");
			}
		}
	}


	//发送IO 控制码

	if (dwRet == 0)
	{
		//cout<<"Send IoCode Error"<<endl;
	}
	if (Temp != NULL)
	{
		free(Temp);
	}

	return;
}