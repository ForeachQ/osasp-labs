#include <Windows.h>
#include <iostream>
#include <string>


#define OUTPUT_STRING "text text text"
#define REPLACE_STRING "REPLACED REPLACED REPLACED"
#define MAX_STRING_SIZE 256

#define REMOTE_PROCESS_PATH L"D:\\Projects\\C++\\osasp3-master\\x64\\Debug\\Victim.exe"
#define LIB_PATH "D:\\Projects\\C++\\osasp3-master\\x64\\Debug\\Dll.dll"
#define FUNC_NAME "replaceString"
#define MAX_PATH_SIZE 256

extern "C" void __cdecl replaceString(std::string, std::string); 
typedef void(_cdecl* importedFuncPointer)(std::string, std::string); 

PROCESS_INFORMATION pi;

void processCleanup(PROCESS_INFORMATION pi) {
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
}

bool consoleCloseHandler(DWORD dwCtrlType) {
    TerminateProcess(pi.hProcess, 0);
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    return true;
}

void staticImport() {
    char outputString[MAX_STRING_SIZE];
    strcpy_s(outputString, OUTPUT_STRING);
    std::cout << outputString << std::endl;

    replaceString(OUTPUT_STRING, REPLACE_STRING);
    std::cout << outputString << std::endl;
}

void dynamicImport() {
    HINSTANCE stringReplaceDll;
    importedFuncPointer replaceStringPointer;

    stringReplaceDll = LoadLibraryA(LIB_PATH);
    replaceStringPointer = (importedFuncPointer)GetProcAddress(stringReplaceDll, FUNC_NAME);

    char outputString[MAX_STRING_SIZE];
    strcpy_s(outputString, OUTPUT_STRING);
    std::cout << outputString << std::endl;

    replaceStringPointer(OUTPUT_STRING, REPLACE_STRING);
    FreeLibrary(stringReplaceDll);
    std::cout << outputString << std::endl;
}

void remoteImport() {
    TCHAR ProcessName[MAX_PATH_SIZE];
    wcscpy_s(ProcessName, REMOTE_PROCESS_PATH);

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    SetConsoleCtrlHandler((PHANDLER_ROUTINE)consoleCloseHandler, true);
    CreateProcess(ProcessName, NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

    int pid;
    std::cout << "enter pid: ";
    std::cin >> pid;
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);


    char libPath[MAX_PATH_SIZE];
    strcpy_s(libPath, LIB_PATH);

    void* pLibRemote = VirtualAllocEx(hProcess, NULL, sizeof(libPath), MEM_COMMIT, PAGE_READWRITE);
    bool i = WriteProcessMemory(hProcess, pLibRemote, libPath, sizeof(libPath), NULL);

    void* startAddr = GetProcAddress(GetModuleHandle(L"Kernel32"), "LoadLibraryA");

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)startAddr, pLibRemote, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);


    DWORD loadedLib;
    GetExitCodeThread(hThread, &loadedLib);
    CloseHandle(hThread);
    VirtualFreeEx(hProcess, pLibRemote, sizeof(libPath), MEM_RELEASE);


    hThread = CreateRemoteThread(hProcess, NULL, 0,
        (LPTHREAD_START_ROUTINE)GetProcAddress(
            GetModuleHandle(L"Kernel32"), "FreeLibrary"),
        (void*)loadedLib,
        0, NULL);

    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);
    CloseHandle(hProcess);

    WaitForSingleObject(pi.hProcess, INFINITE);
    processCleanup(pi);
}

int main()
{
//    staticImport();
//    dynamicImport();
    remoteImport();
}
