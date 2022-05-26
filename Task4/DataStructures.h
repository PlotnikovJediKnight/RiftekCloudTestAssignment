#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <cstddef>
#include <cmath>
#include <stdexcept>

struct GeneratedPoint{
    float x, y, z;
    float n_x, n_y, n_z;

    GeneratedPoint(float _x, float _y, float _z, float nx = -1.0, float ny = -1.0, float nz = -1.0) :
    x(_x), y(_y), z(_z),
    n_x(nx), n_y(ny), n_z(nz) { }

    GeneratedPoint(float value = 0.0) :
        x(value), y(value), z(value),
        n_x(-1.0), n_y(-1.0), n_z(-1.0){ }

    GeneratedPoint(const GeneratedPoint& copy) :
    x(copy.x), y(copy.y), z(copy.z),
    n_x(copy.n_x), n_y(copy.n_y), n_z(copy.n_z) { }

    GeneratedPoint& operator=(const GeneratedPoint& copy){
        x = copy.x;
        y = copy.y;
        z = copy.z;
        n_x = copy.n_x;
        n_y = copy.n_y;
        n_z = copy.n_z;

        return *this;
    }


    const float& operator[](size_t index) const{
        if (index >= 3) throw std::runtime_error("wrong index");

        switch (index){
            case 0: return x;
            case 1: return y;
            case 2: return z;
        }

        throw std::runtime_error("wrong index");
    }

    float& operator[](size_t index) {
        if (index >= 3) throw std::runtime_error("wrong index");

        switch (index){
            case 0: return x;
            case 1: return y;
            case 2: return z;
        }

        throw std::runtime_error("wrong index");
    }
};

inline bool operator<(const GeneratedPoint& lhs, const GeneratedPoint& rhs){
    return (lhs.x < rhs.x && lhs.y < rhs.y && lhs.z < rhs.z);
}

inline bool operator==(const GeneratedPoint& lhs, const GeneratedPoint& rhs){
    return lhs.x == rhs.x &&
           lhs.y == rhs.y &&
           lhs.z == rhs.z;
}

inline GeneratedPoint operator+(const GeneratedPoint& lhs, const GeneratedPoint& rhs) {
    return { lhs.x + rhs.x,
             lhs.y + rhs.y,
             lhs.z + rhs.z};
}

inline GeneratedPoint operator-(const GeneratedPoint& lhs, const GeneratedPoint& rhs) {
    return { lhs.x - rhs.x,
             lhs.y - rhs.y,
             lhs.z - rhs.z};
}

inline GeneratedPoint operator*(const GeneratedPoint& lhs, float rhs){
    return { lhs.x * rhs,
             lhs.y * rhs,
             lhs.z * rhs};
}

inline GeneratedPoint operator/(const GeneratedPoint& lhs, float rhs){
    return { lhs.x / rhs,
             lhs.y / rhs,
             lhs.z / rhs};
}

inline float GetSquaredLength(const GeneratedPoint& vector){
    return  vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

inline float GetRegularLength(const GeneratedPoint& vector){
    return  sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

inline GeneratedPoint CeilVector(const GeneratedPoint& vector){
    return { static_cast<float>(ceil(vector.x)),
             static_cast<float>(ceil(vector.y)),
             static_cast<float>(ceil(vector.z)),
             vector.n_x, vector.n_y, vector.n_z};
}

inline GeneratedPoint IntCastVector(const GeneratedPoint& vector){
    return { static_cast<float>(static_cast<int>(vector.x)),
             static_cast<float>(static_cast<int>(vector.y)),
             static_cast<float>(static_cast<int>(vector.z)),
             vector.n_x, vector.n_y, vector.n_z};
}

inline GeneratedPoint GetMaxVector(const GeneratedPoint& lhs, const GeneratedPoint& rhs){
    if (lhs.x >= rhs.x &&
        lhs.y >= rhs.y &&
        lhs.z >= rhs.z){
        return lhs;
    } else {
        return rhs;
    }
}

inline GeneratedPoint ClampVectors(const GeneratedPoint& v, const GeneratedPoint& lo, const GeneratedPoint& hi){
    if (v < lo) return lo;
    if (hi < v) return hi;
    return v;
}

inline GeneratedPoint CrossProduct(const GeneratedPoint& lhs, const GeneratedPoint& rhs){
    float x1 = lhs.x,
          y1 = lhs.y,
          z1 = lhs.z;

    float x2 = rhs.x,
          y2 = rhs.y,
          z2 = rhs.z;

    float xNorm = y1 * z2 - z1 * y2,
          yNorm = z1 * x2 - x1 * z2,
          zNorm = x1 * y2 - y1 * x2;

    return { xNorm, yNorm, zNorm};
}

inline GeneratedPoint GetUnitVector(const GeneratedPoint& vector){
    float length = sqrt(GetSquaredLength(vector));
    return { vector.x / length, vector.y / length, vector.z / length};
}

inline float DotProduct(const GeneratedPoint& lhs, const GeneratedPoint& rhs){
    float x1 = lhs.x,
          y1 = lhs.y,
          z1 = lhs.z;

    float x2 = rhs.x,
          y2 = rhs.y,
          z2 = rhs.z;

    return x1 * x2 + y1 * y2 + z1 * z2;
}

struct GeneratedColor{
    float r, g, b;
};

#endif // DATASTRUCTURES_H
