#include "common/Debug.h"

namespace umundo {

const char* Debug::relFileName(const char* filename) {
	const char* relPath = filename;
	if(strstr(filename, PROJECT_SOURCE_DIR)) {
		relPath =  filename + strlen(PROJECT_SOURCE_DIR) + 1;
	}
	return relPath;
}

bool Debug::logMsg(int lvl, const char* fmt, const char* filename, const int line, ...) {
	// try to shorten filename
	filename = relFileName(filename);
	char* pathSepPos1 = (char*)filename;
	char* pathSepPos2 = NULL;
	char* pathSepPos3 = NULL;
	char* logDomain = NULL;
	bool skip = false;
	int i = 3;
	/*	while((pathSepPos1 = strchr(pathSepPos1+1, PATH_SEPERATOR)) && --i > 0) {
			pathSepPos3 = pathSepPos2;
			pathSepPos2 = pathSepPos1;
		}
		if (pathSepPos3 != NULL && pathSepPos2 != NULL) {
			int domainLength = pathSepPos2 - pathSepPos3 - 1;
			logDomain = (char*)malloc(domainLength);
			memcpy(logDomain, pathSepPos3 + 1, domainLength);
			logDomain[domainLength] = 0;
	//		printf("%d %s\n", domainLength, logDomain);
			// do we want to skip this message?
			if (strcmp(logDomain, "discovery") == 0 && lvl > LOGLEVEL_DISC)
				skip = true;
			if (strcmp(logDomain, "connection") == 0 && lvl > LOGLEVEL_NET)
				skip = true;
			if (strcmp(logDomain, "common") == 0 && lvl > LOGLEVEL_COMMON)
				skip = true;

			free(logDomain);
		}
		*/
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