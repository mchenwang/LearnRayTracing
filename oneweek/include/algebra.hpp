#ifndef __ALGEBRA_H__
#define __ALGEBRA_H__

#include <type_traits>

template<typename T>
struct vec3 {
    static_assert(std::is_same<T, int>::value ||
                  std::is_same<T, double>::value
                  , "vec type error!");
    T x, y, z;
    vec3() noexcept : x(0), y(0), z(0) {}
    vec3(T x_, T y_, T z_) noexcept : x(x_), y(y_), z(z_) {}

    T operator[] (const size_t i) const {
        if(i > 3 || i < 0) std::cerr << i << " vec access error\n";
        if(i == 0) return x;
        if(i == 1) return y;
        return z;
    }
    T& operator[] (const size_t i) {
        if(i > 3 || i < 0) std::cerr << i << " vec access error\n";
        if(i == 0) return x;
        if(i == 1) return y;
        return z;
    }

    vec3<T> operator+(const vec3<T>& v) {
        return vec3<T>(x + v.x, y + v.y, z + v.z);
    }
    vec3<T> operator-(const vec3<T>& v) {
        return vec3<T>(x - v.x, y - v.y, z - v.z);
    }
    vec3<T> operator*(const vec3<T>& v) {
        return vec3<T>(x * v.x, y * v.y, z * v.z);
    }
    vec3<T> operator*(const T t) {
        return vec3<T>(x * t, y * t, z * t);
    }
    vec3<T>& operator+=(const vec3<T>& v) {
        x += v.x, y += v.y, z += v.z;
        return *this;
    }
    vec3<T>& operator*=(const T t) {
        x *= t, y *= t, z *= t;
        return *this;
    }
    vec3<T>& operator/=(const T t) {
        return *this *= 1.0/t;
    }

    double length() const { return sqrt(length2()); }
    double length2() const { return x * x + y * y + z * z; }
};

template<typename T, typename U>
vec3<double> cross(const vec3<T>& v1, const vec3<U>& v2) {
    return vec3<double>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}
template<typename T, typename U>
double dot(const vec3<T>& v1, const vec3<U>& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

using vec3d = vec3<double>;
using vec3i = vec3<int>;
using point3d = vec3d;
using point3i = vec3i;

#endif