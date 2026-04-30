#pragma once
#include <cmath>

struct Vec2 {
    float x, y;
};

struct Vec3 {
    float x, y, z;
    
    Vec3 operator-(const Vec3& o) const { return {x-o.x, y-o.y, z-o.z}; }
    Vec3 operator+(const Vec3& o) const { return {x+o.x, y+o.y, z+o.z}; }
    
    float Length() const { return std::sqrt(x*x + y*y + z*z); }
    float DistTo(const Vec3& o) const { return (*this - o).Length(); }
};

struct Matrix4x4 {
    float m[4][4];
    
    bool WorldToScreen(const Vec3& world, Vec2& screen, int width, int height) const {
        float w = m[3][0]*world.x + m[3][1]*world.y + m[3][2]*world.z + m[3][3];
        if (w < 0.001f) return false;
        
        float invW = 1.0f / w;
        screen.x = (m[0][0]*world.x + m[0][1]*world.y + m[0][2]*world.z + m[0][3]) * invW;
        screen.y = (m[1][0]*world.x + m[1][1]*world.y + m[1][2]*world.z + m[1][3]) * invW;
        
        screen.x = (width * 0.5f) + (screen.x * width) * 0.5f;
        screen.y = (height * 0.5f) - (screen.y * height) * 0.5f;
        
        return true;
    }
};