#ifndef PORTABILITY_H_2L1YN201
#define PORTABILITY_H_2L1YN201

#include "common/Common.h"

#ifdef ANDROID
#include <wchar.h>
namespace std {
	// prevent error: 'wcslen' is not a member of 'std'
	using ::wcslen;
}
#endif

#ifdef WIN32
#include <stdarg.h>

#ifndef snprintf
#define snprintf _snprintf
#endif

#ifndef strdup
#define strdup _strdup
#endif

#ifndef va_copy
#define va_copy(d,s) ((d) = (s))
#endif

int vasprintf(char **ret, const char *format, va_list args);
int asprintf(char **ret, const char *format, ...);

#endif

#endif /* end of include guard: PORTABILITY_H_2L1YN201 */
