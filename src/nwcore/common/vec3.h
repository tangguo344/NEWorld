/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This file is part of NEWorld.
* NEWorld is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* NEWorld is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef VEC3_H_
#define VEC3_H_

#include <cmath>
#include <type_traits>
#include <utility>

template <typename T>
class Vec3
{
public:
    T x, y, z;

    constexpr Vec3() : x(), y(), z()
    {
    }

    constexpr Vec3(T x_, T y_, T z_) : x(x_), y(y_), z(z_)
    {
    }

    constexpr Vec3(T value) : x(value), y(value), z(value)
    {
    }

    template <typename U, std::enable_if_t<std::is_convertible<T, U>::value, int> = 0>
    constexpr Vec3(const Vec3<U>& rhs) : x(T(rhs.x)), y(T(rhs.y)), z(T(rhs.z))
    {
    }

    // Get the square of vector length
    constexpr T lengthSqr() const
    {
        return x * x + y * y + z * z;
    }

    // Get vector length
    double length() const
    {
        return sqrt(double(lengthSqr()));
    }

    // Get the Euclidean Distance between vectors
    double euclideanDistance(const Vec3& rhs) const
    {
        return (*this - rhs).length();
    }

    // Get the Chebyshev Distance between vectors
    constexpr T chebyshevDistance(const Vec3& rhs) const
    {
        return max(max(abs(x - rhs.x), abs(y - rhs.y)), abs(z - rhs.z));
    }

    // Get the Manhattan Distance between vectors
    constexpr T manhattanDistance(const Vec3& rhs) const
    {
        return abs(x - rhs.x) + abs(y - rhs.y) + abs(z - rhs.z);
    }

    // Normalize vector
    void normalize()
    {
        double l = length();
        x /= l;
        y /= l;
        z /= l;
    }

    bool operator< (const Vec3& rhs) const
    {
        if (x != rhs.x)
            return x < rhs.x;
        if (y != rhs.y)
            return y < rhs.y;
        if (z != rhs.z)
            return z < rhs.z;
        return false;
    }

    constexpr bool operator== (const Vec3& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    Vec3& operator+= (const Vec3& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    Vec3& operator-= (const Vec3& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    Vec3<T>& operator*= (T value)
    {
        x *= value;
        y *= value;
        z *= value;
        return *this;
    }

    Vec3<T>& operator/= (T value)
    {
        x /= value;
        y /= value;
        z /= value;
        return *this;
    }

    constexpr Vec3 operator* (T value) const
    {
        return Vec3(x * value, y * value, z * value);
    }

    constexpr Vec3 operator/ (T value) const
    {
        return Vec3(x / value, y / value, z / value);
    }

    constexpr bool operator!= (const Vec3& rhs) const
    {
        return !(rhs == *this);
    }

    constexpr Vec3 operator+ (const Vec3& rhs) const
    {
        return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
    };

    constexpr Vec3 operator- (const Vec3& rhs) const
    {
        return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
    };

    void swap(Vec3& rhs)
    {
        std::swap(x, rhs.x);
        std::swap(y, rhs.y);
        std::swap(z, rhs.z);
    }

    template <typename... ArgType, typename Func>
    void for_each(Func func, ArgType&&... args) const
    {
        func(x, std::forward<ArgType>(args)...);
        func(y, std::forward<ArgType>(args)...);
        func(z, std::forward<ArgType>(args)...);
    }

    template <typename... ArgType, typename Func>
    void for_each(Func func, ArgType&&... args)
    {
        func(x, std::forward<ArgType>(args)...);
        func(y, std::forward<ArgType>(args)...);
        func(z, std::forward<ArgType>(args)...);
    }

    //TODO: fix it. And tell if "for_each" will change the value of "this".
    template <typename Func>
    Vec3 transform(Func func) const
    {
        return Vec3(func(x), func(y), func(z));
    }

    template <typename Func>
    Vec3 transform(Func func)
    {
        return Vec3(func(x), func(y), func(z));
    }

    template <typename Func>
    static void for_range(T begin, T end, Func func)
    {
        Vec3<T> tmp;
        for (tmp.x = begin; tmp.x < end; ++tmp.x)
            for (tmp.y = begin; tmp.y < end; ++tmp.y)
                for (tmp.z = begin; tmp.z < end; ++tmp.z)
                    func(tmp);
    }

    template <typename Func>
    static void for_range(const Vec3<T>& begin, const Vec3<T>& end, Func func)
    {
        Vec3<T> tmp;
        for (tmp.x = begin.x; tmp.x < end.x; ++tmp.x)
            for (tmp.y = begin.y; tmp.y < end.y; ++tmp.y)
                for (tmp.z = begin.z; tmp.z < end.z; ++tmp.z)
                    func(tmp);
    }

    template <typename U, U base>
    U encode()
    {
        return (x * base + y) * base + z;
    }

    template <typename U, U base>
    static Vec3<U> decode(T arg)
    {
        U z = arg % base;
        arg /= base;
        return Vec3<U>(arg / base, arg % base, z);
    }

    friend Vec3<T> operator- (const Vec3<T>& vec)
    {
        return Vec3<T>(-vec.x, -vec.y, -vec.z);
    }

    template<class Vec3Type>
    Vec3Type conv() const
    {
        return Vec3Type(x, y, z);
    }

private:
    // to solve problems about `abs`, we need this.
    constexpr static T abs(T arg)
    {
        return arg >= 0 ? arg : -arg;
    }

    constexpr static T max(T arg1, T arg2)
    {
        return (arg1 > arg2) ? arg1 : arg2;
    }
};

using Vec3i = Vec3<int>;
using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;

#endif // !VEC3_H_
