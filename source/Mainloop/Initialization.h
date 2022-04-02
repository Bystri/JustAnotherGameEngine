#pragma once

//TODO: it is for timeGetTime in FlashWhileMinimized. Perhaps, I need to delete it or replace
#pragma comment(lib, "Winmm.Lib")

#include <windows.h>

#include <string>

extern bool IsOnlyInstance(LPCWSTR gameTitle);
extern bool CheckStorage(const DWORDLONG diskSpaceNeeded);
extern DWORD ReadCPUSpeed();
extern const TCHAR* GetSaveGameDirectory(HWND hWnd, const TCHAR* gameAppDirectory);

/*
//This class holds operating system-specific tasks. Now it is created for Windows systems;
class JAGE {
	//TODO: this is not allowed to be here
	const TCHAR* GAME_APP_DIRECTORY = L"JAGE\\Just Another Pong\\0.1";
public:
	JAGE() = delete;
	JAGE(const std::wstring& gameTitle);

	bool InitInstance();
	LRESULT OnClose();

	void FlashWhileMinimized();

private:
	//TODO: game title is not allowed to be here
	std::wstring m_gameTitle;
	//TODO: this is not allowed to be here
	TCHAR m_saveGameDirectory[MAX_PATH];
};
*/