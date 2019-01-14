//
//  uiHueGradient.h
//  ColorPick SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiControlBooleanToggle_h
#define ColorPick_iOS_SDL_uiControlBooleanToggle_h


struct uiControlBooleanToggle{

    uiControlBooleanToggle(uiObject* parentObj, const char* labelTxt, bool defaultValue){

        settingsDefaultValue = defaultValue;  // IN CASE we want a "Reset" later...

        Ux* uxInstance = Ux::Singleton(); // some useful helper?

        uiObjectItself = new uiObject();

        uiObjectItself->myUiController = this; // this propagates to the other child objects
//
//        int counter = 0;
//        int offset = 0;
//        float width = 1.0 / 128;

        // 6 chunks of 0-255 rainbow
//        hueGradientHolder = new uiObject();
//        hueGradientHolder->setBoundaryRect(0.0, 0.0, 1.0, 1.0);
//        uiObjectItself->addChild(hueGradientHolder); // add first so controller proagates

        rightSquare = new uiObject();
        rightSquare->setBoundaryRect(0.75, 0.0, 0.25, 1.0);
        rightSquare->squarify();
        rightSquare->stackRight();
        rightSquare->setModeWhereChildCanCollideAndOwnBoundsIgnored();
        uiObjectItself->addChild(rightSquare);


//        rightSquare->hasBackground = true;
//        Ux::setColor(&rightSquare->backgroundColor, 255, 0, 0, 255);


        controlBg = new uiObject();
        controlBg->setBoundaryRect(-1.0, 0.0, 2.0, 1.0);
        rightSquare->addChild(controlBg);

        //controlBg->squarify();
//        controlBg->hasBackground = true;
//        Ux::setColor(&controlBg->backgroundColor, 0, 255, 0, 128);


        controlBgL = new uiObject();
        controlBgL->setBoundaryRect(0.0, 0.0, 0.5, 1.0);
        controlBgL->hasForeground=true;
        //Ux::setColor(&controlBgL->foregroundColor, 0, 255, 0, 255);
        uxInstance->printCharToUiObject(controlBgL, CHAR_ROUNDED_LEFT, DO_NOT_RESIZE_NOW);
        //controlBgL->squarify();
        controlBg->addChild(controlBgL);

        controlBgR = new uiObject();
        controlBgR->setBoundaryRect(0.5, 0.0, 0.5, 1.0);
        controlBgR->hasForeground=true;
        //Ux::setColor(&controlBgR->foregroundColor, 0, 255, 0, 255);
        uxInstance->printCharToUiObject(controlBgR, CHAR_ROUNDED_RIGHT, DO_NOT_RESIZE_NOW);
        //controlBgR->squarify();
        controlBg->addChild(controlBgR);


        checkedState = true;
        controlTog = new uiObject();
        controlTog->setBoundaryRect(0.5, 0.0, 0.5, 1.0);
        controlTog->hasForeground=true;
        //Ux::setColor(&controlTog->foregroundColor, 255, 255, 255, 200);
        uxInstance->printCharToUiObject(controlTog, CHAR_CIRCLE, DO_NOT_RESIZE_NOW);
//                controlTog->hasBackground = true;
//                Ux::setColor(&controlTog->backgroundColor, 255, 0, 0, 255);
//        controlTog->setRoundedCorners(0.5);
        controlTog->is_circular = true; // use a radial collision check...
        //controlTog->squarify();
        controlBg->addChild(controlTog);

        controlTog->setInteraction(&Ux::interactionHZ);//zoomSlider->canCollide = true;
        controlTog->setInteractionCallback(&interactionToggleControl);
        controlTog->setMovementBoundaryRect( 0.0, 0, 1.0, 0.0);

        //controlBg->setShouldCeaseInteractionChek(&Ux::bubbleInteractionIfNonClick);


        controlBg->setClickInteractionCallback(&interactionToggleControl);
        controlBg->setShouldCeaseInteractionChek(&Ux::bubbleInteractionIfNonClick);


        labelText = new uiText(uiObjectItself, 0.061);
        labelText->print(labelTxt);


        updateState(!checkedState);// fix colors, and disable the control

        if( defaultValue ){
            updateState(defaultValue);
        }

//        uiObjectItself->setInteraction(interactionHueBgClicked);
//        uiObjectItself->setInteractionCallback(interactionHueBgClicked);

        //uiObjectItself->setInteractionCallback(Ux::interactionNoOp);
        parentObj->addChild(uiObjectItself);

        resize();
    }

    uiObject* uiObjectItself; // no real inheritance here, this its the uiSqware, I would use self->


    uiObject* rightSquare;

    uiObject* controlBg;
    uiObject* controlTog;
    uiObject* controlBgL; // the above's hueGradientHolder is hueGradientHolder, there is still hueGradient->uiObjectItself between though...
    uiObject* controlBgR;

    uiText *labelText;

    bool settingsDefaultValue;
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
        Ux* myUxRef = Ux::Singleton();
        uiObject* trueInteractionObj = controlTog;
        if( newState ){ // enabled/checked
            trueInteractionObj->setAnimation( myUxRef->uxAnimations->moveTo(trueInteractionObj, 0.5, 0.0, nullptr, nullptr) );
            Ux::setColor(&controlBgL->foregroundColor, 0, 199, 26, 255);
            Ux::setColor(&controlBgR->foregroundColor, 0, 199, 26, 255);
        }else{
            trueInteractionObj->setAnimation( myUxRef->uxAnimations->moveTo(trueInteractionObj, 0.0, 0.0, nullptr, nullptr) );
            Ux::setColor(&controlBgL->foregroundColor, 128, 128, 128, 255);
            Ux::setColor(&controlBgR->foregroundColor, 128, 128, 128, 255);
        }
        checkedState = newState;
    }

    static void interactionToggleControl(uiObject *interactionObj, uiInteraction *delta){
        uiControlBooleanToggle* self = ((uiControlBooleanToggle*)interactionObj->myUiController);
        //uiObject* trueInteractionObj = self->controlTog;
        //SDL_Log("This is our result x: %f", self->controlTog->boundryRect.x);

        bool resultChecked = self->controlTog->boundryRect.x >= 0.25;
        if( self->controlTog->boundryRect.x >= 0.5 ){
            if( self->checkedState ) resultChecked = false;
        }else if( self->controlTog->boundryRect.x <= 0.0 ){
            if( !self->checkedState ) resultChecked = true;
        }
        self->updateState(resultChecked);
    }





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
