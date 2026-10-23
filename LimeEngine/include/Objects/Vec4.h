#pragma once
#include <string>
#include <cmath>
#include "Objects/VecLink.h"

extern "C" { struct lua_State; }

class Vec4 {
public:
    using GetFn = Vec4(*)(void*);
    using SetFn = void(*)(void*, const Vec4&);

    Vec4();
    Vec4(float x);
    Vec4(float x, float y, float z, float w);
    Vec4(const std::string& hex);

    Vec4(const Vec4& v) : x(v.getX()), y(v.getY()), z(v.getZ()), w(v.getW()) {}
    Vec4(Vec4&& v) noexcept = default;

    Vec4 operator+(const Vec4& other) const;
    Vec4 operator-(const Vec4& other) const;
    Vec4 operator*(float scalar) const;
    Vec4 operator/(float scalar) const;
    Vec4 operator*(const Vec4& scalar) const;
    Vec4 operator/(const Vec4& scalar) const;
    bool operator==(const Vec4& other) const;
    Vec4& operator=(const Vec4& v) {
        x = v.getX();
        y = v.getY();
        z = v.getZ();
        w = v.getW();
        owner = nullptr;
        return *this;
    }
    void setTo(const Vec4& other);
    void setTo(float x, float y, float z, float w);

    static Vec4 link(void* o, GetFn g, SetFn s) {
        Vec4 v;
        v.owner = o;
        v.getFn = g;
        v.setFn = s;
        return v;
    }

    float getX() const { return owner ? getFn(owner).x : x; }
    float getY() const { return owner ? getFn(owner).y : y; }
    float getZ() const { return owner ? getFn(owner).z : z; }
    float getW() const { return owner ? getFn(owner).w : w; }
    void setX(float n) {
        if (owner) {
            Vec4 t = getFn(owner);
            t.x = n;
            setFn(owner, t);
        }
        else {
            x = n;
        }
    }
    void setY(float n) {
        if (owner) {
            Vec4 t = getFn(owner);
            t.y = n;
            setFn(owner, t);
        }
        else {
            y = n;
        }
    }
    void setZ(float n) {
        if (owner) {
            Vec4 t = getFn(owner);
            t.z = n;
            setFn(owner, t);
        }
        else {
            z = n;
        }
    }
    void setW(float n) {
        if (owner) {
            Vec4 t = getFn(owner);
            t.w = n;
            setFn(owner, t);
        }
        else {
            w = n;
        }
    }

    std::string getHex();

private:
    float x = 0, y = 0, z = 0, w = 0;
    void* owner = nullptr;
    GetFn getFn = nullptr;
    SetFn setFn = nullptr;
};

namespace Object::Vec4Bind {
    void bind(lua_State* ls);
}