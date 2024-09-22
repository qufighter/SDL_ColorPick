#!/bin/bash

# USAGE:
# allowed args
# DEBUG - first of two args, enables debug builds (console output)
# EXT - make this an EXTENSION build (as in chrome extension) and copy the results there ( call publish-ext.sh )

# sample execution:
# bash build.sh DEBUG DEBUG

# makes debug easier
set -x

rm -fr ./fs
mkdir ./fs
cp -R ../shaders ./fs/shaders
cp -R ../textures ./fs/textures


arg1=$1
arg2=$2
dDefines="HUHITSDEFTHENHULLNOEFFECTIGUESS" #some string needed for -D so we make a random ifdef (hopefully it will not break a thing)
buildFlags="-O2"
echo $arg1

test "$arg1" == "EXT"
if [[ $? -eq 0 ]]; then
    dDefines="COLORPICK_BUILD_FOR_EXT=1"
fi

echo $dDefines


test "$arg1" == "DEBUG" || test "$arg2" == "DEBUG"
if [[ $? -eq 0 ]]; then
    buildFlags=""
fi

# The html page you are running is not emrun-capable. Stdout, stderr and exit(returncode) capture will not work. Recompile the application with the --emrun linker flag to enable this, or pass --no_emrun_detect to emrun to hide this check.

echo "flags are "$buildFlags

#emcc -I../SDL/library/* ../SDLImage/library/* ~/libs/SDL2-2.0.9/embuild/libSDL2.a  ~/libs/SDL2_image-2.0.4/embuild/libSDL2_image.a ../*.cpp -s WASM=1 -o hello.html

#emcc ../*.cpp ../Platform/Emscripten/*.cpp -s WASM=1 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -I../SDL/library/ -I../SDLImage/library/ -I~/libs/SDL2-2.0.9/embuild/build/.libs/libSDL2.a -I~/libs/SDL2_image-2.0.4/embuild/.libs/libSDL2_image.a  -o hello.html

#emcc ../*.cpp ../Platform/Emscripten/*.cpp -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["bmp","png","xpm"]' -s ERROR_ON_UNDEFINED_SYMBOLS=0 -I../SDL/library/ -I../SDLImage/library/ -o hello.html

#emcc -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='[]' -s ERROR_ON_UNDEFINED_SYMBOLS=0 ../*.cpp ../Platform/Emscripten/*.cpp -o hello.html

#ALLOW_MEMORY_GROWTH=1 
#TOTAL_MEMORY=500000000

#emcc ../*.cpp ../Platform/Emscripten/*.cpp -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s STB_IMAGE=1 -s TOTAL_MEMORY=536870912 -s "BINARYEN_TRAP_MODE='js'" -s ERROR_ON_UNDEFINED_SYMBOLS=0 -I../SDL/library/ -I../SDLImage/library/ -I~/libs/SDL2-2.0.9/embuild/build/.libs/libSDL2.a -I~/libs/SDL2_image-2.0.4/embuild/.libs/libSDL2_image.a  -o hello.html --preload-file ./fs@/ --exclude-file .DS_Store  --use-preload-plugins 


# define an ifdef....
#  -D COLORPICK_BUILD_FOR_EXT=1 
emcc ../*.cpp ../Platform/Emscripten/*.cpp -D $dDefines -lidbfs.js -s FORCE_FILESYSTEM=1 -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s STB_IMAGE=1 -s TOTAL_MEMORY=1073741824 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -o hello.html --preload-file ./fs@/ --exclude-file *.DS_Store  --use-preload-plugins -s ALLOW_MEMORY_GROWTH=1 $buildFlags #-s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' # --emrun

if [[ $? -eq 0 ]]; then

    # output to console is supplied by  --emrun
    test "$arg1" == "EXT"
    if [[ $? -eq 0 ]]; then
        ./publish-ext.sh
        echo "publish done..."
    else
        emrun --no_browser --port 8080 .
    fi

else
    echo "ERRORS - please fix the build and try again :)"

    exit 1
fi
