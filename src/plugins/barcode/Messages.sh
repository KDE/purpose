#!/bin/sh
$XGETTEXT `find . -name \*.cpp -o -name \*.qml` -o $podir/purpose_barcode.pot
