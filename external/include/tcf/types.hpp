/**
 * @file
 * @author  Tyrone Davison <t.p.davison@tees.ac.uk>
 * @date    September 2016
 * @version 2.16.0
 *
 *
 * @section DESCRIPTION
 *
 * Basic types for the TCF API.
 *
 *
 * @section LICENSE
 *
 * Copyright (c) 2016 Tyrone Davison <t.p.davison@tees.ac.uk>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#pragma once
#ifndef __TCF_TYPES_2__
#define __TCF_TYPES_2__

#include "config.hpp"
#include <cstdint>

namespace tcf {
inline namespace tcf_2 {

/**
 * Chunk type enumerate via a four character code.
 */
class TCF_API_TYPE Type
{
public:

    Type(const char name[4])
    {
        str[0] = name[0];
        str[1] = name[1];
        str[2] = name[2];
        str[3] = name[3];
    }

    bool operator==(Type rhs)
    {
        return hash == rhs.hash;
    }

    bool operator!=(Type rhs)
    {
        return hash != rhs.hash;
    }

    static Type nullType()
    {
        return Type();
    }

private:

    Type()
    {
        hash = 0;
    }

    union
    {
        uint32_t hash;
        char str[4];
    };
};

/**
 * A generic unrecoverable error exception
 */
class TCF_API_TYPE Exception
{
public:
    virtual const char * what() noexcept
    {
        return "an unknown exception has occurred";
    }
};

/**
 * An unrecoverable file-based error exception
 */
class TCF_API_TYPE FileException : Exception
{
public:
    virtual const char * what() noexcept override
    {
        return "an unrecoverable file-based error has occurred";
    }
};

/**
 * Basic 2d vector value-type.
 */
struct TCF_API_TYPE Vector2
{
    float x, y;
    Vector2() : x(0), y(0) {}
    Vector2(float X, float Y) : x(X), y(Y) {}
};


/**
 * Basic 3d vector value-type.
 */
struct TCF_API_TYPE Vector3
{
    float x, y, z;
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
};


/**
 * Basic 4d vector value-type.
 */
struct TCF_API_TYPE Vector4
{
    float x, y, z, w;
    Vector4() : x(0), y(0), z(0), w(1) {}
    Vector4(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}
};


/**
 * Basic 4x4 matrix value-type.
 */
class TCF_API_TYPE Matrix44
{
public:
    float m00, m10, m20, m30,
          m01, m11, m21, m31,
          m02, m12, m22, m32,
          m03, m13, m23, m33;
public:
    Matrix44() : m00(1), m10(0), m20(0), m30(0),
                 m01(0), m11(1), m21(0), m31(0),
                 m02(0), m12(0), m22(1), m32(0),
                 m03(0), m13(0), m23(0), m33(1) {
    }
    Matrix44(float M00, float M01, float M02, float M03,
             float M10, float M11, float M12, float M13,
             float M20, float M21, float M22, float M23,
             float M30, float M31, float M32, float M33)
        : m00(M00), m10(M10), m20(M20), m30(M30),
          m01(M01), m11(M11), m21(M21), m31(M31),
          m02(M02), m12(M12), m22(M22), m32(M32),
          m03(M03), m13(M13), m23(M23), m33(M33) {
    }
};

} // end namespace tcf_2
} // end namespace tcf

#endif // __TCF_TYPES_2__
