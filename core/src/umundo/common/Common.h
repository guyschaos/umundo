/**
 *  Copyright (C) 2012  Stefan Radomski (stefan.radomski@cs.tu-darmstadt.de)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the FreeBSD license as published by the FreeBSD
 *  project.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  You should have received a copy of the FreeBSD license along with this
 *  program. If not, see <http://www.opensource.org/licenses/bsd-license>.
 */

#ifndef COMMON_H_ANPQOWX0
#define COMMON_H_ANPQOWX0

#include <map>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <stdlib.h>
#include <stdint.h>

#if defined(_WIN32) && !defined(UMUNDO_STATIC)
#	if COMPILING_DLL
#		define DLLEXPORT __declspec(dllexport)
#	else
#		define DLLEXPORT __declspec(dllimport)
#	endif
#else
#	define DLLEXPORT
#endif

// #if defined UNIX || defined IOS || defined IOSSIM
// #include <string.h> // memcpy
// #include <stdio.h> // snprintf
// #endif

#include "portability.h"
#include "umundo/common/Debug.h"

namespace umundo {

using std::string;
using std::map;
using std::set;
using std::vector;
using boost::shared_ptr;
using boost::weak_ptr;

extern string procUUID;

}

#endif /* end of include guard: COMMON_H_ANPQOWX0 */

/**
 * \mainpage umundo-core
 *
 * This is the documentation of umundo-core, a leight-weight implementation of a pub/sub system. Its only responsibility
 * is to deliver byte-arrays from publishers to subscribers on channels. Where a channel is nothing more than an agreed
 * upon ASCII string.
 */
