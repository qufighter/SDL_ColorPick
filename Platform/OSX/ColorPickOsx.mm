
#import "ColorPickOsx.h"

#import <Cocoa/Cocoa.h>

#import "SDL_syswm.h"

@implementation ColorPickOsx

- (NSDictionary *)userInfo {
    return [NSDictionary dictionaryWithObject:[NSDate date] forKey:@"StartDate"];
}

- (void)timerFireMethod:(NSTimer*)theTimer {
    NSPoint mloc = [NSEvent mouseLocation];

    //SDL_Log("%f %f", mloc.x, mloc.y);

    OpenGLContext* openglContext = OpenGLContext::Singleton();
    ColorPickState* colorPickState = ColorPickState::Singleton();

    openglContext->position_x =(openglContext->fullPickImgSurface->clip_rect.w - (int)mloc.x) - (openglContext->fullPickImgSurface->clip_rect.w / 2);
    openglContext->position_y =((int)mloc.y - openglContext->fullPickImgSurface->clip_rect.h) + (openglContext->fullPickImgSurface->clip_rect.h / 2);
    colorPickState->movedxory = true;

    openglContext->renderScene();

//    mloc.x+=mouse_point_offset.x,
//    mloc.y+=mouse_point_offset.y;
//
//
//    // expeirence on non-retna follows: enable log statment named [-reporteding-]
//    // mouse location is adjusted to account for strange error in precison after visiting bottom or right edge of screen.
//    // after visitng bottom locY is reported with trailign decimal  647.003906 when position is actually 648
//    // after visiting right locX is reported with trailing decimal 1657.996094 when posiiton is actually 1657
//    // the misreporting causes the screenshot to be 257 px even though 256 are requested
//    // this - somewhat - shatters hopes of increasd sub-pixel accuracy on retina when image is 2x resolution requested (512x512)
//    // although we will perform rounding here in order that others may provide 0.5 increment to the capture function, the resultant
//    // snapshot of the screen will remain enlarged beyond 512x512 as soon as floats are provided to the capture function
//
//    mloc.x = floor(mloc.x);
//    mloc.y = ceil(mloc.y);

    //[self render:mloc];
}

CGEventRef myLowLevelMouseListener (CGEventTapProxy proxy,CGEventType type,CGEventRef event,void *refcon)
{
    SDL_Log("evtype: %i", type);
    if(type == NX_LMOUSEDOWN){
        //perhaps slow mouse movement
        if(is_picking){
            //NSLog(@"mousedown");
            [(ColorPickOsx*)CFBridgingRelease(refcon) togglePicking];
            return CGEventCreate(NULL);
        }else{
            //NSLog(@"mousedown NOT PICKING");//does not get here
            return event;
        }
    }else if(type == NX_LMOUSEUP){
        //dont forget to disble assistive devices!
        //[(ColorPick*)refcon setOutHex:@"u clicked admit it!"];
        //if(is_picking){
        //    NSLog(@"mouseup");
        //    [(ColorPickOsx*)refcon togglePicking];
            return CGEventCreate(NULL);
        //}else{
            //CGEventTapEnable(mMachPortRef, false);//should never get here...
        //    NSLog(@"mouseup NOT PICKING");
            return event;
        //}
    }else if(type == NX_SCROLLWHEELMOVED){
        SDL_Log("got the mouse wheel");
        if(is_picking){
            SDL_Log("got the mouse wheel is_picking" );
            OpenGLContext* openglContext = OpenGLContext::Singleton();
            if( CGEventGetIntegerValueField(event, kCGScrollWheelEventDeltaAxis1) > 0){
                openglContext->setFishScale(1.0, 1.10f);
            }else{
                openglContext->setFishScale(-1.0, 1.10f);
            }
        }

        return event;
    }else if(type == kCGEventTapDisabledByTimeout || type == kCGEventTapDisabledByUserInput){
        //re enable?
        return event;
    }else
        return event;
    return event;
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{

    if ([keyPath isEqualToString:@"panning"]){
//        NSPoint mv = [pick_zoom_view getMouseMovement];
//        //NSPoint newLoc = NSMakePoint(lkMouseLocation.x + round(mv.x / 4),lkMouseLocation.y + round(mv.y / 4));
//        // retina guess - we want to make the point move 0.5 - this may have implications on non retina especially Y direction
//        NSPoint newLoc = NSMakePoint(lkMouseLocation.x + (round(mv.x) * 0.5),lkMouseLocation.y + (round(mv.y) * 0.5));
//        if([self isMousePosOnScreenRect:newLoc]){
//            lkMouseLocation=newLoc;
//        }
        //NSLog([NSString stringWithFormat:@"lk mouse: %d %d", lkMouseLocation.x, lkMouseLocation.y]);
//        [self render:lkMouseLocation];
    }else if([keyPath isEqualToString:@"panning_begin"]){
//        if( [pick_zoom_view getIsPanning] ){
//            [self sendPickedColorToHistory];
//        }else{
//            [self sendPickedColorToHistoryHelper];
//        }
    }else if([keyPath isEqualToString:@"keyboard"]){
//        [self processKeyboard:[main_window getLastKeycode]];
    }else if([keyPath isEqualToString:@"shield_keyboard"]){
//        [self processKeyboard:[mainPickView getLastKeycode]];
    }else if([keyPath isEqualToString:@"shield_mouseup"]){
        if(is_picking) [self togglePicking];

        OpenGLContext* openglContext = OpenGLContext::Singleton();
        openglContext->generalUx->addCurrentToPickHistory();

    }else if([keyPath isEqualToString:@"color"]){
//        [self setCurrentColor:[color_current color]];
    }
}

- (id) init
{

    self = [super init];

    //mouse_point_offset=NSMakePoint(1,-1);

    pick_window_created=NO;
    is_picking=NO;
    mouse_hooked=NO;

    //pasteBoard = [NSPasteboard generalPasteboard];


    //computeFishEye();
    //[self setFishEye:fishEye];


    OpenGLContext* openglContext = OpenGLContext::Singleton();

    SDL_SysWMinfo wmInfo;

    SDL_GetWindowWMInfo(openglContext->sdlWindow, &wmInfo);

    main_window = wmInfo.info.cocoa.window;

    //https://developer.apple.com/library/mac/#documentation/Carbon/Reference/QuartzEventServicesRef/Reference/reference.html#//apple_ref/c/func/CGEventTapCreate
    mMachPortRef=CGEventTapCreate( kCGAnnotatedSessionEventTap, //kCGHIDEventTap, //crashes vm?
                                  kCGHeadInsertEventTap,
                                  kCGEventTapOptionDefault,
                                  CGEventMaskBit(kCGEventLeftMouseDown) | CGEventMaskBit(kCGEventLeftMouseUp) | CGEventMaskBit(kCGEventScrollWheel),// | CGEventMaskBit(kCGEventMouseMoved),
                                  (CGEventTapCallBack)myLowLevelMouseListener,
                                  (void*)main_window ); //(void*)wmInfo.info.cocoa.window);

    if(!mMachPortRef){
        mouse_hooked=false;
        SDL_Log("MOUSE HOOK -> FALSE");
    }else{
        mouse_hooked=true;
        mMouseEventSrc = CFMachPortCreateRunLoopSource(NULL, mMachPortRef, 0);
        if ( !mMouseEventSrc ){
            mouse_hooked=false;
        }else{
            CFRunLoopAddSource([[NSRunLoop currentRunLoop] getCFRunLoop],  mMouseEventSrc, kCFRunLoopDefaultMode);
            CGEventTapEnable(mMachPortRef, false);
        }
    }



    return self;
}

- (void)togglePicking
{
    if(is_picking){

        [repeatingTimer invalidate];

        if(mouse_hooked) CGEventTapEnable(mMachPortRef, false);

        [self destroyPickWindow];

        //CFRelease( mMachPortRef );
        //CFRelease( mMouseEventSrc );

        is_picking=NO;

        [self sendPickedColorToHistoryHelper];
    }else{

        //[self sendPickedColorToHistory];
        //[color_previous setColor:[color_current color]];

        [self createPickWindow];

         //SUCCESS?!
        if(mouse_hooked) CGEventTapEnable(mMachPortRef, true);

        //[out_hex setStringValue:(@"Hi")];
        //[self render:[NSEvent mouseLocation]];

        repeatingTimer = [NSTimer scheduledTimerWithTimeInterval:0.033
                                                          target:self selector:@selector(timerFireMethod:)
                                                        userInfo:[self userInfo] repeats:YES];
        //self.repeatingTimer = timer;
        is_picking = YES;
    }
}

- (void)createPickWindow{

    if( pick_window_created ){
        return;
    }

    int windowLevel;
    NSRect screenRect;

    /* //good for fullscreen games (though not all displays)
     // Capture the main display
     if (CGCaptureAllDisplays() != kCGErrorSuccess) {
     NSLog( @"Couldn't capture the main display!" );
     // Note: you'll probably want to display a proper error dialog here
     }*/

    // Get the shielding window level
    windowLevel = CGShieldingWindowLevel();
    // Get the screen rect of our main display
    screenRect = [[NSScreen mainScreen] frame];

    NSArray* allscreens = [NSScreen screens];
    if([allscreens count] > 1){
        for(NSScreen* s in allscreens)
            screenRect = NSUnionRect(screenRect, [s frame]);
    }

    // Put up a new window

//    if( mainPickView!= nullptr) [mainPickView release];
//    if( mainPickSubview!= nullptr) [mainPickSubview release];

    mainPickView = [[NSOverlayWindow alloc] initWithContentRect:screenRect
                                              styleMask:NSBorderlessWindowMask
                                                    backing:NSBackingStoreBuffered
                                                      defer:NO screen:[NSScreen mainScreen]];

    mainPickSubview = [[NSOverlayView alloc] initWithFrame:NSZeroRect];

    [[mainPickView contentView] addSubview:mainPickSubview];
    [mainPickView setContentView:mainPickSubview];



    // Create the subview


    [mainPickView setAlphaValue:0.0];
    /*
    [mainPickView setIgnoresMouseEvents:NO];

    [mainPickView setLevel:windowLevel];
    //[mainPickView setBackgroundColor:[NSColor blackColor]];
    [mainPickView makeKeyAndOrderFront:nil];
    */

    pick_window_created=YES;

    [mainPickView addObserver:self forKeyPath:@"shield_keyboard" options:0 context:NULL];
    [mainPickSubview addObserver:self forKeyPath:@"shield_mouseup" options:0 context:NULL];




    //mainPickViewParent=[mainPickView parentWindow];


    origionalWindowLevel = [main_window level];
    //always on top (should do this on pick no matter what!)

    //[main_window setLevel:(windowLevel+1)]; // do not raise!!
}

- (void)destroyPickWindow{
    if(pick_window_created){
        [mainPickSubview removeObserver:self forKeyPath:@"shield_mouseup"];
        [mainPickView removeObserver:self forKeyPath:@"shield_keyboard"];



        [mainPickView orderOut:self];

        [mainPickSubview releaseResources];
        //[mainPickSubview release];  //should be released by teh following...
        //[mainPickView releaseResources];



        [mainPickView close];
        //[mainPickView release];

        //[mainPickView setIsVisible:false];

        //mainPickView = nullptr; // causes error!
        

        [main_window setLevel:(origionalWindowLevel)];
    }
    pick_window_created=NO;
    //[main_window setLevel:(CGShieldingWindowLevel())];
}

- (void)sendPickedColorToHistoryHelper{
//    int newr=(int)round(255*[[color_current color] redComponent]);
//    int newg=(int)round(255*[[color_current color] greenComponent]);
//    int newb=(int)round(255*[[color_current color] blueComponent]);
//
//    [history_data insertObject:[NSString stringWithFormat:@"%d,%d,%d", newr, newg, newb] atIndex:0];
//    [history_list reloadData];
//
//    NSIndexSet *indexSet = [NSIndexSet indexSetWithIndex:0];
//    [history_list selectRowIndexes:indexSet byExtendingSelection:NO];

}

@end
