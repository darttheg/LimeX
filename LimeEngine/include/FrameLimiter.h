#pragma once
#include <chrono>
#include <thread>

class FrameLimiter {
public:
    using clock = std::chrono::steady_clock;
    
    int targetFPS = 60;
    bool vSync = false;
    clock::duration frameDur{};
    clock::time_point start{}, next{};

    FrameLimiter(int fps = 60) {
        setFPS(fps);
    }

    void setVSync(bool v) {
        vSync = v;
        start = next = clock::now();
    }

    void setFPS(int fps) {
        targetFPS = fps;
        frameDur = (fps > 0.0) ? std::chrono::duration_cast<clock::duration>(std::chrono::duration<double>(1.0 / fps)) : clock::duration::zero();
        start = next = clock::now();
    }

    double beginFrame() {
        auto now = clock::now();
        double dt = std::chrono::duration<double>(now - start).count();
        start = now;
        if (dt <= 0.0 || dt > 0.25) dt = (targetFPS > 0) ? 1.0 / targetFPS : 1.0 / 60.0;
        return dt;
    }

    void endFrame() {
        if (vSync || targetFPS <= 0) return;
        next += frameDur;
        auto now = clock::now();

        if (now > next + frameDur * 2) {
            next = now;
            return;
        }

        auto spinThreshold = std::chrono::milliseconds(2);
        if (next - now > spinThreshold) std::this_thread::sleep_until(next - spinThreshold);

        while (clock::now() < next) {}
    }
};