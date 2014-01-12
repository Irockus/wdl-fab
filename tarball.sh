#! /bin/sh
echo "targzipping plugins..."
b=$(date +%d_%m_%Y)

tar -cvzf iPlug-$b.tar.gz -X exclude.txt .

echo "done"
