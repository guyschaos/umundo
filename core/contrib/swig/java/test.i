%module(directors="1") umundoCPP
// import swig typemaps
%include "arrays_java.i"
%include "std_string.i"
%include "inttypes.i" 
%javaconst(1);

%{
/* This ends up in the generated wrapper code */

#include <string>

#include "Test.h"

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

%}

// allow Java receivers to act as callbacks from C++
//%feature("director") umundo::Receiver;

// enable conversion from char*, int to jbytearray
%apply (char *STRING, size_t LENGTH) { (char* buffer, size_t length) }; 

// SWIG does not recognize 'using std::string' from an include
typedef std::string string;

// Parse the header file to generate wrappers
%include "Test.h"

