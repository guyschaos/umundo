#include "umundo/common/Debug.h"
#include <stdio.h>
#include <string.h>
#include "umundo/config.h"

namespace umundo {

const char* Debug::relFileName(const char* filename) {
	const char* relPath = filename;
	if(strstr(filename, PROJECT_SOURCE_DIR)) {
		relPath = filename + strlen(PROJECT_SOURCE_DIR) + 1;
	}
	return relPath;
}

bool Debug::logMsg(int lvl, const char* fmt, const char* filename, const int line, ...) {
	// try to shorten filename
	filename = relFileName(filename);
	char* pathSepPos = (char*)filename;
	bool skip = false;

	while((pathSepPos = strchr(pathSepPos + 1, PATH_SEPERATOR))) {
		if (strncmp(pathSepPos + 1, "common", 6) == 0 && lvl > LOGLEVEL_COMMON)
			return false;
		if (strncmp(pathSepPos + 1, "connection", 10) == 0 && lvl > LOGLEVEL_NET)
			return false;
		if (strncmp(pathSepPos + 1, "discovery", 9) == 0 && lvl > LOGLEVEL_DISC)
			return false;
		if (strncmp(pathSepPos + 1, "s11n", 9) == 0 && lvl > LOGLEVEL_S11N)
			return false;
	}

	if (!skip) {
		const char* severity = NULL;
		if (lvl == 0) severity = "ERROR";
		if (lvl == 1) severity = "WARNING";
		if (lvl == 2) severity = "INFO";
		if (lvl == 3) severity = "DEBUG";

		char* message;
		va_list args;
		va_start(args, line);
		vasprintf(&message, fmt, args);
		va_end(args);

		printf("%s:%d: %s %s\n", filename, line, severity, message);
		free(message);
	}
	return true;
}

}