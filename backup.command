#! /bin/sh

BASEDIR=$(dirname $0)

cd $BASEDIR

echo "zipping plugins..."
b=$(date +%d_%m_%Y)

zip -r iPlug-$b.zip . -x "PCH/*" "ipch/*" "*build/*" "*/Debug/*" "*/Release/*" ".gitignore" "*/build-*" "*/.git/*" "old/*"  "*/dist/*" "*.suo" "*.DS_Store" "*.dmg" "*.ncb" "*.zip" "*.pkg" "*.sdf" "*.ipch"

echo "done"
