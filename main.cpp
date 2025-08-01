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
    openglContext->pixelInteraction.begin(event->common.timestamp, tx, ty);
    //openglContext->generalUx->currentInteraction.begin( (tx*colorPickState->ui_mmv_scale)/win_w, (ty*colorPickState->ui_mmv_scale)/win_h ); // deprecated....

    uiInteraction* fingerInteraction = openglContext->generalUx->interactionForPointerEvent(event);

    openglContext->generalUx->disableControllerCursor();

    // NOTE: if our interaction already has fingerInteraction->fingerStateDown its a second mosue button - and we should cancel - handle it...
    if( fingerInteraction->fingerStateDown ){
        return fingerInteraction; //
    }

    fingerInteraction->begin(event->common.timestamp, (tx*colorPickState->ui_mmv_scale)/win_w, (ty*colorPickState->ui_mmv_scale)/win_h );
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

// THE FOLLOWING ANDROID SPECIFIC IFDEF SOLVED AN ISSUE where we had too much initial velocity, of several pixesl, and we wanted to move just one pixel - we don't need this so much anymore since we have actually handled percise motion in another way, so even if the inital value is large, it will only count for a little based on current zoom level...
//#if __ANDROID__
//            bool wasZero = openglContext->pixelInteraction.isZeroed();  // on android this event won't fire right away - in fact it takes quite a LOT of movement to reach SDL_FINGERMOTION here....
//#endif

            openglContext->pixelInteraction.update(event->common.timestamp, tx, ty);
//#if __ANDROID__
//
//            if( wasZero ){
//                if( openglContext->pixelInteraction.rx > 1.0 ){
//                    openglContext->pixelInteraction.rx = 1.0;
//                }else if( openglContext->pixelInteraction.rx < -1.0 ){
//                    openglContext->pixelInteraction.rx = -1.0;
//                }
//                if( openglContext->pixelInteraction.ry > 1.0 ){
//                    openglContext->pixelInteraction.ry = 1.0;
//                }else if( openglContext->pixelInteraction.ry < -1.0 ){
//                    openglContext->pixelInteraction.ry = -1.0;
//                }
//            }
//#endif
            openglContext->triggerMovement();
        }else{
            //colorPickState->mmovex = event->motion.xrel;
            //colorPickState->mmovey = event->motion.yrel;
//            SDL_Point tmp = getMouseXYforEvent(event);
//            tx = tmp.x; ty=tmp.y;
            fingerInteraction->update(event->common.timestamp, (tx*colorPickState->ui_mmv_scale)/win_w, (ty*colorPickState->ui_mmv_scale)/win_h); // < we COULD update this regardless.. moving it above the IF...  -> not recommended... causes issues where mousup out of the blue (without mouse down) can trigger things....

            //SDL_Log("MOUSE xy perc %f %f", openglContext->generalUx->currentInteraction.px, openglContext->generalUx->currentInteraction.py );
            //SDL_Log("MOUSE xy delta %f %f", openglContext->generalUx->currentInteraction.dx, openglContext->generalUx->currentInteraction.dy );
            // todo combine above into the following call?
            openglContext->generalUx->interactionUpdate(fingerInteraction); // todo whyarg, BETTERARG
            // todo the above would appear to return a "should update??"
            openglContext->renderShouldUpdate = true;
        }
    }
}

void mouseUpEvent(SDL_Event* event){
    fingerDeviceDownCounter-=1;
    if( fingerDeviceDownCounter < 1 ){
        isLockedForZoomUntilFingersZeros = false;
    }

    //SDL_Log("Mouse Button ID: %i", event->button.button); see isSecondInteraction

    uiInteraction* fingerInteraction = openglContext->generalUx->interactionForPointerEvent(event);

    if( fingerInteraction->wasStationary() && event->button.button == 3){
        fingerInteraction->isStationaryRightClick = true;
    }

    SDL_Point tmp = getMouseXYforEvent(event);
    tx = tmp.x; ty=tmp.y;

    if( /*didInteract*/ fingerInteraction->didCollideWithObject ){
        // we may be able to add this, but we need to track velocity better
        fingerInteraction->update(event->common.timestamp, (tx*colorPickState->ui_mmv_scale)/win_w, (ty*colorPickState->ui_mmv_scale)/win_h);
        //SDL_Log("MOUSE xy perc %f %f", openglContext->generalUx->currentInteraction.px, openglContext->generalUx->currentInteraction.py );
        //SDL_Log("MOUSE xy delta %f %f", openglContext->generalUx->currentInteraction.dx, openglContext->generalUx->currentInteraction.dy );
    }else{

        openglContext->pixelInteraction.done(event->common.timestamp, tx, ty );

    }
    if( fingerInteraction->fingerStateDown /*== 1*/ && openglContext->pixelInteraction.dx == 0 && openglContext->pixelInteraction.dy == 0 ){
        // it was a single touch
        //SDL_Log("SDL_FINGERUP was reachd - position did not change" );
        //SDL_Log("MOUSE xy delta %f %f", openglContext->pixelInteraction.dx, openglContext->pixelInteraction.dy );
        // so if we already didInteract....
        // calling triggerInteraction again might change our interaction object....
        // which could be good or mediocure if we are currently doing a drag and drop
        // but in this case we don't want to overwrite our interaction if we already have one

//        // I do not think this following code is needed.... (but may change some things if removed.... this lets us have "mouseup" inetractions without correspondign "mouse down"...
//        if( /*!didInteract*/ !fingerInteraction->didCollideWithObject ){ // didInteract really means didInteractWithUi and when we are, we leave the collected_x and collected_y zero at this time....
//            /*didInteract = */ openglContext->generalUx->triggerInteraction(fingerInteraction, false);
//        }
       // the above actually breaks things... somehow for multitouch pixelInteraction dx and dy are zero and we reach here??? sensibly since nothing moves during zoom??
       // in any case calling interactionComplete below calls triggerInteraction so the redundancy creates and issue
       // where didCollideWithObject becomes set, causing it to appear like a mouse down + mouse up on the same object when in reality its only mouse up
       // TL;DR this all should really only apply (remainign uncommnted code) when not in minigame mode anyway,.....


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
		case USER_EVENT_ENUM::PICK_AGAIN_NOW:
        {
            SDL_Log("USER EVENT - PICK_AGAIN_NOW");
            openglContext->choosePickFromScreen(); // better yet, trigger via general UX press of the correct button??? to bounce it??
            return 0;
        }

        case USER_EVENT_ENUM::PICK_AT_POSITION:
        {
            // SDL_Log("USER EVENT - PICK_AT_POSITION"); // too noisy!
            SDL_Point mmv_result = *((SDL_Point*)event.user.data1);
            if( openglContext->position_x != mmv_result.x || openglContext->position_y != mmv_result.y ){
                openglContext->position_x = mmv_result.x;
                openglContext->position_y = mmv_result.y;
                colorPickState->movedxory = true;
                openglContext->renderShouldUpdate = true; // do not call renderScene from timer thread!
            }
            FREE_FOR_NEW(event.user.data1); // note: perhaps we should allocate this a different way so we can just use SDL_free (windows), keep in mind we allocate for each platform that sends this event...
            return 0;
        }


        case USER_EVENT_ENUM::IDBFS_INITIAL_SYNC_COMPLETED:
        {
            openglContext->setupScene();
            ReshapeWindow();
            return 0;
        }


    }

    //return 1; // not handled, leave it in the queue ?????? only really makes sense if we process events elsewhere right?  tough to say for sure but we'd have to make sure all platforms handle the other events somehow...
    return 0;
}

int EventFilter(void* userdata, SDL_Event* event){

    if( !openglContext->isProgramBooted() ){ // LOADING STILL - tbd we could jsut fitler out some types of events, known crash causing events include mouse interactions presently...
        return 1;
    }

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

                    //openglContext->generalUx->writeOutState(); //no write detected for em

                    return 0;
                case SDL_WINDOWEVENT_SHOWN:
                case SDL_WINDOWEVENT_EXPOSED:
                case SDL_WINDOWEVENT_RESTORED:

                    SDL_Log("\n SDL_WINDOWEVENT_SHOWN || SDL_WINDOWEVENT_EXPOSED || SDL_WINDOWEVENT_RESTORED");
                    openglContext->renderShouldUpdate = true;

                    return 0;


                case SDL_WINDOWEVENT_LEAVE:

                    SDL_Log("\n SDL_WINDOWEVENT_LEAVE");

					// TODO: on windows this doesn't seem to trigger anythign, which is good actually, but we need to (in this case) stop any drag event that is panning...

                    openglContext->generalUx->writeOutState();

                    return 0;

                case SDL_WINDOWEVENT_FOCUS_LOST:

                    SDL_Log("\n SDL_WINDOWEVENT_FOCUS_LOST");

                    openglContext->generalUx->writeOutState();

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

#define makeControllerButtonKeySwitchPartial(providedKeyState) \
        case SDL_CONTROLLER_BUTTON_DPAD_UP: \
            openglContext->keyInteractions.up->providedKeyState(event->cbutton.timestamp); \
            break; \
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN: \
            openglContext->keyInteractions.down->providedKeyState(event->cbutton.timestamp); \
            break; \
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: \
            openglContext->keyInteractions.right->providedKeyState(event->cbutton.timestamp); \
            break; \
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT: \
            openglContext->keyInteractions.left->providedKeyState(event->cbutton.timestamp); \
            break; \
        case SDL_CONTROLLER_BUTTON_A: \
            openglContext->keyInteractions.enter->providedKeyState(event->cbutton.timestamp); \
            break; \

        case SDL_CONTROLLERBUTTONDOWN:{
            SDL_Log("Controller button down %i (SDL_GameControllerButton)", event->cbutton.button);

            switch(event->cbutton.button){
                makeControllerButtonKeySwitchPartial(keydown)
            }

            // this allows the keypress to possibly cancel another keypress that has not yet been released, [[HOWEVER each controller instance should [[really]] have ITS OWN key interaction object]]...... instead of sharing with kbd? (well, fireTV controller generates both KBD and Controlelr events... so THAT ONE should share....) todo find some unifying device id between these?
            openglContext->keyInteractions.someKeyDown(event->cbutton.timestamp);



            openglContext->clearVelocity();
            openglContext->renderShouldUpdate=true;

            break;
        }
        case SDL_CONTROLLERBUTTONUP:
            SDL_Log("Controller button up %i (SDL_GameControllerButton)", event->cbutton.button);

            switch(event->cbutton.button){
                makeControllerButtonKeySwitchPartial(keyup)
            }

            if( event->cbutton.button == SDL_CONTROLLER_BUTTON_A ){
                if( openglContext->keyInteractions.enter->wasNotCanceledByLaterKeypress() ){
                    openglContext->EnterKeyEvent();
                }
            }else if( event->cbutton.button == SDL_CONTROLLER_BUTTON_B ){
                openglContext->BackButtonEvent(); // this can probably work in controller up too.. and maybe we can even cancel that! TODO
            }

            openglContext->clearVelocity();
            openglContext->renderShouldUpdate=true;

            break;

        case SDL_CONTROLLERAXISMOTION:
            SDL_Log("Controller axis motion %i (SDL_GameControllerAxis) value %i", event->caxis.axis, event->caxis.value);
            break;

#ifdef COLORPICK_PLATFORM_DESKTOP
        case SDL_MOUSEWHEEL:
            //SDL_Log("Hello Wheel!!");

            if( openglContext->generalUx->hasCurrentModal() && !openglContext->generalUx->modalWasFalseModal() ){

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

            if( openglContext->generalUx->hasCurrentModal() && !openglContext->generalUx->modalWasFalseModal() ){

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

            if( event->key.repeat ) return 0; // we will handle our own repeats tyvm /s

            // the scancode is easiest to look up in SDL_scancode.h then follow to SDL_keycode.h
            //SDL_Log("1keydown %d %d %s", event->key.keysym.sym, event->key.keysym.scancode,  SDL_GetKeyName(event->key.keysym.sym) );
            openglContext->keyDown(event->key.timestamp, event->key.keysym.sym);

            return 0;
        case SDL_KEYUP:
            // the scancode is easiest to look up in SDL_scancode.h then follow to SDL_keycode.h
            //SDL_Log("1keyup %d %d %s", event->key.keysym.sym, event->key.keysym.scancode,  SDL_GetKeyName(event->key.keysym.sym) );
            openglContext->keyUp(event->key.timestamp, event->key.keysym.sym);

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
//SDL_IsScreenKeyboardShown(SDL_Window *sdl_Window);

            return 0;

        case SDL_TEXTINPUT:
                // handled here for debugging silence on desktop...
            return 0;

        case SDL_APP_LOWMEMORY:{
            SDL_Log("Memory Warning !!!!");
            /* You will get this when your app is paused and iOS wants more memory.
             Release as much memory as possible.
             */

            // WE CAN"T DO MUCH BUT WE CAN SAVE OUR STATE I SUPPOSE?? (OR WILL THIS CRASH US???)

            // maybe destroy animations, all UI objects, and save state?

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
            openglContext->renderShouldUpdate=false;

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
            openglContext->keyInteractions.downCounter = 0;
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

// #ifdef __WIN32__
//             if( event->type == SDL_POLLSENTINEL){
//                 break; // presumably we handled this or will plan to handle it.... above.... for now this makes silence on android....
//             }
// #endif

            if( event->type == SDL_USEREVENT ){
                // handled elsewhere......
                break;//return 0;
            }

		    // this is an INTERNAL event that "Signals the end of an event poll cycle"
            if( event->type == SDL_POLLSENTINEL ){
               break; // noisy console logging
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

    //TODO: confirm - do these events ever reach the event filter?
    // or elaborate on why we insist on processing these here

    // I think the idea is to fire the timer/event in the main thread
    // often from the event thread

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

    if( !openglContext->isProgramBooted() ){ // LOADING STILL (emscripten..) see IDBFS_INITIAL_SYNC_COMPLETED
        SDL_Log("loading....screen....");
        //openglContext->renderUi(); // either one works really...
        ReshapeWindow(); // we won't presently get events though except via js... this makes centering work as the app enters fullscreen
        openglContext->generalUx->updateRenderPositions(); // needed after reshape
        openglContext->renderLoadingUI();
        return SDL_Delay(66); // render slomo...
    }

    //SDL_Log("RENDER SCENE....");

    if( !openglContext->renderShouldUpdate && !openglContext->generalUx->uxAnimations->shouldUpdate ) return SDL_Delay(16);

    openglContext->generalUx->uxAnimations->shouldUpdate = false; // timer will keep reseetting this


    openglContext->renderScene();

#ifdef DEVELOPER_TEST_MODE // NOTE comment this ifdef out if you want to see your rendering pause
    SDL_Log("RENDER SCENE COMPLETED... swapping buffers....");
#endif

    SDL_GL_SwapWindow(sdl_Window); // move into render scene?

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // possibly good idea here?

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

    SDL_GetWindowSize(sdl_Window, &colorPickState->windowWidth, &colorPickState->windowHeight);

    SDL_GL_GetDrawableSize(sdl_Window, &colorPickState->drawableWidth, &colorPickState->drawableHiehgt);

    win_w=colorPickState->windowWidth;
    win_h=colorPickState->windowHeight;

    colorPickState->updateComputedSizes();

    //colorPickState->viewport_ratio = (win_w+1.0f)/win_h;
    SDL_Log("SDL_GetWindowSize %d %d %f", win_w,win_h, colorPickState->viewport_ratio);
    SDL_Log("SDL_GL_GetDrawableSize %d %d %f", colorPickState->drawableWidth,colorPickState->drawableHiehgt, colorPickState->viewport_ratio);

    // NOTE: this log is dangerous since division by zero, and also not dangerous thanks to optimization
    //SDL_Log("SDL_GL_GetDrawableSize %d %d %f", colorPickState->drawableWidth,colorPickState->drawableHiehgt, (colorPickState->drawableWidth+1.0f)/colorPickState->drawableHiehgt);


    if( win_w <= colorPickState->drawableWidth ) {
        // low dpi?
        colorPickState->ui_mmv_scale = 1.0;
    }else{
        colorPickState->ui_mmv_scale = 2.0;
        // even if win_w == SCREEN_WIDTH
        //  not guaranteed to be hidpi !~!!! !
    }
    SDL_Log("UI Retna Density Scale:  %f", colorPickState->ui_mmv_scale );


    //colorPickState->ui_mmv_scale=ui_mmv_scale;


#ifdef DEVELOPER_TEST_MODE
    
    int displayIndex = SDL_GetWindowDisplayIndex(sdl_Window);
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

    for( int modeIndex=0,totalModes=SDL_GetNumDisplayModes(displayIndex); modeIndex<totalModes; modeIndex++){
        SDL_GetDisplayMode(displayIndex, modeIndex, &mode);

        SDL_Log("POSSIBLE: w:%i h:%i %ihz ", mode.w, mode.h, mode.refresh_rate );
    }
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
            size_result, miScreen.x, miScreen.y, miScreen.w, miScreen.h, colorPickState->ui_mmv_scale,
            bounds_result, usableBounds.x, usableBounds.y, usableBounds.w, usableBounds.h,
            int(miScreen.w*colorPickState->ui_mmv_scale), int(miScreen.h*colorPickState->ui_mmv_scale));
#endif
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
    SDL_SetHint(SDL_HINT_VIDEO_ALLOW_SCREENSAVER, "1");
    SDL_SetHint(SDL_HINT_MAC_BACKGROUND_APP, "0");  // as far as I can tell.... this only makes the window not re-enter the background once focused - and also becomes incapable of entering the forground (no menu bar)
#endif
    //SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");

    SDL_Log("hints -----------------------");



    //    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "1");
#ifdef __ANDROID
//    SDL_SetHint(SDL_HINT_ANDROID_TRAP_BACK_BUTTON, "1"); // its trapped by default still as of SDL2-2.0.9 (even though docs say otherwise...)
//    SDL_SetHint(SDL_ANDROID_SEPARATE_MOUSE_AND_TOUCH, "1");
//    SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "1");
#endif


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

    SDL_Log("init -----------------------");

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

    SDL_Log("joy -----------------------");

    /* seed random number generator */
    srand((int)time(NULL));

//    srand((unsigned int)time(NULL));
//    srand((unsigned int)clock());


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
    SDL_Log("ticks -----------------------");

#if !defined(__ANDROID__) && !defined(__EMSCRIPTEN__) 
    // OES (egl?) is already core?? (ish?)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

#else
    // for the else, we are using _ES profile for sure (pretty much) not sure if it eneds to be set though....
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2); // we get 3.0 0n some phoens anyway
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

#endif

    SDL_Log("stencil -----------------------");

//    SDL_Log("setting depth size....");
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // NOTE: some CRUMMY_ANDROID may not support 24 here?
    

    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

//    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,0);
//    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);

//    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
//    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
//    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);


//    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
//    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
//    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
//    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // our UI texture NEEDS alpha... we won't get it without this on some devices (k3, moto 4 play) (commenting out all 4 of these works though)
// as nice as forcing 32bit color might be on some android... it breaks others (even with alpha size set)... we can just rely on nicer devices doing better automatically instead!

    //    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0); // nifty if we can have alpha these days in the screen buffer....
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
#ifdef __MACOSX__
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 4);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
#endif

    // BAD!!!!  check SDL_WINDOWEVENT_MOVED maybe and store this?  or forget it...
    win_pos_x=10;
    win_pos_y=30;
#endif

    SDL_Log("window main -----------------------");

    /* create window and renderer */
    sdl_Window =
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
    if (!sdl_Window) {
        printf("Could not initialize Window\n");
        SDL_Log("%s", SDL_GetError());
        return 1;
    }



    colorPickState->viewport_ratio = (win_w+1.0f)/win_h;


//    renderer = SDL_CreateRenderer(sdl_Window, -1, 0);
//    if (!renderer) {
//        printf("Could not create renderer\n");
//        return 1;
//    }

    SDL_Log("main -----------------------");





    result = openglContext->createContext(sdl_Window);
    if( !result ){
        printf("ERROR: Could not create context\n");
        SDL_Log("%s", SDL_GetError());
        return 1;
    }

    SDL_Log("context created -----------------------");
#ifdef __WINDOWS__
    GLenum error = glewInit(); // Enable GLEW
    if (error != GLEW_OK) {// If GLEW fails
        printf("glew init failed\n");
        return false;
    }else{
        SDL_Log("glew init ok -----------------------");
    }
#endif

// #ifdef __WINDOWS__
//     WIN_GL_InitExtensions(nullptr);
// #endif

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
    GLint resultInt = 0;


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

    if( !openglContext->canMesh() ){
        openglContext->meshes->mesh3d_enabled = false;
    }

#define logGottenGlAtrib(literalAttrib) SDL_GL_GetAttribute(literalAttrib, &resultInt); \
SDL_Log("contexts %s %i", #literalAttrib, resultInt);


#define logGottenGlString(literalAttrib) SDL_Log("glcontexts %s %s", #literalAttrib, glGetString(literalAttrib));

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

    logGottenGlAtrib(SDL_GL_CONTEXT_EGL);
    logGottenGlAtrib(SDL_GL_CONTEXT_FLAGS);
    logGottenGlAtrib(SDL_GL_CONTEXT_PROFILE_MASK);
    logGottenGlAtrib(SDL_GL_ACCELERATED_VISUAL);

    logGottenGlAtrib(SDL_GL_CONTEXT_MAJOR_VERSION);
    logGottenGlAtrib(SDL_GL_CONTEXT_MINOR_VERSION);

    logGottenGlAtrib(SDL_GL_MULTISAMPLEBUFFERS);
    logGottenGlAtrib(SDL_GL_MULTISAMPLESAMPLES);

    logGottenGlString(GL_EXTENSIONS);
    logGottenGlString(GL_VENDOR);
    logGottenGlString(GL_RENDERER);
    logGottenGlString(GL_VERSION);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &resultInt);
    SDL_Log("Max Texture Size %i", resultInt); // BIG TODO HERE - we can by dyanmic about this :)

    int maxSupportedTextureSize = resultInt;
    if( maxSupportedTextureSize > 0 ){
        if(maxSupportedTextureSize < 2048){ // TODO move this messagebox code someplace else!

            int selected;
            SDL_MessageBoxData messagebox;
            SDL_MessageBoxButtonData buttons[] = {
//                    {   0,  SDL_ASSERTION_RETRY,            "Continue" },
                {   SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, SDL_ASSERTION_IGNORE,           "Ignore" },
//                    {   SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT,  SDL_ASSERTION_BREAK,            "Report" },
                {   0,  SDL_ASSERTION_ABORT,            "Quit" }
            };

            SDL_zero(messagebox);
            messagebox.flags = SDL_MESSAGEBOX_WARNING;
            messagebox.window = sdl_Window;
            messagebox.title = "Error - 2048x Textures Required";
            messagebox.message = "ColorPick Requires support for 2048 pixel textures.";// "test message1";
            messagebox.numbuttons = SDL_arraysize(buttons);
            messagebox.buttons = buttons;
            if (SDL_ShowMessageBox(&messagebox, &selected) == 0) {
                //SDL_Log("--- itz %d", selected);

                if( selected == SDL_ASSERTION_ABORT ){
                    // quit...
                    SDL_Quit();
                    exit(1);
                    return 0;
//                    }else if(selected == SDL_ASSERTION_BREAK ){
//                        // report
//                        const char* urlBase = "http://www.vidsbee.com/Contact/?browserinfo=App:NativeColorPick";
//                        SDL_snprintf(shader_error_report, REPORT_SIZE, "%s:%s\n%s", urlBase, file, buffer);
//                        ogg->doOpenURL(shader_error_report); // TODO: we really need to defer this....
                }else if(selected == SDL_ASSERTION_IGNORE ){
//                        ogg->no_more_shader_message_boxes=true;
                }else{
                    // shrug SDL_ASSERTION_RETRY
                }
            }

        }
    }

        /*
         #define glGenVertexArrays glGenVertexArraysOES // danger - use will break rendering on fire TV
         #define glBindVertexArray glBindVertexArrayOES // danger
         #define glDeleteVertexArrays glDeleteVertexArraysOES // d
         */
    if (SDL_GL_ExtensionSupported("glBindVertexArrayOES")) {
        SDL_Log("We apparently have glBindVertexArrayOES" );
    }
    if (SDL_GL_ExtensionSupported("glGenVertexArraysOES")) {
        SDL_Log("We apparently have glGenVertexArraysOES" );
    }
    if (SDL_GL_ExtensionSupported("glBindVertexArray")) {
        SDL_Log("We apparently have glBindVertexArray" );
    }
    if (SDL_GL_ExtensionSupported("glGenVertexArrays")) {
        SDL_Log("We apparently have glGenVertexArrays" );
    }
    if (SDL_GL_ExtensionSupported("GL_EXT_glBindVertexArray")) {
        SDL_Log("We apparently have GL_EXT_glBindVertexArray" );
    }
    if (SDL_GL_ExtensionSupported("GL_EXT_glGenVertexArrays")) {
        SDL_Log("We apparently have GL_EXT_glGenVertexArrays" );
    }
    if (SDL_GL_ExtensionSupported("GL_EXT_glBindVertexArrayOES")) {
        SDL_Log("We apparently have GL_EXT_glBindVertexArrayOES" );
    }
    if (SDL_GL_ExtensionSupported("GL_EXT_glGenVertexArraysOES")) {
        SDL_Log("We apparently have GL_EXT_glGenVertexArraysOES" );
    }


        //        typedef void (APIENTRY * glDebugMessageCallbackKHR_Func)(GLDEBUGPROCKHR callback, const void *userParam);
        //        glDebugMessageCallbackKHR_Func glDebugMessageCallbackKHR_ptr = 0;
        //
        //        glDebugMessageCallbackKHR_ptr = (glDebugMessageCallbackKHR_Func) SDL_GL_GetProcAddress("glDebugMessageCallbackKHR");
        //        if (glDebugMessageCallbackKHR_ptr){
        //            SDL_Log("found dat b");
        //            glDebugMessageCallbackKHR_ptr(on_gl_error, NULL);
        //        }
        //
        //        // Use that extension


#ifdef USE_EVENT_WATCH_FOR_EVENTS
    SDL_AddEventWatch(EventFilter, nullptr); // second param is provided to filter which runs in different thread... void* userdata
#endif

    openglContext->loadShadersAndRenderPrerequisites();


// TBD: evaluate use of IDBFS in the extension too??? this has to be toggled several places (here and in UX)
#if defined(__EMSCRIPTEN__) && !defined(COLORPICK_BUILD_FOR_EXT)

char* pref_path_alloc = openglContext->generalUx->GetPrefPath();

EM_ASM({
    var js_prefs_path = UTF8ToString($0).replace(/\\/$/, "");
    //js_prefs_path = "/libsdl";
    console.log("Note: Enabling IDBFS "+js_prefs_path);
    FS.mkdir(js_prefs_path);
    FS.mount(IDBFS, {autoPersist: true}, js_prefs_path);
    //console.log("Note: enabled IDBFS "+js_prefs_path);

    // populate the memfs with the IDBFS files
    FS.syncfs(true, function (err) {
      // handle callback
        console.log("Note: enabled IDBFS (Indexed DB based settings store) synced with POPULATE TRUE, now loading prefrences & color palette history...");
        __Z21try_reading_prefs_nowii();
        //  we should show loading screen until this occurs instead...
    });
},pref_path_alloc);

SDL_free(pref_path_alloc);

SDL_Log("IDBFS enabled now...");

#endif


#if !defined(__EMSCRIPTEN__) || defined(COLORPICK_BUILD_FOR_EXT)
        // we'll call this later when the IDBFS is synced to memmory for the first time... trace IDBFS_INITIAL_SYNC_COMPLETED
    openglContext->setupScene();
#else
    openglContext->createLoadingUI();
#endif

    ReshapeWindow();


    SDL_EnableScreenSaver(); // this may set some of the above hints automagically


#ifdef COLORPICK_MISSING_MAIN_LOOP

#ifdef __EMSCRIPTEN__
    // void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
    //emscripten_set_main_loop(ShowFrame, 60, 1); 

    emscripten_set_main_loop(ShowFrame, 0, 1);
#else

    //SDL_iPhoneSetEventPump(SDL_TRUE);
    SDL_iPhoneSetAnimationCallback(sdl_Window, 1, ShowFrame, NULL);
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
    SDL_DestroyWindow(sdl_Window);
    SDL_Quit();

//
//#ifdef __WIN32__
//	SDL_Delay(250); //to allow other threads to cease?  stack corruption could be avoided?
//#endif

#endif

    return 0;
}

