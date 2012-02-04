# umundo.core ReadMe

umundo.core is a lightweight publish/subscribe system with a single responsibility: to deliver
byte arrays on channels from publishers to subscribers - a channel is just an agreed upon string
such as "temperature". It uses Bonjour/Avahi for the discovery of nodes and ZeroMQ to deliver the 
byte arrays.

The *u* in uMundo is short for *micro* and does not necessarily mean memory footprint or runtime 
behavior (though we try to keep it sane) - it stands for the amount of source code we actually have 
to write and maintain in order to arrive at a working implementation.

It can be used as a standalone component if you do not need any RPC or serialization.

## Contributors

Stefan Radomski <radomski@tk.informatik.tu-darmstadt.de>

## Status

<table>
    <tr>
        <th>Version</th>
	<td>0.0.1-prealpha</td>
    </tr>
    <tr>
        <th>Description</th>
	<td>umundo.core is functional and runs on Linux, MacOSX, Windows, iOS devices (and compiles and links for Android).
		It is quite possible that it will compile on the various BSDs, at least it can be ported with a minimum of work.
    </tr>
</table>

## Getting Started

Checkout all the umundo sources one directory above. The build-system is not as polished as it ought to be and is still seperated
into all the sub-components (core, s11n, ...) so you will have to build them seperately. If you want to write code using umundo,
you will have to do so within the CMake build-system as we still rely on various macros and flags defined by the build system. 
You can write small samples within the <tt>test</tt> directory of each component and adapt the <tt>CMakeLists.txt</tt> file within
that directory (have a look at the definition for the existing binaries that get build).

## Architecture

There are two major subsystems within umundo.core:

1. The Discovery component allows you to register and query for Nodes 
within a domain. Other components can register NodeQueries
at the Discovery singleton and get notified when 
there are matching Nodes appearing or disappearing.
  
2. The Connection component with its three main classes Node, Publisher and Subscriber enables communication on channels. 
You instantiate a Node within a domain and add Publishers and Subscribers to it. The Node will register itself with the 
Discovery component and automatically hook up your Publishers and Subscribers with remote entities within the same domain.

### Patterns

Throughout umundo.core, the Pimpl or Bridge pattern is applied - as far as I can tell, they are the same. The basic idea here 
is that all abstractions get a concrete implementation only at runtime from a Factory. This hides the concrete implementation 
from all applications and allows us to substitute the implementation without the application knowing. 

There are the following abstractions at the moment:

- [Publisher][/tklab-tud/umundo/blob/master/core/src/connection/Publisher.h]s allow you to send byte-arrays on channels.
- [Subscriber][/tklab-tud/umundo/blob/master/core/src/connection/Subscriber.h]s passes received byte arrays from channels 
to someone implementing the Receiver base class.
- [Node][/tklab-tud/umundo/blob/master/core/src/common/Node.h] is the entity where your Publishers and Receiver live and 
get connected to others in the same domain.
- [Discovery][/tklab-tud/umundo/blob/master/core/src/discovery/Discovery.h] notifies nodes of other nodes.

For every abstraction, there is a set of classes that comply to the following naming conventions:

- *Abstraction* as such is the name of the concrete client visible class. In fact this is the only thing the user is expected to 
see. It gets an implementation from a Factory singleton and has the same inheritance hierarchy as its implementation (minus the
Implementation base class). Its base classes are marked as private and it overrides *every* method to delegate it to its 
implementation.
- *AbstractionImpl* is the abstract base class for a concrete implementor. In addition to the abstraction's base classes it also
inherits Implementation. It gets inherited by concrete implementors who are registered at the Factory.
- *AbstractionConfig* is essentially unused right now but maybe used in the future to make state explicit.

### Design Decisions

During development, I made some design decisions I want to make explicit, in order not to refactor the code back into 
one of the earlier iterations:

**Why isn't every pointer a smart pointer?**
I could not get SWIG to produce readable Java bindings with smart pointers. Also, the user visible API gets somewhat convoluted
when you have to wrap everything in smart pointers. Furthermore it requires some unreadable constructs with the bridge pattern
as we cannot use our smart pointer in a constructor or destructor (i.e. when adding a Node to the Discovery).

**Why do the Abstractions override every method in their inheritance hierarchy?**
This took me a while to figure out and I am still not satisfied with the answer. When we are using abstractions and implementors,
we end up with two virtually identical inheritance hierarchies, one for the abstraction and one for the implementors (e.g. both 
the Node and the NodeImpl are EndPoints). But we want the Implementor to be able to overwrite methods even from higher up in 
the inheritance hierarchy. When the client code deals with an abstraction and sets data defined e.g. in the EndPoint class, the 
implementor won't know about it as it is the abstractions base class that gets modified. On the other hand, if the implementor 
sets its e.g. its port in its EndPoint base class, the abstraction does not know it. The solution is to make the abstraction 
class hierarchy private and delegate *every* call into the implementor. 

This has the undesired consequence that memory for all the members in the abstractions inheritance hierarchy gets allocated but 
never used. The solution would be to define all base classes without data members and define their methods abstract and pure but
then every implementing abstraction and implementor would be forced to actually implement the methods, leading to even more
boilerplate code.

**What's with the *Config classes**
Every implementation is required to overwrite Implementation::init(Configuration*) in order to get non-abstract. Originally I
planned to make all state explicit within a configuration so that we could move entities. I still think it's a good idea, but
for the moment, every implementation ignores its configuration and relies on the abstraction to call the setters for the relevant 
fields prior to calling init() with an empty configuration.

## Language Bindings

The component itself is written in C/C++ and utilizes the STL and some header-only components
from the boost library (the occasional shared_ptr and uuids).

<table>
    <tr><th>Language</th><th>Comments</th></tr>
    <tr>
	<td>C/C++</td> 
	<td>Native language of umundo.core.</td>
    </tr>
    <tr>
	<td>Java</td> 
	<td>Wrappers are generated using SWIG and are available as .jnilib and jars 
		in the respective <tt>lib/platform</tt> folders. There is also a Eclipse project
		template with a small sample program in the <tt>contrib/bindings/java</tt> folder.
		The Java bindings are somewhat *puristic* and will likely need to be extended when 
		we see some actual use.</td>
    </tr>
    <tr>
	<td>Objective-C++</td> 
	<td>A hand-written wrapper for objective-c++ is available in the <tt>contrib/bindings/objc</tt>
		folder. In <tt>contrib/xcode</tt> is a sample Xcode project to deploy on iOS devices.</td>
		Just as the Java bindings, the objective-c++ are very slim, actually there are only three
		classes: Publisher, Subscriber and a Receiver protocol.
    </tr>
    <tr>
	<td>Other</td> 
	<td>Using SWIG, we can generate bindings into a plethora of languages. Have a look at <tt>contrib/swig</tt>
		to see what it takes. Patches are welcome!</td>
    </tr>
</table>

## Using umundo.core

If we did everything right, all you need are the static libraries and the header files for your platform. <tt>TODO: Header 
files are copied from source during compilation and need to be packaged into lib/<tt>. Have a look at the template projects 
in contrib. The basic idea is as follows:

* If you want to send data, instantiate a Publisher. The constructor takes a channel name.
    * send(char* data, size_t length) is available to publish byte arrays on a channel.
* If you want to receive data
    * Implement a Receiver by overwriting received(char* data, size_t length).
    * Instantiate a Subscriber. The constructor takes a channel name and a receiver.
    * Wait for a Publisher to send byte arrays.

## Building from Source

We use CMake as the build-system for umundo.core itself and some custom shell-scripts in <tt>contrib/</tt> to build 
and cross-compile our dependencies. Building the umundo.core is pretty straight-forward if you are familiar with CMake.

We aim to provide most of our dependancies as prebuilt static libraries in <tt>contrib/prebuilt/${library}/${system}/${compiler}/</tt>
in order to allow a seamless build. If you stumble across a dependency on a supported platform, not included as a prebuilt
library, let us know and we will most likely add it.

After building your library will be in <tt>lib/${system}/${compiler}/</tt>.

### Unices

Change to the directory where you checked out the umundo.core, create a new <tt>build/</tt> directory for an 
*out-of-source* build and run cmake && make:

    umundo/core$ mkdir build
    umundo/core$ cd build
    core/build$ cmake ..
    core/build$ make

#### MacOSX XCode

If you happen to be on MacOSX, you can generate an Xcode project:

    umundo/core$ mkdir xcode
    umundo/core$ cd xcode
    core/xcode$ cmake -G Xcode ..
    core/xcode$ open ./umundo.xcodeproj

There are other generators to create artifacts for various IDEs, have a look at <tt>cmake --help</tt> to get a list.

#### Debian Stable - Complete Walkthrough

I downloaded and installed a *fresh* installation of Debian GNU/Linux 6.0.4 stable for i386 from the netinst.iso, here are all the 
steps required to arrive at a static <tt>libumundocore.a</tt> (<tt>sudo</tt> is no actually installed by default, install it and 
add yourself to the <tt>sudo</tt> group or use a root console):

    $ sudo apt-get install cmake g++ avahi-daemon libavahi-client-dev

Then build as described above - this is everything you need to compile a static <tt>libumundocore.a</tt>. If you want to generate 
the optional Java bindings and are not satisfied with the precompiled ones, you also need:
- a JDK 
- >= SWIG 2.0.5 (it's still in SVN - and prior versions won't do)
- >= CMake 2.8.6 (Java support was only added in 2.8.6)

I had some mixed experiences with GCJ in the past, so I went ahead, enabled *non-free* in <tt>/etc/apt/sources.list</tt> 
and installed SUNs Java implementation:

    $ sudo apt-get install sun-java6-jdk 

To generate the wrappers for JNI, checkout and build the current SWIG distribution:

    $ sudo apt-get install subversion autoconf libpcre3-dev bison
    $ svn co https://swig.svn.sourceforge.net/svnroot/swig trunk
    $ cd trunk
    $ ./autogen.sh
    $ ./configure
    $ make
    $ sudo make install
    $ swig --version

This ought to yield version 2.0.5 and is what's required to build the Java wrappers for Debian GNU/Linux 6.0.4 stable. Now all we
need is a current CMake version:

    $ sudo apt-get install wget
    $ sudo apt-get remove cmake cmake-data
    $ wget http://www.cmake.org/files/v2.8/cmake-2.8.7.tar.gz
    $ cd cmake-2.8.7/
    $ ./configure
    $ make
    $ sudo make install
    $ cmake --version

This should say <tt>cmake version 2.8.7</tt>. If you get the bash complaining about not finding cmake, logout and login again. Now
you got everything needed to compile Java wrappers. 

### Windows

In any case, you have to download a current version of<a href="http://www.cmake.org/cmake/resources/software.html">CMake</a>. Once
installed, there is a <tt>CMake (cmake-gui)</tt> in "All Programs"->"CMake 2.8" run it. The GUI is pretty self-explanatory:

- Enter the path to the <tt>umundo/core</tt> directory in "Where is the source code"
- Choose a convenient location to build the binaries
- Hit "Configure"
- Choose your toolchain / compiler

#### MS Visual Studio 10

If you decide to build using Visual Studio, select it form the list (I never tried the IA64 or Win64 variants) and choose the
<tt>default native compilers</tt>. In the log pane at the bottom are some outputs, if everything went ok you should see a 
<tt>Configuration done</tt> and some read marked key-value pairs in the table above. Do not panic, red only means "was changed" - 
just hit "Configure" once more and everything should become non-red.

Most likely CMake will complain about the SWIG executable not being found, that's ok if you do not want to build the Java
Wrappers. Otherwise point CMake to the Executable of a SWIG 2.0.5 deployment (You will most likely need to build from SVN).

I for one just hit "Generate" once everything is non-red and open the folder where I chose to build the libraries. There is a 
Visual Studio 10 Project file you can just double click. Build the solution and your files will show up in the umundo.core root
in the <tt>lib/windows-x86/msvc/</tt> directory somewhere.

Below are some issues when building with MSVC and Visual Studio 10
<table>
    <tr>
		<td>Debug builds will fail while linking libzmq</td>
		<td>Known issue, Release and MinSizeRel builds work fine.</td>
	</tr>
    <tr>
		<td>Unable to start program '...' The system cannot find the file specified</td>
		<td>Actually everything is fine, I guess MSVS tried to execute the library.</td>
	</tr>
    <tr>
		<td>error MSB6006 "cmd.exe" exited with code 9009 while building solution "docs"</td>
		<td>As the error clearly states, building docs just executes "doxygen" in umundo/core/docs, 
			I guess it is not in the <tt>$PATH</tt>.
		</td>
	</tr>
</table>

#### MinGW Makefiles (TODO)

If you want to build with MinGW, choose "MinGW Makefile" (I am not sure what MSYS Makefiles would mean) and create the artifacts
just as above.

TODO: build ZeroMQ as a static library with MinGW 

Below are some issues when building with MinGW
<table>
    <tr>
		<td>... <tt>libgmp-10.dll</tt> is missing ...</td>
		<td>Add the \bin directory of your MinGW installation to the <tt>$PATH</tt> and restart the CMake GUI. 
			Also, consider reading the <a href="http://www.mingw.org/wiki/Getting_Started">getting started</a> 
			document from MinGW.
		</td>
	</tr>
</table>

### Cross Compiling for Android / iOS

