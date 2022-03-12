#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdbool.h>

#include "WinFunc.h"
#include "NVMeSCSIPassThrough.h"
#include "NVMeDeallocate.h"
#include "NVMeIdentify.h"
#include "NVMeIdentifyController.h"
#include "NVMeGetLogPage.h"
#include "NVMeGetFeatures.h"
#include "NVMeSetFeatures.h"
#include "NVMeDeviceSelftest.h"
#include "NVMeFormatNVM.h"
#include "NVMeSMART.h"
#include "NVMeUtils.h"





void vPrintSystemError(unsigned long _ulErrorCode, const char* _strFunc)
{
    WCHAR lpMsgBuf[1024];
    ULONG count;

    count = FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        _ulErrorCode,
        0x0409, // default
        (LPTSTR)&lpMsgBuf,
        sizeof(lpMsgBuf),
        NULL);

    if (count != 0)
    {
        fprintf(stderr, "[E] %s: (error code = %d) %ls\n", _strFunc, _ulErrorCode, lpMsgBuf);
    }
    else
    {
        fprintf(stderr, "[E] Format message failed.  Error: %d\n", GetLastError());
    }
}

// wrapper for DeviceIoControl()
int iIssueDeviceIoControl(
    HANDLE          _hDevice,
    DWORD           _dwControlCode,
    LPVOID          _lpInBuffer,
    DWORD           _nInBufferSize,
    LPVOID          _lpOutBuffer,
    DWORD           _nOutBufferSize,
    LPDWORD         _lpBytesReturned,
    LPOVERLAPPED    _lpOverlapped)
{
    int iResult = -1;
    BOOL iRetIoControl = 0;

    // bResult is nonzero if succeeded, zero if failed
    iRetIoControl = DeviceIoControl(
        _hDevice,
        _dwControlCode,
        _lpInBuffer,
        _nInBufferSize,
        _lpOutBuffer,
        _nOutBufferSize,
        _lpBytesReturned,
        _lpOverlapped);

    if (iRetIoControl == 0)
    {
        vPrintSystemError(GetLastError(), "DeviceIoControl");
    }
    else
    {
        iResult = 0; // succeeded
    }

    return iResult;
}

HANDLE hIssueCreateFile(const char* _strDeviceNo, TCHAR* ptchar)
{
    HANDLE hDevice;
    //TCHAR strDeviceName[256];

    _stprintf_s(ptchar, 256, _T("\\\\.\\PhysicalDrive%hs"), _strDeviceNo);
    hDevice = CreateFile(
        (LPCWSTR)ptchar,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_DEVICE | FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING,
        NULL);

    if (hDevice == INVALID_HANDLE_VALUE)
    {
        vPrintSystemError(GetLastError(), "CreateFile");
    }
    return hDevice;
}

HANDLE hIssueCreateFileEx(TCHAR* tcDevName)
{
    HANDLE hDevice;
    //TCHAR strDeviceName[256];

    hDevice = CreateFile(
        (LPCWSTR)tcDevName,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_DEVICE | FILE_FLAG_OVERLAPPED,
        NULL);

    if (hDevice == INVALID_HANDLE_VALUE)
    {
        vPrintSystemError(GetLastError(), "CreateFile");
    }
    return hDevice;
}

HANDLE hUartIssueCreateFile(const char* _strDeviceNo)
{
    HANDLE hDevice;
    TCHAR strDeviceName[256];
    _stprintf_s(strDeviceName, 256, _T("\\\\.\\%hs"), _strDeviceNo);
    hDevice = CreateFile(
        (LPCWSTR)strDeviceName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_SYSTEM | FILE_FLAG_OVERLAPPED,
        NULL);

    if (hDevice == INVALID_HANDLE_VALUE)
    {
        vPrintSystemError(GetLastError(), "CreateFile");
    }
    return hDevice;
}

static DWORD s_dwOSBuildNumber = 0;

bool bCanUseGetDeviceInternalLog(void)
{
    // if osInfo.dwBuildNumber < 19041, it's before Windows 10 May 2020 Update (2004)
    return ( s_dwOSBuildNumber < 19041 ) ? false : true;
}

typedef NTSTATUS (WINAPI* PRtlGetVersion)(PRTL_OSVERSIONINFOEXW _pInfo);

void vGetOSVersion(void)
{
    RTL_OSVERSIONINFOEXW osInfo;
    ZeroMemory(&osInfo, sizeof(RTL_OSVERSIONINFOEXW));
    osInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);

    HMODULE mod = GetModuleHandle(_T("ntdll.dll"));
    if (mod == 0)
    {
        vPrintSystemError(GetLastError(), "GetModuleHandle");
        return;
    }

    PRtlGetVersion fpGetOSVer = (PRtlGetVersion)GetProcAddress(mod, "RtlGetVersion");
    if (fpGetOSVer == NULL)
    {
        vPrintSystemError(GetLastError(), "GetProcAddress");
        return;
    }

    fpGetOSVer(&osInfo);

    // checking OS version (Windows 10)
    // see also https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_osversioninfoexw
    if ( (osInfo.dwMajorVersion == 10) &&
         (osInfo.dwMinorVersion ==  0) &&
         (osInfo.wProductType == VER_NT_WORKSTATION) ) // not Windows Server 2016/2019
    {
        s_dwOSBuildNumber = osInfo.dwBuildNumber;
        printf("[I] Running on Windows %d build %d", osInfo.dwMajorVersion, s_dwOSBuildNumber);
    }
}

