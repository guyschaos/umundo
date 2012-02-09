# uMundo ReadMe

uMundo is a lightweight refactoring of the Mundo publish/subscribe architecture
using [ZeroMQ](http://www.zeromq.org) and [Protocol Buffers](http://code.google.com/p/protobuf/) 
and [Avahi](http://avahi.org/) or [Bonjour](http://developer.apple.com/opensource/) for discovery.
With lightweight, we do not refer to runtime behavior or memory footprint but the amount of code 
we have to maintain in order to arrive at a working system.

## Contributors

- Stefan Radomski <radomski@tk.informatik.tu-darmstadt.de>
- Felix Heinrichs <felix.heinrichs@cs.tu-darmstadt.de>

## Status

Below are the systems we test umundo on while developing. While these are the systems where we can determine the status
for sure, that is not to say that umundo won't run on variations of these systems.

<table>
    </tr>
    <tr><th>Platform</th><th>Issues</th></tr>
	<tr><td>Mac OSX 10.7</td><td>
		<ul>
			<li>Everything builds and runs just fine.
		</ul>
	</td></tr>

	<tr><td>iOS 5.x</td><td>
		<ul>
			<li>umundo.core compiled and runs without problems but prebuild libraries are missing.
			<li>umundo.s11n still needs cross-compiled protobuf.
			<li>Haven't bothered with older iOS releases.
		</ul>
	</td></tr>
	<tr><td>Windows 7</td><td>
		<ul>
			<li>Everything builds and runs just fine with MS Visual Compiler and Visual Studio 10.
			<li>MinGW still needs precompiled libraries for our build-time dependencies.
			<li>Building the JAR for the JNI wrappers when building from source fails, the <tt>.java</tt> 
				files are still generated though. Just copy/package them by hand or use a jar from another platform.
		</ul>
	</td></tr>
	<tr><td>Debian Linux 6.0.4</td><td>
		<ul>
			<li>Everything builds and runs just fine with GCC.
		</ul>
	</td></tr>
	<tr><td>Android 4.0.3</td><td>
		<ul>
			<li>Everything builds, links and deploys just fine.
			<li>Calls into native code never return though - I guess that means *not running*.
		</ul>
	</td></tr>
    </tr>
</table>

## Dependencies

Below are the dependencies for umundo, distinguished by build dependencies as the things you will need to compile umundo 
from scratch and runtime dependencies as the things you will need to run anything umundo.

### Runtime Dependencies

<table>
    <tr><th>Platform</th><th>Dependency</th><th>Comment</th></tr>
	<tr>
		<td rowspan="1">Mac&nbsp;OSX&nbsp;10.7</td>
		<td>None</td><td>Everything is pre-installed</td>
	</tr>
	<tr>
		<td rowspan="1">iOS 5.x</td>
		<td>None</td><td>Everything is pre-installed</td>
	</tr>
	<tr>
		<td rowspan="1">Windows&nbsp;7</td>
		<td><a href="http://support.apple.com/kb/DL999?viewlocale=en_US">Bonjour</a></td>
		<td>We need a mDNS implementation for discovery and Bonjour is available through an iTunes installation or the somewhat obscure "Bonjour Print Services for Windows".</td>
	</tr>
	<tr>
		<td rowspan="1">Linux</td>
		<td><a href="http://avahi.org/">Avahi</a></td>
		<td>Again, we need a mDNS implementation for discovery and Avahi is preinstalled on most Linux distributions.</td>
	</tr>
</table>

### Build Dependencies

<table>
    <tr><th>Platform</th><th>Dependency</th><th>Version</th><th>Comment</th></tr>
	<tr>
		<td rowspan="5">*Everyone*</td>
			<td><a href="http://www.cmake.org/cmake/resources/software.html">CMake</a><br />required</td>
			<td>>=&nbsp;2.8.6</td>
			<td>The build-system used to build umundo from sources.</td></tr>
		<tr>
			<td><a href="http://www.swig.org/">SWIG</a><br />optional</td>
			<td>>=&nbsp;2.0.5</td>
			<td>Wraps the umundo C/C++ code for Java and other languages. Version 2.0.5 is only in 
				<a href="https://swig.svn.sourceforge.net/svnroot/swig/">SVN</a> at the moment and earlier versions
				are missing some features we are using. <a href="http://www.swig.org/Doc2.0/Windows.html">Building for 
				Win32</a> is not trivial, so let's hope for a 2.0.5 release soon.</td></tr>
		<tr>
			<td><a href="http://code.google.com/p/protobuf/">Protocol&nbsp;Buffers</a><br />required s11n</td>
			<td>2.4.1 works</td>
			<td>Object serializer currently used.</td></tr>
		<tr>
			<td><a href="http://git-scm.com/">Git</a><br />required</td>
			<td></td>
			<td>Versioning control system.</td></tr>
		<tr>
			<td><a href="http://www.stack.nl/~dimitri/doxygen/">Doxygen</a><br />optional</td>
			<td></td>
			<td>Used by <tt>make docs</tt> to generate documentation from source comments.</td>
	</tr>
	<tr>
		<td rowspan="2">Mac OSX</td>
			<td><a href="http://developer.apple.com/xcode/">XCode</a><br />required</td>
			<td>4.2.1 works</td>
			<td>Apples SDK with all the toolchains.</td></tr>
		<tr>
			<td><a href="http://www.macports.org/">MacPorts</a><br />recommended</td>
			<td>>= 2.0.3</td>
			<td>Build system for a wide selection of open-source packages.</td></tr>

	</tr>
	<tr>
	<td rowspan="2">Windows</td>
		<td><a href="http://www.microsoft.com/visualstudio/en-us">Visual&nbsp;Studio&nbsp;10</a><br />required</td>
		<td>v10 pro works</td>
		<td>As a student, you can get your version through MSAA.</td></tr>
	<tr>
		<td><a href="https://developer.apple.com/downloads/index.action?q=bonjour%20sdk%20for%20windows">Bonjour SDK</a><br />optional</td>
		<td>>=&nbsp;2.0.4</td>
		<td>Only available to registered MacOSX or iOS developers, but we distribute an embedded library for your convenience.</td></tr>
	</tr>
	<tr>
	<td rowspan="3">Linux</td>
		<td>Build system<br />required</td>
		<td>g++&nbsp;>=&nbsp;4.4<br />make&nbsp;>=3.81</td>
		<td>For Debian:<br /><tt>$ sudo apt-get install cmake cmake-curses-gui make g++</tt></td></tr>
	<tr>
		<td>JDK<br />optional</td>
		<td>>= 6.x</td>
		<td>For Debian with <emph>non-free<emph>:<br /><tt>$ sudo apt-get install sun-java6-jdk</tt></td></tr>
	<tr>
		<td>Avahi<br />required</td>
		<td>3.x works</td>
		<td>For Debian:<br /><tt>$ sudo apt-get install avahi-daemon libavahi-client-dev</tt></td></tr>
	</tr>
</table>

## Getting Started

Download an [installer](https://github.com/tklab-tud/umundo/tree/master/installer) for your platform and run two instances 
of <tt>umundo-pingpong</tt> from the console or from within your installation directory. If you see some *ping* on both ends, 
your system is setup correctly to run umundo programs. If you see some *ping* without having started a second instance, someone
else in your multicast domain is running <tt>umundo-pingpong</tt> as well.

## Building from Source

The process of building umundo is essentially the same on every platform:

1. Resolve build dependencies for your platform given in table above.
2. Checkout the umundo sources into a convenient directory:

	<tt>git clone git@github.com:tklab-tud/umundo.git</tt>

3. Create a new directory for an *out-of-source* build.
4. Run cmake to create the artifacts for your preferred build-system or development environment.
5. Use your actual build-environment or development environment to build umundo.

If you want to build for another IDE or build-environment, just empty the *out-of-source* folder and start over with cmake. To
get an idea of supported IDEs and build-environments on your platform, type <tt>cmake --help</tt> and look for the *Generators*
section at the end of the output.

### Mac OSX

#### Console / Make

	$ cd /somewhere/convenient/
	$ git clone git@github.com:tklab-tud/umundo.git
	$ mkdir umundo/build && cd umundo/build
	$ cmake ..
	[...]
	-- Build files have been written to: /somewhere/convenient/umundo/build
	$ make

You can test whether everything works by starting one of the sample programs:

	$ ./core/test/umundo-pingpong &
	$ ./core/test/umundo-pingpong &
	received ping
	[...]
	$ killall umundo-pingpong
	
#### Xcode
	
	$ cd /somewhere/even/more/convenient/
	$ git clone git@github.com:tklab-tud/umundo.git
	$ mkdir umundo/build && cd umundo/build
	$ cmake -G Xcode ..
	[...]
	-- Build files have been written to: /somewhere/convenient/umundo/build
	$ open umundo.xcodeproj

### Linux

#### Console / Make

Instructions are literally a verbatim copy of building umundo for MacOSX on the console.

#### Eclipse CDT

	$ cd /somewhere/convenient/
	$ git clone git@github.com:tklab-tud/umundo.git
	$ mkdir umundo/build && cd umundo/build
	$ cmake -G "Eclipse CDT4 - Unix Makefiles" ..
	[...]
	-- Build files have been written to: /somewhere/convenient/umundo/build

Now open Eclipse CDT and import the out-of-source directory as an existing project into workspace, leaving the "Copy projects 
into workspace" checkbox unchecked. There are some more [detailled instruction](http://www.cmake.org/Wiki/Eclipse_CDT4_Generator) available 
in the cmake wiki as well.

### Windows

Building from source on windows is somewhat more involved and instructions are necessarily in prose form.

1. Use git to checkout the source from <tt>git@github.com:tklab-tud/umundo.git</tt> into any convenient directory.
2. Start the CMake-GUI and enter the checkout directory in the "Where is the source code" text field.
3. Choose any convenient directory to build the binaries in.
4. Hit "Configure" and choose your toolchain and compiler - I only tested with Visual Studio 10 (not the IA64 or Win64 variants).
I can say for sure that MinGW won't work for now, as we do not have prebuilt libraries for the the MinGW compiler yet.
	1. CMake will complain about missing SWIG executable (see above for building a current SWIG on windows)
	2. CMake will also complain about protobuf, but the source distribution from Google contains a solution file for Visual Studio
	so you could build it easily. Just point CMake to the directory where you built protobuf by setting the
	<tt>PROTOBUF_SRC_ROOT_FOLDER</tt> field and be done.
5. If you did not <a href="http://www.swig.org/Doc2.0/Windows.html">built SWIG via MinGW</a> you will not get any Java bindings.
6. Press "Configure" some more until no fields are marked red, then press "Generate".
7. Navigate to the directory where you told CMake-GUI to build the libraries and find a umundo solution file for MS Visual Studio
there.
8. Open the solution with MS Visual Studio. Only <tt>Debug</tt> and <tt>Release</tt> builds are supported for now.

## FAQ

<dt><b>Why is the source distribution so large?</b></dt>
<dd>That's the price of convenience. The distribution contains most of our runtime dependencies prebuilt for every 
	system / compiler combination and for debug and release builds.<dd>

<dt><b>How many umundo nodes can I realistically start at once?</b></dt>
<dd>Using the default ZeroMQ implementation and Bonjour discovery on MacOSX, I could start 32 umundo-pingpong instances before
	getting an <tt>Assertion failed: s != retired_fd (tcp_connecter.cpp:278)</tt> within ZeroMQ. I guess this is due to the 
	rather low ulimit for open file-handles on MacOSX (<tt>ulimit -n</tt> gives 256).</dd>

<dt><b>Are these actually questions that are asked frequently?</b><dt>
<dd>No, it's more like a set of questions I can imagine other people might have. It will eventually grow into a real FAQ.</dd>