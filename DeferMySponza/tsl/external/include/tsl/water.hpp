/**
 * @file
 * @author  Tyrone Davison <t.p.davison@tees.ac.uk>
 * @date    September 2016
 * @version 2.16.0
 *
 *
 * @section DESCRIPTION
 *
 * Water simulations to produce geometry dynamically frame-by-frame.
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
#ifndef __TSL_WATER_2__
#define __TSL_WATER_2__

#include "types.hpp"

#include <memory>

namespace tsl {
inline namespace tsl_2 {

/**
 * Produces animated ocean waves using an algorithm similar to that used in
 * the movie Titanic. The output geometry can be tiled together to produce
 * infinite oceans.
 */
class TSL_API_TYPE Water
{
public:
    /**
     * Advances the water simulation and returns the new vertex positions
     * and normals.
     * @param num_steps       The number of simulation steps to take.  Base
     *                        this around the elapsed time and how big you
     *                        defined a step in the constructor.
     * @param position_array  Pointer to an array with sufficient storage to
     *                        recieve the vertex positions.  Use the
     *                        VertexCount method to determine storage
     *                        requirements.  Must not be null.
     * @param position_stride The number of bytes you wish between one position
     *                        and the next in the position_array.  A value of
     *                        zero is equivalent to a value of sizeof(float3).
     * @param normal_array    Pointer to an array with sufficient storage to
     *                        recieve the vertex normal vectors.  Use the
     *                        VertexCount method to determine storage
     *                        requirements.  Must not be null.
     * @param normal_stride   The number of bytes you wish between one normal
     *                        and the next in the normal_array.  A value of
     *                        zero is equivalent to a value of sizeof(float3).
     * @return                The time in seconds which the simulator has
     *                        stepped forward.
     */
    virtual float advance(const int num_steps,
                          float* const position_array,
                          const int position_stride,
                          float* const normal_array,
                          const int normal_stride) = 0;

    /**
     * Compute the number of indices in the ocean mesh.
     */
    virtual int indexCount() const noexcept = 0;

    /**
     * Retrieve the index array of triangle list which describes the mesh.
     * @param index_array  Pointer to an array with sufficient storage to
     *                     receive the index data.  Use the IndexCount method
     *                     to determine storage requirements.
     */
    virtual void getIndices(int * index_array) noexcept = 0;

    /**
     * Compute the number of vertices in the ocean mesh.
     */
    virtual int vertexCount() const noexcept = 0;

    /**
     * TODO
     */
    virtual void release() noexcept = 0;

protected:
    virtual ~Water() noexcept = default;
    
};

/**
 * Constructs a new ocean water simulation object.
 * @param time_step       The size in seconds of each simulation step.
 * @param size            The dimensions in the xy plane of the ocean tile.
 * @param num_points      The number of vertices in each xy direction for the
 *                        resultant mesh.
 * @param scale_factor    Tweak parameter for the amplitude of the waves.
 *                        Start with values in the range [0,1].
 * @param wind_direction  Angle in radians describing the direction of the
 *                        wind across the water.
 * @param wind_speed      Speed in metres per second of the wind across the
 *                        water. Start with 15m/s.
 * @return                New water simulation object.
 */
TSL_API_FUNC
Water * createWater(float time_step,
                    float size,
                    int num_points,
                    float scale_factor,
                    float wind_direction,
                    float wind_speed);

/**
 * TODO
 */
TSL_API_FUNC
void deleteWater(Water * water);

/**
 * TODO
 */
typedef std::unique_ptr<Water, void(*)(Water*)> WaterPtr;

inline
WaterPtr createWaterPtr(float time_step,
                        float size,
                        int num_points,
                        float scale_factor,
                        float wind_direction,
                        float wind_speed)
{
    return WaterPtr(createWater(time_step,
                                size,
                                num_points,
                                scale_factor,
                                wind_direction,
                                wind_speed),
                    deleteWater);
}

} // end namespace tsl_2
} // end namespace tsl

#endif // __TSL_WATER_2__
