#!/bin/sh


#rm -fr ./shaders
#rm -fr ./textures

#cp -R ../shaders ./shaders
#cp -R ../textures ./textures

rm -fr ./fs
mkdir ./fs
cp -R ../shaders ./fs/shaders
cp -R ../textures ./fs/textures

# The html page you are running is not emrun-capable. Stdout, stderr and exit(returncode) capture will not work. Recompile the application with the --emrun linker flag to enable this, or pass --no_emrun_detect to emrun to hide this check.



#emcc -I../SDL/library/* ../SDLImage/library/* ~/libs/SDL2-2.0.9/embuild/libSDL2.a  ~/libs/SDL2_image-2.0.4/embuild/libSDL2_image.a ../*.cpp -s WASM=1 -o hello.html

#emcc ../*.cpp ../Platform/Emscripten/*.cpp -s WASM=1 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -I../SDL/library/ -I../SDLImage/library/ -I~/libs/SDL2-2.0.9/embuild/build/.libs/libSDL2.a -I~/libs/SDL2_image-2.0.4/embuild/.libs/libSDL2_image.a  -o hello.html

#emcc ../*.cpp ../Platform/Emscripten/*.cpp -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["bmp","png","xpm"]' -s ERROR_ON_UNDEFINED_SYMBOLS=0 -I../SDL/library/ -I../SDLImage/library/ -o hello.html

#emcc -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='[]' -s ERROR_ON_UNDEFINED_SYMBOLS=0 ../*.cpp ../Platform/Emscripten/*.cpp -o hello.html

#ALLOW_MEMORY_GROWTH=1 
#TOTAL_MEMORY=500000000

#emcc ../*.cpp ../Platform/Emscripten/*.cpp -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s STB_IMAGE=1 -s TOTAL_MEMORY=536870912 -s "BINARYEN_TRAP_MODE='js'" -s ERROR_ON_UNDEFINED_SYMBOLS=0 -I../SDL/library/ -I../SDLImage/library/ -I~/libs/SDL2-2.0.9/embuild/build/.libs/libSDL2.a -I~/libs/SDL2_image-2.0.4/embuild/.libs/libSDL2_image.a  -o hello.html --preload-file ./fs@/ --exclude-file .DS_Store  --use-preload-plugins 


emcc ../*.cpp ../Platform/Emscripten/*.cpp -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s STB_IMAGE=1 -s TOTAL_MEMORY=1073741824 -s "BINARYEN_TRAP_MODE='clamp'" -s ERROR_ON_UNDEFINED_SYMBOLS=0 -I/Users/saml/git/emscripten-ports/SDL2/include -I/Users/saml/git/emscripten-ports/SDL2_image/ -o hello.html --preload-file ./fs@/ --exclude-file *.DS_Store  --use-preload-plugins -s ALLOW_MEMORY_GROWTH=1 -O2 #-s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' # --emrun 

# thats a debug build... we performed many hacks to ~/.emscripten_ports/sdl2_image/SDL2_image-version_4 to make it work, so the headers we specified above simply match
# hacks documented
# cd ~/.emscripten_ports/sdl2_image/SDL2_image-version_4
# cp ../../sdl2/SDL2-version_17/include/* .
# next we hack two files.... 
# IMG.c
# + #include <stdlib.h>
# SDL_rwops.h
# + include <stdio.h>
# + define HAVE_STDIO_H


# output to console is supplied by  --emrun
emrun --no_browser --port 8080 .
