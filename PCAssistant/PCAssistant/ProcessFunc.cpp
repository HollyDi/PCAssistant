#include "stdafx.h"
#include "ProcessFunc.h"

#include <afxdb.h>
#include <odbcinst.h>

#include <WinVer.h>

#pragma comment(lib, "version.lib")

extern BOOL bIsChecking;
extern HANDLE g_hDevice;
extern CWnd* g_process;


int nProcCount = 0;

DWORD WINAPI QueryProcessFunction(CListCtrl * pListCtrl)
{
	bIsChecking = TRUE;
	QueryProcessList(pListCtrl);
	bIsChecking = FALSE;
	return 0;
}

VOID QueryProcessList(CListCtrl* pListCtrl)
{
	DWORD dwReturnSize = 0;
	DWORD dwRet = 0;

	//发送IO 控制码
	//PVOID hProcessList = NULL;

	int ulItem = 0;	//进程计数

	ULONG_PTR ulPid = 0;
	ulItem = 0;

	while (1)
	{
		TRY
		{
			PROCESSINFO ProcItem = { 0 };

			dwRet = DeviceIoControl(g_hDevice,
				IOCTL(IOCTL_PROC_PROCESSLIST),
				&ulPid,
				sizeof(ULONG_PTR),
				&ProcItem,
				sizeof(PROCESSINFO),
				&dwReturnSize,
				NULL);

			if (dwReturnSize == 0)
			{
				break;
			}
			else
			{
				//发送状态栏消息响应
				CString csStatusMsg;
				csStatusMsg.Format(L"进程列表正在加载。 进程数：%d",ulItem);

				SendStatusDetail(csStatusMsg.GetBuffer());
			}

			if (ProcItem.Eprocess == 0 || ProcItem.Eprocess == NULL)
			{
				break;
			}

			if ((_wcsnicmp(ProcItem.Path,L"UnKnow",wcslen(L"UnKnow")) == 0 && ProcItem.Pid>4))
			{
				goto CONTINUE;
			}

			if (ProcItem.Pid == 0)
			{
				StringCchCopyW(ProcItem.Name,wcslen(L"System Idle Process") + 1,L"System Idle Process");
				StringCchCopyW(ProcItem.Path,wcslen(L"System Idle Process") + 1,L"System Idle Process");
				ProcItem.UserAccess = FALSE;
				ProcItem.PPid = 0xffffffff;
				StringCchCopyW(ProcItem.CompanyName,wcslen(L" ") + 1,L" ");

			}
			else if (ProcItem.Pid == 4)	//System进程
			{
				StringCchCopyW(ProcItem.Name,wcslen(L"System") + 1,L"System");

				WCHAR wzTempDir[260] = { 0 };
				CString Temp;
				GetEnvironmentVariableW(L"windir",wzTempDir,MAX_PATH);

				Temp = wzTempDir;
				Temp += L"\\System32\\ntoskrnl.exe";


				StringCchCopyW(ProcItem.Path,Temp.GetLength() + 1,Temp.GetBuffer());
				ProcItem.UserAccess = FALSE;
				ProcItem.PPid = 0xffffffff;
				StringCchCopyW(ProcItem.CompanyName,
					GetFileCompanyName(ProcItem.Path).GetLength() + 1,
					GetFileCompanyName(ProcItem.Path).GetBuffer());
			}
			else
			{
				WCHAR* szImageFileName = NULL;
				szImageFileName = wcsrchr(ProcItem.Path,'\\');
				szImageFileName++;

				StringCchCopyW(ProcItem.Name,wcslen(szImageFileName) + 1,szImageFileName);

				ProcItem.PPid = ProcItem.PPid;

				if (R3CanOpenProcess((DWORD)ProcItem.Pid) == TRUE)
				{
					ProcItem.UserAccess = TRUE;
				}
				else
				{
					ProcItem.UserAccess = FALSE;
				}

				CString Path, CompanyName;
				Path = ProcItem.Path;
				CompanyName = GetFileCompanyName(Path);
				if (CompanyName.GetLength() == 0)
				{
					CompanyName = L" ";
				}

				StringCchCopyW(ProcItem.CompanyName,CompanyName.GetLength() + 1,CompanyName.GetBuffer());

				if (Is64BitWindows())
				{
					if (_wcsnicmp(ProcItem.CompanyName,L"Microsoft Corporation",wcslen(L"Microsoft Corporation")) != 0)
					{
						if (Is32BitFile(ProcItem.Path) == TRUE)
						{
							CString Name;
							Name = ProcItem.Name;
							Name += L" *32";
							StringCchCopyW(ProcItem.Name,Name.GetLength() + 1,Name.GetBuffer());
						}
					}
				}
			}

			ProcItem.Pid = ProcItem.Pid;

			ProcItem.Eprocess = ProcItem.Eprocess;

			//////////////////////////////////////////////////////////////////////////
			// 开始插入列表

			PPROCESSINFO pProcessInfo = &ProcItem;

			::SendMessageW(g_process->m_hWnd,PROCESSDIG_SEND_INSERT,sizeof(PROCESSINFO),(LPARAM)&pProcessInfo);

			//插入列表结束
			//////////////////////////////////////////////////////////////////////////

		CONTINUE:


			ulPid = ProcItem.Pid + 4;

			ulItem = ulItem + 1;

		}
		CATCH(CMemoryException, e)
		{

		}
		END_CATCH

	}


	bIsChecking = FALSE;

	CString csStatusMsg;
	csStatusMsg.Format(L"进程列表加载完成。 进程数：%d", ulItem);

	SendStatusDetail(csStatusMsg.GetBuffer());

	nProcCount = (int)ulItem;
}


CString GetFileCompanyName(CString strPath)
{
	CString strCompanyName = 0;;

	if (strPath.IsEmpty())
	{
		return NULL;
	}

	if (!strPath.CompareNoCase(L"Idle") || !strPath.CompareNoCase(L"System"))
	{
		return NULL;
	}

	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;

	LPWSTR lpstrFilename = (LPWSTR)(LPCWSTR)strPath;
	DWORD dwHandle = 0;
	DWORD dwVerInfoSize = GetFileVersionInfoSize(lpstrFilename, &dwHandle);

	if (dwVerInfoSize)
	{
		LPVOID Buffer = malloc(sizeof(char)*dwVerInfoSize);

		if (Buffer)
		{
			if (GetFileVersionInfo(lpstrFilename, dwHandle, dwVerInfoSize, Buffer))
			{
				UINT cbTranslate = 0;

				if (VerQueryValue(Buffer, L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &cbTranslate))
				{
					LPCWSTR lpwszBlock = 0;
					UINT    cbSizeBuf = 0;
					WCHAR   wzSubBlock[MAX_PATH] = { 0 };

					if ((cbTranslate / sizeof(struct LANGANDCODEPAGE)) > 0)
					{
						StringCchPrintf(wzSubBlock, sizeof(wzSubBlock) / sizeof(WCHAR),
							L"\\StringFileInfo\\%04x%04x\\CompanyName", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
					}

					if (VerQueryValue(Buffer, wzSubBlock, (LPVOID*)&lpwszBlock, &cbSizeBuf))
					{
						WCHAR wzCompanyName[MAX_PATH] = { 0 };

						StringCchCopy(wzCompanyName, MAX_PATH / sizeof(WCHAR), (LPCWSTR)lpwszBlock);   //将系统中内存的数据拷贝到我们自己内存当中
						strCompanyName = wzCompanyName;
					}
				}
			}

			free(Buffer);
		}
	}

	return strCompanyName;
}


BOOL R3CanOpenProcess(DWORD dwPid)
{
	BOOL bOpen = TRUE;

	DebugPrivilege(SE_DEBUG_NAME, TRUE);

	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | PROCESS_VM_OPERATION, TRUE, dwPid);

	DebugPrivilege(SE_DEBUG_NAME, FALSE);
	if (hProcess)
	{
		CloseHandle(hProcess);
	}
	else
	{
		bOpen = FALSE;
	}

	return bOpen;
}

BOOL DebugPrivilege(const WCHAR *pName, BOOL bEnable)
{
	BOOL              bResult = TRUE;
	HANDLE            hToken;
	TOKEN_PRIVILEGES  TokenPrivileges;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		bResult = FALSE;
		return bResult;
	}
	TokenPrivileges.PrivilegeCount = 1;
	TokenPrivileges.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;

	LookupPrivilegeValue(NULL, pName, &TokenPrivileges.Privileges[0].Luid);
	AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
	if (GetLastError() != ERROR_SUCCESS)
	{
		bResult = FALSE;
	}

	CloseHandle(hToken);
	return bResult;
}


BOOL __stdcall Is32BitFile(const WCHAR * pwszFullPath)
{
	FILE * peFile = NULL;
	_wfopen_s(&peFile, pwszFullPath, L"rb");
	if (peFile == NULL)
	{
		fclose(peFile);
		return -1;
	}

	IMAGE_DOS_HEADER imageDosHeader;
	fread(&imageDosHeader, sizeof(IMAGE_DOS_HEADER), 1, peFile);
	if (imageDosHeader.e_magic != IMAGE_DOS_SIGNATURE)
	{
		fclose(peFile);
		return FALSE;
	}

	IMAGE_NT_HEADERS imageNtHeaders;
	fseek(peFile, imageDosHeader.e_lfanew, SEEK_SET);
	fread(&imageNtHeaders, sizeof(IMAGE_NT_HEADERS), 1, peFile);
	fclose(peFile);
	if (imageNtHeaders.Signature != IMAGE_NT_SIGNATURE)
	{
		return FALSE;
	}

	if (imageNtHeaders.FileHeader.Machine == IMAGE_FILE_MACHINE_I386)
	{
		return TRUE;
	}
	// 	if (imageNtHeaders.FileHeader.Machine == IMAGE_FILE_MACHINE_IA64 ||
	// 		imageNtHeaders.FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64)
	// 	{
	// 		return FALSE;	//64BIT
	// 	}

	return FALSE;
}

VOID LocationExplorer(CString strPath)
{
	if (!strPath.IsEmpty() && PathFileExists(strPath))
	{
		CString strCmd;
		strCmd.Format(L"/select,%s", strPath);
		ShellExecuteW(NULL, L"open", L"explorer.exe", strCmd, NULL, SW_SHOW);
	}
	else
	{
		MessageBox(NULL, L"文件路径错误", NULL, MB_OK | MB_ICONWARNING);
	}
}

BOOL KillProcessByForce(CListCtrl* m_ListCtrl)
{
	POSITION pos = m_ListCtrl->GetFirstSelectedItemPosition();

	ULONG_PTR ulProcessId = 0;

	int nItem = 0;

	while (pos)
	{
		nItem = m_ListCtrl->GetNextSelectedItem(pos);

		ulProcessId = _ttoi(m_ListCtrl->GetItemText(nItem, PROCESS_COLUMN_PID).GetBuffer());

		if (ulProcessId <= 4)
		{
			return FALSE;
		}

	}

	if (g_hDevice == NULL)
	{
		return FALSE;
	}

	ULONG dwReturnSize = 0;
	ULONG dwRet = 0;

	BOOL bFeedback = FALSE;


	dwRet = DeviceIoControl(g_hDevice, IOCTL(IOCTL_PROC_KILLPROCESSBYFORCE),
		&ulProcessId,
		sizeof(ULONG_PTR),
		&bFeedback,
		sizeof(BOOL),
		&dwReturnSize,
		NULL);

	//发送IO 控制码

	if (bFeedback = TRUE)
	{
		m_ListCtrl->DeleteItem(nItem);
	}
	else
	{
		MessageBox(NULL, L"关闭进程失败。", L"天影卫士", 0);
	}

	bIsChecking = FALSE;

	return bFeedback;

}


VOID CheckAttribute(CString strPath)
{
	if (!strPath.IsEmpty())
	{
		SHELLEXECUTEINFO ExecInfor;
		memset(&ExecInfor, 0, sizeof(ExecInfor));
		ExecInfor.fMask = SEE_MASK_INVOKEIDLIST;
		ExecInfor.cbSize = sizeof(ExecInfor);
		ExecInfor.hwnd = NULL;
		ExecInfor.lpVerb = _T("properties");
		ExecInfor.lpFile = strPath;
		ExecInfor.lpParameters = NULL;
		ExecInfor.lpDirectory = NULL;
		ExecInfor.nShow = SW_SHOWNORMAL;
		ExecInfor.hProcess = NULL;
		ExecInfor.lpIDList = 0;
		ExecInfor.hInstApp = 0;
		ShellExecuteEx(&ExecInfor);
	}
	else
	{
		MessageBox(NULL, L"文件路径错误", NULL, MB_OK | MB_ICONWARNING);
	}
}