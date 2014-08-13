/*
Cuckoo Sandbox - Automated Malware Analysis
Copyright (C) 2010-2014 Cuckoo Sandbox Developers

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include "ntapi.h"
#include "hooking.h"
#include "log.h"


HOOKDEF(SC_HANDLE, WINAPI, OpenSCManagerA,
    __in_opt  LPCTSTR lpMachineName,
    __in_opt  LPCTSTR lpDatabaseName,
    __in      DWORD dwDesiredAccess
) {
    SC_HANDLE ret = Old_OpenSCManagerA(lpMachineName, lpDatabaseName,
        dwDesiredAccess);
    LOQ_nonnull("services", "ssl", "MachineName", lpMachineName, "DatabaseName", lpDatabaseName,
        "DesiredAccess", dwDesiredAccess);
    return ret;
}

HOOKDEF(SC_HANDLE, WINAPI, OpenSCManagerW,
    __in_opt  LPWSTR lpMachineName,
    __in_opt  LPWSTR lpDatabaseName,
    __in      DWORD dwDesiredAccess
) {
    SC_HANDLE ret = Old_OpenSCManagerW(lpMachineName, lpDatabaseName,
        dwDesiredAccess);
    LOQ_nonnull("services", "uul", "MachineName", lpMachineName, "DatabaseName", lpDatabaseName,
        "DesiredAccess", dwDesiredAccess);
    return ret;
}

HOOKDEF(SC_HANDLE, WINAPI, CreateServiceA,
    __in       SC_HANDLE hSCManager,
    __in       LPCTSTR lpServiceName,
    __in_opt   LPCTSTR lpDisplayName,
    __in       DWORD dwDesiredAccess,
    __in       DWORD dwServiceType,
    __in       DWORD dwStartType,
    __in       DWORD dwErrorControl,
    __in_opt   LPCTSTR lpBinaryPathName,
    __in_opt   LPCTSTR lpLoadOrderGroup,
    __out_opt  LPDWORD lpdwTagId,
    __in_opt   LPCTSTR lpDependencies,
    __in_opt   LPCTSTR lpServiceStartName,
    __in_opt   LPCTSTR lpPassword
) {
    SC_HANDLE ret = Old_CreateServiceA(hSCManager, lpServiceName,
        lpDisplayName, dwDesiredAccess, dwServiceType, dwStartType,
        dwErrorControl, lpBinaryPathName, lpLoadOrderGroup, lpdwTagId,
        lpDependencies, lpServiceStartName, lpPassword);
    LOQ_nonnull("services", "pss4l3s", "ServiceControlHandle", hSCManager,
        "ServiceName", lpServiceName, "DisplayName", lpDisplayName,
        "DesiredAccess", dwDesiredAccess, "ServiceType", dwServiceType,
        "StartType", dwStartType, "ErrorControl", dwErrorControl,
        "BinaryPathName", lpBinaryPathName,
        "ServiceStartName", lpServiceStartName,
        "Password", lpPassword);
    return ret;
}

HOOKDEF(SC_HANDLE, WINAPI, CreateServiceW,
    __in       SC_HANDLE hSCManager,
    __in       LPWSTR lpServiceName,
    __in_opt   LPWSTR lpDisplayName,
    __in       DWORD dwDesiredAccess,
    __in       DWORD dwServiceType,
    __in       DWORD dwStartType,
    __in       DWORD dwErrorControl,
    __in_opt   LPWSTR lpBinaryPathName,
    __in_opt   LPWSTR lpLoadOrderGroup,
    __out_opt  LPDWORD lpdwTagId,
    __in_opt   LPWSTR lpDependencies,
    __in_opt   LPWSTR lpServiceStartName,
    __in_opt   LPWSTR lpPassword
) {
    SC_HANDLE ret = Old_CreateServiceW(hSCManager, lpServiceName,
        lpDisplayName, dwDesiredAccess, dwServiceType, dwStartType,
        dwErrorControl, lpBinaryPathName, lpLoadOrderGroup, lpdwTagId,
        lpDependencies, lpServiceStartName, lpPassword);
    LOQ_nonnull("services", "puu4lF2u", "ServiceControlHandle", hSCManager,
        "ServiceName", lpServiceName, "DisplayName", lpDisplayName,
        "DesiredAccess", dwDesiredAccess, "ServiceType", dwServiceType,
        "StartType", dwStartType, "ErrorControl", dwErrorControl,
        "BinaryPathName", lpBinaryPathName,
        "ServiceStartName", lpServiceStartName,
        "Password", lpPassword);
    return ret;
}

HOOKDEF(SC_HANDLE, WINAPI, OpenServiceA,
    __in  SC_HANDLE hSCManager,
    __in  LPCTSTR lpServiceName,
    __in  DWORD dwDesiredAccess
) {
    SC_HANDLE ret = Old_OpenServiceA(hSCManager, lpServiceName,
        dwDesiredAccess);
    LOQ_nonnull("services", "psl", "ServiceControlManager", hSCManager,
        "ServiceName", lpServiceName, "DesiredAccess", dwDesiredAccess);
    return ret;
}

HOOKDEF(SC_HANDLE, WINAPI, OpenServiceW,
    __in  SC_HANDLE hSCManager,
    __in  LPWSTR lpServiceName,
    __in  DWORD dwDesiredAccess
) {
    SC_HANDLE ret = Old_OpenServiceW(hSCManager, lpServiceName,
        dwDesiredAccess);
    LOQ_nonnull("services", "pul", "ServiceControlManager", hSCManager,
        "ServiceName", lpServiceName, "DesiredAccess", dwDesiredAccess);
    return ret;
}

HOOKDEF(BOOL, WINAPI, StartServiceA,
    __in      SC_HANDLE hService,
    __in      DWORD dwNumServiceArgs,
    __in_opt  LPCTSTR *lpServiceArgVectors
) {
    BOOL ret = Old_StartServiceA(hService, dwNumServiceArgs,
        lpServiceArgVectors);
    LOQ_bool("services", "pa", "ServiceHandle", hService, "Arguments", dwNumServiceArgs,
        lpServiceArgVectors);
    return ret;
}

HOOKDEF(BOOL, WINAPI, StartServiceW,
    __in      SC_HANDLE hService,
    __in      DWORD dwNumServiceArgs,
    __in_opt  LPWSTR *lpServiceArgVectors
) {
    BOOL ret = Old_StartServiceW(hService, dwNumServiceArgs,
        lpServiceArgVectors);
    LOQ_bool("services", "pA", "ServiceHandle", hService, "Arguments", dwNumServiceArgs,
        lpServiceArgVectors);
    return ret;
}

HOOKDEF(BOOL, WINAPI, ControlService,
    __in   SC_HANDLE hService,
    __in   DWORD dwControl,
    __out  LPSERVICE_STATUS lpServiceStatus
) {
    BOOL ret = Old_ControlService(hService, dwControl, lpServiceStatus);
    LOQ_bool("services", "pl", "ServiceHandle", hService, "ControlCode", dwControl);
    return ret;
}

HOOKDEF(BOOL, WINAPI, DeleteService,
    __in  SC_HANDLE hService
) {
    BOOL ret = Old_DeleteService(hService);
    LOQ_bool("services", "p", "ServiceHandle", hService);
    return ret;
}
