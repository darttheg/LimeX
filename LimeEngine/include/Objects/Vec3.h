#pragma once
#include <string>
#include <cmath>
#include <functional>

extern "C" { struct lua_State; }

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
    Vec3 copy() const { return Vec3(getX(), getY(), getZ()); }

    float getLength() const;
    float getLengthSquared() const;
    float getDistance(const Vec3& other) const;
    float getDistanceSquared(const Vec3& other) const;
    bool isNearlyZero(float epsilon = 1e-6f) const;
    float dot(const Vec3& other) const;
    Vec3 cross(const Vec3& other) const;
    float angle(const Vec3& other) const;
    Vec3 normalize() const;
    Vec3 normalizeRange(float min, float max) const;
    Vec3 clamp(const Vec3& min, const Vec3& max) const;
    Vec3 reflect(const Vec3& dir) const;

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
    void bind(lua_State* ls);
}