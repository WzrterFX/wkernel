#ifndef WKERNEL_H
#define WKERNEL_H

#include <ntifs.h>
#include <ntddk.h>

BOOLEAN DebugLog;

extern NTSTATUS* NTAPI PsGetProcessImageFileName(PEPROCESS process);
extern NTSTATUS NTAPI MmCopyVirtualMemory(PEPROCESS sourceProcess, PVOID sourceAddress, PEPROCESS targetProcess, PVOID targetAddress, SIZE_T bufferSize, KPROCESSOR_MODE previousMode, PSIZE_T returnSize);
extern NTSTATUS NTAPI PsLookupProcessByProcessId(HANDLE processId, PEPROCESS* process);

NTSTATUS ReadKernelMemory(HANDLE processId, DWORD64 address, PVOID buffer, SIZE_T size);
NTSTATUS WriteKernelMemory(HANDLE processId, DWORD64 address, PVOID buffer, SIZE_T size);

NTSTATUS KernelLoad(PDRIVER_OBJECT driver, PUNICODE_STRING registry);
#pragma alloc_text(INIT, KernelLoad);

NTSTATUS KernelUnload(PDRIVER_OBJECT driver);
#pragma alloc_text(PAGE, KernelUnload);

#endif