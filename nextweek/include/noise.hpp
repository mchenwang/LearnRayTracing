#ifndef __NOISE_H__
#define __NOISE_H__

#include "algebra.hpp"

class Noise {
public:
    virtual double GetNoise(const point3d&) const = 0;
};

class PerlinNoise : public Noise {
protected:
    static constexpr int CNT = 256;
    double* rand_double;
    int* permutation_x;
    int* permutation_y;
    int* permutation_z;
    static int* GetPermutation() {
        int* p = new int[CNT];
        for (int i = 0; i < CNT; i++) p[i] = i;
        Shuffle(p, CNT);
        return p;
    }
    static void Shuffle(int* a, int cnt) {
        for (int i = cnt - 1; i > 0; i--) {
            int t = get_random_int(0, i);
            std::swap(a[i], a[t]);
        }
    }
public:
    PerlinNoise() noexcept {
        rand_double = new double[CNT];
        for (int i = 0; i < CNT; i++) rand_double[i] = get_random();
        permutation_x = GetPermutation();
        permutation_y = GetPermutation();
        permutation_z = GetPermutation();
    }
    ~PerlinNoise() noexcept {
        delete[] rand_double;
        delete[] permutation_x;
        delete[] permutation_y;
        delete[] permutation_z;
    }
    double GetNoise(const point3d& p) const override {
        auto i = static_cast<int>(p.x) & 255;
        auto j = static_cast<int>(p.y) & 255;
        auto k = static_cast<int>(p.z) & 255;
        return rand_double[permutation_x[i] ^ permutation_y[j] ^ permutation_z[k]];
    }
};

class TriPerlin : public PerlinNoise {
    double TrilinearInterp(double a[8], double x, double y, double z) const {
        return a[(0<<2)|(0<<1)|(0)] * (1. - x) * (1. - y) * (1. - z) + 
               a[(0<<2)|(0<<1)|(1)] * (1. - x) * (1. - y) * (     z) + 
               a[(0<<2)|(1<<1)|(0)] * (1. - x) * (     y) * (1. - z) + 
               a[(1<<2)|(0<<1)|(0)] * (     x) * (1. - y) * (1. - z) + 
               a[(0<<2)|(1<<1)|(1)] * (1. - x) * (     y) * (     z) + 
               a[(1<<2)|(0<<1)|(1)] * (     x) * (1. - y) * (     z) + 
               a[(1<<2)|(1<<1)|(0)] * (     x) * (     y) * (1. - z) + 
               a[(1<<2)|(1<<1)|(1)] * x * y * z;
    }
public:
    double GetNoise(const point3d& tp) const override {
        point3d p = tp * 4;
        auto u = p.x - floor(p.x);
        auto v = p.y - floor(p.y);
        auto w = p.z - floor(p.z);

        auto i = static_cast<int>(floor(p.x));
        auto j = static_cast<int>(floor(p.y));
        auto k = static_cast<int>(floor(p.z));
        double c[8];

        for (int di=0; di < 2; di++)
            for (int dj=0; dj < 2; dj++)
                for (int dk=0; dk < 2; dk++)
                    c[(di<<2)|(dj<<1)|(dk)] = rand_double[
                        permutation_x[(i+di) & 255] ^
                        permutation_y[(j+dj) & 255] ^
                        permutation_z[(k+dk) & 255]
                    ];

        return TrilinearInterp(c, u, v, w);
    }
};

#endif