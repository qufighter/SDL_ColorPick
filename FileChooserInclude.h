//
//  ColorPick.h
//  ___PROJECTNAME___
//
//  Created by Sam Larison on 8/24/15.
//
//


#ifndef _____PROJECTNAME_nixfilechooserinclude_____ColorPick__
#define _____PROJECTNAME_nixfilechooserinclude_____ColorPick__

#if __ANDROID__
#include "Platform/Android/AndroidFileChooser.h" // platform specific include!
#elif __EMSCRIPTEN__
#include "Platform/Emscripten/EmscriptenFileChooser.h" // platform specific include!
#elif __LINUX__
#include "Platform/LINUX/LinuxFileChooser.h" // platform specific include!
#else
#include "FileChooser.h" // platform specific include!  this is for ios (and osx...)
#endif

#endif /* defined(_____PROJECTNAME_filechooserinclude_____ColorPick__) */
