/**
 * @file
 * @author  Tyrone Davison <t.p.davison@tees.ac.uk>
 * @date    September 2016
 * @version 2.16.0
 *
 *
 * @section DESCRIPTION
 *
 * Basic math representation classes and functions.
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
#ifndef __TSL_GEOMETRY_2__
#define __TSL_GEOMETRY_2__

#include "types.hpp"

#include <memory>

namespace tsl {
inline namespace tsl_2 {

/**
 * TODO
 */
enum TSL_API_TYPE PrimitiveType
{
    PRIMITIVETYPE_POLYGONLIST,
    PRIMITIVETYPE_TRIANGLELIST,
    PRIMITIVETYPE_UNKNOWN = 0x7FFFFFFF
};

/**
 * A mesh constructed from vertices joined into primitives via an array of
 * indices. This is the standard output format of the shape generation methods.
 */
class TSL_API_TYPE IndexedMesh
{
public:

    /**
     * TODO
     */
    virtual unsigned int vertexCount() const noexcept = 0;

    /**
     * Array giving the position of each vertex.
     * Length of the array indicates number of vertices in mesh.
     */
    virtual const Vector3 * positionArray() const noexcept = 0;

    /**
     * Array giving the normal vector of each vertex.
     * Length should be zero or match vertex_array.
     */
    virtual const Vector3 * normalArray() const noexcept = 0;

    /**
     * Array giving the binormal vector of each vertex.
     * Length should be zero or match vertex_array.
     */
    virtual const Vector3 * binormalArray() const noexcept = 0;

    /**
     * Array giving the tangent vector of each vertex.
     * Length should be zero or match vertex_array.
     */
    virtual const Vector3 * tangentArray() const noexcept = 0;

    /**
     * Array giving the uv surface coordinate of each vertex.
     * Length should be zero or match vertex_array.
     */
    virtual const Vector2 * uvArray() const noexcept = 0;

    /**
     * TODO
     */
    virtual unsigned int indexCount() const noexcept = 0;

    /**
     * Array of indices to map vertices into primitives.
     * The data is only meaningful if you know the primitive type.
     * An index of -1 is used to terminate polygons.
     */
    virtual const int * indexArray() const noexcept = 0;

    /**
     * TODO
     */
    virtual PrimitiveType primitiveType() const noexcept = 0;

    /**
     * Compute the number of primitives (e.g. polygons, triangles) described
     * by the mesh's index array.
     * @return  The number of primitives described by the mesh.
     */
    virtual unsigned int primitiveCount() const noexcept = 0;

    /**
     * TODO
     */
    virtual void release() noexcept = 0;

protected:
    virtual ~IndexedMesh() noexcept = default;

};

/**
 * TODO positionData cannot be null if vertexCount > 0, all arrays must be same size
 */
TSL_API_FUNC
IndexedMesh * createIndexedMesh(unsigned int vertexCount,
                                const Vector3 * positionData,
                                const Vector3 * normalData,
                                const Vector3 * binormalData,
                                const Vector3 * tangentData,
                                const Vector2 * uvData,
                                PrimitiveType primitiveType,
                                unsigned int indexCount,
                                const int * indexData) noexcept(false);

/**
* TODO
*/
TSL_API_FUNC
void deleteIndexedMesh(IndexedMesh * mesh);

/**
 * TODO must be same primitive type
 */
TSL_API_FUNC
IndexedMesh * cloneIndexedMeshAsTriangleList(const IndexedMesh * mesh);

/**
 * Clone a polygon mesh but adjust the description into a triangle list.
 * Only valid on a mesh which has index data described as polygons.
 * @param mesh  Pointer to a valid mesh of polygons described using index data.
 * @return      A new mesh object described using a triangle list.
 */
TSL_API_FUNC
IndexedMesh * mergeIndexedMeshes(const IndexedMesh * a,
                                 const IndexedMesh * b);

/**
* TODO
*/
TSL_API_TYPE
typedef std::unique_ptr<IndexedMesh, void(*)(IndexedMesh*)> IndexedMeshPtr;

inline
IndexedMeshPtr createIndexedMeshPtr(unsigned int vertexCount,
                                    const Vector3 * positionData,
                                    const Vector3 * normalData,
                                    const Vector3 * binormalData,
                                    const Vector3 * tangentData,
                                    const Vector2 * uvData,
                                    PrimitiveType primitiveType,
                                    unsigned int indexCount,
                                    const int * indexData)
{
    return IndexedMeshPtr(createIndexedMesh(vertexCount,
                                            positionData,
                                            normalData,
                                            binormalData,
                                            tangentData,
                                            uvData,
                                            primitiveType,
                                            indexCount,
                                            indexData),
                          deleteIndexedMesh);
}

inline
IndexedMeshPtr cloneIndexedMeshAsTriangleListPtr(const IndexedMesh * mesh)
{
    return IndexedMeshPtr(cloneIndexedMeshAsTriangleList(mesh),
                          deleteIndexedMesh);
}

inline
IndexedMeshPtr mergeIndexedMeshesPtr(const IndexedMesh * a,
                                     const IndexedMesh * b)
{
    return IndexedMeshPtr(mergeIndexedMeshes(a, b),
                          deleteIndexedMesh);
}

} // end namespace tsl_2
} // end namespace tsl

#endif // __TSL_GEOMETRY_2__
