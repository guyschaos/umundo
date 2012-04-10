#include <stdlib.h>
#include <assert.h>

// include order matters 
#include "mDNSEmbeddedAPI.h"
#ifdef WIN32
#include "mDNSWin32.h"    // Defines the specific types needed to run mDNS on windows platforms
#else
#include <sys/time.h>
#include "mDNSPosix.h"    // Defines the specific types needed to run mDNS on posix platforms
#endif

#define RR_CACHE_SIZE 500
static CacheEntity rrcachestorage[RR_CACHE_SIZE];
mDNS mDNSStorage;
//struct mDNS_PlatformSupport_struct {};
static int mDNSIsInitialized = 0;
static mDNS_PlatformSupport platformSupport;
const char ProgramName[] = "umundo";
extern mDNSexport void mDNSPosixGetFDSet(mDNS *m, int *nfds, fd_set *readfds, struct timeval *timeout);
extern mDNSexport void mDNSPosixProcessFDSet(mDNS *const m, fd_set *readfds);

mDNSexport int embedded_mDNSInit() {
	mStatus err;
	if (mDNSIsInitialized != 0) {
		return 0;
	}

	err = mDNS_Init(
		&mDNSStorage, 
		&platformSupport, 
		rrcachestorage, 
		RR_CACHE_SIZE, 
		mDNS_Init_AdvertiseLocalAddresses, 
		mDNS_Init_NoInitCallback, 
		mDNS_Init_NoInitCallbackContext
	);
	if (err == 0) {
		mDNSIsInitialized = 1;
	}
	return err;
}

mDNSexport void embedded_mDNSExit() {
	mDNS_Close(&mDNSStorage);
}

mDNSexport int embedded_mDNSmainLoop(struct timeval timeout) {
	int nfds = 0;
	fd_set readfds;
	int result;

	// 1. Set up the fd_set as usual here.
	// This example client has no file descriptors of its own,
	// but a real application would call FD_SET to add them to the set here
	FD_ZERO(&readfds);

	// 2. Set up the timeout.
	// This example client has no other work it needs to be doing,
	// so we set an effectively infinite timeout
//	timeout.tv_sec = 0x3FFFFFFF;
//	timeout.tv_usec = 0;

	assert(timeout.tv_sec < 10);
	// 3. Give the mDNSPosix layer a chance to add its information to the fd_set and timeout
	mDNSPosixGetFDSet(&mDNSStorage, &nfds, &readfds, &timeout);

	// 4. Call select as normal
	result = select(nfds, &readfds, NULL, NULL, &timeout);

	if (result > 0) {
		// 5. Call mDNSPosixProcessFDSet to let the mDNSPosix layer do its work
		mDNSPosixProcessFDSet(&mDNSStorage, &readfds);

		// 6. This example client has no other work it needs to be doing,
		// but a real client would do its work here
		// ... (do work) ...
	}
	return result;
}

