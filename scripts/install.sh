#!/bin/bash

if [ ! $# -eq 2 ]; then
    echo "install.sh PRINTERNAME PPDFILE"
fi

cd $CC_BASE

. $CC_FUNCTIONS

while getopts 'p' c
do
    case $c in
        c) PRINTER_INSTALL=-p ; shift;;
        --) shift; break ;;
    esac
done

PRINTER=$1
PPDFILE=$2
DEVICEURL=$3

try "find cups-config" "type -p cups-config"

FILTER_PATH="`cups-config --serverbin`/filter"
BACKEND_PATH="`cups-config --serverbin`/backend"

try "Install filter binary" "cp $CC_BINARY $FILTER_PATH/"
try "Install lpd backend" "cp $CC_BASE/src/lpd-epilog/lpd-epilog $BACKEND_PATH"
#try "Installing dump backend" "cp $CC_TEST_CODE/dump $BACKEND_PATH"
#try "Fixing file permissions" "chmod u+x $BACKEND_PATH/dump"
[ $PRINTER_INSTALL ] && try "Installing printer Name: $PRINTER PPD: $PPDFILE Device:$DEVICEURL" "lpadmin -E -p $PRINTER -P $PPDFILE -v $DEVICEURL"
