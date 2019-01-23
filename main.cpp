/*
 *  rectangles.c
 *  written by Holmes Futrell
 *  use however you want
 */

#include "main.h"
#include "ColorPick.h" // includes openglContext
#include <time.h>



void DebugStr(const char *c_str ) {
    SDL_Log("%s", c_str);
}

void debugGLerror(){
    debugGLerror("default-error-identifier");
}

void debugGLerror(const char *c_str_label){
#if defined(_DEBUG) || defined(DEBUG) || \
(defined(__GNUC__) && !defined(__OPTIMIZE__))
    GLenum errorno = 1;
    while( errorno != GL_NO_ERROR && errorno ){
        errorno = glGetError();
        if( errorno != GL_NO_ERROR ){
            //SDL_Log("GL Error %d Labeled: %s", errorno, c_str_label);
            switch(errorno){
                case GL_INVALID_ENUM:
                    SDL_Log("GL-Error GL_INVALID_ENUM %s", c_str_label);
                    break;
                case GL_INVALID_VALUE:
                    SDL_Log("GL-Error GL_INVALID_VALUE %s", c_str_label);
                    break;
                case GL_INVALID_OPERATION:
                    SDL_Log("GL-Error GL_INVALID_OPERATION %s", c_str_label);
                    break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:
                    SDL_Log("GL-Error GL_INVALID_FRAMEBUFFER_OPERATION %s", c_str_label);
                    break;
                case GL_OUT_OF_MEMORY:
                    SDL_Log("GL-Error GL_OUT_OF_MEMORY %s", c_str_label);
                    break;
//                case GL_STACK_UNDERFLOW:
//                    SDL_Log("GL-Error GL_STACK_UNDERFLOW %s", c_str_label);
//                    break;
//                case GL_STACK_OVERFLOW:
//                    SDL_Log("GL-Error GL_STACK_OVERFLOW %s", c_str_label);
//                    break;
                default:
                    SDL_Log("GL-Error Open GL Error %d: Ohter error... %s", errorno, c_str_label);
                    break;
            }
        }
    }
#endif
}


int
randomInt(int min, int max)
{
    return min + rand() % (max - min + 1);
}

void
render(SDL_Renderer *renderer) // DEFAULT demo using sdl render, delete me
{

    Uint8 r, g, b;

    /* Clear the screen */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    /*  Come up with a random rectangle */
    SDL_Rect rect;
    rect.w = randomInt(64, 128);
    rect.h = randomInt(64, 128);
    rect.x = randomInt(0, SCREEN_WIDTH);
    rect.y = randomInt(0, SCREEN_HEIGHT);

    /* Come up with a random color */
    r = randomInt(50, 255);
    g = randomInt(50, 255);
    b = randomInt(50, 255);
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    /*  Fill the rectangle in the color */
    SDL_RenderFillRect(renderer, &rect);

    /* update screen */
    SDL_RenderPresent(renderer);
}

typedef void (*voidvoidp)(void *someParam);

int fileDropsAllowed = 0;
int mousStateDown = 0; // really should count fingers down possibly, finger event support multi touch, mosue events dont!
bool multiTouchMode = false;
int tx,ty;

/* forward declarations */
void mouseDownEvent(SDL_Event* event);
void mouseMoveEvent(SDL_Event* event);
void mouseUpEvent(SDL_Event* event);

void beginInteraction(bool isStart){
    SDL_GetMouseState(&tx, &ty);
    //SDL_Log("MOUSE xy %d %d", tx,ty);
    openglContext->pixelInteraction.begin(tx, ty);
    openglContext->generalUx->currentInteraction.begin( (tx*ui_mmv_scale)/win_w, (ty*ui_mmv_scale)/win_h );
    //SDL_Log("MOUSE xy perc %f %f", openglContext->generalUx->currentInteraction.px, openglContext->generalUx->currentInteraction.py );
    didInteract = openglContext->generalUx->triggerInteraction(isStart);
}

void mouseDownEvent(SDL_Event* event){
    mousStateDown += 1;
    //SDL_Log("finger count %i", event->tfinger.);
    if( mousStateDown > 1 ){
        //second touch....
        SDL_Log("SDL_FINGERDOWN second touch");
        mousStateDown--; // doing this so mouseUpEvent has an effect....
        mouseUpEvent(event); // lets treat it as if they stopped interacting....
        //mousStateDown++;
        didInteract=false;
        return;
    }
    openglContext->clearVelocity(); // stop any active panning
    //SDL_SetRelativeMouseMode(SDL_TRUE);
    //SDL_GetRelativeMouseState(&colorPickState->mmovex, &colorPickState->mmovey);
    // SDL_TouchFingerEvent event->tfinger
    //SDL_GetRelativeMouseState(&tx, &ty);
    beginInteraction(true);
    openglContext->renderShouldUpdate = true; // android??
}

void mouseMoveEvent(SDL_Event* event){
    if( mousStateDown == 1 ){
        //SDL_Log("mousStateDown SDL_FINGERMOTION SDL_MOUSEMOTION");
        if( !didInteract ){
            //SDL_GetRelativeMouseState(&colorPickState->mmovex, &colorPickState->mmovey);
#if __ANDROID__
            bool wasZero = openglContext->pixelInteraction.isZeroed();  // on android this event won't fire right away - in fact it takes quite a LOT of movement to reach SDL_FINGERMOTION here....
#endif
            SDL_GetMouseState(&tx, &ty);
            //SDL_Log("MOUSE xy %d %d", tx,ty);
            openglContext->pixelInteraction.update(tx, ty);
#if __ANDROID__

            if( wasZero ){
                if( openglContext->pixelInteraction.rx > 1.0 ){
                    openglContext->pixelInteraction.rx = 1.0;
                }else if( openglContext->pixelInteraction.rx < -1.0 ){
                    openglContext->pixelInteraction.rx = -1.0;
                }
                if( openglContext->pixelInteraction.ry > 1.0 ){
                    openglContext->pixelInteraction.ry = 1.0;
                }else if( openglContext->pixelInteraction.ry < -1.0 ){
                    openglContext->pixelInteraction.ry = -1.0;
                }
            }
#endif
            openglContext->triggerMovement();
        }else{
            //colorPickState->mmovex = event->motion.xrel;
            //colorPickState->mmovey = event->motion.yrel;
            SDL_GetMouseState(&tx, &ty);
            //SDL_Log("MOUSE xy %d %d", tx,ty);
            openglContext->generalUx->currentInteraction.update((tx*ui_mmv_scale)/win_w, (ty*ui_mmv_scale)/win_h);
            //SDL_Log("MOUSE xy perc %f %f", openglContext->generalUx->currentInteraction.px, openglContext->generalUx->currentInteraction.py );
            //SDL_Log("MOUSE xy delta %f %f", openglContext->generalUx->currentInteraction.dx, openglContext->generalUx->currentInteraction.dy );
            // todo combine above into the following call?
            openglContext->generalUx->interactionUpdate(&openglContext->generalUx->currentInteraction); // todo whyarg, BETTERARG
            // todo the above would appear to return a "should update??"
            openglContext->renderShouldUpdate = true; /// TODO interactionUpdate calls generalUx->updateRenderPositions() should cause the UI to update... for now we just redraw everything
        }
    }
}

void mouseUpEvent(SDL_Event* event){
    if( didInteract ){
        // we may be able to add this, but we need to track velocity better
        SDL_GetMouseState(&tx, &ty);
        //SDL_Log("MOUSE xy %d %d", tx,ty);
        openglContext->generalUx->currentInteraction.update((tx*ui_mmv_scale)/win_w, (ty*ui_mmv_scale)/win_h);
        //SDL_Log("MOUSE xy perc %f %f", openglContext->generalUx->currentInteraction.px, openglContext->generalUx->currentInteraction.py );
        //SDL_Log("MOUSE xy delta %f %f", openglContext->generalUx->currentInteraction.dx, openglContext->generalUx->currentInteraction.dy );
    }else{
        SDL_GetMouseState(&tx, &ty);
        //SDL_Log("MOUSE xy %d %d", tx,ty);
        openglContext->pixelInteraction.done(tx, ty );

    }
    if( mousStateDown == 1 && openglContext->pixelInteraction.dx == 0 && openglContext->pixelInteraction.dy == 0 ){
        // it was a single touch
        //SDL_Log("SDL_FINGERUP was reachd - position did not change" );
        //SDL_Log("MOUSE xy delta %f %f", openglContext->pixelInteraction.dx, openglContext->pixelInteraction.dy );
        // so if we already didInteract....
        // calling triggerInteraction again might change our interaction object....
        // which could be good or mediocure if we are currently doing a drag and drop
        // but in this case we don't want to overwrite our interaction if we already have one

        // I do not think this following code is needed....
        if( !didInteract ){ // didInteract really means didInteractWithUi and when we are, we leave the collected_x and collected_y zero at this time....
            didInteract = openglContext->generalUx->triggerInteraction();
        }

        // didInteract = openglContext->generalUx->triggerInteraction() || didInteract; /// !!! collexted x + y is zero
        // we may really test for this outside of the condition above
        // and determine mouse movement using the openglContext->generalUx->currentInteraction
        // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        // TODO the following may be important for swipey events since fingerup will have velocity or not????
        // todo openglContext->generalUx->currentInteraction.update not called anywhere here ! (except we are now, rbove)
        // maybe its okay?
        if( ! didInteract ){
            // we have missed the UI and BG clicked oh now what
            // this breaks multigesture somehow
            // this feature will move
        }else{
            //
            //openglContext->generalUx->interactionUpdate(uiInteraction *delta);
        }
    }else{
        openglContext->triggerVelocity(openglContext->pixelInteraction.vx, openglContext->pixelInteraction.vy); // args unused....
    }

    if( mousStateDown == 1 ){
        didInteract = false;
        didInteract = openglContext->generalUx->interactionComplete(&openglContext->generalUx->currentInteraction); // MOUSEUP
        // at this point we culd really need to render an update
        if( didInteract ){
            //SDL_Log("MARKED FOR UPDATE - WILL IT RENDER????");
            openglContext->renderShouldUpdate=true;
            // this seems to not guarantee render?!
        }
    }
    didInteract = false;
    mousStateDown = 0; // because each finger being released will reach this block... we need to avoid the last finger from being treated as click still...
    //            mousStateDown -= 1;
    //            if( mousStateDown < 0 ){
    //                mousStateDown=0;
    //            }
    //SDL_SetRelativeMouseMode(SDL_FALSE); // bit unsure if this mouse stuff will work for touch
    //SDL_GetRelativeMouseState(&colorPickState->mmovex, &colorPickState->mmovey);
}


int EventFilter(void* userdata, SDL_Event* event){
    switch ( event->type ){
        case SDL_WINDOWEVENT:
        {

//            SDL_WINDOWEVENT_NONE,           /**< Never used */
//            SDL_WINDOWEVENT_SHOWN,          /**< Window has been shown */
//            SDL_WINDOWEVENT_HIDDEN,         /**< Window has been hidden */
//            SDL_WINDOWEVENT_EXPOSED,        /**< Window has been exposed and should be
//                                             redrawn */
//            SDL_WINDOWEVENT_MOVED,          /**< Window has been moved to data1, data2
//                                             */
//            SDL_WINDOWEVENT_RESIZED,        /**< Window has been resized to data1xdata2 */
//            SDL_WINDOWEVENT_SIZE_CHANGED,   /**< The window size has changed, either as
//                                             a result of an API call or through the
//                                             system or user changing the window size. */
//            SDL_WINDOWEVENT_MINIMIZED,      /**< Window has been minimized */
//            SDL_WINDOWEVENT_MAXIMIZED,      /**< Window has been maximized */
//            SDL_WINDOWEVENT_RESTORED,       /**< Window has been restored to normal size
//                                             and position */
//            SDL_WINDOWEVENT_ENTER,          /**< Window has gained mouse focus */
//            SDL_WINDOWEVENT_LEAVE,          /**< Window has lost mouse focus */
//            SDL_WINDOWEVENT_FOCUS_GAINED,   /**< Window has gained keyboard focus */
//            SDL_WINDOWEVENT_FOCUS_LOST,     /**< Window has lost keyboard focus */
//            SDL_WINDOWEVENT_CLOSE,          /**< The window manager requests that the window be closed */
//            SDL_WINDOWEVENT_TAKE_FOCUS,     /**< Window is being offered a focus (should SetWindowInputFocus() on itself or a subwindow, or ignore) */
//            SDL_WINDOWEVENT_HIT_TEST        /**< Window had a hit test that wasn't SDL_HITTEST_NORMAL. */

            switch( event->window.event ){
                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    SDL_Log("\n SDL_WINDOWEVENT_RESIZED || SDL_WINDOWEVENT_SIZE_CHANGED");

                    //SDL_Log("RESIZED (lowdpi size wouldbe) %d %d", event->window.data1,event->window.data2);
                    ReshapeWindow(); // on android this needs to occur occur in the main thread.. this call will fire a subsequent event in the main thread...
                    return 0;
                case SDL_WINDOWEVENT_MINIMIZED:
                case SDL_WINDOWEVENT_HIDDEN:

                    SDL_Log("\n SDL_WINDOWEVENT_MINIMIZED || SDL_WINDOWEVENT_HIDDEN");

                    return 0;
                case SDL_WINDOWEVENT_SHOWN:
                case SDL_WINDOWEVENT_EXPOSED:
                case SDL_WINDOWEVENT_RESTORED:

                    SDL_Log("\n SDL_WINDOWEVENT_SHOWN || SDL_WINDOWEVENT_EXPOSED || SDL_WINDOWEVENT_RESTORED");
                    openglContext->renderShouldUpdate = true;

                    return 0;
            }
            return 0;
        }
#ifdef COLORPICK_PLATFORM_DESKTOP
        case SDL_MOUSEWHEEL:
            //SDL_Log("Hello Wheel!!");

            if( openglContext->generalUx->hasCurrentModal() ){

                // we may need to see what item is under our pointer....
                // only trouble comes if we are already clicked when scrolling ?

                beginInteraction(false);  // we can see if the last interaction is complete first????
                openglContext->generalUx->wheelOrPinchInteraction(event->wheel.y);

            }else{
                openglContext->clearVelocity();
                openglContext->renderShouldUpdate = true;
                openglContext->setFishScale(event->wheel.y, 0.10f);
            }

            return 0;
#endif
        case SDL_MULTIGESTURE: // http://lazyfoo.net/tutorials/SDL/55_multitouch/index.php
            //SDL_Log("Hello Gesture!!");

            openglContext->clearVelocity();
            openglContext->renderShouldUpdate = true;
            openglContext->setFishScale(event->mgesture.dDist, 40.0f);

            // we'll still get a SDL_FINGERDOWN and fingerup for each finger...

            return 0;


#ifndef COLORPICK_PLATFORM_DESKTOP
        case SDL_FINGERDOWN:
#else
        case SDL_MOUSEBUTTONDOWN: // if platform desktop?  duplicate
#endif
            SDL_Log("SDL_FINGERDOWN SDL_MOUSEBUTTONDOWN");
            mouseDownEvent(event);
            return 0;
#ifndef COLORPICK_PLATFORM_DESKTOP
        case SDL_FINGERMOTION:
#else
        case SDL_MOUSEMOTION:
#endif
            mouseMoveEvent(event);
            return 0;
#ifndef COLORPICK_PLATFORM_DESKTOP
        case SDL_FINGERUP: // http://lazyfoo.net/tutorials/SDL/54_touches/index.php
#else
        case SDL_MOUSEBUTTONUP:
#endif
            SDL_Log("SDL_FINGERUP SDL_MOUSEBUTTONUP");
            mouseUpEvent(event);
            return 0;
        case SDL_KEYDOWN:
            //SDL_Log("1keydown %d", event->key.keysym.sym);
            //openglContext->keyDown(event->key.keysym.sym);
            if (event->key.keysym.sym == SDLK_AC_BACK || event->key.keysym.sym == SDLK_BACKSPACE || event->key.keysym.sym == SDLK_ESCAPE){
                //SDL_Log("back/esc pressed");
                openglContext->generalUx->endCurrentModal();
            }
            if(event->key.keysym.sym == SDLK_AC_HOME){
                // nope only works on windows..... maybe android, but how to screenshot android?
                //SDL_Log("home pressed - maybe ios screenshot? time for gimicky marketing ploy");
            }
            return 0;
        case SDL_KEYUP:
            openglContext->keyUp(event->key.keysym.sym);

//            /* Keyboard events */
//            SDL_KEYDOWN        = 0x300, /**< Key pressed */
//            SDL_KEYUP,                  /**< Key released */
//            SDL_TEXTEDITING,            /**< Keyboard text editing (composition) */
//            SDL_TEXTINPUT,              /**< Keyboard text input */
//            SDL_KEYMAPCHANGED,          /**< Keymap changed due to a system event such as an
//                                         input language or keyboard layout change.


//                                         SDL_HINT_RETURN_KEY_HIDES_IME                  */
//SDL_StartTextInput
//SDL_IsTextInputActive //Return whether or not Unicode text input events are enabled.
//SDL_StopTextInput
//SDL_SetTextInputRect //used as a hint for IME and on-screen keyboard placement
//SDL_HasScreenKeyboardSupport
//SDL_IsScreenKeyboardShown(SDL_Window *window);

            return 0;
//        case SDL_MOUSEBUTTONDOWN:
//        //    mousStateDown += 1;
//            //SDL_SetRelativeMouseMode(SDL_TRUE);
//            //SDL_GetRelativeMouseState(&colorPickState->mmovex, &colorPickState->mmovey);
//
//            return 0;
//        case SDL_MOUSEMOTION:
//            // SDL_Log("Mouse Motion X,Y %d,%d", event->motion.xrel, event->motion.yrel);
//            if( mousStateDown = 1 ){
//                //SDL_GetRelativeMouseState(&colorPickState->mmovex, &colorPickState->mmovey);
//                //colorPickState->mmovex = event->motion.xrel;
//                //colorPickState->mmovey = event->motion.yrel; // still like simplicity of these, not working well once SDL_SetRelativeMouseMode(SDL_TRUE), which captures hte cursor nicely and is OK in window mode....
//            }
//            return 0;
//        case SDL_MOUSEBUTTONUP:
//        //    mousStateDown -= 1;
//            //SDL_SetRelativeMouseMode(SDL_FALSE);
//            //SDL_GetRelativeMouseState(&colorPickState->mmovex, &colorPickState->mmovey);
//
//            return 0;

        case SDL_APP_LOWMEMORY:{
            SDL_Log("Memory Warning !!!!");
            /* You will get this when your app is paused and iOS wants more memory.
             Release as much memory as possible.
             */
            return 0;
        }
        case SDL_APP_TERMINATING:{
            /* Terminate the app.
             Shut everything down before returning from this function.
             */
           // openglContext->generalUx->writeOutState();
            SDL_Log("SDL_APP_TERMINATING !!!!");
            return 0;
        }
        case SDL_APP_WILLENTERBACKGROUND:{
            /* Prepare your app to go into the background.  Stop loops, etc.
             This gets called when the user hits the home button, or gets a call.
             */
            openglContext->renderShouldUpdate = false;

            colorPickState->appInForeground = false;

            openglContext->generalUx->writeOutState();
            SDL_Log("SDL_APP_WILLENTERBACKGROUND !!!!");
            return 0;
        }
        case SDL_APP_DIDENTERBACKGROUND:{
            /* This will get called if the user accepted whatever sent your app to the background.
             If the user got a phone call and canceled it, you'll instead get an SDL_APP_DIDENTERFOREGROUND event and restart your loops.
             When you get this, you have 5 seconds to save all your state or the app will be terminated.
             Your app is NOT active at this point.
             */
            SDL_Log("SDL_APP_DIDENTERBACKGROUND !!!!");
            return 0;
        }
        case SDL_APP_WILLENTERFOREGROUND:{
            /* This call happens when your app is coming back to the foreground.
             Restore all your state here.
             */
            mousStateDown = 0; // zero finger counter
            colorPickState->appInForeground = true;
            openglContext->renderShouldUpdate = true;

            SDL_Log("SDL_APP_WILLENTERFOREGROUND !!!!");
            return 0;
        }
        case SDL_APP_DIDENTERFOREGROUND:{
            /* Restart your loops here.
             Your app is interactive and getting CPU again.
             */
            colorPickState->appInForeground = true;
            SDL_Log("SDL_APP_DIDENTERFOREGROUND !!!!");
            return 0;
        }
        case SDL_DROPBEGIN:{
            SDL_Log("SDL_DROPBEGIN !!!!");
            fileDropsAllowed=1;
            return 0;
        }
        case SDL_DROPCOMPLETE:{
            SDL_Log("SDL_DROPCOMPLETE !!!!");
            fileDropsAllowed=0;
            return 0;
        }
        case SDL_DROPFILE:{
            SDL_Log("SDL_DROPFILE !!!! %s", event->drop.file );

            if( fileDropsAllowed > 0 ){
                SDL_Log("Actually Loading: %s", event->drop.file );
                SDL_Surface *myCoolSurface = openglContext->textures->LoadSurface(event->drop.file);
                if( myCoolSurface != NULL ){ // todo something should really accept file path? openglContext->imageWasSelectedCb
                    myCoolSurface = openglContext->textures->ConvertSurface(myCoolSurface);
                    openglContext->imageWasSelectedCb(myCoolSurface);
                }
                fileDropsAllowed--;
            }
            SDL_free(event->drop.file);
            return 0;
        }
        default:

#ifndef COLORPICK_PLATFORM_DESKTOP
            if( event->type == SDL_MOUSEMOTION ){
                break; // we handled this as SDL_FINGERMOTION
            }
#endif
            // see instead (of the above) SDL_HINT_TOUCH_MOUSE_EVENTS (actually that broke android?)
            SDL_Log("unrecognized event; type %i", event->type );
            break;
            
    }
    return 1;/* No special processing, add it to the event queue */
}

void ShowFrame(void*)
{

//    currentTime = SDL_GetTicks();
//    openglContext->updateFrame(currentTime - lastTimerTime);


    //SDL_Log("RENDER SCENE....");

    if( !openglContext->renderShouldUpdate && !openglContext->generalUx->uxAnimations->shouldUpdate ) return SDL_Delay(16);

    openglContext->generalUx->uxAnimations->shouldUpdate = false; // timer will keep reseetting this


    openglContext->renderScene();

#ifdef DEVELOPER_TEST_MODE
    SDL_Log("RENDER SCENE COMPLETED... swapping buffers....");
#endif

    SDL_GL_SwapWindow(window); // move into render scene?
}

static Uint32 my_reshape_callbackfunc(Uint32 interval, void* parm){

    SDL_Event event;
    SDL_UserEvent userevent;
    userevent.type = SDL_USEREVENT;
    userevent.code = USER_EVENT_ENUM::VIEW_RECENTLY_ROTATED;
//    userevent.data1 = (void*)&pickerForPercentV;
//    userevent.data2 = &self->lastPickPercent; // &percent; // waste arg
    event.type = SDL_USEREVENT;
    event.user = userevent;
    SDL_PushEvent(&event);
    return 0; // end timer
    //return interval;
}

void ReshapeWindow(){
    ReshapeWindow(false);
}

void ReshapeWindow(bool fromMain){
    // just in case we didn't get hi-dpi from SDL_WINDOW_ALLOW_HIGHDPI we can see what size we actually got here

    SDL_GetWindowSize(window, &colorPickState->windowWidth, &colorPickState->windowHeight);

    SDL_GL_GetDrawableSize(window, &colorPickState->drawableWidth, &colorPickState->drawableHiehgt);

    win_w=colorPickState->windowWidth;
    win_h=colorPickState->windowHeight;

    colorPickState->updateComputedSizes();

    //colorPickState->viewport_ratio = (win_w+1.0f)/win_h;
    SDL_Log("SDL_GetWindowSize %d %d %f", win_w,win_h, colorPickState->viewport_ratio);


    SDL_Log("SDL_GL_GetDrawableSize %d %d %f", colorPickState->drawableWidth,colorPickState->drawableHiehgt, (colorPickState->drawableWidth+1.0f)/colorPickState->drawableHiehgt);


    if( win_w <= colorPickState->drawableWidth ) {
        // low dpi?
        ui_mmv_scale = 1.0;
    }else{
        ui_mmv_scale = 2.0;
        // even if win_w == SCREEN_WIDTH
        //  not guaranteed to be hidpi !~!!! !
    }

    colorPickState->ui_mmv_scale=ui_mmv_scale;

    // none of this experimental stuff really works:
    int displayIndex = SDL_GetWindowDisplayIndex(window);
    float ddpi=1.0f;
    float hdpi=1.0f;
    float vdpi=1.0f;
    SDL_Rect usableBounds;
    SDL_Rect miScreen;
    int dpi_result = SDL_GetDisplayDPI(displayIndex, &ddpi, &hdpi, &vdpi);

    //SDL_Log(SDL_GetError());

    int bounds_result =  SDL_GetDisplayUsableBounds(displayIndex, &usableBounds);

    int size_result =  SDL_GetDisplayBounds(displayIndex, &miScreen);

    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(displayIndex, &mode);


    //int modeIndex=0;

#ifdef DEVELOPER_TEST_MODE
    for( int modeIndex=0,totalModes=SDL_GetNumDisplayModes(displayIndex); modeIndex<totalModes; modeIndex++){
        SDL_GetDisplayMode(displayIndex, modeIndex, &mode);

        SDL_Log("POSSIBLE: w:%i h:%i %ihz ", mode.w, mode.h, mode.refresh_rate );
    }
#endif

    //SDL_SetWindowDisplayMode(<#SDL_Window *window#>, <#const SDL_DisplayMode *mode#>)



    //SDL_GetDisplayMode(<#int displayIndex#>, <#int modeIndex#>, SDL_DisplayMode *mode)
    // Uint32 format;              /**< pixel format */
    //    int w;                      /**< width, in screen coordinates */
    //    int h;                      /**< height, in screen coordinates */
    //    int refresh_rate;           /**< refresh rate (or zero for unspecified) */
    //    void *driverdata;           /**< driver-specific data, initialize to 0 */
    //} SDL_DisplayMode;

    SDL_Log("HERE IS WHAT DISPPLAY %i REPORtS: (dpiRrslt: %i ddpi %f hdpi %f vdpi %f)\n\
            \t Screeen Rect(%i): %i %i %i %i rDetectedDpiScaleIs: %f\n\
            \t Usable Bounds Rect(%i): %i %i %i %i \n\
            finally then %i %i" ,
            displayIndex, dpi_result, ddpi, hdpi, vdpi,
            size_result, miScreen.x, miScreen.y, miScreen.w, miScreen.h, ui_mmv_scale,
            bounds_result, usableBounds.x, usableBounds.y, usableBounds.w, usableBounds.h,
            int(miScreen.w*ui_mmv_scale), int(miScreen.h*ui_mmv_scale));

    //if( )


    openglContext->reshapeWindow(colorPickState->drawableWidth, colorPickState->drawableHiehgt);

#ifdef __ANDROID__
    if( !fromMain ){
        // after rotation, the viewport pespective matrix seems to undergo a delayed update (since we are using default)
        SDL_AddTimer(250, my_reshape_callbackfunc, nullptr);
    }
#endif

}


int main(int argc, char *argv[]) {

//    lastTimerTime = SDL_GetTicks();

    //SDL_Renderer *renderer;
    bool result = false;


    /*
     
SDL_INIT_TIMER
timer subsystem
SDL_INIT_AUDIO
audio subsystem
SDL_INIT_VIDEO
video subsystem; automatically initializes the events subsystem
SDL_INIT_JOYSTICK
joystick subsystem; automatically initializes the events subsystem
SDL_INIT_HAPTIC
haptic (force feedback) subsystem
SDL_INIT_GAMECONTROLLER
controller subsystem; automatically initializes the joystick subsystem
SDL_INIT_EVENTS
events subsystem
SDL_INIT_EVERYTHING
all of the above subsystems
SDL_INIT_NOPARACHUTE
compatibility; this flag is ignored
     
     if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
     
     */
//
    SDL_SetHint(SDL_HINT_VIDEO_ALLOW_SCREENSAVER, "1");
    SDL_SetHint(SDL_HINT_MAC_BACKGROUND_APP, "0");  // as far as I can tell.... this only makes the window not re-enter the background once focused - and also becomes incapable of entering the forground (no menu bar)
//    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "1");
//#ifdef __ANDROID
//    SDL_SetHint(SDL_ANDROID_SEPARATE_MOUSE_AND_TOUCH, "1");
//#endif

    /* initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0) {
        printf("Could not initialize SDL\n");
        return 1;
    }

    /* seed random number generator */
    srand(time(NULL));


//    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
//    SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 0);


    // TODO recall window position?  useful possibly for desktop platforms....


    /* create window and renderer */
    window =
        SDL_CreateWindow(NULL, 0, 0, win_w, win_h,
                         SDL_WINDOW_OPENGL
#if !TARGET_OS_SIMULATOR
                         | SDL_WINDOW_ALLOW_HIGHDPI
#endif
                         //| SDL_WINDOW_ALLOW_HIGHDPI
                         | SDL_WINDOW_RESIZABLE
                         //| SDL_WINDOW_MAXIMIZED
                         //| SDL_WINDOW_BORDERLESS
        );
    if (!window) {
        printf("Could not initialize Window\n");
        return 1;
    }

    //
    //SDL_HINT_IDLE_TIMER_DISABLED
    SDL_EnableScreenSaver();

    colorPickState->viewport_ratio = (win_w+1.0f)/win_h;


//    renderer = SDL_CreateRenderer(window, -1, 0);
//    if (!renderer) {
//        printf("Could not create renderer\n");
//        return 1;
//    }

    SDL_Log("main -----------------------");


#ifdef USE_EVENT_WATCH_FOR_EVENTS
    SDL_AddEventWatch(EventFilter, nullptr); // second param is provided to filter which runs in different thread... void* userdata
#endif



    result = openglContext->createContext(window);
    if( !result ){
        printf("Could not create context\n");
        return 1;
    }else{
        //ReshapeWindow();
        openglContext->setupScene();
        ReshapeWindow();
    }




#if __IPHONEOS__

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 0);

    //SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 256);

    //SDL_iPhoneSetEventPump(SDL_TRUE);
    SDL_iPhoneSetAnimationCallback(window, 1, ShowFrame, NULL);

#else
    int done = 0;
    SDL_Event event;


    while (!done) {
        while (SDL_PollEvent(&event)) {

            switch ( event.type ){
                case SDL_QUIT:
                    done = 1;
                    openglContext->generalUx->writeOutState();
                    return 0;


                case SDL_USEREVENT:  // maybe move all handing of these to , say main thread? (instead of coincidentally same thread on android)
                {
                    switch( event.user.code ){
#ifdef MAIN_THREAD_ANIMATIONS
                        case USER_EVENT_ENUM::ANIMATE_MAIN_THREAD:
                        {
                            SDL_Log("USER EVENT 3 - ANIMATE_MAIN_THREAD");
                            openglContext->generalUx->uxAnimations->updateAnimationsMain(true);
                            break;//return 0;
                        }
#endif
#ifdef __ANDROID__
                        case USER_EVENT_ENUM::IMAGE_SELECTOR_READY:
                        {
                            SDL_Log("USER EVENT 0 - get picked image on android");
                            getImagePathFromMainThread();
                            break;//return 0;
                        }
#endif
                        case USER_EVENT_ENUM::NEW_HUE_CHOSEN:
                        {
                            SDL_Log("USER EVENT 1 - new hue gradient bg color");
                            void (*p) (void*) = (voidvoidp)event.user.data1;
                            p(event.user.data2);
                            // TODO instea
                            //SDL_FlushEvent(SDL_USEREVENT);
                            break;//return 0;
                        }
                        case USER_EVENT_ENUM::GENERIC_ARBITRARY_CALL:
                        {
                            SDL_Log("USER EVENT GENERIC_ARBITRARY_CALL");
                            void (*p) (void*) = (voidvoidp)event.user.data1;
                            p(event.user.data2);
                            break;//return 0;
                        }
                        case USER_EVENT_ENUM::VIEW_RECENTLY_ROTATED:
                        {
                            SDL_Log("USER EVENT 2 - rotation delayted update");
                            ReshapeWindow(true);
                            openglContext->renderShouldUpdate = true;
                            break;//return 0;
                        }
                    }

                    break;//return 0;
                }
            }
#ifndef USE_EVENT_WATCH_FOR_EVENTS
            EventFilter(nullptr, &event);
            // should we return teh reuslt of this??
#endif

            //break; // TODO instead of breakign out of the loop here, we should have really just handled the event above
        }
        //render(renderer);

        ShowFrame(NULL);



    }


    /* shutdown SDL */
    openglContext->destroyContext();
    SDL_DestroyWindow(window);
    SDL_Quit();
#endif

    return 0;
}

