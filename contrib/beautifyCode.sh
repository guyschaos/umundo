#!/bin/bash

# see http://astyle.sourceforge.net/astyle.html
# run from project root as sh ./contrib/tidy_source.sh

set -e

ME=`basename $0`
DIR="$( cd "$( dirname "$0" )" && pwd )"
CWD=`pwd`

astyle  \
	--style=java \
	--indent=tab \
	--recursive ${DIR}/../core/src/*.cpp ${DIR}/../core/src/*.h
	
find ${DIR}/../core/src/ -iname '*.orig' -exec rm {} \;