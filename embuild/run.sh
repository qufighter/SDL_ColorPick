#!/bin/sh

# this has no effect with how we embed the files so JS can read them
# in theory though, if we were running from life FS folder, then updating that folder (absent any caching)
# we could just refresh and read hte new files... so in theory this speeds up development for shader/texture changes
# in practice though the following lines are only truly effective in build.sh
rm -fr ./fs
mkdir ./fs
cp -R ../shaders ./fs/shaders
cp -R ../textures ./fs/textures


emrun --no_browser --port 8080 .
