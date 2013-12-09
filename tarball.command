#! /bin/sh

BASEDIR=$(dirname $0)

cd $BASEDIR

echo "zipping plugins..."
b=$(date +%d_%m_%Y)

tar -cvzf iPlug-$b.tar.gz -X exclude.txt .

echo "done"
