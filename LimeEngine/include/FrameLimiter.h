#pragma once
#include <chrono>
#include <thread>

class FrameLimiter {
public:
    FrameLimiter(float fps = 60.0) {
        setFPS(fps);
        prev = next = clock::now();
    }

    using clock = std::chrono::steady_clock;

    int targetFPS = 60;
    bool vSync = false;
    clock::duration frameDur{};
    clock::time_point prev{}, next{};

    void setVSync(bool v) {
        vSync = v;
        prev = next = clock::now();
    }

    void setFPS(int fps) {
        targetFPS = fps;
        frameDur = (fps > 0.0) ? std::chrono::duration_cast<clock::duration>(std::chrono::duration<double>(1.0 / fps)) : clock::duration::zero();
        prev = next = clock::now();
    }

    double beginFrame() {
        auto now = clock::now();
        double dt = std::chrono::duration<double>(now - prev).count();
        prev = now;
        if (dt <= 0.0 || dt > 0.1) dt = 1.0 / targetFPS;
        return dt;
    }

    void endFrame() {
        if (vSync || targetFPS <= 0) return;
        next += frameDur;
        std::this_thread::sleep_until(next);
        if (clock::now() - next > frameDur * 3) next = clock::now();
    }
};