
#include "Initialization.h"

#include <direct.h>
#include <tchar.h>
#include <ShlObj_core.h>

#include <iostream>

bool IsOnlyInstance(LPCWSTR gameTitle) {
	//Find the window. If active, set and return false
	//Only one gane instance may have this mutex at time
	HANDLE handle = CreateMutex(NULL, TRUE, gameTitle);

	if (GetLastError() != ERROR_SUCCESS) {
		HWND hWnd = FindWindow(gameTitle, NULL);
		if (hWnd) {
			ShowWindow(hWnd, SW_SHOWNORMAL);
			SetFocus(hWnd);
			SetForegroundWindow(hWnd);
			SetActiveWindow(hWnd);
			return false;
		}
	}

	return true;
}

//Check storage for enough space in bytes
bool CheckStorage(const DWORDLONG diskSpaceNeeded) {
	int const drive = _getdrive();
	struct _diskfree_t diskfree;

	_getdiskfree(drive, &diskfree);

	unsigned _int64 const neededClusters = diskSpaceNeeded / (diskfree.sectors_per_cluster * diskfree.bytes_per_sector);

	if (diskfree.avail_clusters < neededClusters) {
		std::cerr << "CheckStorage Failure: Not enough physucal storage.";
		return false;
	}

	return true;
}

DWORD ReadCPUSpeed()
{
	DWORD BufSize = sizeof(DWORD);
	DWORD dwMHz = 0;
	DWORD type = REG_DWORD;
	HKEY hKey;

	long lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
		0, KEY_READ, &hKey);

	if (lError == ERROR_SUCCESS)
		RegQueryValueEx(hKey, L"~MHz", NULL, &type, (LPBYTE)&dwMHz, &BufSize);

	return dwMHz;
}

const TCHAR* GetSaveGameDirectory(HWND hWnd, const TCHAR* gameAppDirectory)
{
	HRESULT hr;
	static TCHAR m_SaveGameDirectory[MAX_PATH];
	TCHAR userDataPath[MAX_PATH];

	hr = SHGetSpecialFolderPath(hWnd, userDataPath, CSIDL_APPDATA, true);

	_tcscpy_s(m_SaveGameDirectory, userDataPath);
	_tcscat_s(m_SaveGameDirectory, _T("\\"));
	_tcscat_s(m_SaveGameDirectory, gameAppDirectory);

	if (0xffffffff == GetFileAttributes(m_SaveGameDirectory))
		if (SHCreateDirectoryEx(hWnd, m_SaveGameDirectory, NULL) != ERROR_SUCCESS)
			return false;

	_tcscat_s(m_SaveGameDirectory, _T("\\"));

	return m_SaveGameDirectory;
}