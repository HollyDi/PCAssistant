#pragma once

#if DBG
#define dprintf DbgPrint
#else
#define dprintf
#endif

#include <ntifs.h>
#include "common.h"

#include "Process.h"

ULONG_PTR KeGetObjectType(PVOID Object);

typedef
ULONG_PTR(*pfnObGetObjectType)(PVOID pObject);
NTSTATUS EnumProcessPrivilege(WCHAR* szOutBuffer, ULONG_PTR ProcessID, ULONG_PTR ulOutSize);
