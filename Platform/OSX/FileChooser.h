//
//  FileChooser.h
//  ColorPick SDL
//
//  Created by Sam Larison on 8/13/16.
//
//

// USED BY IOS(osx?) EXCLUSIVELY MOVE TO IOS FOLDER


#ifndef ColorPick_iOS_SDL_FileChooser_h
#define ColorPick_iOS_SDL_FileChooser_h

#include "../../ColorPick.h"

#define COLORPICK_OSX_NATIVE_DIALOGUE_CODE 1

#ifndef COLORPICK_OSX_NATIVE_DIALOGUE_CODE
#include "external_static/portable-file-dialogs.h"
#endif

void beginImageSelector();
void beginScreenshotSeleced();
bool openURL(const std::string &url);
void requestReview();




#endif
