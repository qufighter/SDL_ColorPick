#!/bin/sh

DEST="../../../../git/qufighter.github.io/ColorPick/Mobile/WebDemo/"

echo $DEST

ls $DEST

cp ./hello* `echo $DEST`
cp ./_index.html $DEST"index.html"
cp ./_fullscreen.html $DEST"fullscreen.html"

open $DEST

echo "done?"
