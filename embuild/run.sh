#!/bin/sh


rm -fr ./fs
mkdir ./fs
cp -R ../shaders ./fs/shaders
cp -R ../textures ./fs/textures


emrun --no_browser --port 8080 .
