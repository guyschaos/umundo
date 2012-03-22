#ifndef DEBUG_H_Z6YNJLCS
#define DEBUG_H_Z6YNJLCS

#include "umundo/common/Common.h"

#include <stdarg.h> ///< variadic functions

/// Log a message with error priority
#define LOG_ERR(fmt, ...) Debug::logMsg(0, fmt, __FILE__, __LINE__,  ##__VA_ARGS__);
#define LOG_WARN(fmt, ...) Debug::logMsg(1, fmt, __FILE__, __LINE__,  ##__VA_ARGS__);
#define LOG_INFO(fmt, ...) Debug::logMsg(2, fmt, __FILE__, __LINE__,  ##__VA_ARGS__);
#define LOG_DEBUG(fmt, ...) Debug::logMsg(3, fmt, __FILE__, __LINE__,  ##__VA_ARGS__);

// never strip logging
#if 0
#ifndef BUILD_DEBUG
#define LOG_ERR(fmt, ...) 1
#define LOG_WARN(fmt, ...) 1
#define LOG_INFO(fmt, ...) 1
#define LOG_DEBUG(fmt, ...) 1
#endif
#endif

#ifdef __GNUC__
#ifndef ANDROID
#define HAVE_EXECINFO
#endif
#endif

namespace umundo {

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
#ifdef HAVE_EXECINFO
	static void abortWithStackTraceOnSignal(int sig);
	static void stackTraceSigHandler(int sig);
#endif
#ifndef HAVE_EXECINFO
	// noop with non-gcc compilers
	static void abortWithStackTraceOnSignal(int sig) {};
	static void stackTraceSigHandler(int sig) {};
#endif
};

}

#endif /* end of include guard: DEBUG_H_Z6YNJLCS */
