#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

void InjectDll(HANDLE hProcess, HANDLE hThread, const std::wstring& path)
{
	size_t strSize = (path.size() + 1) * sizeof(WCHAR);
	LPVOID pBuf = VirtualAllocEx(hProcess, 0, strSize, MEM_COMMIT, PAGE_READWRITE);
	if (pBuf == NULL)
		return;

	SIZE_T written;
	if (!WriteProcessMemory(hProcess, pBuf, path.c_str(), strSize, &written))
		return;

	LPVOID pLoadLibraryW = GetProcAddress(GetModuleHandle(L"kernel32"), "LoadLibraryW");
	QueueUserAPC((PAPCFUNC)pLoadLibraryW, hThread, (ULONG_PTR)pBuf);
}

std::wstring GetExecutableDir()
{
	WCHAR buf[MAX_PATH];
	GetModuleFileName(nullptr, buf, MAX_PATH);
	PathRemoveFileSpec(buf);
	return buf;
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	std::wstring path = GetExecutableDir();

	//Name of the bridge, on current executable directory
	path += L"\\ManagedBridge.dll";

	STARTUPINFO startInfo = { 0 };
	PROCESS_INFORMATION procInfo = { 0 };

	//Use this if you want to run unpackaged
	WCHAR cmdline[] = L"C:\\Windows\\System32\\RuntimeBroker.exe";

	//Use this if you want to run as packaged (you have to bundle the broker)
	//WCHAR cmdline[] = L"TinyBroker.exe";

	startInfo.cb = sizeof(startInfo);

	if (CreateProcess(nullptr, cmdline, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &startInfo, &procInfo))
	{
		InjectDll(procInfo.hProcess, procInfo.hThread, path);
		ResumeThread(procInfo.hThread);
	}

	return 0;
}