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

int testversion(); // export for EMscripen only... test is this is needed or not??

void getImagePathFromMainThread();
void beginImageSelector();
bool openURL(char* &url);
void requestReview();

#endif
#endif
