#include <shlwapi.h>
#include <string>

using namespace std;

wchar_t szSelfModuleFullPath[512] = L"";
wchar_t szSelfModuleDirPath[512] = L"";
wstring libcurl_filepath = L"";
BOOL WINAPI DllMain(HMODULE selfDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
		GetModuleFileNameW(selfDLL, szSelfModuleFullPath, _countof(szSelfModuleFullPath));
		wcscpy_s(szSelfModuleDirPath, szSelfModuleFullPath);
		PathRemoveFileSpec(szSelfModuleDirPath);
		libcurl_filepath = szSelfModuleDirPath + L"\\"s + L"libcurl.dll";
		LoadLibrary(libcurl_filepath.data());
		break;
	case DLL_PROCESS_ATTACH:
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

