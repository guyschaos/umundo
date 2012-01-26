#ifndef DEBUG_H_Z6YNJLCS
#define DEBUG_H_Z6YNJLCS

#include "stdInc.h"
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

// #define DEBUG_CTOR(x) printf("Constructing %s\n", x);
// #define DEBUG_DTOR(x) printf("Destructing %s\n", x);
#define DEBUG_CTOR(x)
#define DEBUG_DTOR(x)

#define LOG_ERR(fmt, ...) Debug::logMsg(0, fmt, __FILE__, __LINE__,  ##__VA_ARGS__);
#define LOG_WARN(fmt, ...) Debug::logMsg(1, fmt, __FILE__, __LINE__,  ##__VA_ARGS__);
#define LOG_INFO(fmt, ...) Debug::logMsg(2, fmt, __FILE__, __LINE__,  ##__VA_ARGS__);
#define LOG_DEBUG(fmt, ...) Debug::logMsg(3, fmt, __FILE__, __LINE__,  ##__VA_ARGS__);
//#define LOG_ERR
//#define LOG_WARN
//#define LOG_INFO
//#define LOG_DEBUG

namespace umundo {

class Debug {
public:
	static const char* relFileName(const char* filename);
	static bool logMsg(int lvl, const char* fmt, const char* filename, const int line, ...);
};

}

#endif /* end of include guard: DEBUG_H_Z6YNJLCS */
