
# Building from Source

## Build Dependencies

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

## Mac OSX

### Console / Make

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
	
### Xcode
	
	$ cd /somewhere/even/more/convenient/
	$ git clone git@github.com:tklab-tud/umundo.git
	$ mkdir umundo/build && cd umundo/build
	$ cmake -G Xcode ..
	[...]
	-- Build files have been written to: /somewhere/convenient/umundo/build
	$ open umundo.xcodeproj

## Linux

### Console / Make

Instructions are literally a verbatim copy of building umundo for MacOSX on the console.

### Eclipse CDT

	$ cd /somewhere/convenient/
	$ git clone git@github.com:tklab-tud/umundo.git
	$ mkdir umundo/build && cd umundo/build
	$ cmake -G "Eclipse CDT4 - Unix Makefiles" ..
	[...]
	-- Build files have been written to: /somewhere/convenient/umundo/build

Now open Eclipse CDT and import the out-of-source directory as an existing project into workspace, leaving the "Copy projects 
into workspace" checkbox unchecked. There are some more [detailled instruction](http://www.cmake.org/Wiki/Eclipse_CDT4_Generator) available 
in the cmake wiki as well.

### Debian Stable - Complete Walkthrough

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

## Windows

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
