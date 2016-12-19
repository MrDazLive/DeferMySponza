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
#ifndef __TCF_READER_2__
#define __TCF_READER_2__

#include "types.hpp"

#include <memory>

namespace tcf {
inline namespace tcf_2 {

/**
 * Provides methods to read a chunk file.
 */
class TCF_API_TYPE Reader
{
public:

    /**
     * Open an existing chunk file for input.
     *
     * This command is not valid if a file is already open.
     * The requested file must exist and must be a chunk file.
     */
    virtual void openFile(const char * filepath) = 0;

    /**
     * Read a buffer of data from the input file.
     *
     * The destination buffer must be large enough to receive the number
     * of bytes of data requested.
     * The return value is the number of bytes written to the destination
     * buffer.
     * This command will not read more data than is available in the chunk.
     * This command is only valid if a chunk is open for reading and
     * there is data remaining to be read.
     */
    virtual unsigned int readData(unsigned int numberOfBytes, void * dst) = 0;

    /**
     * Get the number of bytes of data remaining in the current chunk.
     *
     * This command is only valid if a chunk is open for reading.
     */
    virtual unsigned int remainingData() const = 0;

    /**
     * Skip reading of the remaining chunk data.
     *
     * This command is only valid if a chunk is open for reading and
     * there is data remaining to be read.
     */
    virtual void skipData() = 0;

    /**
     * Determine if another chunk is available for reading.
     *
     * This command is only valid if a file is already open.
     * This command is not valid until any chunk data is read.
     */
    virtual bool hasChunk() const = 0;

    /**
     * Skip reading onto the next peer chunk.
     *
     * This command is only valid when hasChunk returns true.
     */
    virtual void skipChunk() = 0;

    /**
     * Open the next chunk available.
     *
     * This command is only valid when hasChunk returns true.
     */
    virtual void openChunk() = 0;

    /**
     * Get the type of the current open chunk.
     *
     * This command is only valid if a chunk is open for reading.
     */
    virtual Type chunkType() const = 0;

    /**
     * Close the current chunk being read.
     *
     * This command is only valid if a chunk is open for reading.
     * Any data and sub-chunks not already read will be skipped.
     */
    virtual void closeChunk() = 0;

    /**
     * Terminate reading of the current input file.
     *
     * This command is only valid if a file is already open.
     */
    virtual void closeFile() = 0;

	/**
	* Tag the object for deletion. It is nolonger safe to use after this call.
	*/
	virtual void release() noexcept = 0;

protected:
    virtual ~Reader() noexcept = default;

};

/**
 * TODO
 */
TCF_API_FUNC Reader * createReader();

/**
 * TODO
 */
TCF_API_FUNC void deleteReader(Reader * reader);

/**
* TODO
*/
TCF_API_TYPE typedef std::unique_ptr<Reader, void(*)(Reader*)> ReaderPtr;

/**
* TODO
*/
inline ReaderPtr createReaderPtr()
{
    return ReaderPtr(createReader(), deleteReader);
}

} // end namespace tcf_2
} // end namespace tcf

#endif // __TCF_READER_2__
