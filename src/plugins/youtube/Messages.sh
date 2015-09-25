#!/bin/sh
$EXTRACTRC `find . -name \*.rc` `find . -name \*.ui` >> rc.cpp
$XGETTEXT `find . -not -path \*/tests/\* -name \*.cpp -o -name \*.cc -o -name \*.h` -o $podir/purpose_youtube.pot
rm -f rc.cpp
