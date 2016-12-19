/**
 * @file
 * @author  Tyrone Davison <t.p.davison@tees.ac.uk>
 * @date    September 2016
 * @version 2.16.0
 *
 *
 * @section DESCRIPTION
 *
 * Methods to read and write basic images in TCF chunks.
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
#ifndef __TCF_IMAGE_2__
#define __TCF_IMAGE_2__

#include "types.hpp"
#include <memory>

namespace tcf {
inline namespace tcf_2 {

class Reader;
class Writer;

class TCF_API_TYPE Image
{
public:

    enum ComponentType
    {
        kInteger,
        kFloatingPoint,
        kUnknown = 0x7FFFFFFF
    };

    virtual unsigned int width() const noexcept = 0;

    virtual unsigned int height() const noexcept = 0;

    virtual unsigned int componentsPerPixel() const noexcept = 0;

    virtual unsigned int bytesPerComponent() const noexcept = 0;

    virtual ComponentType componentType() const noexcept = 0;

    virtual const char * name() const noexcept = 0;

    virtual void * pixelData() noexcept = 0;

    virtual const void * pixelData() const noexcept = 0;

    virtual unsigned int sizeInBytes() const noexcept = 0;

    virtual void release() noexcept = 0;

protected:
    virtual ~Image() noexcept = default;

};

/**
 * TODO
 */
TCF_API_FUNC Image * createImage(unsigned int width,
                                 unsigned int height,
                                 unsigned int components_per_pixel,
                                 unsigned int bytes_per_component,
                                 Image::ComponentType component_type,
                                 const char * name);

/**
 * TODO
 */
TCF_API_FUNC void deleteImage(Image * image);

/**
 * Test if the currently open chunk contains data for a tcf::Image.
 * Use this method when reading tcf::Image data that has been embedded
 * within another TCF file type.
 * @param   reader  A tcf::Reader with an open file and open chunk.
 * @return          True if the chunk type matches that of a tcf::Image.
 */
TCF_API_FUNC bool chunkIsImage(const Reader * reader) noexcept;

/**
 * Read the tcf::Image data within the open chunk.
 * It is assumed that the user has verified the chunk contains valid data.
 * The open chunk is closed before the method exits.
 * @param   reader  A tcf::Reader with an open file and open chunk.
 * @return          A tcf::Image object initialised with the chunk contents.
 */
TCF_API_FUNC Image * readImage(Reader * reader);

/**
 * Write the given tcf::Image data as a child chunk within the current scope.
 * The user is responsible for ensuring the tcf::Image data is valid.
 * The new chunk is ended before the method exits.
 * @param   image   A valid tcf::Image object.
 * @param   writer  A tcf::Writer with an open file.
 */
TCF_API_FUNC void writeImage(const Image * image, Writer * writer);

/**
* TODO
*/
TCF_API_TYPE typedef std::unique_ptr<Image, void(*)(Image*)> ImagePtr;

/**
* TODO
*/
inline
ImagePtr createImagePtr(unsigned int width,
                        unsigned int height,
                        unsigned int components_per_pixel,
                        unsigned int bytes_per_component,
                        Image::ComponentType component_type,
                        const char * name)
{
    return ImagePtr(createImage(width,
                                height,
                                components_per_pixel,
                                bytes_per_component,
                                component_type,
                                name),
                    deleteImage);
}

} // end namespace tcf_2
} // end namespace tcf

#endif // __TCF_IMAGE_2__
