#pragma once
#include <string>
#include <cmath>
#include <functional>

class Vec4 {
public:
    using Getter = std::function<Vec4()>;
    using Setter = std::function<void(const Vec4&)>;
    Getter get;
    Setter set;

    float getX() const { if (get) return get().x; else return x; }
    float getY() const { if (get) return get().y; else return y; }
    float getZ() const { if (get) return get().z; else return z; }
    float getW() const { if (get) return get().w; else return w; }
    void setX(float v) { if (get) { auto t = get(); t.x = v; set(t); } else x = v; }
    void setY(float v) { if (get) { auto t = get(); t.y = v; set(t); } else y = v; }
    void setZ(float v) { if (get) { auto t = get(); t.z = v; set(t); } else z = v; }
    void setW(float v) { if (get) { auto t = get(); t.w = v; set(t); } else w = v; }

    Vec4(Getter g, Setter s) : get(std::move(g)), set(std::move(s)) {}
private:
    float x, y, z, w = 0.0;
};