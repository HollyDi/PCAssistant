#pragma once

#include <ntifs.h>
#include "common.h"


#define MAX_PATH 260

NTSTATUS DispatchControlForSystem(PIO_STACK_LOCATION  IrpSp, PVOID OutputBuffer, ULONG_PTR* ulRet);
VOID InitSystemGlobalVariable();
