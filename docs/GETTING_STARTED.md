# Getting started

uMundo tries to make usage for a developer as convenient as is reasonable. All the libraries we rely upon are available
as precompiled, static libraries and are directly included in the uMundo libraries. 

To get started, download an [installer](http://umundo.tk.informatik.tu-darmstadt.de/installer/) for your platform and run two 
instances of <tt>umundo-pingpong</tt> from the console or from within your installation directory. If you see some *io* on both 
ends, your system is setup correctly to run umundo programs.

You can export <tt>UMUNDO_LOGLEVEL=[0..4]</tt> to increase the global log-level, with 4 being the most verbose. You can also
increase/descrease log-levels per component by exporting <tt>UMUNDO_LOGLEVEL_[COMMON|NET|DISC|S11N]=[0..4]</tt>. With some terminals,
colored output is available via <tt>UMUNDO_LOGCOLORS=[ON|OFF]</tt>. Have a look at the 
<a href="https://github.com/tklab-tud/umundo/blob/master/core/src/umundo/common/Debug.cpp">Debug.cpp</a> file to see how
the various options are used.

## Dependencies

Below are the runtime dependencies for uMundo as the prerequisites to install on a platform in order to develop with and run
uMundo applications.

### Runtime Dependencies

<table>
    <tr><th>Platform</th><th>Dependency</th><th>Comment</th></tr>
	<tr>
		<td rowspan="1">Mac&nbsp;OSX&nbsp;10.7</td>
		<td>None</td><td>Everything is pre-installed.</td>
	</tr>
	<tr>
		<td rowspan="1">iOS 5.x</td>
		<td>None</td><td>Everything is pre-installed.</td>
	</tr>
	<tr>
		<td rowspan="1">Windows&nbsp;7</td>
		<td>None</td>
		<td>We include an embedded mDNS implementation. It is, however, preferred to install bonjour as a service available through an 
			iTunes installation or the somewhat obscure "<a href="http://support.apple.com/kb/DL999?viewlocale=en_US">Bonjour Print Services 
			for Windows</a>". You will have to recompile uMundo with <tt>DISC_BONJOUR_EMBED=OFF</tt> to use the system-wide service.</td>
	</tr>
	<tr>
		<td rowspan="1">Linux</td>
		<td><a href="http://avahi.org/">Avahi</a></td>
		<td>Again, we need a mDNS implementation for discovery and Avahi is preinstalled on most Linux distributions. If Avahi is not an option, 
			you can rebuild uMundo with the embedded Bonjour mDNS server.</td>
	</tr>
	<tr>
		<td rowspan="1">Android</td>
		<td>None</td><td>We build the Android native library with an embedded Bonjour mDNS server, so everything is included.</td>
	</tr>
</table>

### Basic Concept

You start by instantiating a node and specify its domain in the constructor. Then you add
publishers and subscribers specifying their channel name in the constructor. The subscriber
also requires a receiver when invoking its constructor. There are some basic facts that you 
can rely upon, or that we would at least consider bugs if observed otherwise:

1. Two nodes can only see each other if they are in the same domain.
	1. Using the default constructor of a node implicitly uses the default domain.
2. A publisher will send messages to a subscriber if they have the same channel name <b>and</b> were added to at least
one pair of nodes within the same domain.
	1. Publishers will not send messages to subscribers within the same node.
2. A publisher or subscriber can belong to several nodes in different domains.
3. You can use the same receiver for many subscribers but only one receiver per subscriber.

### Using Java Bindings

For umundo.core there is a [sample Eclipse project](https://github.com/tklab-tud/umundo/tree/master/contrib/samples/java). The API
is the same as in C++, but there are some caveats:

1. You have to subclass <tt>org.core.umundo.Receiver</tt> and override receive(Message msg) to receive messages from a Subscriber.
2. SWIG can not generate Interaces and Java does not support multiple inheritance so the API is a little cumbersome at times.
3. You have to explicitly hold references to every Receiver and eventual Greeters as they are garbage collected otherwise.

#### Java Archive

The JAR file in the installers and in the maven repository uses a static initializer method to automatically load its 
platform-specific JNI library from the JAR itself. If this fails, you can still load the JNI library per <tt>System.load()</tt>. 
The JAR built for android only contains the native JNI code for the android platform and can not be used on desktop installations.
There are three reasons for seperating the android and the desktop jar: 1) The library name is the same for android and linux, 2)
the android jar can be smaller and 3) renaming the android jar with CMake is a hassle.

The automatic loading of the JNI libraries contained in the Jar is only performed when uMundo is built with <tt>DIST_PREPARE</tt>
to allow custom builds to load the debug JNI library.

#### Maven Repository

We maintain a maven repository for the desktop umundocore JAR file with everything included for MacOSX, Linux 32/64-Bit and Windows. 
In your <tt>pom.xml</tt> add a new repository and include the umundocore dependency:

    <repositories>
      ...
      <repository>
        <id>tu.darmstadt.umundo</id>
        <name>uMundo at TK</name>
        <url>http://umundo.tk.informatik.tu-darmstadt.de/maven2/</url>
      </repository>
    </repositories>

    <dependencies>
      ...
      <dependency>
        <groupId>org.umundo</groupId>
        <artifactId>umundocore</artifactId>
        <version>0.0.4</version>
      </dependency>
    </dependencies>
