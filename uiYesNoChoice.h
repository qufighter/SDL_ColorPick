
#ifndef ColorPick_iOS_SDL_uiYesNoChoice_h
#define ColorPick_iOS_SDL_uiYesNoChoice_h


struct uiYesNoChoice{


    uiYesNoChoice(uiObject* parentObj, Float_Rect boundaries, bool animateContainerInstead){


        Ux* uxInstance = Ux::Singleton(); // some useful helper?

        uiObjectItself = new uiObject();
        yes = new uiObject();
        no = new uiObject();
        text = new uiObject();
        text_holder = new uiObject();

        //
        //        uiObjectItself->addChild(round_tl);
        //        uiObjectItself->addChild(middle);
        //        uiObjectItself->addChild(round_br);


        uiObjectItself->myUiController = this; // this propagates to the other child objects


        //uiObjectItself->setInteractionCallback(tileClickedFn);


        //deleteColorPreview = new uiViewColor(uiObjectItself, Float_Rect(0.0, 0.0, 1.0, 0.27777777777778), false);





        yes->hasBackground=true;
        yes->hasForeground=true;
        Ux::setColor(&yes->backgroundColor, 32, 0, 0, 128);
        Ux::setColor(&yes->foregroundColor, 255, 0, 0, 192); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        uxInstance->printCharToUiObject(yes, CHAR_CHECKMARK_ICON, DO_NOT_RESIZE_NOW);
        yes->setClickInteractionCallback(defaultOkFn);
        yes->setRoundedCorners(0.5);

        no->hasBackground=true;
        no->hasForeground=true;
        Ux::setColor(&no->backgroundColor, 32, 0, 0, 128);
        Ux::setColor(&no->foregroundColor, 255, 255, 255, 192); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        uxInstance->printCharToUiObject(no, CHAR_CANCEL_ICON, DO_NOT_RESIZE_NOW);
        //no->rotate(180.0f);
        no->setClickInteractionCallback(defaultCancelFn);
        no->setRoundedCorners(0.5);



        yes->addChild(text_holder);
        //text_holder->squarifyKeepHz();
        //text_holder->squarify();
//        text_holder->hasBackground=true;
//        Ux::setColor(&text_holder->backgroundColor, 0, 255, 0, 128); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        text_holder->setBoundaryRect( 0.5, 1.0, 0.5, 0.5);

        text->hasForeground=true;
        Ux::setColor(&text->foregroundColor, 255, 0, 0, 255);
        //text->containText=true;
        uxInstance->printStringToUiObject(text, "x000000", DO_NOT_RESIZE_NOW);
        text->setBoundaryRect( -3.5, 0, 1.0, 1.0); // on right


        text_holder->addChild(text);


        yes->squarify();
        no->squarify();

        no->is_circular = false; // easier to click no

        uiObjectItself->addChild(yes);
        uiObjectItself->addChild(no);


        uiObjectItself->setInteractionCallback(Ux::interactionNoOp);

        parentObj->addChild(uiObjectItself);

        if( animateContainerInstead ){
            uiObjToAnimate = parentObj;
        }else{
            uiObjToAnimate = uiObjectItself;
        }

        yesClickedFn = nullptr;
        noClickedFn = nullptr;

        uiObjectItself->setBoundaryRect(&boundaries);


        //maths
        float w = 0.25;
        float h = w;
        float hh = h * 0.5;
        float hw = w * 0.5;
        float pad = 0.12;

        //no->setBoundaryRect( 0.0+pad, 0.5-hh, w, h);
        yes->setBoundaryRect( 1.0-w-pad, 0.5-hh, w, h); // on right

        //text_holder->setBoundaryRect( 1.0-(hw)-pad, 0.125, w/2, h);


        w = 0.35;
        h = w;
        hh = h * 0.5;
        hw = w * 0.5;
        pad = 0.12;

        no->setBoundaryRect( 0.0+pad, 0.5-hh, w, h);
        ///yes->setBoundaryRect( 1.0-w-pad, 0.5-hh, w, h); // on left

        text_length = 2;

        resize();
    }

    //anInteractionFn tileClicked=nullptr;

    uiObject* uiObjToAnimate; // defaults to uiObjectItself....
    uiObject* uiObjectItself; // no real inheritance here, this its the uiSqware, I would use self->
    uiObject *text;
    uiObject *text_holder;
    uiObject *yes;
    uiObject *no;

    //uiViewColor* deleteColorPreview;

    anInteractionFn yesClickedFn;
    anInteractionFn noClickedFn;

    float text_length;

    // GENERICS - GENRAL PURPOSE use for whatever
    uiObject *myTriggeringUiObject;

    void display(uiObject *p_myTriggeringUiObject, anInteractionFn p_yesClickedFn, anInteractionFn p_noClickedFn, int numberToDelete){

        if( numberToDelete > 1 ){
            int maxLen = 5;
            char* total_del = (char*)SDL_malloc( sizeof(char) * maxLen );
            if( numberToDelete < 1000 ){
                SDL_snprintf(total_del, maxLen, "x%i", numberToDelete);
            }else{
                SDL_snprintf(total_del, maxLen, "xAll");
            }
            display( p_myTriggeringUiObject,  p_yesClickedFn,  p_noClickedFn, total_del);

            SDL_free(total_del);
        }else{
            display( p_myTriggeringUiObject,  p_yesClickedFn,  p_noClickedFn);
        }
    }

    void display(uiObject *p_myTriggeringUiObject, anInteractionFn p_yesClickedFn, anInteractionFn p_noClickedFn, char* message){

        display( p_myTriggeringUiObject,  p_yesClickedFn,  p_noClickedFn);
        text_length = (float)SDL_strlen(message);
        Ux* uxInstance = Ux::Singleton();
        if( text_length <= 7 ){
            uxInstance->printStringToUiObject(text, message, DO_NOT_RESIZE_NOW);
            text->boundryRect.x =  text_length * -0.5;
            text_holder->show();
        }
//        }else{
//            uxInstance->printStringToUiObject(text, "xAll", DO_NOT_RESIZE_NOW);
//            text_length = 4;
//        }
//        text->boundryRect.x =  text_length * -0.5;
//        text_holder->show();

        resize();
    }

    void display(uiObject *p_myTriggeringUiObject, anInteractionFn p_yesClickedFn, anInteractionFn p_noClickedFn){

        text_holder->hide();

        myTriggeringUiObject = p_myTriggeringUiObject;

        Ux* uxInstance = Ux::Singleton(); // some useful helper?

        noClickedFn = p_noClickedFn;
        yesClickedFn = p_yesClickedFn;

        //yes->setClickInteractionCallback(p_yesClickedFn); // probably changing this...

        uiObjToAnimate->setAnimation( uxInstance->uxAnimations->slideDownFullHeight(uiObjToAnimate) );// returns uiAminChain*

        uiObjToAnimate->is_being_viewed_state = true;

    }

    void hide(){
        Ux* uxInstance = Ux::Singleton();

        uiObjToAnimate->setAnimation( uxInstance->uxAnimations->slideUp(uiObjToAnimate) );// returns uiAminChain*

        uiObjToAnimate->is_being_viewed_state = false;

    }

    static void defaultOkFn(uiObject *interactionObj, uiInteraction *delta){

        //Ux* uxInstance = Ux::Singleton();
        uiYesNoChoice* self = ((uiYesNoChoice*)interactionObj->myUiController);

        if( self->uiObjToAnimate->isAnimating() ){
            // yes is too easy to click right now by accident, we are not done animating in.... this should be based on which choice is dangerous, default yes

            /// this is worth 50 points easily though... and can cancel the dialogue?!!
            // the multiplier is based on size in pallete?
            // maybe based on possible points accumlated from tiles in pallete
            // which Ux can tell us~ conveniently....
            // problem being this is generic dialogue
            // and even instances would need configurable callback...

            return;
        }

        if( self->yesClickedFn != nullptr ){
            self->yesClickedFn(self->myTriggeringUiObject, delta);
        }

        self->hide();
    }

    static void defaultCancelFn(uiObject *interactionObj, uiInteraction *delta){
        //Ux* uxInstance = Ux::Singleton();
        uiYesNoChoice* self = ((uiYesNoChoice*)interactionObj->myUiController);

        if( self->uiObjectItself->isAnimating() ){

            /// this is worth 50 points easily though... and can cancel the dialogue?!!
            // the multiplier is based on size in pallete?
            // maybe based on possible points accumlated from tiles in pallete
            // which Ux can tell us~ conveniently....
            // problem being this is generic dialogue
            // and even instances would need configurable callback...

            //return;
        }


        if( self->noClickedFn != nullptr ){
            self->noClickedFn(self->myTriggeringUiObject, delta);
        }

        self->hide();
    }

    void resize(){
        Ux* uxInstance = Ux::Singleton();

        //uiObjectItself->setBoundaryRect(&boundaries);





        if( uxInstance->widescreen ){
            text_holder->setBoundaryRect( 0.5, 1.0, 0.5, 0.5);
        }else{

            float baseWidth = 1.53;
            if( text_length < 3 ){
                baseWidth=1.0;
            }
            text_holder->setBoundaryRect( 0.5, 1.0, baseWidth/text_length, baseWidth/text_length);
        }




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
