#!/bin/bash

cd $CC_BASE

[ $CC_DEBUG ] && set -x

[ ! -d "/tmp" ] && mkdir "./tmp"

export RASTER_OFF="y"
[ $# -ne 2 -a $# -ne 3 ] && error "Usage: $0 ps-file outdir options-file commonoptions-file" 1
file="$1"
optionsfile="$2"
commonoptionsfile="$3"

export CHARSET=utf-8
export CONTENT_TYPE=application/pdf
#export CUPS_CACHEDIR=/private/var/spool/cups/cache
export CUPS_DATADIR=`cups-config --datadir`
export CUPS_FILETYPE=document
export CUPS_SERVERROOT=`cups-config --serverroot`
export DEVICE_URI=file:/tmp/passthrough.ps
export FINAL_CONTENT_TYPE=printer/passthrough
export LANG=en_US.UTF-8
#export PATH=/usr/libexec/cups/filter:/usr/bin:/usr/sbin:/bin:/usr/bin
export PPD="$CUPS_DATADIR/ppd/passthrough.ppd"
export PRINTER=passthrough
#export SOFTWARE=CUPS/1.4.3
export USER=root
if [ -f $commonoptionsfile ]; then
  read < $commonoptionsfile commonoptions
fi

if [ -f $optionsfile ]; then
  read < $optionsfile options
fi

[ $CC_VERBOSE ] && options="$options Debug"

# using the new targetstdout option of try
cutfile="`mktemp`"
verboseexec "$CC_RENDER2CUT 32 kintel $file 1 \"$commonoptions $options $CC_FILTER_OPTIONS\" $file" > $cutfile
verboseexec "$CC_CUT2EPILOG $cutfile /dev/stdout"
rm -fr $cutfile &> /dev/null

exit $?
