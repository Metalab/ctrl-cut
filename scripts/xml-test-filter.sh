#!/bin/bash

cd $CC_TEST_DATA/xml

XQ=`type -p xqilla`
function usage { echo "test-filter.sh [-v] filtername"; }
function green { echo -e "\033[32;1m $1 \033[0m"; tput sgr0; }
function red { echo -e "\033[31;1m $1 \033[0m"; tput sgr0; }
function verbose  { [ $VERBOSE ] && echo $@; $@; }
function error { echo "error: $1" >&2; exit 1; }
function check { 
    echo -en "\tcheck: $1 $2 "
    cd "$1"
    cmd="$XQ -i ctrl-cut.xml $2"
    [ $VERBOSE ] && echo -n "($cmd)";
    echo -n " ... "
    result="`$cmd`"
    [ "$result" == "0" ] && green "OK" || red $result
    cd ..
}

### main

[ $# -eq 0 ] && usage

for filter in "$@"; do
    echo "testing $filter..."
    for xql in `ls $filter/*.xql`; do
        check `basename $filter` `basename $xql`;
    done
done

