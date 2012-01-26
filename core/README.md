# uMundo-core ReadMe

uMundo-core is a lightweight publish/subscribe system with a single responsibility: to deliver
byte arrays on channels from publishers to subscribers - a channel is just an agreed upon string
such as "temperature". It uses the Bonjour/Avahi for the discovery of nodes and ZeroMQ to deliver 
the byte arrays.

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
	<td>uMundo-core is functional and runs on Linux, MacOSX, Windows, Android and iOS devices.
		It is quite possible that it will compile on the various BSDs, at least it can be ported 
		with a minimum of work.
    </tr>
</table>

## Philosophy

We aim to provide a good "walk up and use" experience. With a few exceptions, everything you need
is available within the distribution. There is a <tt>lib/</tt> folder with uMundo-core prebuilt as static
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
	<td>Native language of uMundo-core.</td>
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

## Using uMundo-Core

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

We use CMake as the build-system for uMundo-core itself and some custom shell-scripts in <tt>contrib/</tt> to build 
and cross-compile our dependencies. Building the uMundo-core is pretty straight-forward if you are familiar with CMake.

### Unices

Change to the directory where you checked out the uMundo-core, create a new <tt>build/</tt> directory for an 
*out-of-source* build and run cmake & make:

    umundo/core$ mkdir build
    umundo/core$ cd build
    core/build$ cmake ..
    core/build$ make

If you happen to be on MacOSX, you can generate an Xcode project just as easy:

    umundo/core$ mkdir xcode
    umundo/core$ cd xcode
    core/xcode$ cmake -G Xcode ..
    core/xcode$ open ./umundo.xcodeproj

There are other generators to create artifacts for various IDEs, have a look at <tt>cmake --help</tt> to get a list.

### Windows

Once you have downloaded and installed CMake, run the CMake-GUI and point the source directory to the directory where
you checked-out uMundo-core, choose any directory as the build directory. I only tested the MS Visual Studio 10 generator
but there is no reason why MinGW or others should not work as well.

### Cross Compiling for Android / iOS

