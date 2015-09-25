#!/bin/sh
$EXTRACTRC `find . -name \*.rc` `find . -name \*.ui` >> rc.cpp
$XGETTEXT *.cpp *.h -o $podir/libpurpose_widgets.pot
rm -f rc.cpp
