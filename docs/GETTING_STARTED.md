# Getting started

uMundo tries to make usage as convenience for a developer as is reasonable. All the libraries we rely upon are available
as precompiled, static libraries and are directly included in the resulting libraries in <tt>/lib</tt>. 

To get started, download an [installer](https://github.com/tklab-tud/umundo/tree/master/installer) for your platform and run two 
instances of <tt>umundo-pingpong</tt> from the console or from within your installation directory. If you see some *io* on both 
ends, your system is setup correctly to run umundo programs.

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
		<td><a href="http://support.apple.com/kb/DL999?viewlocale=en_US">Bonjour</a></td>
		<td>We need a mDNS implementation for discovery and Bonjour is available through an iTunes installation or the somewhat obscure "Bonjour Print Services for Windows".</td>
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

<table class="image" border="1px" style="float: right;">
<caption align="bottom">Figure 1: User visible object model of umundo.core</caption>
<tr><td><img width="300px" src="https://github.com/tklab-tud/umundo/blob/master/docs/user-visible-object-model.png" /></td></tr>
</table>

The basic idea with umundo is always to establish a structure as is given in figure 1.
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

For umundo.core there is a [sample Eclipse project](https://github.com/tklab-tud/umundo/tree/master/core/contrib/bindings/java). You
can add it to your Eclipse workspace as follows:

<b>outdated<b>

1. In Eclipse choose <tt>File->New->Java Project</tt>.
2. Choose any project name.
3. Uncheck the <tt>Use default location</tt> checkbox and navigate to <tt>core/contrib/bindings/java</tt>.
4. Click <tt>Next</tt> and verify that <tt>umundocoreSwigJNI.jar</tt> is referenced as a library. Do not mind the 
<tt>darwin-i386</tt> in the relative path, the jar is platform independent.
5. Click <tt>Finish</tt> and open the <a href="https://github.com/tklab-tud/umundo/blob/master/core/contrib/bindings/java/src/org/mundo/samples/TestPubSub.java"><tt>org.mundo.samples.TestPubSub</tt></a> java class.
	- If you are not on Mac OSX, adapt the path to the native library within the <tt>System.setProperty()</tt> call.
	- If you have installed the native libraries or the jars somewhere else, make sure to adapt the paths.
6. Start the program and see the message being transferred from the publisher to the subscriber.

If you did not checkout the source, you can still get everything you need by grabbing the prebuilt <tt>umundocoreSwig.[so|dll|jnilib]</tt>
from the library folder of your respective platform.

- Get your JNI umundo.core library for [Windows](https://github.com/tklab-tud/umundo/blob/master/lib/windows-x86/msvc/Release/umundocoreSwig.dll),
[Linux](https://github.com/tklab-tud/umundo/blob/master/lib/linux-i686/gnu/Release/libumundocoreSwig.so) or
[MacOSX](https://github.com/tklab-tud/umundo/blob/master/lib/darwin-i386/gnu/Release/libumundocoreSwig.jnilib)
- Download the [corresponding jar] (https://github.com/tklab-tud/umundo/blob/master/lib/darwin-i386/gnu/Release/umundocoreSwigJNI.jar) with the java classes.
