#pragma once
#include <string>
#include <cmath>
#include <functional>

class Application;

class Vec2 {
public:
    Vec2();
    Vec2(float x, float y);
    Vec2(float x);

    Vec2 operator+(const Vec2& other) const;
    Vec2 operator-(const Vec2& other) const;
    Vec2 operator*(float scalar) const;
    Vec2 operator/(float scalar) const;
    bool operator==(const Vec2& other) const;

    using Getter = std::function<Vec2()>;
    using Setter = std::function<void(const Vec2&)>;
    Getter get;
    Setter set;
    Vec2(Getter g, Setter s) : get(std::move(g)), set(std::move(s)) {}

    float getX() const { if (get) return get().x; else return x; }
    float getY() const { if (get) return get().y; else return y; }
    void setX(float v) { if (get) { auto t = get(); t.x = v; set(t); } else x = v; }
    void setY(float v) { if (get) { auto t = get(); t.y = v; set(t); } else y = v; }
private:
    float x, y = 0.0;
};

namespace Object::Vec2Bind {
    void bind(Application* app);
}