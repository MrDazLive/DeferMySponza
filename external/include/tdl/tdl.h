/**
 * @file
 * @author    Tyrone Davison <t.p.davison@tees.ac.uk>
 * @date      September 2016
 * @version   1.16.0
 *
 *
 * @section DESCRIPTION
 *
 * A small C library to parse a variety of data formats from byte streams.
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
#ifndef __TDL__
#define __TDL__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef TDL_EXPORTS
#define TDL_API __declspec(dllexport)
#else
#define TDL_API __declspec(dllimport)
#endif


typedef void * tdlError;

TDL_API const char * tdlGetErrorDescription(tdlError * error);

TDL_API void tdlFreeError(tdlError * error);


typedef struct _tdlStream tdlStream;

TDL_API tdlStream * tdlCreateStreamFromUri(const char * uri,
                                           tdlError ** error);

TDL_API int tdlIsStreamOpen(tdlStream * stream);

TDL_API int tdlGetStreamKnownSize(tdlStream * stream);

TDL_API void tdlReadStream(tdlStream * stream,
                           tdlError ** error,
                           size_t * count,
                           char * buffer);

TDL_API void tdlFreeStream(tdlStream * stream);


typedef struct _tdlStringParser tdlStringParser;

TDL_API tdlStringParser * tdlCreateAsciiParser(tdlStream * stream,
                                               tdlError ** error);

TDL_API void tdlFreeStringParser(tdlStringParser * parser);


typedef void * tdlString;

TDL_API tdlString * tdlCreateStringFromParser(tdlStringParser * parser,
                                              tdlError ** error);

TDL_API const char * tdlGetStringData(tdlString * string);

TDL_API void tdlFreeString(tdlString * string);


typedef struct _tdlImageParser tdlImageParser;

TDL_API tdlImageParser * tdlCreatePngParser(tdlStream * stream,
                                            tdlError ** error);

TDL_API void tdlFreeImageParser(tdlImageParser * parser);


typedef struct _tdlImage tdlImage;

TDL_API tdlImage * tdlCreateImageFromParser(tdlImageParser * parser,
                                            tdlError ** error);

TDL_API size_t tdlGetImageWidth(tdlImage * image);

TDL_API size_t tdlGetImageHeight(tdlImage * image);

TDL_API size_t tdlGetImageComponentsPerPixel(tdlImage * image);

TDL_API size_t tdlGetImageBytesPerComponent(tdlImage * image);

TDL_API void * tdlGetImageData(tdlImage * image);

TDL_API void * tdlGetImagePixel(tdlImage * image,
                                size_t x,
                                size_t y);

TDL_API void tdlFreeImage(tdlImage * image);


#ifdef __cplusplus
}
#endif

#endif // __TDL__
