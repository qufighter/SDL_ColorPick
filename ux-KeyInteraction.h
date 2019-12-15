//
//  uiKeyInteraction
//  ColorPick SDL
//
//  Created by Sam Larison on 12/5/19.
//
//


#ifndef __ColorPick_iOS_SDL__uiKeyInteraction__
#define __ColorPick_iOS_SDL__uiKeyInteraction__

#define INITIAL_KEYDOWN_DELAY 400

// TODO: each keyboard, or controller, should use it's own uiKeyInteractions PROBABLY... unless its severly cooperative gameplay type!!!
//       (well, fireTV controller generates both KBD and Controller events... so THAT ONE should share.... NBD if it doesn't just some keys won't be able to cancel others!)

struct uiKeyInteractions // used to track interactions that repeat, may also be triggered by controller keys because why not!
{

    struct uiKeyInteraction{ // used to track indiviual "keys" which in reality may be toggled by multiple "keys"....

        uiKeyInteraction(uiKeyInteractions* p_parent){
            parent = p_parent;
            defaults();
        }

        void defaults(){
            timestamp = 0;
            is_new = false;
            was_new = false;
        }

        void keydown(Uint32 time){
            if( timestamp == 0 ){
                timestamp = time + INITIAL_KEYDOWN_DELAY; // idea: for inital delay, populate in future?
                is_new = true;
                parent->downCounter++;
            }
            keydown_timestamp = time;
            // parent->lastKeyDownTime = time; this would be a different mode, where we only count "known keys" downtime....
        }

        void keyup(Uint32 time){
            if( timestamp != 0 ){
                defaults();
                if( parent->downCounter > 0 ) parent->downCounter--; // Uint32 would loop to large nubmers, better safe than sorry...
            }
        }

        bool isPressed(Uint32 time){
            if(!timestamp) return false;
            int dist = time - timestamp;
            was_new = is_new;
            if( dist > 0 ){
                timestamp = time;
                return true;
            }else{
                if( is_new ){
                    is_new=false;
                    return true;
                }
                return false; // no time elapsed
            }
        }

        bool wasCanceledByLaterKeypress(){
            return parent->lastKeyDownTime > keydown_timestamp;
        }

        bool wasNotCanceledByLaterKeypress(){
            return parent->lastKeyDownTime <= keydown_timestamp;
        }

        Uint32 timestamp;
        Uint32 keydown_timestamp;
        bool is_new;
        bool was_new;
        uiKeyInteractions* parent;
    }; //end struct uiKeyInteraction

    // uiKeyInteractions continuedß
    Uint32 downCounter;
    Uint32 lastKeyDownTime;

    uiKeyInteraction* up;
    uiKeyInteraction* down;
    uiKeyInteraction* left;
    uiKeyInteraction* right;
    uiKeyInteraction* enter;
    uiKeyInteraction* zoomIn;
    uiKeyInteraction* zoomOut;

    uiKeyInteractions(){
        downCounter = 0;
        up =      new uiKeyInteraction(this);
        down =    new uiKeyInteraction(this);
        left =    new uiKeyInteraction(this);
        right =   new uiKeyInteraction(this);
        enter =   new uiKeyInteraction(this);
        zoomOut = new uiKeyInteraction(this);
        zoomIn =  new uiKeyInteraction(this);
    };

#define makeKeysatetSwitch(keystateFnName) \
    switch(k){\
        case SDLK_UP: \
            up->keystateFnName(timestamp); \
            break; \
        case SDLK_DOWN: \
            down->keystateFnName(timestamp); \
            break; \
        case SDLK_RIGHT: \
            right->keystateFnName(timestamp); \
            break; \
        case SDLK_LEFT: \
            left->keystateFnName(timestamp); \
            break; \
        case SDLK_RETURN: \
        case SDLK_KP_ENTER: \
            enter->keystateFnName(timestamp); \
            break; \
        case SDLK_AUDIOREWIND: \
        case SDLK_KP_MINUS: \
            zoomOut->keystateFnName(timestamp); \
            break; \
        case SDLK_AUDIOFASTFORWARD: \
        case SDLK_KP_PLUS: \
            zoomIn->keystateFnName(timestamp); \
            break; \
    }

    void someKeyDown(Uint32 timestamp){ //cheaper since it avoids switch below, useful for controllers where we are already switching the key codes....
        lastKeyDownTime = timestamp;
    }

    void keyDown(Uint32 timestamp, SDL_Keycode k){
        someKeyDown(timestamp);
        makeKeysatetSwitch(keydown)
    }

    void keyUp(Uint32 timestamp, SDL_Keycode k){
        makeKeysatetSwitch(keyup)
    }

    float downDuration(Uint32 timestamp, float ceiling_seconds, float multiplier){
        // returns 1.0 or if keys pressed,
        // returns a number from 1.0 - ceiling_seconds
        // you define now many seconds it takes to achieve max speed of seconds * multiplier
        if( hasPressedKeys() ){

            float elapsed = (timestamp - lastKeyDownTime) * 0.001f;
            if( elapsed > ceiling_seconds ){
                elapsed = ceiling_seconds;
            }
            // now we COULD scale this so its from 0.0 - 1.0 but the low side is useless, and multipler scales it anyway
            return elapsed * multiplier;
            if( elapsed < 1.0 ){
                elapsed = 1.0;
            }
        }
        return 1.0;
    }

    bool hasPressedKeys(){
        return downCounter > 0;
    }

};// uiKeyInteraction;

#endif
