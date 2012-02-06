#ifndef DEBUG_H_Z6YNJLCS
#define DEBUG_H_Z6YNJLCS

#include "common/Common.h"

#include <stdarg.h> ///< variadic functions

// #define DEBUG_CTOR(x) printf("Constructing %s\n", x);
// #define DEBUG_DTOR(x) printf("Destructing %s\n", x);
#define DEBUG_CTOR(x)
#define DEBUG_DTOR(x)

/// Log a message with error priority
#define LOG_ERR(fmt, ...) Debug::logMsg(0, fmt, __FILE__, __LINE__,  ##__VA_ARGS__);
#define LOG_WARN(fmt, ...) Debug::logMsg(1, fmt, __FILE__, __LINE__,  ##__VA_ARGS__);
#define LOG_INFO(fmt, ...) Debug::logMsg(2, fmt, __FILE__, __LINE__,  ##__VA_ARGS__);
#define LOG_DEBUG(fmt, ...) Debug::logMsg(3, fmt, __FILE__, __LINE__,  ##__VA_ARGS__);
//#define LOG_ERR
//#define LOG_WARN
//#define LOG_INFO
//#define LOG_DEBUG

namespace umundo {

enum LogLevel {
	ERR = 0,
	WARN = 1,
	INFO = 2,
	DEBUG = 3
};

/**
 * Macros and static functions used for debugging.
 *
 * All umundo logging is to be done using one of the following macros:
 * - #LOG_ERR(fmt, ...)
 * - #LOG_WARN(fmt, ...)
 * - #LOG_INFO(fmt, ...)
 * - #LOG_DEBUG(fmt, ...)
 *
 * These macros will take care of calling Debug::logMsg() for you. By using macros, we can simply remove them in release builds. 
 * The macros will return a boolean to allow logging in lazy evaluated expressions:
 *
 * trueForSuccess() || LOG_WARN("Failed to succeed");
 */
class Debug {
public:
	static const char* relFileName(const char* filename);
	static bool logMsg(int lvl, const char* fmt, const char* filename, const int line, ...);
};

}

#endif /* end of include guard: DEBUG_H_Z6YNJLCS */
