/**
 * @file
 * @author  Tyrone Davison <t.p.davison@tees.ac.uk>
 * @date    September 2016
 * @version 2.16.0
 *
 *
 * @section DESCRIPTION
 *
 * Methods to read and write a simple scene of multiple geometries.
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
#ifndef __TCF_SIMPLESCENE_2__
#define __TCF_SIMPLESCENE_2__

#include "types.hpp"

#include <memory>

namespace tcf {
inline namespace tcf_2 {

class Reader;
class Writer;

class TCF_API_TYPE SimpleScene
{
public:

    enum Topology
    {
        kIndexedTriangleList = 1,
        kUnknown = 0x7FFFFFFF
    };

    class Mesh
    {
    public:
        virtual Topology topology() const noexcept = 0;
        virtual const char * name() const noexcept = 0;
        virtual unsigned int indexCount() const noexcept = 0;
        virtual const int * indexArray() const noexcept = 0;
        virtual int * indexArray() noexcept = 0;
        virtual unsigned int vertexCount() const noexcept = 0;
        virtual const Vector3 * positionArray() const noexcept = 0;
        virtual Vector3 * positionArray() noexcept = 0;
        virtual const Vector3 * normalArray() const noexcept = 0;
        virtual Vector3 * normalArray() noexcept = 0;
        virtual const Vector3 * tangentArray() const noexcept = 0;
        virtual Vector3 * tangentArray() noexcept = 0;
        virtual const Vector2 * uvArray() const noexcept = 0;
        virtual Vector2 * uvArray() noexcept = 0;
        virtual unsigned int instanceCount() const noexcept = 0;
        virtual const Matrix44 * transformationArray() const noexcept = 0;
        virtual Matrix44 * transformationArray() noexcept = 0;
        virtual const int * materialArray() const noexcept = 0;
        virtual int * materialArray() noexcept = 0;

        virtual void setGeometry(Topology topology,
                                 unsigned int vertexCount,
                                 bool hasNormalData,
                                 bool hasTangentData,
                                 bool hasUvData,
                                 unsigned int indexCount) = 0;

        virtual void setInstances(unsigned int instanceCount,
                                  bool hasMaterialData) = 0;

    protected:
        virtual ~Mesh() noexcept = default;
    };

    virtual Mesh * createMesh(const char * name) = 0;
    virtual unsigned int meshCount() const = 0;
    virtual const Mesh * findMeshByIndex(unsigned int index) const = 0;
    virtual Mesh * findMeshByIndex(unsigned int index) = 0;

    virtual void release() noexcept = 0;

protected:
    virtual ~SimpleScene() noexcept = default;

};

/**
 * TODO
 */
TCF_API_FUNC SimpleScene * createSimpleScene();

/**
* TODO
*/
TCF_API_FUNC void deleteSimpleScene(SimpleScene * scene);

/**
 * Test if the currently open chunk contains data for a tcf::SimpleScene.
 * Use this method when reading tcf::SimpleScene data that has been embedded
 * within another TCF file type.
 * @param   reader  A tcf::Reader with an open file and open chunk.
 * @return          True if the chunk type matches that of a tcf::SimpleScene.
 */
TCF_API_FUNC bool chunkIsSimpleScene(const Reader * reader);

/**
 * Read the tcf::SimpleScene data within the open chunk.
 * It is assumed that the user has verified the chunk contains valid data.
 * The open chunk is closed before the method exits.
 * @param   reader  A tcf::Reader with an open file and open chunk.
 * @return          A tcf::SimpleScene object initialised with the chunk data.
 */
TCF_API_FUNC SimpleScene * readSimpleScene(Reader * reader);

/**
 * Write the given tcf::SimpleScene as a child chunk within the current scope.
 * The user is responsible for ensuring the tcf::SimpleScene data is valid.
 * The new chunk is ended before the method exits.
 * @param   scene   A valid tcf::SimpleScene object.
 * @param   writer  A tcf::Writer with an open file.
 */
TCF_API_FUNC void writeSimpleScene(const SimpleScene * scene, Writer * writer);

/**
* TODO
*/
TCF_API_TYPE
typedef std::unique_ptr<SimpleScene, void(*)(SimpleScene*)> SimpleScenePtr;

/**
* TODO
*/
inline SimpleScenePtr createSimpleScenePtr()
{
    return SimpleScenePtr(createSimpleScene(), deleteSimpleScene);
}

} // end namespace tcf_2
} // end namespace tcf

#endif // __TCF_SIMPLESCENE_2__
