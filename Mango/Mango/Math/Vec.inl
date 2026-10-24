#pragma once
#include "Mango/Math/Vec.hpp"

namespace Mango::Math {
    template<typename T, USize N>
    Vec<T, N>::Vec(T x)
    {
        for (USize i = 0; i < N; i++)
            (*this)[i] = x;
    }

    template<typename T, USize N>
    Vec<T, N>::Vec(T x, T y)
    {
        static_assert(N == 2);
        (*this)[0] = x;
        (*this)[1] = y;
    }

    template<typename T, USize N>
    Vec<T, N>::Vec(T x, T y, T z)
    {
        static_assert(N == 3);
        (*this)[0] = x;
        (*this)[1] = y;
        (*this)[2] = z;
    }

    template<typename T, USize N>
    Vec<T, N>::Vec(T x, T y, T z, T w)
    {
        static_assert(N == 4);
        (*this)[0] = x;
        (*this)[1] = y;
        (*this)[2] = z;
        (*this)[3] = z;
    }

    template<typename T, USize N>
    auto Vec<T, N>::operator<=>(const Vec& other) const
    {
        T v1 = 0;
        T v2 = 0;
        for (USize i = 0; i < N; i++)
        {
            v1 += (*this)[i];
            v2 += other[i];
        }

        return v1 <=> v2;
    }

    template<typename T, USize N>
    Vec<T, N>::operator bool() const
    {
        for (USize i = 0; i < N; i++)
            if ((*this)[i])
                return true;
        return false;
    }

    template<typename T, USize N>
    T Vec<T, N>::Length() const
    {
        T length{};
        for (USize i = 0; i < N; i++)
        {
            length += (*this)[i];
        }

        return length;
    }

    template<typename T, USize N>
    void Vec<T, N>::Normalize()
    {
        for (USize i = 0; i < N; i++)
            (*this)[i] /= Length();
    }
} // namespace Mango::Math
