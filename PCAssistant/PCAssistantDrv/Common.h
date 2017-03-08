#pragma once 

#include <ntifs.h>

#define DEVICE_NAME                  L"\\Device\\PCAssistantDevice"
#define LINK_NAME                    L"\\DosDevices\\PCAssistantLink"

enum ENUM_IOCTL
{
	IOCTL_PROC = 0x100,				//进程相关
	IOCTL_PROC_SENDSELFPID,			//发送自己的PID到驱动层
	IOCTL_PROC_PROCESSCOUNT,				//进程计数
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

typedef
NTSTATUS
(*pfnRtlGetVersion)(OUT PRTL_OSVERSIONINFOW lpVersionInformation);

typedef enum WIN_VERSION {
	WINDOWS_UNKNOW,
	WINDOWS_XP,
	WINDOWS_7,
	WINDOWS_8,
	WINDOWS_8_1
} WIN_VERSION;

WIN_VERSION GetWindowsVersion();
PVOID GetFunctionAddressByName(WCHAR *szFunction);
BOOLEAN IsUnicodeStringValid(PUNICODE_STRING uniString);