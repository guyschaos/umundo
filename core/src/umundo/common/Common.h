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
