/*
 *  rectangles.c
 *  written by Holmes Futrell
 *  use however you want
 */

#include "main.h"
#include "ColorPick.h" // includes openglContext
#include <time.h>



void DebugStr(const char *c_str ) {
    SDL_Log("%c", c_str);
}

void debugGLerror(){
    debugGLerror("default-error-identifier");
}

void debugGLerror(const char *c_str_label){
    GLenum errorno = 1;
    while( errorno != GL_NO_ERROR && errorno ){
        errorno = glGetError();
        if( errorno != GL_NO_ERROR ){
            SDL_Log("GL Error %d Labeled: %s", errorno, c_str_label);
            switch(errorno){
                case GL_INVALID_ENUM:
                    SDL_Log("GL_INVALID_ENUM");
                    break;
                case GL_INVALID_VALUE:
                    SDL_Log("GL_INVALID_VALUE");
                    break;
                case GL_INVALID_OPERATION:
                    SDL_Log("GL_INVALID_OPERATION");
                    break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:
                    SDL_Log("GL_INVALID_FRAMEBUFFER_OPERATION");
                    break;
                case GL_OUT_OF_MEMORY:
                    SDL_Log("GL_OUT_OF_MEMORY");
                    break;
//                case GL_STACK_UNDERFLOW:
//                    SDL_Log("GL_STACK_UNDERFLOW");
//                    break;
//                case GL_STACK_OVERFLOW:
//                    SDL_Log("GL_STACK_OVERFLOW");
//                    break;
                default:
                    SDL_Log("Open GL: Ohter error...");
                    break;
            }
        }
    }
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


bool mousStateDown = false; // really should count fingers down possibly, finger event support multi touch, mosue events dont!
bool multiTouchMode = false;
int EventFilter(void* userdata, SDL_Event* event){
    switch ( event->type ){
        case SDL_WINDOWEVENT:
        {
            switch( event->window.event ){
                case SDL_WINDOWEVENT_RESIZED:
                    //SDL_Log("RESIZED (lowdpi size wouldbe) %d %d", event->window.data1,event->window.data2);
                    ReshapeWindow();
                    return 0;
                case SDL_WINDOWEVENT_MINIMIZED:
                    break;
            }
        }
        case SDL_MULTIGESTURE: // http://lazyfoo.net/tutorials/SDL/55_multitouch/index.php
            // this event triggers on desktop when entering/leaving window!
            SDL_Log("Hello Gesture!!");

            openglContext->renderShouldUpdate = true;
            openglContext->setFishScale(event->mgesture.dDist);

            return 0;


        case SDL_FINGERDOWN:
            if( mousStateDown ){
                //second touch....
                SDL_Log("SDL_FINGERDOWN second touch");
                mousStateDown = 0;
                return 0;
            }
            openglContext->clearVelocity(); // stop any active panning
            mousStateDown = 1;
            //SDL_SetRelativeMouseMode(SDL_TRUE);
            //SDL_GetRelativeMouseState(&colorPickState->mmovex, &colorPickState->mmovey);

            int tx,ty;
            SDL_GetRelativeMouseState(&tx, &ty);
            collected_x =0;
            collected_y =0;
            input_velocity_x = 0;
            input_velocity_y = 0;

            SDL_Log("SDL_FINGERDOWN");




            SDL_GetMouseState(&tx, &ty);
            SDL_Log("MOUSE xy %d %d", tx,ty);
            openglContext->generalUx->currentInteraction.begin( (tx*ui_mmv_scale)/win_w, (ty*ui_mmv_scale)/win_h );
            SDL_Log("MOUSE xy perc %f %f", openglContext->generalUx->currentInteraction.px, openglContext->generalUx->currentInteraction.py );

            didInteract = openglContext->generalUx->triggerInteraction();



            return 0;
        case SDL_FINGERMOTION:
            if( mousStateDown ){

                if( !didInteract ){



                    SDL_GetRelativeMouseState(&colorPickState->mmovex, &colorPickState->mmovey);


                    // velocity has a multiplier the closer
                    //OpenGLContext->fishEyeScalePct is to zero
                    // below 0.1
                    //
                    if( openglContext->fishEyeScalePct < 0.05 ){
                        float factor = (0.05 - openglContext->fishEyeScalePct);
                        colorPickState->mmovex *= 1 + (80.0 * factor);
                        colorPickState->mmovey *= 1 + (80.0 * factor);
                    }

                    // todo some equivilent near other exterme end
                    // where movements are scaled down?

                    collected_x += colorPickState->mmovex;
                    collected_y += colorPickState->mmovey;

   //TODO                 // there is a FLAW here - start dragging in one direction, then change direction AND release before reachign the origin point... it will fly in teh wrong direction
                    // this is because the collected_x is really just dx (difference from origin) and not true measure of velocity
                    // take a loook at uiInteraction........

                    input_velocity_x +=collected_x*0.1; //TIMING
                    input_velocity_y +=collected_y*0.1;

                    SDL_Log("MOUSE xy %d %d", colorPickState->mmovex,colorPickState->mmovey);
                    openglContext->renderShouldUpdate = true;

                }else{
                //colorPickState->mmovex = event->motion.xrel;
                //colorPickState->mmovey = event->motion.yrel;

//                    SDL_GetRelativeMouseState(&colorPickState->mmovex, &colorPickState->mmovey);
//                    collected_x += colorPickState->mmovex;
//                    collected_y += colorPickState->mmovey;


                    SDL_GetMouseState(&tx, &ty);
                    SDL_Log("MOUSE xy %d %d", tx,ty);
                    openglContext->generalUx->currentInteraction.update((tx*ui_mmv_scale)/win_w, (ty*ui_mmv_scale)/win_h,  (collected_x*ui_mmv_scale)/win_w, (collected_y*ui_mmv_scale)/win_h );
                    SDL_Log("MOUSE xy perc %f %f", openglContext->generalUx->currentInteraction.px, openglContext->generalUx->currentInteraction.py );
                    SDL_Log("MOUSE xy delta %f %f", openglContext->generalUx->currentInteraction.dx, openglContext->generalUx->currentInteraction.dy );


                    // todo combine above into the following call?
                    openglContext->generalUx->interactionUpdate(&openglContext->generalUx->currentInteraction); // todo whyarg, BETTERARG
                    // todo the above would appear to return a "should update??"

                    openglContext->renderShouldUpdate = true; /// TODO interactionUpdate calls generalUx->updateRenderPositions() should cause the UI to update... for now we just redraw everything

                    SDL_Log("MOUSE DELTA xy %d %d", collected_x,collected_y);
                }

            }
            return 0;
        case SDL_FINGERUP: // http://lazyfoo.net/tutorials/SDL/54_touches/index.php
            if( mousStateDown && collected_x == 0 && collected_y == 0 ){
                // it was a single touch
                SDL_Log("SDL_FINGERUP was reach - see if position did not change %d, %d",collected_x,collected_y );

                // we may be able to add this, but we need to track velocity better
                SDL_GetMouseState(&tx, &ty);
                SDL_Log("MOUSE xy %d %d", tx,ty);
                openglContext->generalUx->currentInteraction.update((tx*ui_mmv_scale)/win_w, (ty*ui_mmv_scale)/win_h,  (collected_x*ui_mmv_scale)/win_w, (collected_y*ui_mmv_scale)/win_h );
                SDL_Log("MOUSE xy perc %f %f", openglContext->generalUx->currentInteraction.px, openglContext->generalUx->currentInteraction.py );
                SDL_Log("MOUSE xy delta %f %f", openglContext->generalUx->currentInteraction.dx, openglContext->generalUx->currentInteraction.dy );


                // so if we already didInteract....
            // calling triggerInteraction again might change our interaction object....
                // which could be good or mediocure if we are currently doing a drag and drop
                // but in this case we don't want to overwrite our interaction if we already have one

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
                    openglContext->chooseFile(); // this breaks multigesture somehow

                    // this feature will move

                }else{
                    //
                    //openglContext->generalUx->interactionUpdate(uiInteraction *delta);

                }


            }else{
                openglContext->triggerVelocity(input_velocity_x, input_velocity_y);
            }
            mousStateDown = 0;
            didInteract = false;


            didInteract = openglContext->generalUx->interactionComplete(&openglContext->generalUx->currentInteraction); // MOUSEUP

            // at this point we culd really need to render an update
            if( didInteract ){
                SDL_Log("MARKED FOR UPDATE - WILL IT RENDER????");
                openglContext->renderShouldUpdate=true;
                // this seems to not guarantee render?!
            }


            //SDL_SetRelativeMouseMode(SDL_FALSE); // bit unsure if this mouse stuff will work for touch
            //SDL_GetRelativeMouseState(&colorPickState->mmovex, &colorPickState->mmovey);

            SDL_Log("SDL_FINGERUP");

            return 0;

        case SDL_KEYDOWN:
            //openglContext->keyDown(event->key.keysym.sym);
            return 0;
        case SDL_KEYUP:
            //SDL_Log("key %d", event->key.keysym.sym);
            //openglContext->keyUp(event->key.keysym.sym);
            return 0;
//        case SDL_MOUSEBUTTONDOWN:
//        //    mousStateDown = 1;
//            //SDL_SetRelativeMouseMode(SDL_TRUE);
//            //SDL_GetRelativeMouseState(&colorPickState->mmovex, &colorPickState->mmovey);
//
//            return 0;
//        case SDL_MOUSEMOTION:
//            // SDL_Log("Mouse Motion X,Y %d,%d", event->motion.xrel, event->motion.yrel);
//            if( mousStateDown ){
//                //SDL_GetRelativeMouseState(&colorPickState->mmovex, &colorPickState->mmovey);
//                //colorPickState->mmovex = event->motion.xrel;
//                //colorPickState->mmovey = event->motion.yrel; // still like simplicity of these, not working well once SDL_SetRelativeMouseMode(SDL_TRUE), which captures hte cursor nicely and is OK in window mode....
//            }
//            return 0;
//        case SDL_MOUSEBUTTONUP:
//        //    mousStateDown = 0;
//            //SDL_SetRelativeMouseMode(SDL_FALSE);
//            //SDL_GetRelativeMouseState(&colorPickState->mmovex, &colorPickState->mmovey);
//
//            return 0;

        case SDL_APP_LOWMEMORY:
            SDL_Log("Memory Warning !!!!");
            /* You will get this when your app is paused and iOS wants more memory.
             Release as much memory as possible.
             */
            return 0;

        case SDL_APP_TERMINATING:
            /* Terminate the app.
             Shut everything down before returning from this function.
             */
           // openglContext->generalUx->writeOutState();
            SDL_Log("SDL_APP_TERMINATING !!!!");
            return 0;

        case SDL_APP_WILLENTERBACKGROUND:
            /* Prepare your app to go into the background.  Stop loops, etc.
             This gets called when the user hits the home button, or gets a call.
             */
            openglContext->renderShouldUpdate = false;

            openglContext->generalUx->writeOutState();
            SDL_Log("SDL_APP_WILLENTERBACKGROUND !!!!");
            return 0;
        case SDL_APP_DIDENTERBACKGROUND:
            /* This will get called if the user accepted whatever sent your app to the background.
             If the user got a phone call and canceled it, you'll instead get an SDL_APP_DIDENTERFOREGROUND event and restart your loops.
             When you get this, you have 5 seconds to save all your state or the app will be terminated.
             Your app is NOT active at this point.
             */
            SDL_Log("SDL_APP_DIDENTERBACKGROUND !!!!");
            return 0;
        case SDL_APP_WILLENTERFOREGROUND:
            /* This call happens when your app is coming back to the foreground.
             Restore all your state here.
             */
            openglContext->renderShouldUpdate = true;
            SDL_Log("SDL_APP_WILLENTERFOREGROUND !!!!");
            return 0;
        case SDL_APP_DIDENTERFOREGROUND:
            /* Restart your loops here.
             Your app is interactive and getting CPU again.
             */
            SDL_Log("SDL_APP_DIDENTERFOREGROUND !!!!");
            return 0;

        default:
            SDL_Log("unrecognized event");
            break;
            
    }
    return 1;/* No special processing, add it to the event queue */
}

void ShowFrame(void*)
{

//    currentTime = SDL_GetTicks();
//    openglContext->updateFrame(currentTime - lastTimerTime);




    if( !openglContext->renderShouldUpdate && !openglContext->generalUx->uxAnimations->shouldUpdate ) return SDL_Delay(1);

    openglContext->generalUx->uxAnimations->shouldUpdate = false; // timer will keep reseetting this

// TODO: TIMING!!!!

    input_velocity_x *= 0.1;
    input_velocity_y *= 0.1; // debatably we need to use timing, and always decrease these unless they are below the threshold


    openglContext->renderScene();



    SDL_GL_SwapWindow(window); // move into render scene?
}


void ReshapeWindow(){
    // just in case we didn't get hi-dpi from SDL_WINDOW_ALLOW_HIGHDPI we can see what size we actually got here

    SDL_GetWindowSize(window, &colorPickState->windowWidth, &colorPickState->windowHeight);

    win_w=colorPickState->windowWidth;
    win_h=colorPickState->windowHeight;

    colorPickState->viewport_ratio = (win_w+1.0f)/win_h;
    SDL_Log("SDL_GetWindowSize %d %d %f", win_w,win_h, colorPickState->viewport_ratio);

    SDL_GL_GetDrawableSize(window, &colorPickState->drawableWidth, &colorPickState->drawableHiehgt);

    openglContext->reshapeWindow(colorPickState->drawableWidth, colorPickState->drawableHiehgt);

    SDL_Log("SDL_GL_GetDrawableSize %d %d %f", colorPickState->drawableWidth,colorPickState->drawableHiehgt, (colorPickState->drawableWidth+1.0f)/colorPickState->drawableHiehgt);


    if( win_w < SCREEN_WIDTH ) {
        // low dpi?
        ui_mmv_scale = 1.0;
    }else{
        // even if win_w == SCREEN_WIDTH
        //  not guaranteed to be hidpi !~!!! !
    }

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


    int modeIndex=0;
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
            size_result, miScreen.x, miScreen.y, miScreen.w, miScreen.h, ui_mmv_scale,
            bounds_result, usableBounds.x, usableBounds.y, usableBounds.w, usableBounds.h,
            int(miScreen.w*ui_mmv_scale), int(miScreen.h*ui_mmv_scale));

    //if( )

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

    /* initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0) {
        printf("Could not initialize SDL\n");
        return 1;
    }

    /* seed random number generator */
    srand(time(NULL));


//    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
//    SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 0);



    /* create window and renderer */
    window =
        SDL_CreateWindow(NULL, 0, 0, win_w, win_h,
                         SDL_WINDOW_OPENGL
                         | (TARGET_OS_SIMULATOR?0:SDL_WINDOW_ALLOW_HIGHDPI)
                       //  | SDL_WINDOW_ALLOW_HIGHDPI
                         | SDL_WINDOW_RESIZABLE
                         //| SDL_WINDOW_MAXIMIZED
                         //| SDL_WINDOW_BORDERLESS
        );
    if (!window) {
        printf("Could not initialize Window\n");
        return 1;
    }

    //SDL_SetHint(SDL_HINT_VIDEO_ALLOW_SCREENSAVER, "1");
    //SDL_HINT_IDLE_TIMER_DISABLED
    SDL_EnableScreenSaver();

    colorPickState->viewport_ratio = (win_w+1.0f)/win_h;


//    renderer = SDL_CreateRenderer(window, -1, 0);
//    if (!renderer) {
//        printf("Could not create renderer\n");
//        return 1;
//    }

    SDL_Log("main -----------------------");



    SDL_AddEventWatch(EventFilter, nullptr); // second param is provided to filter which runs in different thread... void* userdata




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
                    return 0;
            }


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

