#include <ntifs.h>
#include <ntimage.h>

#define SEC_IMAGE 0x01000000

typedef struct _SYSTEM_SERVICE_TABLE64 {
	PVOID  		ServiceTableBase;
	PVOID  		ServiceCounterTableBase;
	ULONG64  	NumberOfServices;
	PVOID  		ParamTableBase;
} SYSTEM_SERVICE_TABLE64, *PSYSTEM_SERVICE_TABLE64;

typedef struct _SYSTEM_SERVICE_TABLE32 {
	PVOID   ServiceTableBase;
	PVOID   ServiceCounterTableBase;
	ULONG32 NumberOfServices;
	PVOID   ParamTableBase;
} SYSTEM_SERVICE_TABLE32, *PSYSTEM_SERVICE_TABLE32;

ULONG_PTR  GetFuncAddress(char* szFuncAddress);
ULONG_PTR GetKeServiceDescriptorTable64();
LONG GetSSDTApiFunIndex(IN LPSTR lpszFunName);
ULONG_PTR GetSSDTApiFunAddress(ULONG_PTR ulIndex, ULONG_PTR SSDTDescriptor);
ULONG_PTR GetSSDTFunctionAddress32(ULONG_PTR ulIndex, ULONG_PTR SSDTDescriptor);
ULONG_PTR GetSSDTFunctionAddress64(ULONG_PTR ulIndex, ULONG_PTR SSDTDescriptor);
NTSTATUS MapFileInUserSpace(IN LPWSTR lpszFileName, IN HANDLE ProcessHandle OPTIONAL,
	OUT PVOID *BaseAddress,
	OUT PSIZE_T ViewSize OPTIONAL);