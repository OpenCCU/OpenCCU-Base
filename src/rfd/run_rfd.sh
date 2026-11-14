#!/bin/sh
PWD=`dirname $0`
ELVUTILSPATH=$PWD/../base/libs/elvutils
XMLRPCPATH=$PWD/../base/libs/xmlrpc
HSSCOMMPATH=$PWD/../libs/libhsscomm
XMLPARSERPATH=$PWD/../libs/xmlparser
TCLPATH=$PWD/../base/tcl/unix
RFDPATH=$PWD/rfd

LD_LIBRARY_PATH=$ELVUTILSPATH:$XMLRPCPATH:$HSSCOMMPATH:$XMLPARSERPATH:$TCLPATH $RFDPATH $*
