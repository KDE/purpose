#!/bin/sh
$EXTRACTRC `find . -name \*.rc` `find . -name \*.ui` >> rc.cpp
$XGETTEXT *.cpp *.h -o $podir/libpurpose_quick.pot
rm -f rc.cpp
