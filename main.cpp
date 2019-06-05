/*
 *  rectangles.c
 *  written by Holmes Futrell
 *  use however you want
 */

#include "main.h"
#include "ColorPick.h" // includes openglContext
#include <time.h>

void DebugStr(const char *c_str ) {
#ifdef COLORPICK_DEBUG_MODE
    SDL_Log("%s", c_str);
#endif
}

void debugGLerror(const char *c_str_label){
#ifdef COLORPICK_DEBUG_MODE
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


//int // this is just Ux::randomInt lets remove this one?
//randomInt(int min, int max)
//{
//    return min + rand() % (max - min + 1);
//}
//
//void
//render(SDL_Renderer *renderer) // DEFAULT demo using sdl render, delete me
//{
//
//    Uint8 r, g, b;
//
//    /* Clear the screen */
//    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//    SDL_RenderClear(renderer);
//
//    /*  Come up with a random rectangle */
//    SDL_Rect rect;
//    rect.w = randomInt(64, 128);
//    rect.h = randomInt(64, 128);
//    rect.x = randomInt(0, SCREEN_WIDTH);
//    rect.y = randomInt(0, SCREEN_HEIGHT);
//
//    /* Come up with a random color */
//    r = randomInt(50, 255);
//    g = randomInt(50, 255);
//    b = randomInt(50, 255);
//    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
//
//    /*  Fill the rectangle in the color */
//    SDL_RenderFillRect(renderer, &rect);
//
//    /* update screen */
//    SDL_RenderPresent(renderer);
//}

typedef void (*voidvoidp)(void *someParam);

int fileDropsAllowed = 0;

// TODO we need to drop mousStateDown
//int mousStateDown = 0; // really should count fingers down possibly, finger event support multi touch, mosue events dont!
//bool multiTouchMode = false; // UNUSED VAR!!!! always on though right?

int fingerDeviceDownCounter = 0;
bool isLockedForZoomUntilFingersZeros=false;
int tx,ty;

/* forward declarations */
void mouseDownEvent(SDL_Event* event);
void mouseMoveEvent(SDL_Event* event);
void mouseUpEvent(SDL_Event* event);

SDL_Point getMouseXYforEvent(SDL_Event* event){
    SDL_Point result = {0,0};
    if( event->type == SDL_FINGERDOWN || event->type == SDL_FINGERMOTION || event->type == SDL_FINGERUP ){
        result.x = (int)(event->tfinger.x * win_w);
        result.y = (int)(event->tfinger.y * win_h);
//    }else if ( event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEMOTION || event->type == SDL_MOUSEBUTTONUP ){
//        result.x = event->motion.x;
//        result.y = event->motion.y; // commented this out since its basically teh same as our default handler below... - until we formerly support multi-mouse - then we should handle both up here probably!
    }else{
        //SDL_Log("this is unexpected... (probably occurs but unexpected none the less...)");
        // note: this is basically equivilent to the above, and works perfectly for ALL mosue related events (even SDL_MOUSEWHEEL which is how we get here...)
        SDL_GetMouseState(&result.x, &result.y);
    }
    return result;
}

uiInteraction* beginInteraction(SDL_Event* event, bool isStart){
    SDL_Point tmp = getMouseXYforEvent(event);
    tx = tmp.x; ty=tmp.y; // TODO: lets remove tx ty too...
    openglContext->pixelInteraction.begin(tx, ty);
    //openglContext->generalUx->currentInteraction.begin( (tx*ui_mmv_scale)/win_w, (ty*ui_mmv_scale)/win_h ); // deprecated....

    uiInteraction* fingerInteraction = openglContext->generalUx->interactionForPointerEvent(event);

    // NOTE: if our interaction already has fingerInteraction->fingerStateDown its a second mosue button - and we should cancel - handle it...
    if( fingerInteraction->fingerStateDown ){
        return fingerInteraction; //
    }

    fingerInteraction->begin( (tx*ui_mmv_scale)/win_w, (ty*ui_mmv_scale)/win_h );
    if( isStart ){
        fingerInteraction->fingerStateDown = true; // maybe use an integer for better tracking?  fingers only have one button though....
        openglContext->pixelInteraction.fingerStateDown = true;
    }

    //SDL_Log("MOUSE xy perc %f %f", openglContext->generalUx->currentInteraction.px, openglContext->generalUx->currentInteraction.py );
    /*didInteract = actually this meant did collide, now we haev fingerInteraction->didCollideWithObject */
    openglContext->generalUx->triggerInteraction(fingerInteraction, isStart);

    return fingerInteraction;
}

void mouseDownEvent(SDL_Event* event){
    fingerDeviceDownCounter+=1;
    if( fingerDeviceDownCounter > 1 ){
        isLockedForZoomUntilFingersZeros = true;
    }
//    mousStateDown += 1;
//    //SDL_Log("finger count %i", event->tfinger.);
//    if( mousStateDown > 1 ){
//        //second touch....
//        //SDL_Log("SDL_FINGERDOWN second touch (or second mouse button...)");
//        if( event->type == SDL_FINGERDOWN ){
//            SDL_Log("SDL_FINGERDOWN second touch finger %i", event->tfinger.fingerId ); // fingers 0,1,2,3.... 1.844674407370955e19
//        }
//        mousStateDown--; // doing this so mouseUpEvent has an effect....
//        mouseUpEvent(event); // lets treat it as if they stopped interacting....
//        //mousStateDown++;
//        didInteract=false;
//        return;
//    }
    openglContext->clearVelocity(); // stop any active panning
    //SDL_SetRelativeMouseMode(SDL_TRUE);
    //SDL_GetRelativeMouseState(&colorPickState->mmovex, &colorPickState->mmovey);
    // SDL_TouchFingerEvent event->tfinger
    //SDL_GetRelativeMouseState(&tx, &ty);
    uiInteraction* fingerInteraction = beginInteraction(event, true); // returns uiInteraction* fingerInteraction
    openglContext->renderShouldUpdate = true; // android??


}

void mouseMoveEvent(SDL_Event* event){
    uiInteraction* fingerInteraction = openglContext->generalUx->interactionForPointerEvent(event);

    if( fingerInteraction->fingerStateDown /* == 1 */ ){

        //uiInteraction* fingerInteraction = openglContext->generalUx->interactionForPointerEvent(event); // dupe!

        SDL_Point tmp = getMouseXYforEvent(event);
        tx = tmp.x; ty=tmp.y;

        //SDL_Log("mousStateDown SDL_FINGERMOTION SDL_MOUSEMOTION");
        if( /*!didInteract*/ !fingerInteraction->didCollideWithObject && fingerDeviceDownCounter == 1 && !isLockedForZoomUntilFingersZeros ){
            //SDL_GetRelativeMouseState(&colorPickState->mmovex, &colorPickState->mmovey);
#if __ANDROID__
            bool wasZero = openglContext->pixelInteraction.isZeroed();  // on android this event won't fire right away - in fact it takes quite a LOT of movement to reach SDL_FINGERMOTION here....
#endif

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
//            SDL_Point tmp = getMouseXYforEvent(event);
//            tx = tmp.x; ty=tmp.y;
            fingerInteraction->update((tx*ui_mmv_scale)/win_w, (ty*ui_mmv_scale)/win_h); // < we COULD update this regardless.. moving it above the IF...  -> not recommended... causes issues where mousup out of the blue (without mouse down) can trigger things....

            //SDL_Log("MOUSE xy perc %f %f", openglContext->generalUx->currentInteraction.px, openglContext->generalUx->currentInteraction.py );
            //SDL_Log("MOUSE xy delta %f %f", openglContext->generalUx->currentInteraction.dx, openglContext->generalUx->currentInteraction.dy );
            // todo combine above into the following call?
            openglContext->generalUx->interactionUpdate(fingerInteraction); // todo whyarg, BETTERARG
            // todo the above would appear to return a "should update??"
            openglContext->renderShouldUpdate = true; /// TODO interactionUpdate calls generalUx->updateRenderPositions() should cause the UI to update... for now we just redraw everything
        }
    }
}

void mouseUpEvent(SDL_Event* event){
    fingerDeviceDownCounter-=1;
    if( fingerDeviceDownCounter < 1 ){
        isLockedForZoomUntilFingersZeros = false;
    }

    uiInteraction* fingerInteraction = openglContext->generalUx->interactionForPointerEvent(event);

    SDL_Point tmp = getMouseXYforEvent(event);
    tx = tmp.x; ty=tmp.y;

    if( /*didInteract*/ fingerInteraction->didCollideWithObject ){
        // we may be able to add this, but we need to track velocity better
        fingerInteraction->update((tx*ui_mmv_scale)/win_w, (ty*ui_mmv_scale)/win_h);
        //SDL_Log("MOUSE xy perc %f %f", openglContext->generalUx->currentInteraction.px, openglContext->generalUx->currentInteraction.py );
        //SDL_Log("MOUSE xy delta %f %f", openglContext->generalUx->currentInteraction.dx, openglContext->generalUx->currentInteraction.dy );
    }else{

        openglContext->pixelInteraction.done(tx, ty );

    }
    if( fingerInteraction->fingerStateDown /*== 1*/ && openglContext->pixelInteraction.dx == 0 && openglContext->pixelInteraction.dy == 0 ){
        // it was a single touch
        //SDL_Log("SDL_FINGERUP was reachd - position did not change" );
        //SDL_Log("MOUSE xy delta %f %f", openglContext->pixelInteraction.dx, openglContext->pixelInteraction.dy );
        // so if we already didInteract....
        // calling triggerInteraction again might change our interaction object....
        // which could be good or mediocure if we are currently doing a drag and drop
        // but in this case we don't want to overwrite our interaction if we already have one

        // I do not think this following code is needed.... (but may change some things if removed.... this lets us have "mouseup" inetractions without correspondign "mouse down"...
        if( /*!didInteract*/ !fingerInteraction->didCollideWithObject ){ // didInteract really means didInteractWithUi and when we are, we leave the collected_x and collected_y zero at this time....
            /*didInteract = */ openglContext->generalUx->triggerInteraction(fingerInteraction);
        }

        // didInteract = openglContext->generalUx->triggerInteraction() || didInteract; /// !!! collexted x + y is zero
        // we may really test for this outside of the condition above
        // and determine mouse movement using the openglContext->generalUx->currentInteraction
        // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        // TODO the following may be important for swipey events since fingerup will have velocity or not????
        // todo openglContext->generalUx->currentInteraction.update not called anywhere here ! (except we are now, rbove)
        // maybe its okay?
        if( /*!didInteract*/ !fingerInteraction->didCollideWithObject ){
            // we have missed the UI and BG clicked oh now what
            // this breaks multigesture somehow
            // this feature will move
            if( openglContext->pixelInteraction.wasStationary() ){
                //SDL_Log("No movement BG click");
                if( openglContext->generalUx->wouldLooseIfColorAdded() ){
                    //openglContext->generalUx->uxAnimations->scale_bounce(openglContext->generalUx->addHistoryBtn, -0.005);
                }else{
                    openglContext->generalUx->uxAnimations->scale_bounce(openglContext->generalUx->addHistoryBtn, 0.005);
                }
            }
        }else{
            //
            //openglContext->generalUx->interactionUpdate(uiInteraction *delta);
        }
    }else{
        openglContext->triggerVelocity(openglContext->pixelInteraction.vx, openglContext->pixelInteraction.vy); // args unused....
    }

    if( fingerInteraction->fingerStateDown /* == 1 */ ){
        // at this point we culd really need to render an update
        if( openglContext->generalUx->interactionComplete(fingerInteraction) ){ // MOUSEUP
            //SDL_Log("MARKED FOR UPDATE - WILL IT RENDER????");
            openglContext->renderShouldUpdate=true;
            // this seems to not guarantee render?!
        }
    }
    //didInteract = false;
    fingerInteraction->didCollideWithObject = false; // because we are done now.... not sure if this is really needed since begin will reset it, but just to be safe....
    fingerInteraction->fingerStateDown = false; // or decrement?
    //mousStateDown = 0; // because each finger being released will reach this block... we need to avoid the last finger from being treated as click still...
    //            mousStateDown -= 1;
    //            if( mousStateDown < 0 ){
    //                mousStateDown=0;
    //            }
    //SDL_SetRelativeMouseMode(SDL_FALSE); // bit unsure if this mouse stuff will work for touch
    //SDL_GetRelativeMouseState(&colorPickState->mmovex, &colorPickState->mmovey);
}

void BackButtonEvent(){
    if( openglContext->generalUx->hasCurrentModal() ){
        openglContext->generalUx->endCurrentModal();
    }else{
#ifdef __ANDROID__
        SDL_AndroidBackButton();
#endif
    }
}

// TODO: this might need to be in the animation loop for ios!!!!!!!
int MainThreadUserEventHandler(SDL_Event* p_event){

    SDL_Event event = *p_event;

    switch( event.user.code ){
#ifdef MAIN_THREAD_ANIMATIONS
        case USER_EVENT_ENUM::ANIMATE_MAIN_THREAD: // NOTE on ios it still won't really be main thread.... right?  we'd want to process these events in the animation handler instead right?
        {
            SDL_Log("USER EVENT 3 - ANIMATE_MAIN_THREAD");
            openglContext->generalUx->uxAnimations->updateAnimationsMain(true);
            return 0;
        }
#endif
#ifdef __ANDROID__
        case USER_EVENT_ENUM::IMAGE_SELECTOR_READY:
        {
            SDL_Log("USER EVENT 0 - get picked image on android");
            getImagePathFromMainThread();
            return 0;
        }
#endif
        case USER_EVENT_ENUM::NEW_HUE_CHOSEN:
        {
            SDL_Log("USER EVENT 1 - new hue gradient bg color");
            void (*p) (void*) = (voidvoidp)event.user.data1;
            p(event.user.data2);
            // TODO instea
            //SDL_FlushEvent(SDL_USEREVENT);
            return 0;
        }
        case USER_EVENT_ENUM::GENERIC_ARBITRARY_CALL:
        {
            SDL_Log("USER EVENT GENERIC_ARBITRARY_CALL");
            void (*p) (void*) = (voidvoidp)event.user.data1;
            p(event.user.data2);
            return 0;
        }
        case USER_EVENT_ENUM::VIEW_RECENTLY_ROTATED:
        {
            SDL_Log("USER EVENT 2 VIEW_RECENTLY_ROTATED");
            ReshapeWindow(true);
            openglContext->renderShouldUpdate = true;
            return 0;
        }
        case USER_EVENT_ENUM::MESH_FILE_READ:
        {
            SDL_Log("USER EVENT MESH_FILE_READ");
            openglContext->meshes->completeMeshLoading();
            return 0;
        }
        case USER_EVENT_ENUM::RENDER_VIEW_AGAIN:
        {
            SDL_Log("USER EVENT - RENDER_VIEW_AGAIN");
            openglContext->renderShouldUpdate = true;
            return 0;
        }
    }

    //return 1; // not handled, leave it in the queue ?????? only really makes sense if we process events elsewhere right?  tough to say for sure but we'd have to make sure all platforms handle the other events somehow...
    return 0;
}

int EventFilter(void* userdata, SDL_Event* event){

    //SDL_Log("WE GOT AN EVENT; type %i", event->type );
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


                case SDL_WINDOWEVENT_LEAVE:

                    SDL_Log("\n SDL_WINDOWEVENT_LEAVE");

                    return 0;

                case SDL_WINDOWEVENT_FOCUS_LOST:

                    SDL_Log("\n SDL_WINDOWEVENT_FOCUS_LOST");

                    return 0;

                case SDL_WINDOWEVENT_FOCUS_GAINED:

                    SDL_Log("\n SDL_WINDOWEVENT_FOCUS_GAINED");
                    openglContext->renderShouldUpdate = true;

                    return 0;

            }
            return 0;
        }
        /*
event is maybe going to have
         SDL_ControllerAxisEvent caxis;
            SDL_ControllerButtonEvent cbutton;
            SDL_ControllerDeviceEvent cdevice;
         */
        case SDL_CONTROLLERDEVICEADDED:
            SDL_Log("Controller was added %i (need to open it)", event->cdevice.which);
            //see SDL_GameControllerOpen
            break;

        case SDL_CONTROLLERDEVICEREMOVED:
            SDL_Log("Controller was rm %i (need to close it?)", event->cdevice.which);
            break;

        case SDL_CONTROLLERBUTTONDOWN:{
            SDL_Log("Controller button down %i (SDL_GameControllerButton)", event->cbutton.button);


            switch(event->cbutton.button){
                case SDL_CONTROLLER_BUTTON_DPAD_UP:
                    colorPickState->mmovey=1;
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                    colorPickState->mmovey=-1;
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                    colorPickState->mmovex=-1;
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                    colorPickState->mmovex=1;
                    break;
                case SDL_CONTROLLER_BUTTON_A:

                    SDL_Log("BUTTON A");

                    break;
                case SDL_CONTROLLER_BUTTON_B:
                    SDL_Log("BUTTON B");
                    BackButtonEvent();
                    break;
            }


            //openglContext->has_velocity=false;
            openglContext->renderShouldUpdate=true;

            /*

             case SDLK_UP:
             colorPickState->mmovey=1;
             break;
             case SDLK_DOWN:
             colorPickState->mmovey=-1;
             break;
             case SDLK_RIGHT:
             colorPickState->mmovex=-1;
             break;
             case SDLK_LEFT:
             colorPickState->mmovex=1;
             */

            break;
        }
        case SDL_CONTROLLERBUTTONUP:
            SDL_Log("Controller button up %i (SDL_GameControllerButton)", event->cbutton.button);
            break;

        case SDL_CONTROLLERAXISMOTION:
            SDL_Log("Controller axis motion %i (SDL_GameControllerAxis) value %i", event->caxis.axis, event->caxis.value);
            break;

#ifdef COLORPICK_PLATFORM_DESKTOP
        case SDL_MOUSEWHEEL:
            //SDL_Log("Hello Wheel!!");

            if( openglContext->generalUx->hasCurrentModal() ){

                // we may need to see what item is under our pointer....
                // only trouble comes if we are already clicked when scrolling ?

                // fingerInteraction is a bad name... fingerOrMouseInteraction is better.....
                uiInteraction* fingerInteraction = beginInteraction(event, false);  // we can see if the last interaction is complete first????
                openglContext->generalUx->wheelOrPinchInteraction(fingerInteraction, event->wheel.y);

            }else{
                openglContext->clearVelocity();
                openglContext->renderShouldUpdate = true;
                openglContext->setFishScale(event->wheel.y, 0.10f);
            }

            return 0;
#endif
        case SDL_MULTIGESTURE: // http://lazyfoo.net/tutorials/SDL/55_multitouch/index.php
            //SDL_Log("Hello Gesture!!");

            if( openglContext->generalUx->hasCurrentModal() ){

                // todo - send it to the modal.... (and or minigame as case may be ?)
            }else{
                openglContext->clearVelocity();
                openglContext->renderShouldUpdate = true;
                openglContext->setFishScale(event->mgesture.dDist, 40.0f);
            }

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
                BackButtonEvent();
            }else if(event->key.keysym.sym == SDLK_AC_HOME){
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

        case SDL_APP_LOWMEMORY:{
            SDL_Log("Memory Warning !!!!");
            /* You will get this when your app is paused and iOS wants more memory.
             Release as much memory as possible.
             */

            // WE CAN"T DO MUCH BUT WE CAN SAVE OUR STATE I SUPPOSE?? (OR WILL THIS CRASH US???)

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
            //mousStateDown = 0; // zero finger counter
            // maybe we should go through all our currentInteractions and zero those out???
            fingerDeviceDownCounter = 0;
            isLockedForZoomUntilFingersZeros = false;
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
            openglContext->renderShouldUpdate = true;
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
                }
                openglContext->imageWasSelectedCb(myCoolSurface);
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
            if( event->type == SDL_JOYAXISMOTION ){
                break; // presumably we handled this or will plan to handle it.... above.... for now this makes silence on android....
            }

            // see instead (of the above) SDL_HINT_TOUCH_MOUSE_EVENTS (actually that broke android?)
            SDL_Log("unrecognized event; type %02x", event->type );
            break;
            
    }
    return 1;/* No special processing, add it to the event queue */
}

void ShowFrame() {
    ShowFrame(nullptr);
}

void ShowFrame(void*)
{

//    currentTime = SDL_GetTicks();
//    openglContext->updateFrame(currentTime - lastTimerTime);


#ifdef COLORPICK_MISSING_MAIN_LOOP
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch ( event.type ){
            case SDL_USEREVENT:  // we normally process all these in the main thread.... there are some exceptions for ios
            {
                MainThreadUserEventHandler(&event);
                break;//return 0;
            }
        }
    }
#endif


    //SDL_Log("RENDER SCENE....");

    if( !openglContext->renderShouldUpdate && !openglContext->generalUx->uxAnimations->shouldUpdate ) return SDL_Delay(16);

    openglContext->generalUx->uxAnimations->shouldUpdate = false; // timer will keep reseetting this


    openglContext->renderScene();

#ifdef DEVELOPER_TEST_MODE // NOTE comment this ifdef out if you want to see your rendering pause
    SDL_Log("RENDER SCENE COMPLETED... swapping buffers....");
#endif

    SDL_GL_SwapWindow(window); // move into render scene?

    SDL_Delay(33); // we need some delay EITEHR WAY (old android) since we will render again RIGHT AWAY otherwise... and the buffer isn't really swapped, so we just cleard our acutal frame AAAAAGGGGGHGHH
    // arguably if we measure the time for render, we should subtract that... since 33 will otheriwse be too much time.... CRUMMY_ANDROID

    //SDL_Delay(600); // note here for CRUMMY_ANDROID testing if applicable
}

// it is used, on android, but maybe can be interchanged with "render_one_more_frame"
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


    // NOTE: this log is dangerous since division by zero, and also not dangerous thanks to optimization
    //SDL_Log("SDL_GL_GetDrawableSize %d %d %f", colorPickState->drawableWidth,colorPickState->drawableHiehgt, (colorPickState->drawableWidth+1.0f)/colorPickState->drawableHiehgt);


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


#ifdef COLORPICK_DEBUG_MODE
    // we consider this the "debug" build.... ?
#else
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_ERROR);
    //SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN);
    // todo, some of our SDL_Log that are maybe Error - are just INFO level....
    //SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Notice: Log Level Error");
#endif

//    lastTimerTime = SDL_GetTicks();

    //SDL_Renderer *renderer;
    bool result = false;



    //SDL_SetHint(SDL_HINT_ANDROID_SEPARATE_MOUSE_AND_TOUCH, "1"); // setting this for fear of a duplicate event, we really want one type or the ohter???  // NEVERMIND: we only listen for ONE type of event (#ifndef COLORPICK_PLATFORM_DESKTOP then fingers), so we need both driggered.

    //SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");

#ifndef __EMSCRIPTEN__

    SDL_SetHint(SDL_HINT_IOS_HIDE_HOME_INDICATOR, "0");
    SDL_SetHint(SDL_HINT_IDLE_TIMER_DISABLED, "0");
    //SDL_SetHint(SDL_HINT_VIDEO_DOUBLE_BUFFER, "1"); // tell it we only want double (not tripple) buffer... bad for IOS... ?
    SDL_SetHint(SDL_HINT_ANDROID_TRAP_BACK_BUTTON, "1"); // its trapped by default still as of SDL2-2.0.9 (even though docs say otherwise...)
    SDL_SetHint(SDL_HINT_VIDEO_ALLOW_SCREENSAVER, "1");
    SDL_SetHint(SDL_HINT_MAC_BACKGROUND_APP, "0");  // as far as I can tell.... this only makes the window not re-enter the background once focused - and also becomes incapable of entering the forground (no menu bar)
#endif
    //SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");



    //    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "1");
//#ifdef __ANDROID
//    SDL_SetHint(SDL_ANDROID_SEPARATE_MOUSE_AND_TOUCH, "1");
//#endif


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

    /* initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_GAMECONTROLLER) < 0) {
        printf("Could not initialize SDL\n");
        return 1;
    }

    if( SDL_NumJoysticks() > 0 ){
        SDL_Log("joysticks found %i", SDL_NumJoysticks());
        if( SDL_IsGameController(0) ){
            SDL_GameControllerOpen(0); // returns an SDL_GameController*
            SDL_Log("game controller open");
        }else{
            SDL_JoystickOpen(0);
            SDL_Log("joystick open");
        }
    }

    /* seed random number generator */
    srand((int)time(NULL));


//    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
//    SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 0);
//    SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 1);

    // TODO recall window position?  useful possibly for desktop platforms....

//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); // to try 3.0 headers, and manifest no.. see OpenGL ES  _  Android Developers.html ... if that fails try sdl 2.0.8 ?
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

// UX read settings begin....

#if !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)
    // OES (egl?) is already core?? (ish?)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

#else
    // for the else, we are using _ES profile for sure (pretty much) not sure if it eneds to be set though....
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

#endif

//    SDL_Log("setting depth size....");
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // NOTE: some CRUMMY_ANDROID may not support 24 here?
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

//    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,0);
//    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);

//    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
//    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
//    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
//    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0); // nifty if we can have alpha these days in the screen buffer....
//    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
//    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 5);
//    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 0);
//    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
//SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 0);

// this is nifty on fire TV since we won't have a reshape/resize...we just start fullscreen... meh though
    // oddly this seems to help fire tv a lot.... hmm... muliti display android or windowed mode: seems like this will break it though
#if defined(__ANDROID__)
//    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);
//    SDL_DisplayMode sdpm;
//    SDL_GetDisplayMode(0, 0, &sdpm);
//    win_w=sdpm.w;
//    win_h=sdpm.h;
#endif

//SDL_GL_SetSwapInterval(0);

    int win_pos_x=0;
    int win_pos_y=0;

#ifdef COLORPICK_PLATFORM_DESKTOP

    // this enables 4x on osx....  it works on iphone but is slow in simulator
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 4);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);


    // BAD!!!!  check SDL_WINDOWEVENT_MOVED maybe and store this?  or forget it...
    win_pos_x=512;
    win_pos_y=512;
#endif

    /* create window and renderer */
    window =
        SDL_CreateWindow(NULL, win_pos_x, win_pos_y, win_w, win_h,
                         SDL_WINDOW_OPENGL
                         | SDL_WINDOW_SHOWN
#if !TARGET_OS_SIMULATOR
                         | SDL_WINDOW_ALLOW_HIGHDPI
#endif
                         //| SDL_WINDOW_ALLOW_HIGHDPI
                         | SDL_WINDOW_RESIZABLE
                         //| SDL_WINDOW_MAXIMIZED
                         //| SDL_WINDOW_FULLSCREEN
                         //| SDL_WINDOW_BORDERLESS
        );
    if (!window) {
        printf("Could not initialize Window\n");
        return 1;
    }

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

//SDL_RENDERER_PRESENTVSYNC
 //       SDL_GL_SetSwapInterval(0); // keep vsync off....??? (again may need to set this earlier??)

        //SDL_EGL_SetSwapInterval(0);

// there has to be a better way to do this... but its what we got...


#ifndef GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX 0x9049
#endif
#ifndef TEXTURE_FREE_MEMORY_ATI
#define TEXTURE_FREE_MEMORY_ATI 0x87FC
#endif


        // we will check the memories now... 3d might be too much...
        int resultInt = 0;


        glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &resultInt);
        SDL_Log("We got GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX as: %i", resultInt);
        glGetIntegerv(TEXTURE_FREE_MEMORY_ATI, &resultInt);
        SDL_Log("We got TEXTURE_FREE_MEMORY_ATI as: %i", resultInt);


//#define stringify(x) _stringify(x)
//#define _stringify(x) #x
//#define logGottenGlAtrib(literalAttrib) do{ \
//int resultInt; \
//SDL_GL_GetAttribute(literalAttrib, &resultInt); \
//SDL_Log("contexts %s %i", stringify(literalAttrib), resultInt); \
//} while(0)


#define logGottenGlAtrib(literalAttrib) SDL_GL_GetAttribute(literalAttrib, &resultInt); \
SDL_Log("contexts %s %i", #literalAttrib, resultInt);


        logGottenGlAtrib(SDL_GL_DEPTH_SIZE);
        if( resultInt < 1 ){
            openglContext->meshes->mesh3d_enabled = false; // no depth buffer = no meshes...
            SDL_Log("DISABLED MESH 3D no depth");
        }
        logGottenGlAtrib(SDL_GL_STENCIL_SIZE);
        if( resultInt < 1 ){
            openglContext->meshes->mesh3d_enabled = false; // no stencil buffer = no meshes...
            SDL_Log("DISABLED MESH 3D no stencil");
        }
        logGottenGlAtrib(SDL_GL_DOUBLEBUFFER);
        logGottenGlAtrib(SDL_GL_RETAINED_BACKING);

        logGottenGlAtrib(SDL_GL_BUFFER_SIZE);
//
        logGottenGlAtrib(SDL_GL_RED_SIZE);
        logGottenGlAtrib(SDL_GL_GREEN_SIZE);
        logGottenGlAtrib(SDL_GL_BLUE_SIZE);
        logGottenGlAtrib(SDL_GL_ALPHA_SIZE);

        logGottenGlAtrib(SDL_GL_CONTEXT_MAJOR_VERSION);
        logGottenGlAtrib(SDL_GL_CONTEXT_MINOR_VERSION);

        //SDL_Log("Open GL says we are %s", glGetString(GL_VERSION));

        //ReshapeWindow();
        openglContext->setupScene();
        ReshapeWindow();
    }









    SDL_EnableScreenSaver(); // this may set some of the above hints automagically


#ifdef COLORPICK_MISSING_MAIN_LOOP

#ifdef __EMSCRIPTEN__
    // void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
    //emscripten_set_main_loop(ShowFrame, 60, 1); 

    emscripten_set_main_loop(ShowFrame, 0, 1);
#else
    //    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    //SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 0);  // <<< this probably does NOTHING here after context is created....

    //SDL_iPhoneSetEventPump(SDL_TRUE);
    SDL_iPhoneSetAnimationCallback(window, 1, ShowFrame, NULL);
#endif

#else // not COLORPICK_MISSING_MAIN_LOOP
    int done = 0;
    SDL_Event event;


    while (!done) {
        while (SDL_PollEvent(&event)) {

            switch ( event.type ){
                case SDL_QUIT:
                    done = 1;
                    openglContext->generalUx->writeOutState();
                    return 0;


                case SDL_USEREVENT:  // we normally process all these in the main thread.... there are some exceptions for ios
                {
                    MainThreadUserEventHandler(&event);
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

//        SDL_Log("Main loop - ShowFrame");
        ShowFrame(NULL);



    }


    /* shutdown SDL */
    openglContext->destroyContext();
    SDL_DestroyWindow(window);
    SDL_Quit();
#endif

    return 0;
}

