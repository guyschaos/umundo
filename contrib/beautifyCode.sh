#!/bin/bash

# see http://astyle.sourceforge.net/astyle.html
# run from project root as sh ./contrib/tidy_source.sh

astyle  \
	--style=java \
	--indent=tab \
	--recursive  src/*.cpp src/*.h
	
find src -iname '*.orig' -exec rm {} \;