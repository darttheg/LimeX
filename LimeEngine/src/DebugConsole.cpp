#include "DebugConsole.h"

#include "Application.h"

#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <fstream>

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

    if (endApp) {
        // Shutdown
    }
}

void DebugConsole::SetEnable(bool v) {
}

void DebugConsole::Clear() {
    consoleLines.clear();
}

void DebugConsole::Init() {
    // Init

    created = true;
}

void DebugConsole::Create() {
    if (created) return;

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

    consoleLines.push_back(Line(type, full));
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
    memUsed = memMB;
    if (!created) return;

    std::string out = "Lime | mem: ";
    out += std::to_string(memMB);
    out += " MB";

    /*
    ImGui::Begin("Lime Console");
    ImGui::Text("Hello Lime!");
    ImGui::Separator();
    ImGui::Text("This is a console.");
    ImGui::End();
    */
}

void DebugConsole::WriteOutputLog() {
    if (!writeOutput) return;
    std::ofstream outLog("output.log", std::ios::out | std::ios::trunc);
    if (!outLog.is_open()) return;

    for (const Line& line : consoleLines)
        outLog << line.content << "\n";

    outLog.close();
}