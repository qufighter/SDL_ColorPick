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


static bool mouse_hooked = false;
static bool is_picking = false;
static bool pick_window_created=NO;

@interface ColorPickOsx : NSWindowController{

    int origionalWindowLevel;//used so when not picking we go back to our previous window level - important for when comptuer locks during sleep!

    CFMachPortRef mMachPortRef;
    CFRunLoopSourceRef mMouseEventSrc;

    NSTimer* repeatingTimer;
    
    NSOverlayWindow *mainPickView;
    NSOverlayView *mainPickSubview;
    NSWindow *main_window;

    NSWindow * mainPickViewParent;
//
//@public


}


- (void)createPickWindow;
- (void)destroyPickWindow;
- (void)togglePicking;
- (void)sendPickedColorToHistoryHelper;

@end

#endif
