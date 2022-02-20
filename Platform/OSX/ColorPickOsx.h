//
//  FileChooser.h
//  ColorPick SDL
//
//  Created by Sam Larison on 8/13/16.
//
//

// USED BY IOS(osx?) EXCLUSIVELY MOVE TO IOS FOLDER


#ifndef ColorPick_iOS_SDL_ColorPickOsx_h
#define ColorPick_iOS_SDL_ColorPickOsx_h

#include "SDL.h"

#import "NSOverlayWindow.h"
#import "NSOverlayView.h"

#include "ColorPick.h"

//#import <Cocoa/Cocoa.h>
//#import <Foundation/Foundation.h>
//#import <ApplicationServices/ApplicationServices.h>
//#import <IOKit/IOKitLib.h>
//#import <IOKit/Graphics/IOGraphicsLib.h>
//#include <CommonCrypto/CommonDigest.h>


@interface ColorPickOsx : NSObject{

    NSWindowLevel origionalWindowLevel;//used so when not picking we go back to our previous window level - important for when comptuer locks during sleep!

//    CFMachPortRef mMachPortRef;
//    CFRunLoopSourceRef mMouseEventSrc;
//    NSTimer* repeatingTimer;

    SDL_TimerID sdlTimerId;

    NSOverlayWindow *mainPickView;
    NSOverlayView *mainPickSubview;
    NSWindow *main_window;

//    NSWindow * mainPickViewParent;

    bool pick_window_created;
    bool mouse_hooked;

@public

    bool is_picking;
}

- (void)takeScreenshot;
- (void)createPickWindow;
- (void)destroyPickWindow;
- (void)togglePicking;
- (void)processKeyboard:(int)keyCode;

@end

#endif
