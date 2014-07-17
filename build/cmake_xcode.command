#!/bin/bash
BASEDIR=$(dirname $0)
mkdir -p $BASEDIR/osx
cd $BASEDIR/osx
cmake ../.. -GXcode
