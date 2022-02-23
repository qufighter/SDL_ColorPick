//
//  FileChooser.h
//  ColorPick SDL
//
//  Created by Sam Larison on 8/13/16.
//
//

// USED BY LINUX / UNIX
#ifdef __LINUX__

#ifndef ColorPick_linx_SDL_FileChooser_h
#define ColorPick_linx_SDL_FileChooser_h

#include "../../ColorPick.h"


void getImagePathFromMainThread();
void beginImageSelector();
void beginScreenshotSeleced();
bool openURL(char* &url);
void requestReview();

bool enoughMemoryForMeshes();

#endif
#endif
