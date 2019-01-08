//
//  uiHueGradient.h
//  ColorPick SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiControlAchievementToggle_h
#define ColorPick_iOS_SDL_uiControlAchievementToggle_h


struct uiControlAchievementToggle{

    uiControlAchievementToggle(uiObject* parentObj, const char* labelTxt){

        Ux* uxInstance = Ux::Singleton(); // some useful helper?

        uiObjectItself = new uiObject();

        uiObjectItself->myUiController = this; // this propagates to the other child objects


        rightSquare = new uiObject();
        rightSquare->setBoundaryRect(0.75, 0.0, 0.25, 1.0);
        rightSquare->squarify();
        rightSquare->stackRight();
        rightSquare->setModeWhereChildCanCollideAndOwnBoundsIgnored();
        uiObjectItself->addChild(rightSquare);


//        rightSquare->hasBackground = true;
//        Ux::setColor(&rightSquare->backgroundColor, 255, 0, 0, 255);
        rightSquare->hasForeground = true;
        Ux::setColor(&rightSquare->foregroundColor, 128, 128, 128, 255);
//        uxInstance->printCharToUiObject(rightSquare, CHAR_CANCEL_ICON, DO_NOT_RESIZE_NOW);


        labelText = new uiText(uiObjectItself, 0.09);
        labelText->print(labelTxt);

        updateState(false);

        parentObj->addChild(uiObjectItself);

        resize();
    }

    uiObject* uiObjectItself; // no real inheritance here, this its the uiSqware, I would use self->

    uiObject* rightSquare;

    uiText *labelText;

    bool checkedState;

    Uint8 value(){
        return (Uint8)checkedState;
    }

    void setValue(Uint8 providedVal){
        if( providedVal > 0 ){
            updateState(true);
        }else{
            updateState(false);
        }
    }

    void updateState(bool newState){
        Ux* uxInstance = Ux::Singleton();
        if( newState ){ // enabled/checked
            labelText->color(255, 255, 255, 255);
            Ux::setColor(&rightSquare->foregroundColor, 0, 199, 26, 255);
            uxInstance->printCharToUiObject(rightSquare, CHAR_CHECKMARK_ICON, DO_NOT_RESIZE_NOW);

        }else{
            labelText->color(128, 128, 128, 255);
            Ux::setColor(&rightSquare->foregroundColor, 128, 128, 128, 255);
            uxInstance->printCharToUiObject(rightSquare, CHAR_CANCEL_ICON, DO_NOT_RESIZE_NOW);
        }
        checkedState = newState;
    }

//    static void interactionToggleControl(uiObject *interactionObj, uiInteraction *delta){
//        uiControlAchievementToggle* self = ((uiControlAchievementToggle*)interactionObj->myUiController);
//    }

    void resize(){

        Ux* uxInstance = Ux::Singleton();

        if( uxInstance->widescreen ){ // widescreen

        }else{
        }

        update();
    }

    void update(){ // w/h
        uiObjectItself->updateRenderPosition();
    }


};


#endif
