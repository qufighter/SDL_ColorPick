#!/bin/sh

cd ../../emsdk

ls

source ./emsdk_env.sh

cd ../SDL_ColorPick/embuild

echo " if it did not work, try source ./activate_embuild.sh"

echo " if it did work, remember to execute build with `bash ./build.sh DEBUG DEBUG`"
