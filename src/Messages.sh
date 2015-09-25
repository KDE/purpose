#!/bin/sh
$EXTRACTRC `find . -name \*.rc` `find . -name \*.ui` >> rc.cpp
$XGETTEXT *.cpp *.h -o $podir/libpurpose.pot
rm -f rc.cpp
