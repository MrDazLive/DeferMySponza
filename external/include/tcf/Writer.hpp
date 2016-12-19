/**
 * @file
 * @author  Tyrone Davison <t.p.davison@tees.ac.uk>
 * @date    September 2016
 * @version 2.16.0
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
#ifndef __TCF_WRITER_2__
#define __TCF_WRITER_2__

#include "types.hpp"

#include <memory>

namespace tcf {
inline namespace tcf_2 {

/**
 * Interface that provides methods to write data to a new chunk file.
 */
class TCF_API_TYPE Writer
{
public:
    /**
     * Create a new output chunk file.
     *
     * This command is not valid if a file is already open.
     */
    virtual void createFile(const char * filepath) = 0;

    /**
     * Create and begin writing a new chunk in the open file.
     *
     * This command is only valid if a file is open for output.
     * If a chunk is currently being written, calling this command
     * ends the data section of the current chunk and creates a sub-chunk.
     * If the first chunk created isn't of type HEAD, one of this type
     * will automatically be created and inserted.
     */
    virtual void createChunk(Type) = 0;

    /**
     * Write a buffer of data to the file.
     *
     * This command is only valid if a chunk is currently being written,
     * and no sub-chunks have been created yet.
     */
    virtual void writeData(const void * data, unsigned int numberOfBytes) = 0;

    /**
     * Declare the end of the current chunk being written.
     *
     * This command is only valid if a chunk is being written.
     */
    virtual void endChunk() = 0;

    /**
     * Declare the end of the output file.
     *
     * This command is only valid if a file is open for output.
     * Any chunks currently being written are ended automatically.
     * A HEAD chunk is automatically added if one hasn't been already.
     */
    virtual void endFile() = 0;

	/**
	 * Tag the object for deletion. It is nolonger safe to use after this call.
	 */
	virtual void release() noexcept = 0;

protected:
    virtual ~Writer() noexcept = default;
};

/**
 * TODO
 */
TCF_API_FUNC Writer * createWriter();

/**
* TODO
*/
TCF_API_FUNC void deleteWriter(Writer * writer);

/**
* TODO
*/
TCF_API_TYPE
typedef std::unique_ptr<Writer, void(*)(Writer*)> WriterPtr;

/**
* TODO
*/
inline WriterPtr createWriterPtr()
{
    return WriterPtr(createWriter(), deleteWriter);
}

} // end namespace tcf_2
} // end namespace tcf

#endif // __TCF_WRITER_2__
