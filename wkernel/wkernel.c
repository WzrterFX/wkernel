#include "wkernel.h"

#define DEVICE_NAME             L"\\Device\\wkernel"
#define DOS_DEVICE_NAME         L"\\DosDevices\\wkernel"
#define CLIENT_DLL              L"\csgo\bin\win64\client.dll"

#define IO_INIT_WKERNEL         CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0001, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_GET_KERNEL_MODULE    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0002, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_READ_KERNEL_MEMORY   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0003, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_WRITE_KERNEL_MEMORY  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0004, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

UNICODE_STRING deviceName;
UNICODE_STRING dosDeviceName;
PDEVICE_OBJECT device;

HANDLE cs2Id;
PEPROCESS cs2Procces;
DWORD64 client;

HANDLE cheatId;
PEPROCESS cheatProcces;

//
typedef struct _KERNEL_INIT_DATA_REQUEST
{
    DWORD64 gameId;
    DWORD64 cheatId;
    NTSTATUS result;
} KERNEL_INIT_DATA_REQUEST, * PKERNEL_INIT_DATA_REQUEST;

typedef struct _KERNEL_GET_MODULES_REQUEST
{
    DWORD64 module;
    NTSTATUS result;
} KERNEL_GET_MODULES_REQUEST, * PKERNEL_GET_MODULES_REQUEST;

typedef struct _KERNEL_READ_REQUEST
{
    DWORD64 address;
    DWORD64 response;
    SIZE_T size;
    NTSTATUS result;
} KERNEL_READ_REQUEST, PKERNEL_READ_REQUEST;

typedef struct _KERNEL_WRITE_REQUEST
{
    DWORD64 address;
    DWORD64 response;
    SIZE_T size;
    NTSTATUS result;
} KERNEL_WRITE_REQUEST, PKERNEL_WRITE_REQUEST;

NTSTATUS OpenWkernel(PDEVICE_OBJECT device, PIRP irp)
{
    irp->IoStatus.Status = STATUS_SUCCESS;
    irp->IoStatus.Information = NULL;

    IoCompleteRequest(irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

NTSTATUS CloseWkernel(PDEVICE_OBJECT device, PIRP irp)
{
    irp->IoStatus.Status = STATUS_SUCCESS;
    irp->IoStatus.Information = NULL;

    IoCompleteRequest(irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

void InitWkernelData(PIRP irp)
{
    PKERNEL_INIT_DATA_REQUEST initData = (PKERNEL_INIT_DATA_REQUEST)irp->AssociatedIrp.SystemBuffer;

    if (initData)
    {
        cs2Id = (HANDLE)initData->gameId;
        cheatId = (HANDLE)initData->cheatId;

        initData->result = PsLookupProcessByProcessId(cs2Id, &cs2Procces);
        initData->result |= PsLookupProcessByProcessId(cheatId, &cheatProcces);
    }
}

NTSTATUS WkernelControl(PDEVICE_OBJECT device, PIRP Irp)
{

}

void ImageLoadCallback(PUNICODE_STRING fullName, HANDLE processId, PIMAGE_INFO imageInfo)
{
    if (wcsstr(fullName->Buffer, CLIENT_DLL))
    {
        DbgPrintEx(0, 0, "successfully loaded client.dll with base 0x%p", imageInfo->ImageBase);

        client = (DWORD64)imageInfo->ImageBase;
    }
}

NTSTATUS ReadKernelMemory(HANDLE processId, DWORD64 address, PVOID buffer, SIZE_T size)
{
    NTSTATUS status;

    HANDLE pProcess = NULL;
    SIZE_T bytesRead;

    status = PsLookupProcessByProcessId(processId, &pProcess);
    if (!NT_SUCCESS(status))
        return DbgPrintEx(0, 0, "failed to lookup process %lu bytes at address 0x%llX \n", size, address);

    if (!MmIsAddressValid((PVOID)address))
        return DbgPrintEx(0, 0, "failed to access memory %lu bytes at address 0x%llX \n", size, address);

    status = MmCopyVirtualMemory(pProcess, (PVOID)address, cheatProcces, buffer, size, KernelMode, &bytesRead);
    if (!NT_SUCCESS(status))
        return DbgPrintEx(0, 0, "failed to read memory %lu bytes at address 0x%llX \n", size, address);

    DbgPrintEx(0, 0, "successfully read %lu bytes from 0x%llX \n", bytesRead, address);

    return status;
}

NTSTATUS WriteKernelMemory(HANDLE processId, DWORD64 address, PVOID buffer, SIZE_T size)
{
    NTSTATUS status;

    HANDLE pProcess = NULL;
    SIZE_T bytesWritten;

    status = PsLookupProcessByProcessId(processId, &pProcess);
    if (!NT_SUCCESS(status))
        return DbgPrintEx(0, 0, "failed to lookup process %lu bytes at address 0x%llX \n", size, address);

    if (!MmIsAddressValid((PVOID)address))
        return DbgPrintEx(0, 0, "failed to access memory %lu bytes at address 0x%llX \n", size, address);

    status = MmCopyVirtualMemory(cheatProcces, buffer, pProcess, (PVOID)address, size, KernelMode, &bytesWritten);
    if (!NT_SUCCESS(status))
        return DbgPrintEx(0, 0, "failed to write memory %lu bytes at address 0x%llX \n", size, address);

    DbgPrintEx(0, 0, "successfully write %lu bytes from 0x%llX \n", bytesWritten, address);

    return status;
}

NTSTATUS KernelLoad(PDRIVER_OBJECT driver, PUNICODE_STRING registry)
{
    driver->DriverUnload = KernelUnload;

    RtlInitUnicodeString(&deviceName, DEVICE_NAME);
    RtlInitUnicodeString(&dosDeviceName, DOS_DEVICE_NAME);

    if (NT_SUCCESS(IoCreateDevice(driver, 0, &deviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &device)))
    {
        IoCreateSymbolicLink(&dosDeviceName, &deviceName);
        DbgPrintEx(0, 0, "successfully bridge created \n");

        driver->MajorFunction[IRP_MJ_CREATE] = OpenWkernel;
        driver->MajorFunction[IRP_MJ_CLOSE] = CloseWkernel;
        driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = WkernelControl;

        device->Flags |= DO_DIRECT_IO;
        device->Flags &= ~DO_DEVICE_INITIALIZING;

        PsSetLoadImageNotifyRoutine(ImageLoadCallback);
    }

    DbgPrintEx(0, 0, "wkernel loaded \n");

    return STATUS_SUCCESS;
}

NTSTATUS KernelUnload(PDRIVER_OBJECT driver)
{
    PsRemoveLoadImageNotifyRoutine(ImageLoadCallback);
    IoDeleteSymbolicLink(&dosDeviceName);
    IoDeleteDevice(driver->DeviceObject);

    DbgPrintEx(0, 0, "wkernel unloaded \n");

    return STATUS_SUCCESS;
}