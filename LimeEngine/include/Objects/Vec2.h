#pragma once
#include <string>
#include <cmath>
#include "Objects/VecLink.h"

extern "C" { struct lua_State; }

class Vec2 {
public:
    using GetFn = Vec2(*)(void*);
    using SetFn = void(*)(void*, const Vec2&);

    Vec2();
    Vec2(float x, float y);
    Vec2(float x);
    
    Vec2(const Vec2& v) : x(v.getX()), y(v.getY()) {}
    Vec2(Vec2&& v) noexcept = default;

    Vec2 operator+(const Vec2& other) const;
    Vec2 operator-(const Vec2& other) const;
    Vec2 operator*(float scalar) const;
    Vec2 operator/(float scalar) const;
    Vec2 operator*(const Vec2& scalar) const;
    Vec2 operator/(const Vec2& scalar) const;
    Vec2& operator=(const Vec2& v) {
        x = v.getX();
        y = v.getY();
        owner = nullptr;
        return *this;
    }
    bool operator==(const Vec2& other) const;

    float getLength() const;
    float getLengthSquared() const;
    float getDistance(const Vec2& other) const;
    float getDistanceSquared(const Vec2& other) const;
    bool isNearlyZero(float epsilon = 1e-6f) const;
    float dot(const Vec2& other) const;
    float cross(const Vec2& other) const;
    float angle(const Vec2& other) const;
    Vec2 normalize() const;
    Vec2 normalizeRange(float min, float max) const;
    Vec2 clamp(const Vec2& min, const Vec2& max) const;
    void setTo(const Vec2& other);
    void setTo(float x, float y);

    static Vec2 link(void* o, GetFn g, SetFn s) {
        Vec2 v;
        v.owner = o;
        v.getFn = g;
        v.setFn = s;
        return v;
    }

    float getX() const { return owner ? getFn(owner).x : x; }
    float getY() const { return owner ? getFn(owner).y : y; }
    void setX(float n) {
        if (owner) {
            Vec2 t = getFn(owner);
            t.x = n;
            setFn(owner, t);
        }
        else {
            x = n;
        }
    }
    void setY(float n) {
        if (owner) {
            Vec2 t = getFn(owner);
            t.y = n;
            setFn(owner, t);
        }
        else {
            y = n;
        }
    }
private:
    float x = 0, y = 0;
    void* owner = nullptr;
    GetFn getFn = nullptr;
    SetFn setFn = nullptr;
};

namespace Object::Vec2Bind {
    void bind(lua_State* ls);
}