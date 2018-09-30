//
//  FileChooser.h
//  ColorPick SDL
//
//  Created by Sam Larison on 8/13/16.
//
//

// USED BY ANDROID
#ifdef __ANDROID__

#ifndef ColorPick_iOS_SDL_FileChooser_h
#define ColorPick_iOS_SDL_FileChooser_h

#include "../../ColorPick.h"


void getImagePathFromMainThread();
void beginImageSelector();
bool openURL(const std::string &url);

#endif
#endif
