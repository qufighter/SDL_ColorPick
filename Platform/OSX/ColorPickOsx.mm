
#import "ColorPickOsx.h"

#import <Cocoa/Cocoa.h>

#import "SDL_syswm.h"

@implementation ColorPickOsx

//- (NSDictionary *)userInfo {
//    return [NSDictionary dictionaryWithObject:[NSDate date] forKey:@"StartDate"];
//}

// Once we have our image, we need to get it into an SDL_Surface
static SDL_Surface* Create_SDL_Surface_From_CGImage_RGB(CGImageRef image_ref)
{
    /* This code is adapted from Apple's Documentation found here:
     * http://developer.apple.com/documentation/GraphicsImaging/Conceptual/OpenGL-MacProgGuide/index.html
     * Listing 9-4††Using a Quartz image as a texture source.
     * Unfortunately, this guide doesn't show what to do about
     * non-RGBA image formats so I'm making the rest up.
     * All this code should be scrutinized.
     */

    size_t w = CGImageGetWidth(image_ref);
    size_t h = CGImageGetHeight(image_ref);
    CGRect rect = {{0, 0}, {static_cast<CGFloat>(w), static_cast<CGFloat>(h)}};

    CGImageAlphaInfo alpha = CGImageGetAlphaInfo(image_ref);
    //size_t bits_per_pixel = CGImageGetBitsPerPixel(image_ref);
    size_t bits_per_component = 8;

    SDL_Surface* surface;
    Uint32 Amask;
    Uint32 Rmask;
    Uint32 Gmask;
    Uint32 Bmask;

    CGContextRef bitmap_context;
    CGBitmapInfo bitmap_info;
    CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();

    if (alpha == kCGImageAlphaNone ||
        alpha == kCGImageAlphaNoneSkipFirst ||
        alpha == kCGImageAlphaPremultipliedFirst || // added
        alpha == kCGImageAlphaPremultipliedLast || // added
        alpha == kCGImageAlphaNoneSkipLast) {
        bitmap_info = kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Host; /* XRGB */
        Amask = 0x00000000;
    } else {
        /* kCGImageAlphaFirst isn't supported */
        //bitmap_info = kCGImageAlphaFirst | kCGBitmapByteOrder32Host; /* ARGB */
        if( alpha == kCGImageAlphaFirst ){
            bitmap_info = kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Host; /* ARGB */
        }else{
            bitmap_info = kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Host; /* ARGB */
        }
        Amask = 0xFF000000;
    }

    Rmask = 0x00FF0000;
    Gmask = 0x0000FF00;
    Bmask = 0x000000FF;

    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, Rmask, Gmask, Bmask, Amask);
    if (surface)
    {
        // Sets up a context to be drawn to with surface->pixels as the area to be drawn to
        bitmap_context = CGBitmapContextCreate(
                                               surface->pixels,
                                               surface->w,
                                               surface->h,
                                               bits_per_component,
                                               surface->pitch,
                                               color_space,
                                               bitmap_info
                                               );

        // Draws the image into the context's image_data
        CGContextDrawImage(bitmap_context, rect, image_ref);

        CGContextRelease(bitmap_context);

        // FIXME: Reverse the premultiplied alpha
        if ((bitmap_info & kCGBitmapAlphaInfoMask) == kCGImageAlphaPremultipliedFirst) {
            int i, j;
            Uint8 *p = (Uint8 *)surface->pixels;
            for (i = surface->h * surface->pitch/4; i--; ) {
#if __LITTLE_ENDIAN__
                Uint8 A = p[3];
                if (A) {
                    for (j = 0; j < 3; ++j) {
                        p[j] = (p[j] * 255) / A;
                    }
                }
#else
                Uint8 A = p[0];
                if (A) {
                    for (j = 1; j < 4; ++j) {
                        p[j] = (p[j] * 255) / A;
                    }
                }
#endif /* ENDIAN */
                p += 4;
            }
        }
    }

    if (color_space)
    {
        CGColorSpaceRelease(color_space);
    }

    return surface;
}
static SDL_Surface* Create_SDL_Surface_From_CGImage_Index(CGImageRef image_ref)
{
    size_t w = CGImageGetWidth(image_ref);
    size_t h = CGImageGetHeight(image_ref);
    size_t bits_per_pixel = CGImageGetBitsPerPixel(image_ref);
    size_t bytes_per_row = CGImageGetBytesPerRow(image_ref);

    SDL_Surface* surface;
    SDL_Palette* palette;
    CGColorSpaceRef color_space = CGImageGetColorSpace(image_ref);
    CGColorSpaceRef base_color_space = CGColorSpaceGetBaseColorSpace(color_space);
    size_t num_components = CGColorSpaceGetNumberOfComponents(base_color_space);
    size_t num_entries = CGColorSpaceGetColorTableCount(color_space);
    uint8_t *entry, entries[num_components * num_entries];

    /* What do we do if it's not RGB? */
    if (num_components != 3) {
        SDL_SetError("Unknown colorspace components %lu", num_components);
        return NULL;
    }
    if (bits_per_pixel != 8) {
        SDL_SetError("Unknown bits_per_pixel %lu", bits_per_pixel);
        return NULL;
    }

    CGColorSpaceGetColorTable(color_space, entries);
    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, bits_per_pixel, 0, 0, 0, 0);
    if (surface) {
        uint8_t* pixels = (uint8_t*)surface->pixels;
        CGDataProviderRef provider = CGImageGetDataProvider(image_ref);
        NSData* data = (id)CFBridgingRelease(CGDataProviderCopyData(provider));
        //[data autorelease];
        const uint8_t* bytes = (const unsigned char*)[data bytes];
        size_t i;

        palette = surface->format->palette;
        for (i = 0, entry = entries; i < num_entries; ++i) {
            palette->colors[i].r = entry[0];
            palette->colors[i].g = entry[1];
            palette->colors[i].b = entry[2];
            entry += num_components;
        }

        for (i = 0; i < h; ++i) {
            SDL_memcpy(pixels, bytes, w);
            pixels += surface->pitch;
            bytes += bytes_per_row;
        }
    }
    return surface;
}
// todo export this?
static SDL_Surface* Create_SDL_Surface_From_CGImage(CGImageRef image_ref)
{
    CGColorSpaceRef color_space = CGImageGetColorSpace(image_ref);
    CGColorSpaceModel color_space_model = CGColorSpaceGetModel(color_space);

    if (color_space_model == kCGColorSpaceModelIndexed) {
        return Create_SDL_Surface_From_CGImage_Index(image_ref);
    } else {
        return Create_SDL_Surface_From_CGImage_RGB(image_ref);
    }
}



- (void)takeScreenshot {
    //int h = CGDisplayPixelsHigh(kCGDirectMainDisplay); //(useful for takings screenshot since it accepts corrd from top left of main display)

    NSRect screenRect;
    screenRect = [[NSScreen mainScreen] frame];

    NSArray* allscreens = [NSScreen screens];
    if([allscreens count] > 1){
        for(NSScreen* s in allscreens)
            screenRect = NSUnionRect(screenRect, [s frame]);
    }

    CGRect grect=CGRectMake(screenRect.origin.x,screenRect.origin.y,screenRect.size.width,screenRect.size.height);

    CGImageRef img = CGDisplayCreateImageForRect(kCGDirectMainDisplay,grect);

    // ios code? (included above!)
    SDL_Surface *myCoolSurface = Create_SDL_Surface_From_CGImage(img);

    OpenGLContext* openglContext = OpenGLContext::Singleton();
	// by passing 0,0 we will ensure that we get the right snap step... trust me...  without it, there are some issues retrunign to pick mode, or even panning quick and ending up in the wrong place
	// there are some alternate soltuions, to try to get the CORRECT mouse position (eg maybe we could pass in screen coord of the click that triggered this function call...)

    SDL_Point gm_result = {0,0};
    SDL_GetGlobalMouseState(&gm_result.x, &gm_result.y);
    openglContext->imageWasSelectedCb(myCoolSurface, false, gm_result.x, gm_result.y);

    CGImageRelease(img);
}


static Uint32 color_pick_osx_timer_fire(Uint32 interval, void* parm){
    NSPoint mloc = [NSEvent mouseLocation];


    //SDL_Log("%f %f", mloc.x, mloc.y);

    OpenGLContext* openglContext = OpenGLContext::Singleton();
    ColorPickState* colorPickState = ColorPickState::Singleton();

    openglContext->position_x =(openglContext->fullPickImgSurface->clip_rect.w - (int)mloc.x) - (openglContext->fullPickImgSurface->clip_rect.w / 2);
    openglContext->position_y =((int)mloc.y - openglContext->fullPickImgSurface->clip_rect.h) + (openglContext->fullPickImgSurface->clip_rect.h / 2);

    // todo: note; theory; all 3 platforms suffer from the fact that this is occuring in another thread...
    // which means our position_x  position_y updates may or may not be consumed, or may change mid move...
    // as a better approach lets 1) push an event 2) let those be consumed by the main thread

    colorPickState->movedxory = true;
    openglContext->renderShouldUpdate = true; // do not call renderScene from timer thread!


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

    return interval; // keeps timer alive!
}


//CGEventRef myLowLevelMouseListener (CGEventTapProxy proxy,CGEventType type,CGEventRef event,void *refcon)
//{
//    ColorPickOsx* aself;
//    aself = CFBridgingRelease(refcon);
//
//    SDL_Log("evtype: %i", type);
//    if(type == NX_LMOUSEDOWN){
//        //perhaps slow mouse movement
//        if(aself->is_picking){
//            //NSLog(@"mousedown");
//            [(ColorPickOsx*)CFBridgingRelease(refcon) togglePicking];
//            return CGEventCreate(NULL);
//        }else{
//            //NSLog(@"mousedown NOT PICKING");//does not get here
//            return event;
//        }
//    }else if(type == NX_LMOUSEUP){
//        //dont forget to disble assistive devices!
//        //[(ColorPick*)refcon setOutHex:@"u clicked admit it!"];
//        //if(is_picking){
//        //    NSLog(@"mouseup");
//        //    [(ColorPickOsx*)refcon togglePicking];
//            return CGEventCreate(NULL);
//        //}else{
//            //CGEventTapEnable(mMachPortRef, false);//should never get here...
//        //    NSLog(@"mouseup NOT PICKING");
//            return event;
//        //}
//    }else if(type == NX_SCROLLWHEELMOVED){
//        SDL_Log("got the mouse wheel");
//        if(aself->is_picking){
//            SDL_Log("got the mouse wheel is_picking" );
//            OpenGLContext* openglContext = OpenGLContext::Singleton();
//            if( CGEventGetIntegerValueField(event, kCGScrollWheelEventDeltaAxis1) > 0){
//                openglContext->setFishScale(1.0, 1.10f);
//            }else{
//                openglContext->setFishScale(-1.0, 1.10f);
//            }
//        }
//
//        return event;
//    }else if(type == kCGEventTapDisabledByTimeout || type == kCGEventTapDisabledByUserInput){
//        //re enable?
//        return event;
//    }else
//        return event;
//    return event;
//}

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
        [self processKeyboard:[mainPickView getLastKeycode]];

    }else if([keyPath isEqualToString:@"shield_scrollywheel"]){
        if( [mainPickSubview getLastScrollDir] > 0 ){
            openglContext->setFishScale(1.0, 1.10f);
        }else{
            openglContext->setFishScale(-1.0, 1.10f);
        }
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

    // all this for scroll wheel? we found a better way...
//    //https://developer.apple.com/library/mac/#documentation/Carbon/Reference/QuartzEventServicesRef/Reference/reference.html#//apple_ref/c/func/CGEventTapCreate
//    mMachPortRef=CGEventTapCreate( kCGAnnotatedSessionEventTap, //kCGHIDEventTap, //crashes vm?
//                                  kCGHeadInsertEventTap,
//                                  kCGEventTapOptionDefault,
//                                  CGEventMaskBit(kCGEventLeftMouseDown) | CGEventMaskBit(kCGEventLeftMouseUp) | CGEventMaskBit(kCGEventScrollWheel),// | CGEventMaskBit(kCGEventMouseMoved),
//                                  (CGEventTapCallBack)myLowLevelMouseListener,
//                                  (void*)self ); //(void*)wmInfo.info.cocoa.window);
//
//    if(!mMachPortRef){
//        mouse_hooked=false;
//        SDL_Log("MOUSE HOOK -> FALSE");
//    }else{
//        mouse_hooked=true;
//        mMouseEventSrc = CFMachPortCreateRunLoopSource(NULL, mMachPortRef, 0);
//        if ( !mMouseEventSrc ){
//            mouse_hooked=false;
//        }else{
//            CFRunLoopAddSource([[NSRunLoop currentRunLoop] getCFRunLoop],  mMouseEventSrc, kCFRunLoopDefaultMode);
//            CGEventTapEnable(mMachPortRef, false);
//        }
//    }



    return self;
}

- (void)togglePicking
{
    if(is_picking){

        SDL_RemoveTimer(sdlTimerId);

        //if(mouse_hooked) CGEventTapEnable(mMachPortRef, false);

        [self destroyPickWindow];

        is_picking=NO;

    }else{

        [self createPickWindow];

         //SUCCESS?!
        //if(mouse_hooked) CGEventTapEnable(mMachPortRef, true);

        sdlTimerId = SDL_AddTimer(30, color_pick_osx_timer_fire, self);

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
    [mainPickSubview addObserver:self forKeyPath:@"shield_scrollywheel" options:0 context:NULL];


    //mainPickViewParent=[mainPickView parentWindow];


    origionalWindowLevel = [main_window level];
    //always on top (should do this on pick no matter what!)

    //[main_window setLevel:(windowLevel+1)]; // do not raise!!
}

- (void)destroyPickWindow{
    if(pick_window_created){
        [mainPickSubview removeObserver:self forKeyPath:@"shield_mouseup"];
        [mainPickSubview removeObserver:self forKeyPath:@"shield_scrollywheel"];
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

        OpenGLContext* openglContext = OpenGLContext::Singleton();
        SDL_RaiseWindow(openglContext->getSdlWindow());
    }
    pick_window_created=NO;
    //[main_window setLevel:(CGShieldingWindowLevel())];
}

static Uint32 pick_again_soon(Uint32 interval, void* parm) {
    // to make just this timer, fire on main thread - push event!
    SDL_Event event;
    SDL_UserEvent userevent;
    userevent.type = SDL_USEREVENT;
    userevent.code = USER_EVENT_ENUM::PICK_AGAIN_NOW;
    event.type = SDL_USEREVENT;
    event.user = userevent;
    SDL_PushEvent(&event);
    return 0;
}

- (void)processKeyboard:(int)keyCode{
    switch( keyCode ) {
            //THE arrow keys should MODIFY the latest (top most) history entry !!!!!!!!!!!!!!!!!!!!!!!!!
        case 126:       // up arrow
            //[self translatPosition:NSMakePoint(0,arrowKeyMoveAmt)];
            break;
        case 125:       // down arrow
            //[self translatPosition:NSMakePoint(0,-arrowKeyMoveAmt)];
            break;
        case 124:       // right arrow
            //[self translatPosition:NSMakePoint(arrowKeyMoveAmt,0)];
            break;
        case 123:       // left arrow
            //[self translatPosition:NSMakePoint(-arrowKeyMoveAmt,0)];
            break;
        case 53://esc
            if(is_picking) [self togglePicking];//NSLog(@"esc!");
            break;
        case 15://r
        case 38://j
            if( is_picking ){
                //[self destroyPickWindow];
                [self togglePicking];
                SDL_AddTimer(250, pick_again_soon, nullptr);
                //[self takeScreenshot];
                //[self createPickWindow];
                //NSLog(@"r/j!");
            }
            break;
        case 76://enter
        case 36://return
            [self togglePicking];//NSLog(@"return/enter!");
            break;
        default:
            //NSLog(@"Key pressed: %@", keyCode);
            break;
    }
}

@end
