//
//  FileChooser.h
//  ColorPick SDL
//
//  Created by Sam Larison on 8/13/16.
//
//

// USED BY LINUX / UNIX
#ifdef __WINDOWS__

#ifndef ColorPick_windiows_SDL_FileChooser_h
#define ColorPick_windiows_SDL_FileChooser_h

#include "../../ColorPick.h"
#include "./COLORPICK_WIN_CLR/ColorPickWinClr.h"
#include "portable-file-dialogs.h"



void getImagePathFromMainThread();
void beginImageSelector();
void beginScreenshotSeleced();
bool openURL(char* url);
void requestReview();


#endif
#endif
