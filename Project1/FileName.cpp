#include <windows.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <commdlg.h>

#pragma comment(lib, "shell32.lib")
// Link as a Windows GUI app so no console window is created
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
// needed for GetOpenFileName
#pragma comment(lib, "comdlg32.lib")

DWORD FindProcessByName(const wchar_t* processName)
{
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return 0;

    if (Process32First(hSnapshot, &pe32))
    {
        do
        {
            if (wcscmp(pe32.szExeFile, processName) == 0)
            {
                CloseHandle(hSnapshot);
                return pe32.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
    // Determine ini path: same folder/name as this exe, but with .ini extension
    WCHAR modulePath[MAX_PATH];
    DWORD mn = GetModuleFileNameW(NULL, modulePath, MAX_PATH);
    if (mn == 0 || mn == MAX_PATH)
    {
        MessageBoxW(NULL, L"Failed to get module path.", L"Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    // build ini path
    WCHAR iniPath[MAX_PATH];
    wcscpy_s(iniPath, modulePath);
    // replace extension with .ini
    WCHAR* ext = wcsrchr(iniPath, L'.');
    if (ext) *ext = L'\0';
    wcscat_s(iniPath, MAX_PATH, L".ini");

    // try read game path from ini (section Game, key Path)
    WCHAR gamePath[MAX_PATH] = {0};
    GetPrivateProfileStringW(L"Game", L"Path", L"", gamePath, MAX_PATH, iniPath);

    if (wcslen(gamePath) == 0)
    {
        // ask user to select executable
        OPENFILENAMEW ofn = {0};
        WCHAR szFile[MAX_PATH] = {0};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFilter = L"Executable\0*.exe\0All Files\0*.*\0";
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER;
        ofn.lpstrTitle = L"Select Game Executable";

        if (!GetOpenFileNameW(&ofn))
        {
            // user cancelled
            return 0;
        }

        // save selected path to ini
        WritePrivateProfileStringW(L"Game", L"Path", szFile, iniPath);
        wcscpy_s(gamePath, MAX_PATH, szFile);
    }

    // Use ShellExecuteEx to get a process handle, then wait for the process to exit.
    SHELLEXECUTEINFOW sei = {0};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.hwnd = NULL;
    sei.lpVerb = L"runas";
    sei.lpFile = gamePath;
    sei.lpParameters = NULL;
    sei.lpDirectory = NULL;
    sei.nShow = SW_SHOW;

    if (!ShellExecuteExW(&sei))
    {
        DWORD err = GetLastError();
        WCHAR buf[128];
        swprintf_s(buf, L"ShellExecuteEx failed with error code: %lu", err);
        MessageBoxW(NULL, buf, L"Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    // Wait until the launched process exits. This keeps this program running until the game closes.
    if (sei.hProcess)
    {
        WaitForSingleObject(sei.hProcess, INFINITE);
        CloseHandle(sei.hProcess);
    }

    return 0;
}
