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
    template<typename U>
    vec3(vec3<U>&& v) noexcept : x((T)v.x), y((T)v.y), z((T)v.z) {}

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

    vec3<T> operator+(const vec3<T>& v) const {
        return vec3<T>(x + v.x, y + v.y, z + v.z);
    }
    vec3<T> operator-(const vec3<T>& v) const {
        return vec3<T>(x - v.x, y - v.y, z - v.z);
    }
    vec3<T> operator*(const vec3<T>& v) const {
        return vec3<T>(x * v.x, y * v.y, z * v.z);
    }
    vec3<T> operator*(const T t) const {
        return vec3<T>(x * t, y * t, z * t);
    }
    vec3<T> operator/(const T t) const {
        return vec3<T>(x / t, y / t, z / t);
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

    vec3<T> normalize() const {
        double t = length();
        return vec3<T>((double)x / t, (double)y / t, (double)z / t);
    }
    vec3<T>& normalized() {
        double t = length();
        x /= t, y /= t, z /= t;
        return *this;
    }

    bool is_zero_vec() const {
        constexpr double EPS = 0.000000001;
        return std::abs(x) < EPS && std::abs(y) < EPS && std::abs(z) < EPS;
    }
};

template<typename T, typename U>
vec3<double> cross(const vec3<T>& v1, const vec3<U>& v2) {
    return vec3<double>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}
template<typename T, typename U>
double dot(const vec3<T>& v1, const vec3<U>& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
template<typename T>
vec3<double> operator*(const double x, const vec3<T>& v) {
    return vec3<double>(x * v.x, x * v.y, x * v.z);
}

using vec3d = vec3<double>;
using vec3i = vec3<int>;
using point3d = vec3d;
using point3i = vec3i;

inline double get_random(double min = 0., double max = 1.) {
    return min + (max - min) * (rand() / (RAND_MAX + 1.));
}
inline vec3d get_random_vec3d(double min = 0., double max = 1.) {
    return vec3d(get_random(min, max), get_random(min, max), get_random(min, max));
}

#endif