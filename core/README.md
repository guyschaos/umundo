# umundo.core ReadMe

umundo.core is a lightweight publish/subscribe system with a single responsibility: to deliver
byte arrays on channels from publishers to subscribers - a channel is just an agreed upon string
such as "temperature". It uses Bonjour/Avahi for the discovery of nodes and ZeroMQ to deliver the 
byte arrays.

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
	<td>umundo.core is functional and runs on Linux, MacOSX, Windows, Android and iOS devices.
		It is quite possible that it will compile on the various BSDs, at least it can be ported 
		with a minimum of work.
    </tr>
</table>

## Philosophy

We aim to provide a good "walk up and use" experience. With a few exceptions, everything you need
is available within the distribution. There is a <tt>lib/</tt> folder with umundo.core prebuilt as static
libraries for all supported platforms. We also distribute prebuilt libraries for all dependencies in 
<tt>contrib/prebuilt/</tt>.

The *u* in uMundo is short for *micro* and does not necessarily mean memory footprint or runtime 
behavior (though we try to keep it sane) - it stands for the amount of source code we actually have 
to write and maintain in order to arrive at a working implementation. It also means that you can
jump right in, there are only about 3k LoC and the majority are the wrappers for Bonjour/Avahi and 
boilerplate code. The actual work is done in <tt>src/connection/zeromq/ZeroMQNode.cpp</tt>.

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

