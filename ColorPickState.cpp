//
//  ColorPick.c
//  ___PROJECTNAME___
//
//  Created by Sam Larison on 8/24/15.
//
//

#include "ColorPickState.h"

//ColorPickState::pInstance = NULL;
//bool ColorPickState::ms_bInstanceCreated = false;
//ColorPickState* ColorPickState::pInstance = NULL;
//
//ColorPickState* ColorPickState::Singleton() {
//    if(!ms_bInstanceCreated){
//        pInstance = new ColorPickState();
//        ms_bInstanceCreated=true;
//    }
//    return pInstance;
//}
//
//
//ColorPickState::ColorPickState(void) {
//    //constructor
//    mmovex=0;
//    mmovey=0;
//
//}
//



ColorPickState* ColorPickState::Singleton() {
    if(!cps_ms_bInstanceCreated){
        cps_pInstance = new ColorPickState();
        cps_ms_bInstanceCreated=true;
    }
    return cps_pInstance;
}

ColorPickState::ColorPickState(void) {
    //constructor
    mmovex=0;
    mmovey=0;

    appInForeground = true;

    viewport_ratio = 1.0;

}
