#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mscoree.h>
#include <metahost.h>
#include <string>
#include <Shlwapi.h>
#include <synchapi.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "mscoree.lib")

std::wstring GetExecutableDir(HMODULE lpParam)
{
	WCHAR buf[MAX_PATH];
	GetModuleFileName(lpParam, buf, MAX_PATH);
	PathRemoveFileSpec(buf);
	return buf;
}

DWORD WINAPI LocalThread(LPVOID lpParam)
{
	ICLRMetaHost* pMetaHost = NULL;
	CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (LPVOID*)&pMetaHost);

	ICLRRuntimeInfo* pRuntimeInfo = NULL;
	pMetaHost->GetRuntime(L"v4.0.30319", IID_ICLRRuntimeInfo, (LPVOID*)&pRuntimeInfo);

	ICLRRuntimeHost* pClrRuntimeHost = NULL;
	pRuntimeInfo->GetInterface(CLSID_CLRRuntimeHost, IID_ICLRRuntimeHost, (LPVOID*)&pClrRuntimeHost);

	pClrRuntimeHost->Start();

	DWORD dwRet = 0;

	std::wstring path = GetExecutableDir((HMODULE)lpParam);

	//File name of the .NET program to execute
	path += L"\\ManagedProgram.dll";

	//ManagedProgram <- Your .NET namespace for entrypoint | Program <- class name for entry point | AltMain <- entry point | Parameter <- the argument you pass to the .NET program
	auto res = pClrRuntimeHost->ExecuteInDefaultAppDomain(path.c_str(), L"ManagedProgram.Program", L"AltMain", L"Parameter", &dwRet);

	if (res != 0)
		MessageBox(NULL, std::to_wstring(res).c_str(), L"Error code", 0);

	pClrRuntimeHost->Release();

	pRuntimeInfo->Release();
	pMetaHost->Release();

	exit(0);

	//actually useless.
	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, LocalThread, hModule, NULL, NULL);
		break;
	default:
        break;
    }
    return TRUE;
}

