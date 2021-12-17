#ifndef __NOISE_H__
#define __NOISE_H__

#include "algebra.hpp"

class Noise {
public:
    virtual double Turb(const point3d&) const = 0;
};

class PerlinNoise : public Noise {
    static constexpr int CNT = 256;
    static constexpr int TURB_DEPTH = 7;
    vec3d* rand_vec;
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
    static double SmoothStep(double x) {
        return x * x * (3. - 2. * x);
    }
    double TrilinearInterp(vec3d c[8], double x, double y, double z) const {
        double ret = 0.;
        double xx = SmoothStep(x);
        double yy = SmoothStep(y);
        double zz = SmoothStep(z);
        for (int di=0; di < 2; di++)
            for (int dj=0; dj < 2; dj++)
                for (int dk=0; dk < 2; dk++) {
                    vec3d w(x - di, y - dj, z - dk);
                    ret += (xx * di + (1 - xx) * (1 - di))
                         * (yy * dj + (1 - yy) * (1 - dj))
                         * (zz * dk + (1 - zz) * (1 - dk))
                         * dot(c[(di<<2)|(dj<<1)|(dk)], w);
                }
        return ret;
    }
    double GetNoise(const point3d& p) const {
        auto u = p.x - floor(p.x);
        auto v = p.y - floor(p.y);
        auto w = p.z - floor(p.z);
        auto i = static_cast<int>(floor(p.x));
        auto j = static_cast<int>(floor(p.y));
        auto k = static_cast<int>(floor(p.z));
        vec3d c[8];

        for (int di=0; di < 2; di++)
            for (int dj=0; dj < 2; dj++)
                for (int dk=0; dk < 2; dk++)
                    c[(di<<2)|(dj<<1)|(dk)] = rand_vec[
                        permutation_x[(i+di) & 255] ^
                        permutation_y[(j+dj) & 255] ^
                        permutation_z[(k+dk) & 255]
                    ];

        return TrilinearInterp(c, u, v, w);
    }
public:
    PerlinNoise() noexcept {
        rand_vec = new vec3d[CNT];
        for (int i = 0; i < CNT; i++) rand_vec[i] = get_random_vec3d(-1, 1).normalize();
        permutation_x = GetPermutation();
        permutation_y = GetPermutation();
        permutation_z = GetPermutation();
    }
    ~PerlinNoise() noexcept {
        delete[] rand_vec;
        delete[] permutation_x;
        delete[] permutation_y;
        delete[] permutation_z;
    }
    double Turb(const point3d& p) const override {
        auto accum = 0.0;
        auto temp_p = p;
        auto weight = 1.0;

        for (int i = 0; i < TURB_DEPTH; i++) {
            accum += weight * GetNoise(temp_p);
            weight *= 0.5;
            temp_p *= 2;
        }

        return fabs(accum);
    }
};

#endif