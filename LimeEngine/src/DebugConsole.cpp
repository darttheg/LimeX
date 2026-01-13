#include "DebugConsole.h"

#include "Application.h"

#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <fstream>

static void CreateDebugConsole() {
    if (GetConsoleWindow() == nullptr) {
        if (!AttachConsole(ATTACH_PARENT_PROCESS))
            AllocConsole();
    }

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    FILE* fp = nullptr;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONOUT$", "r", stdin);
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
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

static WORD getColorFromType(MESSAGE_TYPE type) {
    switch (type) {
    case MESSAGE_TYPE::RED: return FOREGROUND_RED | FOREGROUND_INTENSITY;
        break;
    case MESSAGE_TYPE::GREEN: return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
    case MESSAGE_TYPE::BLUE: return FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        break;
    case MESSAGE_TYPE::YELLOW: return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
    default: return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }
}

DebugConsole::DebugConsole() {
}

DebugConsole::~DebugConsole() {
    Close(true);
}

void DebugConsole::Close(bool endApp) {
    if (endApp)
        WriteOutputLog();

    if (!created) return;

    Clear();
    created = false;

    if (endApp)
        FreeConsole();
}

void DebugConsole::SetEnable(bool v) {
    HWND h = GetConsoleWindow();
    if (h)
        ShowWindow(h, v ? SW_SHOW : SW_HIDE);
    else if(!created)
        Create();
}

void DebugConsole::Clear() {
    consoleLines.clear();
}

void DebugConsole::Create() {
    if (created) return;

    CreateDebugConsole();
    SetConsoleTitleW(L"Lime Console");
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    consoleHandle = (void*)h;

    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (GetConsoleScreenBufferInfo(h, &info))
        defaultAttr = info.wAttributes;
    else
        defaultAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

    SetWindowPos(GetConsoleWindow(), nullptr, 0, 0, 420, 480, SWP_NOMOVE | SWP_NOZORDER);

    if (h && !consoleLines.empty()) {
        for (const Line& line : consoleLines)
            AddLineToConsole(line);
    }

    created = true;
}

void DebugConsole::SetAppOwner(Application* owner) {
    app = owner;
}

void DebugConsole::AddLineToConsole(Line l) {
    SetConsoleTextAttribute(consoleHandle, getColorFromType(l.type));
    DWORD written = 0;
    WriteConsoleA(consoleHandle, l.content.c_str(), (DWORD)l.content.size(), &written, nullptr);
    WriteConsoleA(consoleHandle, "\n", 1, &written, nullptr);
    SetConsoleTextAttribute(consoleHandle, defaultAttr);
}

void DebugConsole::Log(const char* msg, MESSAGE_TYPE type) {
    std::string time = getTime();
    std::string full = time + " " + msg;

    consoleLines.push_back(Line(type, full));
    if (created)
        AddLineToConsole(Line(type, full));
}

void DebugConsole::Log(std::string msg, MESSAGE_TYPE type) {
    Log(msg.c_str(), type);
}

void DebugConsole::PostError(const char* msg, bool close) {
    if (endOnError) close = true;

    Log(std::string("<!> " + std::string(msg)).c_str(), MESSAGE_TYPE::RED);

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

void DebugConsole::Warn(const char* msg) {
    std::string full = "<?> ";
    full += msg;
    Log(full.c_str(), MESSAGE_TYPE::YELLOW);
}

void DebugConsole::Warn(std::string msg) {
    Warn(msg.c_str());
}

void DebugConsole::Update(int memMB) {
    memUsed = memMB;
    if (!created) return;

    std::wstring out = L"Lime | mem: " + std::to_wstring(memMB) + L" mb";
    SetConsoleTitleW(out.c_str());
}

void DebugConsole::WriteOutputLog() {
    if (!writeOutput) return;
    std::ofstream outLog("output.log", std::ios::out | std::ios::trunc);
    if (!outLog.is_open()) return;

    for (const Line& line : consoleLines)
        outLog << line.content << "\n";

    outLog.close();
}