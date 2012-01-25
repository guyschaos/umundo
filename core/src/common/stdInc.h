#ifndef STDINC_H_XA3W5TTL
#define STDINC_H_XA3W5TTL

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

#ifdef WIN32
#include <stdarg.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#endif

#include "config.h"
#include "portability.h"
#include "common/Debug.h"

namespace umundo {

using std::string;
using std::map;
using std::set;
using boost::shared_ptr;
using boost::weak_ptr;

}

#endif /* end of include guard: STDINC_H_XA3W5TTL */
