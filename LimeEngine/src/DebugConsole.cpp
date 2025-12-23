#include "DebugConsole.h"

#include "Application.h"

#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <fstream>

#include <Windows.h>

DebugConsole::DebugConsole() {
}

DebugConsole::~DebugConsole() {
}

void DebugConsole::Close() {
    if (writeOutput) {
        std::ofstream outFile("output.log");
        if (outFile) outFile << out << std::endl;
    }
}

void DebugConsole::Create() {
    if (created) return;

    AllocConsole();
    FILE* consoleOut;
    freopen_s(&consoleOut, "CONIN$", "r", stdin);

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!out.empty() && hConsole != INVALID_HANDLE_VALUE) {
        DWORD written;
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        WriteConsoleA(hConsole, out.c_str(), (DWORD)out.size(), &written, nullptr);
    }

    FILE* fpIn;
    freopen_s(&fpIn, "CONIN$", "w", stdin);
    FILE* fpOut;
    freopen_s(&fpOut, "CONOUT$", "w", stdout);
    FILE* fpErr;
    freopen_s(&fpErr, "CONOUT$", "w", stderr);

    SetConsoleTitle(L"Lime Console");

    created = true;
}

void DebugConsole::SetAppOwner(Application* owner) {
    app = owner;
}

const char* getTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm;
    localtime_s(&now_tm, &now_c);
    std::stringstream ss;
    ss << std::put_time(&now_tm, "[%H:%M:%S]");
    static std::string timeStr;
    timeStr = ss.str();
    return timeStr.c_str();
}

void DebugConsole::Log(const char* msg, MESSAGE_TYPE type) {
    std::string time = getTime();
    std::string full = time + " " + msg;

    if (doOutput) out = out + full + "\n";
    if (!created) return;

    WORD cur = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    if (type == MESSAGE_TYPE::RED)
        cur = FOREGROUND_RED | FOREGROUND_INTENSITY;
    else if (type == MESSAGE_TYPE::GREEN)
        cur = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    else if (type == MESSAGE_TYPE::BLUE)
        cur = FOREGROUND_BLUE | FOREGROUND_INTENSITY;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole != INVALID_HANDLE_VALUE) {
        SetConsoleTextAttribute(hConsole, cur);

        DWORD written;
        WriteConsoleA(hConsole, full.c_str(), (DWORD)strlen(full.c_str()), &written, nullptr);
        WriteConsoleA(hConsole, "\n", 1, &written, nullptr);

        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}

void DebugConsole::Log(std::string msg, MESSAGE_TYPE type) {
    Log(msg.c_str(), type);
}

void DebugConsole::PostError(const char* msg, bool close) {
    if (endOnError) close = true;

    Log(std::string("Lime encountered an error: " + std::string(msg)).c_str(), MESSAGE_TYPE::RED);

    if (close) {
        std::string src = std::string("Lime encountered an error:\n" + std::string(msg)).c_str();

        std::wstring wStr = std::wstring(src.begin(), src.end());
        const wchar_t* wCharStr = wStr.c_str();

        MessageBox(nullptr, wStr.c_str(), TEXT("Lime Error"), MB_ICONEXCLAMATION);

        if (app) app->EndApp();
    }
}

void DebugConsole::PostError(std::string msg, bool close) {
    PostError(msg.c_str(), close);
}

void DebugConsole::Update(int memMB) {
    // SetConsoleTitle(); ... fps, mem, driver type

    std::wstring out = L"Lime | mem: ";
    out += std::to_wstring(memMB);
    out += L" MB";

    memUsed = memMB;

    SetConsoleTitle(out.c_str());
}

void DebugConsole::WriteOutputLog() {
    std::ofstream outLog("output.log");
    if (outLog) outLog << out << std::endl;
    outLog.close();
}