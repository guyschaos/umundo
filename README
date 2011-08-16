# zMundo ReadMe

zMundo is a lightweight clone of the Mundo publish/subscribe architecture
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
	<td> Currently, zMundo is in the preAlpha phase, 
	meaning that it is only conceptualized and active 
	development has not yet started.
    </tr>
</table>

## Dependencies

The dependencies of zMundo are listed below. Note that this
list is only relevant for developers aiming to build these
dependencies for a specific platform. zMundo ships with binaries
for all officially supported platforms.

<table>
    <tr><th>Dependency</th><th>Version</th><th>Comments</th></tr>
    <tr>
	<td>[ZeroMQ](http://www.zeromq.org)</td>
	<td>2.1</td>
	<td>Can also be installed globally</td>
    </tr>
    <tr>
	<td>[Google Protocol Buffers](http://code.google.com/p/protobuf/)</td>
	<td>2.4.1</td>
	<td> </td>
    </tr>
</table>

## Directory Structure

The directory structure is as follows:

 1. there is a special subfolder for everything related to the c++
    port of zmundo (docs, libs etc.)
 2. there is another special subfolder for everything related to the
    java part of zmundo
 3. there is a folder containing the protocol buffer interfaces used
    to describe data structures relevant to both ports
 4. there is a folder containing everything else that is shared between
    the two ports

The tree then is the following:

   +---cpp (1)
   +---etc (4)
   |   +---contrib
   |   +---doc
   |   +---lib
   |   \---tools
   +---ifc (3)
   |   \---zmundo
   \---java (2)
