%module(directors="1") umundocoreCPP
// import swig typemaps
%include "arrays_java.i"
%include "std_string.i"
%include "std_vector.i"
%include "inttypes.i" 
//%include "boost_shared_ptr.i"

// SWIG does not recognize 'using std::string' from an include
typedef std::string string;
typedef std::vector vector;

// used to get all the keys in message meta information
%template(StringVector) std::vector<std::string>;

%javaconst(1);

//**************************************************
// This ends up in the generated wrapper code
//**************************************************

%{
#include "../../../../core/src/umundo/common/EndPoint.h"
#include "../../../../core/src/umundo/connection/Node.h"
#include "../../../../core/src/umundo/common/Message.h"
#include "../../../../core/src/umundo/thread/Thread.h"
#include "../../../../core/src/umundo/connection/Publisher.h"
#include "../../../../core/src/umundo/connection/Subscriber.h"

#if 0
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
	using umundo::Debug;
	LOG_ERR("This is mundo.core speaking!");
	return JNI_VERSION_1_2;
}
#endif

#ifdef ANDROID
// google forgot imaxdiv in the android ndk r7 libc?!
#ifndef imaxdiv
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
#endif

using std::string;
using std::vector;
using std::map;
using boost::shared_ptr;
using namespace umundo;
%}

//*************************************************/


// allow Java receivers to act as callbacks from C++
%feature("director") umundo::Receiver;

// enable conversion from char*, int to jbytearray
%apply (char *STRING, size_t LENGTH) { (const char* data, size_t length) }; 

// ignore these functions in every class
%ignore setChannelName(string);
%ignore setUUID(string);
%ignore setPort(uint16_t);
%ignore setIP(string);
%ignore setTransport(string);
%ignore setRemote(bool);
%ignore setHost(string);
%ignore setDomain(string);

// ignore class specific functions
%ignore operator!=(NodeStub* n) const;
%ignore operator<<(std::ostream&, const NodeStub*);

// rename functions
%rename(equals) operator==(NodeStub* n) const;
%rename(waitSignal) wait;


//******************************
// Beautify Message interface
//******************************

// ignore ugly std::map return
%ignore umundo::Message::getMeta();
%ignore umundo::Message::setData(string const &);
%ignore umundo::Message::Message(string);

// import java.util.HashMap
%typemap(javaimports) umundo::Message %{ 
import java.util.HashMap; 
%}

// provide convinience methods within Message Java class
%typemap(javacode) umundo::Message %{
	public HashMap<String, String> getMeta() {
		HashMap<String, String> keys = new HashMap<String, String>();
		for (int i = 0; i < getKeys().size(); i++) {
			keys.put(getKeys().get(i), getMeta(getKeys().get(i)));
		}
		return keys;
	}
%}

//******************************
// Ignore whole classes
//******************************

%ignore Implementation;
%ignore Configuration;
%ignore NodeConfig;
%ignore PublisherConfig;
%ignore SubscriberConfig;
%ignore NodeImpl;
%ignore PublisherImpl;
%ignore SubscriberImpl;
%ignore Mutex;
%ignore Thread;
%ignore Monitor;
%ignore MemoryBuffer;


//***********************************************
// Parse the header file to generate wrappers
//***********************************************

%include "../../../../core/src/umundo/common/Message.h"
%include "../../../../core/src/umundo/thread/Thread.h"
%include "../../../../core/src/umundo/common/Implementation.h"
%include "../../../../core/src/umundo/common/EndPoint.h"
%include "../../../../core/src/umundo/connection/Publisher.h"
%include "../../../../core/src/umundo/connection/Subscriber.h"
%include "../../../../core/src/umundo/connection/Node.h"

