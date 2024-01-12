#include "wkernel.h"

#define DEVICE_NAME L"\\Device\\wkernel"
#define DDOS_DEVICE_NAME L"\\DosDevices\\wkernel"

#define IOCTL_READ_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_WRITE_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

NTSTATUS ReadKernelMemory(HANDLE processId, DWORD64 address, PVOID buffer, SIZE_T size)
{
    HANDLE pProcess;
    SIZE_T bytesRead;

    if (!NT_SUCCESS(PsLookupProcessByProcessId(processId, &pProcess)))
        return STATUS_NOT_FOUND;

    NTSTATUS status = MmCopyVirtualMemory(pProcess, (PVOID)address, PsGetCurrentProcess(), buffer, size, KernelMode, &bytesRead);

    if (!NT_SUCCESS(status))
        DbgPrintEx(0, 0, "failed to read %lu bytes from 0x%llX \n", size, address);
    else
       DbgPrintEx(0, 0, "successfully read %lu bytes from 0x%llX \n", bytesRead, address);

    return status;
}

NTSTATUS WriteKernelMemory(HANDLE processId, DWORD64 address, PVOID buffer, SIZE_T size)
{
    HANDLE pProcess;
    SIZE_T bytesWritten;

    if (!NT_SUCCESS(PsLookupProcessByProcessId(processId, &pProcess)))
        return STATUS_NOT_FOUND;

    NTSTATUS status = MmCopyVirtualMemory(PsGetCurrentProcess(), buffer, pProcess, (PVOID)address, size, KernelMode, &bytesWritten);

    if (!NT_SUCCESS(status))
        DbgPrintEx(0, 0, "failed to write %lu bytes to 0x%llX \n", size, address);
    else
        DbgPrintEx(0, 0, "successfully wrote %lu bytes to 0x%llX \n", bytesWritten, address);

    return status;
}

NTSTATUS WkernelControl(PDEVICE_OBJECT deviceObject, PIRP irp)
{
    NTSTATUS status = STATUS_SUCCESS;

    return status;
}

NTSTATUS KernelLoad(PDRIVER_OBJECT driver, PUNICODE_STRING registry)
{
    DbgPrintEx(0, 0, "wkernel loaded \n");

    driver->DriverUnload = KernelUnload;

    return STATUS_SUCCESS;
}

NTSTATUS KernelUnload(PDRIVER_OBJECT driver)
{
    DbgPrintEx(0, 0, "wkernel unloaded \n");

    return STATUS_SUCCESS;
}