//
//  uiHueGradient.h
//  ColorPick SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiControlButton_h
#define ColorPick_iOS_SDL_uiControlButton_h


struct uiControlButton{

    uiControlButton(uiObject* parentObj, const char* labelTxt, anInteractionFn tileClickedFn){


        label_chars = labelTxt;

        //Ux* uxInstance = Ux::Singleton(); // some useful helper?

        uiObjectItself = new uiObject();

        uiObjectItself->myUiController = this; // this propagates to the other child objects

        float margin = 0.05;
        margins = new uiObject();
        margins->setBoundaryRect(margin, margin, 1.0 - margin - margin, 1.0 - margin - margin);
        uiObjectItself->addChild(margins);

//        uiObjectItself->hasBackground=true;
//        Ux::setColor(&uiObjectItself->backgroundColor, 255, 128, 128, 128);


//        rightSquare = new uiObject();
//        rightSquare->setBoundaryRect(0.75, 0.0, 0.25, 1.0);
//        rightSquare->squarify();
//        rightSquare->stackRight();
//        rightSquare->setModeWhereChildCanCollideAndOwnBoundsIgnored();
//        uiObjectItself->addChild(rightSquare);
//        rightSquare->hasForeground = true;
//        Ux::setColor(&rightSquare->foregroundColor, 128, 128, 128, 255);

//        rightSquare->hasBackground = true;
//        Ux::setColor(&rightSquare->backgroundColor, 255, 0, 0, 255);
//        uxInstance->printCharToUiObject(rightSquare, CHAR_CANCEL_ICON, DO_NOT_RESIZE_NOW);

        labelText = new uiText(margins, 0.065);

        labelText->backgroundColor(255, 0, 255, 96);

        labelText->backgroundRound(0.2);

        labelText->backgroundClickCallback(tileClickedFn);

        labelText->backgroundTouchBeginCallback(&interactionButtonTouched);

        labelText->color(255, 255, 255, 255);

        //labelText->scale(1.0); //
        //labelText->pad(0.5,0.25); //
        labelText->pad(0.5,0.05); //

        labelText->align(uiText::CENTER)->print(labelTxt);

        //uiObjectItself->hasBackground=false;
        //Ux::setColor(&uiObjectItself->backgroundColor, 255, 128, 128, 128);

        //uiObjectItself->setClickInteractionCallback(tileClickedFn);

        updateState(false);



        parentObj->addChild(uiObjectItself);

        resize();
    }

    uiObject* uiObjectItself; // no real inheritance here, this its the uiSqware, I would use self->
    uiObject* margins;
//    uiObject* rightSquare;

    uiText *labelText;

    const char* label_chars;

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

    uiControlButton* size(float size){
        labelText->size(size);
        return this;
    }

    static void interactionButtonTouched(uiObject *interactionObj, uiInteraction *delta){
        Ux* myUxRef = Ux::Singleton();
//        uiText* textObj = (uiText*)interactionObj->myUiController;
//        myUxRef->uxAnimations->scale_bounce(textObj->uiObjectItself, 0.006);
        myUxRef->uxAnimations->scale_bounce(interactionObj, 0.006);
    }

    void updateState(bool newState){

    }

//    static void interactionToggleControl(uiObject *interactionObj, uiInteraction *delta){
//        uiControlButton* self = ((uiControlButton*)interactionObj->myUiController);
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
