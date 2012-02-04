%module(directors="1") umundocoreCPP
// import swig typemaps
%include "arrays_java.i"
%include "std_string.i"
%include "inttypes.i" 
# %include "boost_shared_ptr.i"

%javaconst(1);

%{
/* This ends up in the generated wrapper code */
#include "../../../../src/common/EndPoint.h"
#include "../../../../src/common/Node.h"
#include "../../../../src/thread/Thread.h"
#include "../../../../src/connection/Publisher.h"
#include "../../../../src/connection/Subscriber.h"

#ifdef ANDROID
// google forgot imaxdiv in the android ndk r7 libc?!
imaxdiv_t imaxdiv(intmax_t numer, intmax_t denom) {
	imaxdiv_t res;
	res.quot=0; res.rem=0;
	while(numer >= denom) {
		res.quot++;
		numer -= denom;
	}
	res.rem = numer;
	return res;
}

#endif

using std::string;
using boost::shared_ptr;
using namespace umundo;
%}

// allow Java receivers to act as callbacks from C++
%feature("director") umundo::Receiver;

// enable conversion from char*, int to jbytearray
%apply (char *STRING, size_t LENGTH) { (char* buffer, size_t length) }; 

// SWIG does not recognize 'using std::string' from an include
typedef std::string string;

// rename / ignore overloaded operators
%rename(equals) operator==(NodeStub* n) const;
%ignore operator!=(NodeStub* n) const;
%ignore operator<<(std::ostream&, const NodeStub*);

// ignore whole classes
%ignore Implementation;
%ignore Configuration;
%ignore NodeConfig;
%ignore PublisherConfig;
%ignore SubscriberConfig;
%ignore NodeImpl;
%ignore PublisherImpl;
%ignore SubscriberImpl;
%ignore Mutex;

// Parse the header file to generate wrappers
%include "../../../../src/thread/Thread.h"
%include "../../../../src/common/Implementation.h"
%include "../../../../src/common/EndPoint.h"
%include "../../../../src/connection/Publisher.h"
%include "../../../../src/connection/Subscriber.h"
%include "../../../../src/common/NodeStub.h"
%include "../../../../src/common/Node.h"

