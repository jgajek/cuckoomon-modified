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
#include "pipe.h"
#include "hook_sleep.h"
#include "misc.h"

void set_hooks_dll(const wchar_t *library, int len);

HOOKDEF2(NTSTATUS, WINAPI, LdrLoadDll,
    __in_opt    PWCHAR PathToFile,
    __in_opt    ULONG Flags,
    __in        PUNICODE_STRING ModuleFileName,
    __out       PHANDLE ModuleHandle
) {

    //
    // In the event that loading this dll results in loading another dll as
    // well, then the unicode string (which is located in the TEB) will be
    // overwritten, therefore we make a copy of it for our own use.
    //

    COPY_UNICODE_STRING(library, ModuleFileName);

    NTSTATUS ret = Old2_LdrLoadDll(PathToFile, Flags, ModuleFileName,
        ModuleHandle);

    if (hook_info()->depth_count == 1) {
		if (!wcsncmp(library.Buffer, L"\\??\\", 4) || library.Buffer[1] == L':')
	        LOQspecial_ntstatus("system", "lFP", "Flags", Flags, "FileName", library.Buffer,
		       "BaseAddress", ModuleHandle);
		else
			LOQspecial_ntstatus("system", "loP", "Flags", Flags, "FileName", &library,
				"BaseAddress", ModuleHandle);
	}

    //
    // Check this DLL against our table of hooks, because we might have to
    // place some new hooks.
    //

    if(NT_SUCCESS(ret)) {
		// unoptimized, but easy
		add_all_dlls_to_dll_ranges();
        set_hooks_dll(library.Buffer, library.Length >> 1);
    }

    return ret;
}

HOOKDEF2(BOOL, WINAPI, CreateProcessInternalW,
    __in_opt    LPVOID lpUnknown1,
    __in_opt    LPWSTR lpApplicationName,
    __inout_opt LPWSTR lpCommandLine,
    __in_opt    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    __in_opt    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    __in        BOOL bInheritHandles,
    __in        DWORD dwCreationFlags,
    __in_opt    LPVOID lpEnvironment,
    __in_opt    LPWSTR lpCurrentDirectory,
    __in        LPSTARTUPINFO lpStartupInfo,
    __out       LPPROCESS_INFORMATION lpProcessInformation,
    __in_opt    LPVOID lpUnknown2
) {

    BOOL ret = Old2_CreateProcessInternalW(lpUnknown1, lpApplicationName,
        lpCommandLine, lpProcessAttributes, lpThreadAttributes,
        bInheritHandles, dwCreationFlags | CREATE_SUSPENDED, lpEnvironment,
        lpCurrentDirectory, lpStartupInfo, lpProcessInformation, lpUnknown2);

    if(ret != FALSE) {
        pipe("PROCESS:%d,%d", lpProcessInformation->dwProcessId,
            lpProcessInformation->dwThreadId);

        // if the CREATE_SUSPENDED flag was not set, then we have to resume
        // the main thread ourself
        if((dwCreationFlags & CREATE_SUSPENDED) == 0) {
            ResumeThread(lpProcessInformation->hThread);
        }

        disable_sleep_skip();
    }

    if (hook_info()->depth_count == 1) {
        LOQspecial_bool("process", "uupllpp", "ApplicationName", lpApplicationName,
            "CommandLine", lpCommandLine, "CreationFlags", dwCreationFlags,
            "ProcessId", lpProcessInformation->dwProcessId,
            "ThreadId", lpProcessInformation->dwThreadId,
            "ProcessHandle", lpProcessInformation->hProcess,
            "ThreadHandle", lpProcessInformation->hThread);
    }

    return ret;
}

static GUID _CLSID_CUrlHistory =	  { 0x3C374A40L, 0xBAE4, 0x11CF, 0xBF, 0x7D, 0x00, 0xAA, 0x00, 0x69, 0x46, 0xEE };
static GUID _CLSID_InternetExplorer = { 0x0002DF01L, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 };

static char *known_object(IID *app, IID *iface)
{
	if (!memcmp(app, &_CLSID_CUrlHistory, sizeof(*app)))
		return "CUrlHistory";
	else if (!memcmp(app, &_CLSID_InternetExplorer, sizeof(*app)))
		return "InternetExplorer";

	return NULL;
}

HOOKDEF2(HRESULT, WINAPI, CoCreateInstance,
	__in    REFCLSID rclsid,
	__in	LPUNKNOWN pUnkOuter,
	__in	DWORD dwClsContext,
	__in	REFIID riid,
	__out	LPVOID *ppv
	) {
	IID id1;
	IID id2;
	char idbuf1[40];
	char idbuf2[40];
	char *known;
	HRESULT ret = Old2_CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);

	memcpy(&id1, rclsid, sizeof(id1));
	memcpy(&id2, riid, sizeof(id2));
	sprintf(idbuf1, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", id1.Data1, id1.Data2, id1.Data3,
		id1.Data4[0], id1.Data4[1], id1.Data4[2], id1.Data4[3], id1.Data4[4], id1.Data4[5], id1.Data4[6], id1.Data4[7]);
	sprintf(idbuf2, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", id2.Data1, id2.Data2, id2.Data3,
		id2.Data4[0], id2.Data4[1], id2.Data4[2], id2.Data4[3], id2.Data4[4], id2.Data4[5], id2.Data4[6], id2.Data4[7]);

	if ((known = known_object(&id1, &id2)))
		LOQspecial_hresult("com", "spss", "rclsid", idbuf1, "ClsContext", dwClsContext, "riid", idbuf2, "KnownObject", known);
	else
		LOQspecial_hresult("com", "sps", "rclsid", idbuf1, "ClsContext", dwClsContext, "riid", idbuf2);

	return ret;
}
