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

%template(StringVector) std::vector<std::string>;

%javaconst(1);

//**************************************************
// This ends up in the generated wrapper code
//**************************************************

%{
#include "../../../../core/src/umundo/common/EndPoint.h"
#include "../../../../core/src/umundo/common/Node.h"
#include "../../../../core/src/umundo/common/Message.h"
#include "../../../../core/src/umundo/thread/Thread.h"
#include "../../../../core/src/umundo/connection/Publisher.h"
#include "../../../../core/src/umundo/connection/Subscriber.h"

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
using std::vector;
using std::map;
using boost::shared_ptr;
using namespace umundo;
%}

// allow Java receivers to act as callbacks from C++
%feature("director") umundo::Receiver;

// enable conversion from char*, int to jbytearray
%apply (char *STRING, size_t LENGTH) { (char* buffer, size_t length) }; 


// rename / ignore overloaded operators
%rename(equals) operator==(NodeStub* n) const;
%ignore operator!=(NodeStub* n) const;
%ignore operator<<(std::ostream&, const NodeStub*);

%ignore umundo::Publisher::getNode() const;
%ignore umundo::Publisher::setNode(shared_ptr<NodeStub> node);
%ignore umundo::PublisherStub::getNode() const;
%ignore umundo::PublisherStub::setNode(shared_ptr<NodeStub> node);

//******************************
// Beautify Message interface
//******************************

// ignore ugly std::map return
%ignore umundo::Message::getMeta();

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


//***********************************************
// Parse the header file to generate wrappers
//***********************************************

%include "../../../../core/src/umundo/common/Message.h"
%include "../../../../core/src/umundo/thread/Thread.h"
%include "../../../../core/src/umundo/common/Implementation.h"
%include "../../../../core/src/umundo/common/EndPoint.h"
%include "../../../../core/src/umundo/connection/Publisher.h"
%include "../../../../core/src/umundo/connection/Subscriber.h"
%include "../../../../core/src/umundo/common/NodeStub.h"
%include "../../../../core/src/umundo/common/Node.h"

