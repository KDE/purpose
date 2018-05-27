#!/bin/sh
$XGETTEXT `find . -not -path \*/tests/\* -name \*.cpp -o -name \*.cc -o -name \*.qml -o -name \*.h` -o $podir/purpose_phabricator.pot

