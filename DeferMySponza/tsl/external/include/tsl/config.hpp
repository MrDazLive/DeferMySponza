/**
 * @file
 * @author  Tyrone Davison <t.p.davison@tees.ac.uk>
 * @date    September 2016
 * @version 2.16.0
 *
 *
 * @section DESCRIPTION
 *
 * Compilation configuration settings for the TSL API.
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
#ifndef __TSL_CONFIG_2__
#define __TSL_CONFIG_2__

#define TSL_MAJOR_VERSION 2
#define TSL_MINOR_VERSION 0

#ifndef TSL_API_TYPE
#define TSL_API_TYPE
#endif // TSL_API_TYPE

#ifndef TSL_API_CLASS
#ifdef TSL_EXPORTS
#define TSL_API_CLASS __declspec(dllexport)
#else
#define TSL_API_CLASS __declspec(dllimport)
#endif // TSL_EXPORTS
#endif // TSL_API_CLASS

#ifndef TSL_API_FUNC
#ifdef TSL_EXPORTS
#define TSL_API_FUNC extern "C" __declspec(dllexport)
#else
#define TSL_API_FUNC extern "C" __declspec(dllimport)
#endif // TSL_EXPORTS
#endif // TSL_API_FUNC

#endif // __TSL_CONFIG_2__
