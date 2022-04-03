/**
 * @file init.h
 * @author Alex Olson (aolson1714@gmail.com)
 * @brief internal header file to initialize datatypes in the correct order
 * @version 0.1
 * @date 2022-04-03
 * 
 * @copyright MIT Copyright (c) 2022 Alex Olson. All rights reserved. details at bottom of file.
 * 
 */

#ifndef __DATATYPES_INIT_H__
#define __DATATYPES_INIT_H__

/** 
 * this is here so that we can use as little data as possible in this datatype.
 * what happens is that the datatype used to index the queue will always be the smallest 
 * kind of integer that can index the whole queue.
 * 
 * if you only need 10 indexes, the data saved for doing this would be 3 bytes. which if you are saving uint8 values then
 * you have a 18.75% smaller datatype with no change whatsoever in capabilities.
 */
namespace __DATATYPES__HELPER__ {
    template<bool FITS8, bool FITS16> struct Index {
		using Type = uint32_t;
	};

	template<> struct Index<false, true> {
		using Type = uint16_t;
	};

	template<> struct Index<true, true> {
		using Type = uint8_t;
	};
}

// used to hide the jumble from users
#define __IT_TYPE__(v) typename __DATATYPES__HELPER__::Index<(v<UINT8_MAX-1),(v<UINT16_MAX-1)>::Type

#include "datatypes/Signaling.h"
#include "datatypes/Queue.h"
#include "datatypes/Stack.h"

#endif

/**
 * MIT License
 * 
 * Copyright (c) 2022 Alex Olson
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */