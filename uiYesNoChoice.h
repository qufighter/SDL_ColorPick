
#ifndef ColorPick_iOS_SDL_uiYesNoChoice_h
#define ColorPick_iOS_SDL_uiYesNoChoice_h


struct uiYesNoChoice{


    uiYesNoChoice(uiObject* parentObj, Float_Rect boundaries){


        Ux* uxInstance = Ux::Singleton(); // some useful helper?

        uiObjectItself = new uiObject();
        yes = new uiObject();
        no = new uiObject();

        //
        //        uiObjectItself->addChild(round_tl);
        //        uiObjectItself->addChild(middle);
        //        uiObjectItself->addChild(round_br);


        uiObjectItself->myUiController = this; // this propagates to the other child objects


        //uiObjectItself->setInteractionCallback(tileClickedFn);



        yes->hasBackground=true;
        yes->hasForeground=true;
        Ux::setColor(&yes->backgroundColor, 32, 0, 0, 128);
        Ux::setColor(&yes->foregroundColor, 255, 255, 255, 192); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        uxInstance->printCharToUiObject(yes, CHAR_ARR_UP, DO_NOT_RESIZE_NOW);
        yes->setInteractionCallback(defaultOkFn);

        no->hasBackground=true;
        no->hasForeground=true;
        Ux::setColor(&no->backgroundColor, 32, 0, 0, 128);
        Ux::setColor(&no->foregroundColor, 255, 255, 255, 192); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        uxInstance->printCharToUiObject(no, CHAR_ARR_UP, DO_NOT_RESIZE_NOW);
        no->rotate(180.0f);
        no->setInteractionCallback(defaultCancelFn);


        yes->squarify_keep_hz = true;


        uiObjectItself->addChild(yes);
        uiObjectItself->addChild(no);


        uiObjectItself->setInteractionCallback(Ux::interactionNoOp);

        parentObj->addChild(uiObjectItself);

        resize(boundaries);
    }

    //anInteractionFn tileClicked=nullptr;

    uiObject* uiObjectItself; // no real inheritance here, this its the uiSqware, I would use self->
    uiObject *yes;
    uiObject *no;

    anInteractionFn yesClickedFn;
    anInteractionFn noClickedFn;

    // GENERICS - GENRAL PURPOSE use for whatever
    uiObject *myTriggeringUiObject;

    void display(uiObject *p_myTriggeringUiObject, anInteractionFn p_yesClickedFn, anInteractionFn p_noClickedFn){

        myTriggeringUiObject = p_myTriggeringUiObject;

        Ux* uxInstance = Ux::Singleton(); // some useful helper?

        noClickedFn = p_noClickedFn;
        yesClickedFn = p_yesClickedFn;

        //yes->setInteractionCallback(p_yesClickedFn); // probably changing this...

        uiObjectItself->setAnimation( uxInstance->uxAnimations->slideDownFullHeight(uiObjectItself) );// returns uiAminChain*

        uiObjectItself->is_being_viewed_state = true;

    }

    void hide(){
        Ux* uxInstance = Ux::Singleton();

        uiObjectItself->setAnimation( uxInstance->uxAnimations->slideUp(uiObjectItself) );// returns uiAminChain*

        uiObjectItself->is_being_viewed_state = false;

    }

    static void defaultOkFn(uiObject *interactionObj, uiInteraction *delta){
        //Ux* uxInstance = Ux::Singleton();
        uiYesNoChoice* self = ((uiYesNoChoice*)interactionObj->myUiController);

        if( self->yesClickedFn != nullptr ){
            self->yesClickedFn(self->myTriggeringUiObject, delta);
        }

        self->hide();
    }

    static void defaultCancelFn(uiObject *interactionObj, uiInteraction *delta){
        //Ux* uxInstance = Ux::Singleton();
        uiYesNoChoice* self = ((uiYesNoChoice*)interactionObj->myUiController);

        if( self->noClickedFn != nullptr ){
            self->noClickedFn(self->myTriggeringUiObject, delta);
        }

        self->hide();
    }

    void resize(Float_Rect boundaries){

        uiObjectItself->setBoundaryRect(&boundaries);

        float w = 0.25;
        float h = w;
        float hh = h * 0.5;
        float hw = w * 0.5;

        float pad = 0.12;

        no->setBoundaryRect( 0.0+pad, 0.5-hh, w, h);
        yes->setBoundaryRect( 1.0-w-pad, 0.5-hh, w, h); // on right

        update();
    }

    void update(){ // w/h

        // does this get realle whenever reshape?


//        if( aspect > 1.0 ){
//            is_vertical = false; //hopefully unused var
//
//        }else{
//            is_vertical = true; //hopefully unused var
//
//
//        }
//
//        round_tl->setBoundaryRect( 0.0, 0.0, 1.0, 1.0);
//        middle->setBoundaryRect( 0.0, 0.0, 1.0, 1.0);
//        round_br->setBoundaryRect( 0.0, 0.0, 1.0, 1.0);


        uiObjectItself->updateRenderPosition();

        //Ux* uxInstance = Ux::Singleton();

        // what the

//        sprintf(resultText6char, "%02x%02x%02x", color->r, color->g, color->b);
//        Ux::setColor(&hexValueText->backgroundColor,color->r, color->g, color->b, 255);
//        uxInstance->printStringToUiObject(hexValueText, resultText6char, DO_NOT_RESIZE_NOW);
//
//        sprintf(resultText6char, "%3d", color->r);
//        rgbRedText->backgroundColor.a = color->r;
//        uxInstance->printStringToUiObject(rgbRedText, resultText6char, DO_NOT_RESIZE_NOW);
//
//        sprintf(resultText6char, "%3d", color->g);
//        rgbGreenText->backgroundColor.a = color->g;
//        uxInstance->printStringToUiObject(rgbGreenText, resultText6char, DO_NOT_RESIZE_NOW);
//
//        sprintf(resultText6char, "%3d", color->b);
//        rgbBlueText->backgroundColor.a = color->b;
//        uxInstance->printStringToUiObject(rgbBlueText, resultText6char, DO_NOT_RESIZE_NOW);


        //SDL_free(resultText6char);
    }

};


#endif
