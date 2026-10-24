#pragma once
#include "Mango/Common/CommonTypes.hpp"

namespace Mango::Math {
    namespace Detail {
        template<typename T, USize N>
        struct Storage
        {
            T Data[N]{};

            T& operator[](USize i) { return Data[i]; }
            const T& operator[](USize i) const { return Data[i]; }
        };

        template<typename T>
        struct Storage<T, 2>
        {
            union
            {
                T X, R;
            };

            union
            {
                T Y, G;
            };

            T& operator[](USize i) { return *(this + i * sizeof(T)); }
            const T& operator[](USize i) const { return *(this + i * sizeof(T)); }
        };

        template<typename T>
        struct Storage<T, 3>
        {
            union
            {
                T X, R;
            };

            union
            {
                T Y, G;
            };

            union
            {
                T Z, B;
            };

            T& operator[](USize i) { return *(this + i * sizeof(T)); }
            const T& operator[](USize i) const { return *(this + i * sizeof(T)); }
        };

        template<typename T>
        struct Storage<T, 4>
        {
            union
            {
                T X, R;
            };

            union
            {
                T Y, G;
            };

            union
            {
                T Z, B;
            };

            union
            {
                T W, A;
            };

            T& operator[](USize i) { return *(this + i * sizeof(T)); }
            const T& operator[](USize i) const { return *(this + i * sizeof(T)); }
        };
    } // namespace Detail

    template<typename T, USize N>
    struct Vec : Detail::Storage<T, N>
    {
        Vec(T x);
        Vec(T x, T y);
        Vec(T x, T y, T z);
        Vec(T x, T y, T z, T w);

        auto operator<=>(const Vec& other) const;

        explicit operator bool() const;

        T Length() const;

        void Normalize();
    };
    
    using Vec2 = Vec<F32, 2>;
    using Vec3 = Vec<F32, 3>;
    using Vec4 = Vec<F32, 4>;
} // namespace Mango::Math
