/**
 * @file
 * @author  Tyrone Davison <t.p.davison@tees.ac.uk>
 * @date    September 2016
 * @version 2.16.0
 *
 *
 * @section DESCRIPTION
 *
 * Functions to create NURBS shapes.
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
#ifndef __TSL_NURBS_2__
#define __TSL_NURBS_2__

#include "geometry.hpp"

namespace tsl {
inline namespace tsl_2 {

/**
 * Compute an indexed mesh for a NURBS surface.
 * @param ucount          Number of control points in the u parametric
 *                        direction. Must be greater than one.
 * @param uknot           Array of knot values for the u parametric direction.
 * @param uknot_count     Number of elements in the uknot argument.
 * @param umin            Starting u value to generate the surface.
 *                        Currently ignored.
 * @param umax            Ending u value to generate the surface.
 *                        Currently ignored.
 * @param vcount          Number of control points in the v parametric
 *                        direction. Must be greater than one.
 * @param vknot           Array of knot values for the v parametric direction.
 * @param vknot_count     Number of elements in the vknot argument.
 * @param vmin            Starting v value to generate the surface.
 *                        Currently ignored.
 * @param vmax            Ending v value to generate the surface.
 *                        Currently ignored.
 * @param control_points  Array of homogeneous points (w is the NURBS weight)
 *                        describing the surface.  The array order must be
 *                        U-major and its size match ucount*vcount.
 * @param subdivisions_u  Control the number of divisions in the u parametric
 *                        direction for the generated mesh.
 * @param subdivisions_v  Control the number of divisions in the v parametric
 *                        direction for the generated mesh.
 * @return                An indexed mesh containing the generated output.
 */
TSL_API_FUNC
IndexedMesh * createNurbsSurface(unsigned int ucount,
                                 const float * uknot,
                                 unsigned int uknot_count,
                                 float umin,
                                 float umax,
                                 unsigned int vcount,
                                 const float * vknot,
                                 unsigned int vknot_count,
                                 float vmin,
                                 float vmax,
                                 const Vector4 * control_points,
                                 unsigned int subdivisions_u,
                                 unsigned int subdivisions_v);


/**
 * TODO
 */
TSL_API_FUNC
void deleteCurve(const Vector3 * curve);

/**
 * Compute an array of sample points for a NURBS curve.
 * @param knots           Array of knot values.
 * @param umin            Starting value to generate the curve.
 * @param umax            Ending value to generate the curve.
 * @param control_points  Array of homogeneous points (w is the NURBS weight)
 *                        describing the curve.
 * @param subdivisions    Control the number of sample points in the output.
 * @param sample_count    Optional output pointer for the generated sample 
 *                        point count.
 * @return                An arraying containing the generated output.
 */
TSL_API_FUNC
const Vector3 * createNurbsCurve(const float * knots,
                                 unsigned int knot_count,
                                 float umin,
                                 float umax,
                                 const Vector4 * control_points,
                                 unsigned int control_point_count,
                                 unsigned int subdivisions,
                                 unsigned int * sample_count);

TSL_API_TYPE
typedef std::unique_ptr<const Vector3[], void(*)(const Vector3*)> CurvePtr;

inline
IndexedMeshPtr createNurbsSurfacePtr(unsigned int ucount,
                                     const float * uknot,
                                     unsigned int uknot_count,
                                     float umin,
                                     float umax,
                                     unsigned int vcount,
                                     const float * vknot,
                                     unsigned int vknot_count,
                                     float vmin,
                                     float vmax,
                                     const Vector4 * control_points,
                                     unsigned int subdivisions_u,
                                     unsigned int subdivisions_v)
{
    return IndexedMeshPtr(createNurbsSurface(ucount,
                                             uknot,
                                             uknot_count,
                                             umin,
                                             umax,
                                             vcount,
                                             vknot,
                                             vknot_count,
                                             vmin,
                                             vmax,
                                             control_points,
                                             subdivisions_u,
                                             subdivisions_v),
                          deleteIndexedMesh);
}

inline
CurvePtr createNurbsCurvePtr(const float * knots,
                             unsigned int knot_count,
                             float umin,
                             float umax,
                             const Vector4 * control_points,
                             unsigned int control_point_count,
                             unsigned int subdivisions,
                             unsigned int * sample_count)
{
    return CurvePtr(createNurbsCurve(knots,
                                     knot_count,
                                     umin,
                                     umax,
                                     control_points,
                                     control_point_count,
                                     subdivisions,
                                     sample_count), deleteCurve);
}

} // end namespace tsl_2
} // end namespace tsl

#endif // __TSL_NURBS_2__
