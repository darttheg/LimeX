#pragma once
#include <string>
#include <cmath>
#include <functional>

class Application;

class Vec3 {
public:
    Vec3();
    Vec3(float x);
    Vec3(float x, float y, float z);

    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;
    Vec3 operator*(float scalar) const;
    Vec3 operator/(float scalar) const;
    bool operator==(const Vec3& other) const;

    using Getter = std::function<Vec3()>;
    using Setter = std::function<void(const Vec3&)>;
    Getter get;
    Setter set;

    float getX() const { if (get) return get().x; else return x; }
    float getY() const { if (get) return get().y; else return y; }
    float getZ() const { if (get) return get().z; else return z; }
    void setX(float v) { if (get) { auto t = get(); t.x = v; set(t); } else x = v; }
    void setY(float v) { if (get) { auto t = get(); t.y = v; set(t); } else y = v; }
    void setZ(float v) { if (get) { auto t = get(); t.z = v; set(t); } else z = v; }

    Vec3(Getter g, Setter s) : get(std::move(g)), set(std::move(s)) {}
private:
    float x, y, z = 0.0;
};

namespace Object::Vec3Bind {
    void bind(Application* app);
}