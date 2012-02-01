#ifndef STDINC_H_XA3W5TTL
#define STDINC_H_XA3W5TTL

#include "config.h"
#include "portability.h"

#include <map>
#include <set>
#include <string>
#include <sstream>
#include <iostream>
#include <stdint.h>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#if defined UNIX || defined IOS || defined IOSSIM
#include <stdlib.h>
#include <netinet/in.h>
#endif

#ifdef ANDROID
#include <arpa/inet.h>
#endif

#ifdef WIN32
#include <stdarg.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#endif

#include "common/Debug.h"

namespace umundo {

using std::string;
using std::map;
using std::set;
using boost::shared_ptr;
using boost::weak_ptr;

}

#endif /* end of include guard: STDINC_H_XA3W5TTL */

/**
 * \mainpage umundo-core
 *
 * This is the documentation of umundo-core, a leight-weight implementation of a pub/sub system. Its only responsibility
 * is to deliver byte-arrays from publishers to subscribers on channels. Where a channel is nothing more than an agreed
 * upon ASCII string.
 */



