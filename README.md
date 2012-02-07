# uMundo ReadMe

uMundo is a lightweight clone of the Mundo publish/subscribe architecture
over [ZeroMQ](http://www.zeromq.org) and [Protocol
Buffers](http://code.google.com/p/protobuf/).

## Contributors

 Stefan Radomski <radomski@tk.informatik.tu-darmstadt.de>
 Felix Heinrichs <felix.heinrichs@cs.tu-darmstadt.de>

## Status

<table>
    <tr>
        <th>Version</th>
	<td>0.0.1-prealpha</td>
    </tr>
    <tr>
        <th>Description</th>
	<td> Currently, uMundo is in the preAlpha phase, 
	meaning that it is only conceptualized and active 
	development has not yet started.
    </tr>
</table>

## Dependencies

The dependencies of uMundo are listed below. Note that this
list is only relevant for developers aiming to build these
dependencies for a specific platform. uMundo ships with binaries
for all officially supported platforms.

<table>
    <tr><th>Dependency</th><th>Version</th><th>Comments</th></tr>
    <tr>
	<td><a href="http://support.apple.com/kb/DL999?viewlocale=en_US">Bonjour</a></td> 
	<td>latest</td>
	<td>Has to be installed globally; use avahi for linux</td>
    </tr>
    <tr>
	<td><a href="http://code.google.com/p/protobuf/">
               Google Protocol Buffers</a></td> 
	<td>2.4.1</td>
	<td>Only required for uMundoSerialization</td>
    </tr>
    <tr>
	<td><a href="http://www.cmake.org/cmake/resources/software.html">
               cMake</a></td> 
	<td>latest</td>
	<td>Build system</td>
    </tr>
</table>
