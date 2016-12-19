/**
 * @file
 * @author  Tyrone Davison <t.p.davison@tees.ac.uk>
 * @date    September 2016
 * @version 2.16.0
 *
 *
 * @section DESCRIPTION
 *
 * Functions to create basic shapes.
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
#ifndef __TSL_SHAPES_2__
#define __TSL_SHAPES_2__

#include "geometry.hpp"

namespace tsl {
inline namespace tsl_2 {

/**
 * Generate an indexed mesh of polygons that describes a box.
 * @param X            Size of the box in the x axis.  Must be positive.
 * @param Y            Size of the box in the y axis.  Must be positive.
 * @param Z            Size of the box in the z axis.  Must be positive.
 * @param subdivisions Control the tesselation of the box faces.
 *                     Must be greater than zero.
 * @return             An indexed mesh containing the generated output.
 */
TSL_API_FUNC
IndexedMesh * createBox(float X, float Y, float Z,
                        unsigned int subdivisions);

/**
 * Generate an indexed mesh of polygons that describes a cube.
 * @param size         Dimension of the cube in each of the x, y, z axes.
 *                     Must be positive.
 * @param subdivisions Control the tesselation of the cube faces.
 *                     Must be greater than zero.
 * @return             An indexed mesh containing the generated output.
 */
TSL_API_FUNC
IndexedMesh * createCube(float size,
                         unsigned int subdivisions);

/**
 * Generate an indexed mesh of polygons that describes a sphere.
 * @param radius       Radius of the sphere.  Must be positive.
 * @param subdivisions Control the tesselation of the generated output.
 *                     Must be greater than zero.
 * @return             An indexed mesh containing the generated output.
 */
TSL_API_FUNC
IndexedMesh * createSphere(float radius,
                           unsigned int subdivisions);

/**
 * Generate an indexed mesh of polygons that describes a cylinder.
 * @param radius       Radius of the cylinder in the xy plane.
 *                     Must be positive.
 * @param height       Height of the cylinder in the z axis.
 *                     Must be positive.
 * @param subdivisions Control the tesselation of the generated output.
 *                     Must be greater than zero.
 * @return             An indexed mesh containing the generated output.
 */
TSL_API_FUNC
IndexedMesh * createCylinder(float radius,
                             float height,
                             unsigned int subdivisions);

/**
 * Generate an indexed mesh of polygons that describes a cone.
 * @param radius       Radius of the cone in the xy plane.  Must be positive.
 * @param height       Height of the cone in the z axis.  Must be positive.
 * @param subdivisions Control the tesselation of the generated output.
 *                     Must be greater than zero.
 * @return             An indexed mesh containing the generated output.
 */
TSL_API_FUNC
IndexedMesh * createCone(float radius,
                          float height,
                          unsigned int subdivisions);

/**
 * Generate an indexed mesh of polygons that describes a disc.
 * @param radius       Radius of the disc in the xy plane.  Must be positive.
 * @param height       Height of the disc in the z axis.  The sign of the
 *                     height dicates the direction of the disc normals.
 * @param subdivisions Control the tesselation of the generated output.
 *                     Must be greater than zero.
 * @return             An indexed mesh containing the generated output.
 */
TSL_API_FUNC
IndexedMesh * createDisk(float radius,
                         float height,
                         unsigned int subdivisions);



inline
IndexedMeshPtr createBoxPtr(float X, float Y, float Z,
                            unsigned int subdivisions)
{
    return IndexedMeshPtr(createBox(X, Y, Z, subdivisions),
                          deleteIndexedMesh);
}

inline
IndexedMeshPtr createCubePtr(float size,
                             unsigned int subdivisions)
{
    return IndexedMeshPtr(createCube(size, subdivisions),
                          deleteIndexedMesh);
}

inline
IndexedMeshPtr createSpherePtr(float radius,
                               unsigned int subdivisions)
{
    return IndexedMeshPtr(createSphere(radius, subdivisions),
                          deleteIndexedMesh);
}

inline
IndexedMeshPtr createCylinderPtr(float radius,
                                 float height,
                                 unsigned int subdivisions)
{
    return IndexedMeshPtr(createCylinder(radius, height, subdivisions),
                          deleteIndexedMesh);
}

inline
IndexedMeshPtr createConePtr(float radius,
                             float height,
                             unsigned int subdivisions)
{
    return IndexedMeshPtr(createCone(radius, height, subdivisions),
                          deleteIndexedMesh);
}

inline
IndexedMeshPtr createDiskPtr(float radius,
                             float height,
                             unsigned int subdivisions)
{
    return IndexedMeshPtr(createDisk(radius, height, subdivisions),
                          deleteIndexedMesh);
}

} // end namespace tsl_2
} // end namespace tsl

#endif // __TSL_SHAPES_2__
