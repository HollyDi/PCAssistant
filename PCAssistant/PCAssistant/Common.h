#pragma once
#include "stdafx.h"
#include <WinIoCtl.h>
#include <winsvc.h>

typedef struct _COLUMN_STRUCT
{
	WCHAR*	szTitle;
	UINT    nWidth;
}COLUMN_STRUCT;

enum ENUM_DLG_MSG	//子对话框、主对话框之间通信枚举
{
	MESSAGE_STATUSDETAIL = 5000,	//修改状态栏详细信息
	MESSAGE_STATUSTIP
};

enum HS_ENUM_IOCTL
{
	IOCTL_PROC = 0x100,					//进程相关
	IOCTL_PROC_SENDSELFPID,				//发送自己的PID到驱动层
	IOCTL_PROC_PROCESSCOUNT,			//进程计数
	IOCTL_PROC_PROCESSLIST,				//列举进程列表
	IOCTL_PROC_PROTECTPROCESS,			//保护进程
	IOCTL_PROC_KILLPROCESSBYFORCE,		//强制关闭进程
	IOCTL_PROC_PROCESSTHREAD,			//线程
	IOCTL_PROC_PROCESSTHREADMODULE,		//线程模块
	IOCTL_PROC_PROCESSPRIVILEGE,			//列举进程权限
	IOCTL_PROC_PRIVILEGE_ADJUST,			//改变进程权限
	IOCTL_PROC_PROCESSHANDLE,			//句柄
	IOCTL_PROC_PROCESSWINDOW,			//窗口
	IOCTL_PROC_PROCESSMODULE,			//进程模块
	IOCTL_PROC_PROCESSMEMORY,			//进程内存

	IOCTL_MODU = 0x180,					//模块相关
	IOCTL_MODU_MODULELIST,				//列举系统模块列表
	IOCTL_MODU_REMOVEMODULE,				//卸载系统模块

	IOCTL_KRNL = 0x200,					//内核钩子相关
	IOCTL_KRNL_SSDTLIST,					//列举SSDT列表
	IOCTL_KRNL_KISRVTAB,					//获取服务表
	IOCTL_KRNL_RESUMESSDT,				//恢复SSDT函数
	IOCTL_KRNL_SSSDTLIST,				//列举SSSDT列表
	IOCTL_KRNL_WIN32KSERVICE,			//获取Win32k服务表
	IOCTL_KRNL_KRNLFILE,					//获取内核文件		//具体哪个文件在InputBuffer的布尔型中确定
	IOCTL_KRNL_KRNLIAT,					//获取内核导入表
	IOCTL_KRNL_KRNLEAT,					//获取内核导出表

	IOCTL_SYSK = 0x280,					//内核相关
	IOCTL_SYSK_SYSTHREAD,				//内核线程
	IOCTL_SYSK_IOTIMER,					//IOTIMER
	IOCTL_SYSK_OPERIOTIMER,				//IOTIMER切换
	IOCTL_SYSK_REMOVEIOTIMER,			//IOTIMER移除
	IOCTL_SYSK_CALLBACKLIST,				//系统回调：驱动加载
	IOCTL_SYSK_REMOVECALLBACK,			//删除系统回调
	IOCTL_SYSK_DPCTIMER,					//DPCTimer
	IOCTL_SYSK_REMOVEDPCTIMER,			//DPCTimer删除
	IOCTL_SYSK_FILTERDRIVER,				//过滤驱动
	IOCTL_SYSK_FILTERUNLOAD,				//卸载过滤驱动
};

typedef enum _WIN_VERSION
{
	WindowsNT,
	Windows2000,
	WindowsXP,
	Windows2003,
	WindowsVista,
	Windows7,
	Windows8,
	Windows8_1,
	Windows10,
	WinUnknown
}WIN_VERSION;

enum DLG_NUM
{
	DIALOG_PROCESS = 0,
	DIALOG_MOUDLE,
	DIALOG_SYSTEM,
	DIALOG_TIME,

};

#define DEVICE_NAME			L"\\Device\\PCAssistantDevice"
#define LINK_NAME			L"\\\\.\\PCAssistantLink"

#define IOCTL(i)			 \
	CTL_CODE                 \
	(                        \
	FILE_DEVICE_UNKNOWN,     \
	i,						 \
	METHOD_NEITHER,          \
	FILE_ANY_ACCESS          \
	)

BOOL Is64BitWindows();
BOOL LoadNTDriver(WCHAR* lpszDriverName, WCHAR* lpszDriverPath);
BOOL UnloadNTDriver(WCHAR* szSvrName);
VOID SendStatusDetail(LPCWSTR szBuffer);
VOID SendStatusTip(LPCWSTR szBuffer);
WIN_VERSION  GetWindowsVersion();
CString TrimPath(WCHAR * wzPath);
CString GetLongPath(CString szPath);
ULONG_PTR GetKernelBase(char* szNtosName);
char *Strcat(char *Str1, char *Str2);
CHAR* GetSystemDir();
DWORD FileLen(char* szFileName);
CHAR* LoadDllContext(char* szFileName);
int Reloc(ULONG_PTR NewBase, ULONG_PTR OrigBase);
CHAR *GetTempNtdll();
DWORD GetSpecialIndex(char *FunctionName);
