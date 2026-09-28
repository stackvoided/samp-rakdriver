#pragma once

struct Vector3 {
    float x{ 0.0f };
    float y{ 0.0f };
    float z{ 0.0f };

    Vector3() = default;
    Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    Vector3 operator+(const Vector3& other) const { return { x + other.x, y + other.y, z + other.z }; }
    Vector3 operator-(const Vector3& other) const { return { x - other.x, y - other.y, z - other.z }; }
    Vector3 operator*(float scalar) const { return { x * scalar, y * scalar, z * scalar }; }
};
