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

struct uiKeyInteractions;//{
//    void incrementDownCount();
//    void decrementDownCount();
//}; // forward decl




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
        }

        void keydown(Uint32 time){
            if( timestamp == 0 ){
                timestamp = time + INITIAL_KEYDOWN_DELAY; // idea: for inital delay, populate in future?
                is_new = true;
                parent->downCounter++;
            }
        }

        void keyup(Uint32 time){
            if( timestamp != 0 ){
                defaults();
                parent->downCounter--;
            }
        }

        bool isPressed(Uint32 time){
            if(!timestamp) return false;
            int dist = time - timestamp;
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

        Uint32 timestamp;
        bool is_new;
        uiKeyInteractions* parent;
    };


    Uint32 downCounter;

    uiKeyInteraction* up;
    uiKeyInteraction* down;
    uiKeyInteraction* left;
    uiKeyInteraction* right;

    uiKeyInteraction* zoomIn;
    uiKeyInteraction* zoomOut;

    uiKeyInteractions(){
        downCounter = 0;
        up =      new uiKeyInteraction(this);
        down =    new uiKeyInteraction(this);
        left =    new uiKeyInteraction(this);
        right =   new uiKeyInteraction(this);
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
        case SDLK_AUDIOREWIND: \
        case SDLK_KP_MINUS: \
            zoomOut->keystateFnName(timestamp); \
            break; \
        case SDLK_AUDIOFASTFORWARD: \
        case SDLK_KP_PLUS: \
            zoomIn->keystateFnName(timestamp); \
            break; \
    }

    void keyDown(Uint32 timestamp, SDL_Keycode k){
        makeKeysatetSwitch(keydown)
    }

    void keyUp(Uint32 timestamp, SDL_Keycode k){
        makeKeysatetSwitch(keyup)
    }

    bool hasPressedKeys(){
        return downCounter > 0;
    }

};// uiKeyInteraction;

#endif
