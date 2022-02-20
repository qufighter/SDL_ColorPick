//
//  FileChooser.h
//  ColorPick SDL
//
//  Created by Sam Larison on 8/13/16.
//
//

// USED BY __EMSCRIPTEN__
#ifdef __EMSCRIPTEN__

#ifndef ColorPick_emscripten_SDL_FileChooser_h
#define ColorPick_emscripten_SDL_FileChooser_h

#include "../../ColorPick.h"

//EMSCRIPTEN_KEEPALIVE int testversion(); // export for EMscripen only... the keepalive can go here.... or...

void getImagePathFromMainThread();
void beginImageSelector();
void beginScreenshotSeleced();
bool openURL(char* &url);
void requestReview();

void emscripen_copy_to_clipboard(char* url);

#if defined(__EMSCRIPTEN__) && defined(COLORPICK_BUILD_FOR_EXT)
    // if u need a macro....
#endif

#endif
#endif
