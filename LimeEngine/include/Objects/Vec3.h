#pragma once
#include <string>
#include <cmath>
#include "Objects/VecLink.h"

extern "C" { struct lua_State; }

class Vec3 {
public:
    using GetFn = Vec3(*)(void*);
    using SetFn = void(*)(void*, const Vec3&);

    Vec3();
    Vec3(float x);
    Vec3(float x, float y, float z);

    Vec3(const Vec3& v) : x(v.getX()), y(v.getY()), z(v.getZ()) {}
    Vec3(Vec3&& v) noexcept = default;

    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;
    Vec3 operator*(float scalar) const;
    Vec3 operator*(const Vec3& scalar) const;
    Vec3 operator/(float scalar) const;
    Vec3 operator/(const Vec3& scalar) const;
    Vec3& operator=(const Vec3& v) {
        x = v.getX();
        y = v.getY();
        z = v.getZ();
        owner = nullptr;
        return *this;
    }
    bool operator==(const Vec3& other) const;

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
    void setTo(const Vec3& other);
    void setTo(float x, float y, float z);

    static Vec3 link(void* o, GetFn g, SetFn s) {
        Vec3 v;
        v.owner = o;
        v.getFn = g;
        v.setFn = s;
        return v;
    }

    float getX() const { return owner ? getFn(owner).x : x; }
    float getY() const { return owner ? getFn(owner).y : y; }
    float getZ() const { return owner ? getFn(owner).z : z; }
    void setX(float n) {
        if (owner) {
            Vec3 t = getFn(owner);
            t.x = n;
            setFn(owner, t);
        }
        else {
            x = n;
        }
    }
    void setY(float n) {
        if (owner) {
            Vec3 t = getFn(owner);
            t.y = n;
            setFn(owner, t);
        }
        else {
            y = n;
        }
    }
    void setZ(float n) {
        if (owner) {
            Vec3 t = getFn(owner);
            t.z = n;
            setFn(owner, t);
        }
        else {
            z = n;
        }
    }
private:
    float x = 0, y = 0, z = 0;
    void* owner = nullptr;
    GetFn getFn = nullptr;
    SetFn setFn = nullptr;
};

namespace Object::Vec3Bind {
    void bind(lua_State* ls);
}