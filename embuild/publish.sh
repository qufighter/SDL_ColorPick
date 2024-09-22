#!/bin/bash

DEST="../../qufighter.github.io/ColorPick/Mobile/WebDemo/"

# makes debug easier
set -x



arg1=$1
echo "arg: "$arg1

test "$arg1" == "DEBUG"
if [[ $? -eq 0 ]]; then
    DEST="../../qufighter.github.io/ColorPick/Mobile/WebDemoDebug/"
fi

echo $DEST

ls $DEST

cp ./hello* `echo $DEST`
cp ./_index.html $DEST"index.html"
cp ./_fullscreen.html $DEST"fullscreen.html"

open $DEST

echo "done?"
