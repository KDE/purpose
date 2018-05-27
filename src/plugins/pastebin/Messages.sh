#!/bin/sh
$XGETTEXT `find . -not -path \*/tests/\* -name \*.cpp -o -name \*.cc -o -name \*.h` -o $podir/purpose_pastebin.pot
