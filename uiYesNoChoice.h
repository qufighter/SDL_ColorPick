
#ifndef ColorPick_iOS_SDL_uiYesNoChoice_h
#define ColorPick_iOS_SDL_uiYesNoChoice_h

// event code will be "YES_NO_RESULTS" enum...
typedef void (*scoreDisplayFn)(uiObject *interactionObj, int eventCode, int quantitySelected);


struct uiYesNoChoice{


    uiYesNoChoice(uiObject* parentObj, Float_Rect boundaries, bool animateContainerInstead){

        isDisplayed=false;
        Ux* uxInstance = Ux::Singleton(); // some useful helper?


        uiObjectItself = new uiObject();
        additionalUiObjectContainer = new uiObject();
        addSomeMoreHolder = new uiObject();
        addSomeMore = new uiObject();
        addSomeMoreText = new uiObject();
        yes = new uiObject();
        no = new uiObject();
        text = new uiObject();
        text_holder = new uiObject();

        //
        //        uiObjectItself->addChild(round_tl);
        //        uiObjectItself->addChild(middle);
        //        uiObjectItself->addChild(round_br);


        uiObjectItself->myUiController = this; // this propagates to the other child objects
        uiObjectItself->hasBackground = true;

        //uiObjectItself->setInteractionCallback(tileClickedFn);


        //deleteColorPreview = new uiViewColor(uiObjectItself, Float_Rect(0.0, 0.0, 1.0, 0.27777777777778), false);

//        addSomeMoreHolder->hasBackground=false;
//        addSomeMoreHolder->hasForeground=false;
        addSomeMoreHolder->doesInFactRender = false;
        //addSomeMoreHolder->squarify();



        addSomeMore->hasBackground=true;
        addSomeMore->hasForeground=true;
        Ux::setColor(&addSomeMore->backgroundColor, 32, 0, 0, 128);
        Ux::setColor(&addSomeMore->foregroundColor, 255, 255, 255, 212); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        uxInstance->printCharToUiObject(addSomeMore, CHAR_CIRCLE_PLAIN, DO_NOT_RESIZE_NOW);
        //uxInstance->printStringToUiObject(addSomeMore, "+499", DO_NOT_RESIZE_NOW);
        addSomeMore->setRoundedCorners(0.5);


        addSomeMoreText->hasForeground=true;
        Ux::setColor(&addSomeMoreText->foregroundColor, 255, 255, 255, 212); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        addSomeMoreText->containText = true;
        //addSomeMoreText->containTextPadding=0.15; // we toggle this later...
        addSomeMoreText->scale(1.8); // this effects the text scale / letter spacing
        uxInstance->printCharToUiObject(addSomeMoreText, ' ', DO_NOT_RESIZE_NOW);


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
        //uxInstance->printStringToUiObject(text, "x000000", DO_NOT_RESIZE_NOW);
        text->setBoundaryRect( -3.5, 0, 1.0, 1.0); // on right
        text->scale(1.5); // this effects the text scale / letter spacing


        text_holder->addChild(text);


        yes->squarify();
        no->squarify();

        no->is_circular = false; // easier to click no

        uiObjectItself->addChild(additionalUiObjectContainer);

        uiObjectItself->addChild(addSomeMoreHolder);

        addSomeMoreHolder->addChild(addSomeMore);
        addSomeMore->addChild(addSomeMoreText);

        uiObjectItself->addChild(yes);
        uiObjectItself->addChild(no);

        addSomeMore->setClickInteractionCallback(defaultAddMoreFn);

        uiObjectItself->setInteractionCallback(Ux::interactionNoOp);

        parentObj->addChild(uiObjectItself);

        if( animateContainerInstead ){
            uiObjToAnimate = parentObj;
        }else{
            uiObjToAnimate = uiObjectItself;
        }

        yesClickedFn = nullptr;
        noClickedFn = nullptr;
        scoreDisplayCallback = &defaultScoringFn;

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
        last_num_delete = 1;

        resize();
    }

    //anInteractionFn tileClicked=nullptr;

    uiObject* uiObjToAnimate; // defaults to uiObjectItself....
    uiObject* uiObjectItself; // no real inheritance here, this its the uiSqware, I would use self->
    uiObject *text;
    uiObject *text_holder;
    uiObject *yes;
    uiObject *no;
    uiObject *addSomeMoreHolder;
    uiObject *addSomeMore;
    uiObject *addSomeMoreText;

    uiObject *additionalUiObjectContainer;


    //uiViewColor* deleteColorPreview;

    anInteractionFn yesClickedFn;
    anInteractionFn noClickedFn;
    anInteractionFn additionalActionSelectedFn;
    anInteractionFn additionalActionFn;
    scoreDisplayFn scoreDisplayCallback;

    int additional_number_to_show;

    float text_length;
    int last_num_delete;  // we should rename this, number to effect really... and influences scoring.
    bool isDisplayed; // only one
    bool fastYesAllowed;
    /// we should consider a yes/no ID of current ticks,.....

    // GENERICS - GENRAL PURPOSE use for whatever
    uiObject *myTriggeringUiObject;

    void assignScoringProcessor(scoreDisplayFn scoreHandler){
        // if you pass nullptr you can disable the scoring....
        scoreDisplayCallback = scoreHandler;
    }

    void setFontForMessageText(){
        // since we contain text, we need to do some funny shifting of our styles... while we print text
        // however this would chagne "addSomeMoreText" instead now... which is already set?
//        addSomeMore->hasBackground = false;
//        addSomeMore->hasForeground=true;
//        Ux::setColor(&addSomeMore->foregroundColor, 200, 200, 200, 192);
    }

    void resetFontAdtlButtonApperance(){
        addSomeMore->hasBackground = true;
        //Ux::setColor(&addSomeMore->foregroundColor, 200, 200, 200, 192);
        addSomeMore->hasForeground = true;
    }

    void displayAdditionalUiObject(uiObject* objToShow){
        additionalUiObjectContainer->addChild(objToShow);
    }

    uiYesNoChoice* allowFastYes(){
        fastYesAllowed = true;
        return setBackground(25, 25, 25, 192);
    }

    uiYesNoChoice* setBackground(int r, int g, int b, int a){
        Ux::setColor(&uiObjectItself->backgroundColor, r, g, b, a);
        return this;
    }


    void showAdditionalMessageCharAtLocation(int character, int location){
        Ux* uxInstance = Ux::Singleton();
        if( location < addSomeMoreText->childListIndex ){
            uxInstance->printCharToUiObject(addSomeMoreText->childList[location], character, DO_NOT_RESIZE_NOW);
        }
    }

    void displayAdditionalMessage(const char* message){
        additionalActionFn = nullptr;
        additionalActionSelectedFn=nullptr;
        Ux* uxInstance = Ux::Singleton();

        setFontForMessageText();
        addSomeMoreText->containTextPadding=0.0;
        uxInstance->printStringToUiObject(addSomeMoreText, message, DO_NOT_RESIZE_NOW);
        // containText should possibly have some other settings about margins?  (just use uiText

        Ux::setColor(&addSomeMore->foregroundColor, 200, 200, 200, 192);

        // resetFontAdtlButtonApperance() // instead we hide it
        addSomeMore->hasBackground = false;
        addSomeMore->hasForeground = false;

        float w = 0.5;
        float hw = w * 0.5;
        addSomeMoreHolder->setBoundaryRect( 0.5-hw, 0.25-hw, w,w);
        addSomeMore->setBoundaryRect( 0, 0, 1, 1);
//
//        if( additionalActionFn != nullptr ){
//            addSomeMoreHolder->showAndAllowInteraction();
//        }
        addSomeMoreHolder->show();
    }

    void displayAdditionalMessageBottom(const char* message){
        displayAdditionalMessage(message);
        float w = 0.5;
        float hw = w * 0.5;
        addSomeMoreHolder->setBoundaryRect( 0.5-hw, 0.75-hw, w,w);
    }


    void displayAdditionalAction(anInteractionFn p_additionalSelectedFn, anInteractionFn p_additionalYesClickedFn, int numberToShow){
        additionalActionFn = p_additionalYesClickedFn;
        additionalActionSelectedFn = p_additionalSelectedFn;
        additional_number_to_show = numberToShow;
        Ux* uxInstance = Ux::Singleton();

        setFontForMessageText();
        addSomeMoreText->containTextPadding=0.18;///15;
        // just scale matrix up and be done with text spacing....
        uxInstance->printStringToUiObject(addSomeMoreText, convertIntegerToString(numberToShow, '+'), DO_NOT_RESIZE_NOW);
        // containText should possibly have some other settings about margins?

        resetFontAdtlButtonApperance();


        float w = (0.35 + 0.25) * 0.5;
        float hw = w * 0.5;
        addSomeMoreHolder->setBoundaryRect( 0.5-hw, 0.25-hw, w,w);
        addSomeMore->setBoundaryRect( 0, 0, 1, 1);

        addSomeMoreHolder->showAndAllowInteraction();
    }

    void showStringNearOkButton(char* message){
        text_length = (float)SDL_strlen(message);
        Ux* uxInstance = Ux::Singleton();
        if( text_length <= 7 ){
            uxInstance->printStringToUiObject(text, message, DO_NOT_RESIZE_NOW);
            text->boundryRect.x =  text_length * -0.5;
            text_holder->show();

            // this is to possibly be some optional flag... to reset next time... not sure about this really  xQTY or xDELETE-COUNT
//            if( text_length > 1 && *message == 'x' ){
//                uxInstance->printCharToUiObject(text->childList[0], CHAR_CLOSE_ICON, DO_NOT_RESIZE_NOW);
//            }
        }
        resize();
    }

//    uiObject* getStringNearOkButton(){
//        return text;
//    }

    char* convertIntegerToString(int inputInt){
        return convertIntegerToString(inputInt, 'x');
    }
    char* convertIntegerToString(int inputInt, char xcode){
        Ux* myUxRef = Ux::Singleton();
        if( inputInt < 10000 ){
            SDL_snprintf(myUxRef->print_here, myUxRef->max_print_here, "%c%i", xcode, inputInt);
        }else{
            SDL_snprintf(myUxRef->print_here, myUxRef->max_print_here, "%cAll", xcode);
        }
        return myUxRef->print_here;
    }

    void updateNumberToEffectWhenYes(int numberToEffectWhenYes){
        last_num_delete = numberToEffectWhenYes;
    }

    void display(uiObject *p_myTriggeringUiObject, anInteractionFn p_yesClickedFn, anInteractionFn p_noClickedFn, int numberToEffectWhenYes){
        if( isDisplayed ) return;
        if( numberToEffectWhenYes > 1 ){
            display( p_myTriggeringUiObject,  p_yesClickedFn,  p_noClickedFn, convertIntegerToString(numberToEffectWhenYes));
            last_num_delete = numberToEffectWhenYes;
        }else{
            display( p_myTriggeringUiObject,  p_yesClickedFn,  p_noClickedFn);
        }
    }

    void display(uiObject *p_myTriggeringUiObject, anInteractionFn p_yesClickedFn, anInteractionFn p_noClickedFn, char* message){
        if( isDisplayed ) return;
        display( p_myTriggeringUiObject,  p_yesClickedFn,  p_noClickedFn);
        showStringNearOkButton(message);
    }

    void display(uiObject *p_myTriggeringUiObject, anInteractionFn p_yesClickedFn, anInteractionFn p_noClickedFn){
        if( isDisplayed ) return;
        last_num_delete = 1;
        isDisplayed = true;
        fastYesAllowed=false;
        setBackground(65, 0, 65, 192);

        addSomeMoreHolder->hideAndNoInteraction();
        additionalUiObjectContainer->empty();


        text_holder->hide();

        myTriggeringUiObject = p_myTriggeringUiObject;

        Ux* uxInstance = Ux::Singleton(); // some useful helper?

        noClickedFn = p_noClickedFn;
        yesClickedFn = p_yesClickedFn;
        scoreDisplayCallback = &defaultScoringFn; // we have a default scoring function always set now... arguably this should be activated when needed instead...

        //yes->setClickInteractionCallback(p_yesClickedFn); // probably changing this...

        uiObjToAnimate->setAnimation( uxInstance->uxAnimations->slideDownFullHeight(uiObjToAnimate) );// returns uiAminChain*

        uiObjToAnimate->is_being_viewed_state = true;

        uxInstance->updateModal(uiObjectItself, defaultCancelFn);

    }

    void hide(){
        Ux* uxInstance = Ux::Singleton();

        uiObjToAnimate->setAnimation( uxInstance->uxAnimations->slideUp(uiObjToAnimate) );// returns uiAminChain*

        uiObjToAnimate->is_being_viewed_state = false;

        uxInstance->endModal(uiObjectItself);

        isDisplayed=false;


    }

    static void defaultAddMoreFn(uiObject *interactionObj, uiInteraction *delta){
        uiYesNoChoice* self = ((uiYesNoChoice*)interactionObj->myUiController);

        if( self->additionalActionFn == nullptr ) return; // this is the n on-interactive variant and has no effect when clicked

        self->addSomeMoreHolder->hideAndNoInteraction();

        self->last_num_delete = self->additional_number_to_show + 1;

        self->showStringNearOkButton(self->convertIntegerToString(self->last_num_delete));

        if( self->additionalActionSelectedFn != nullptr ){
            // todo some of the processing above... can maybe move into this custom handler...
            self->additionalActionSelectedFn(self->myTriggeringUiObject, delta);
        }

        self->yesClickedFn = self->additionalActionFn;
    }

    // default scoreDisplayFn
    static void defaultScoringFn(uiObject *interactionObj, int eventCode, int quantitySelected){
        Ux* uxInstance = Ux::Singleton();
        switch(eventCode){
            case YES_NO_RESULTS::RESULT_YES_FAST:
                uxInstance->defaultScoreDisplay->display(interactionObj, 10 * quantitySelected, SCORE_EFFECTS::MOVE_UP);
                //uxInstance->defaultScoreDisplay->displayExplanation(" Much Risk ");
                uxInstance->defaultScoreDisplay->displayAchievement(Ux::uiSettingsScroller::UI_ACHEIVEMENT_MUCH_RISK);
                break;
            case YES_NO_RESULTS::RESULT_YES:
                uxInstance->defaultScoreDisplay->display(interactionObj, 2 * quantitySelected, SCORE_EFFECTS::MOVE_UP);
                break;
            case YES_NO_RESULTS::RESULT_NO_FAST:
                uxInstance->defaultScoreDisplay->display(interactionObj, 10, SCORE_EFFECTS::MOVE_UP);
                //uxInstance->defaultScoreDisplay->displayExplanation(" No Thanks ");
                uxInstance->defaultScoreDisplay->displayAchievement(Ux::uiSettingsScroller::UI_ACHEIVEMENT_NO_FAST);
                break;
            case YES_NO_RESULTS::RESULT_NO:
            default:
                uxInstance->defaultScoreDisplay->display(interactionObj, 1, SCORE_EFFECTS::MOVE_UP);
                break;
        }
    }

    static void defaultOkFn(uiObject *interactionObj, uiInteraction *delta){

        Ux* uxInstance = Ux::Singleton();
        uiYesNoChoice* self = ((uiYesNoChoice*)interactionObj->myUiController);
        int resultCode = YES_NO_RESULTS::RESULT_YES;

        if( self->uiObjToAnimate->isAnimating() && !self->fastYesAllowed ){
            // yes is too easy to click right now by accident, we are not done animating in.... this should be based on which choice is dangerous, default yes

            /// this is worth 50 points easily though... and can cancel the dialogue?!!
            // the multiplier is based on size in pallete?
            // maybe based on possible points accumlated from tiles in pallete
            // which Ux can tell us~ conveniently....
            // problem being this is generic dialogue
            // and even instances would need configurable callback...

            resultCode = YES_NO_RESULTS::RESULT_YES_FAST;
            uxInstance->uxAnimations->spin_negative(self->yes, 15);

            if( self->scoreDisplayCallback != nullptr ){
                self->scoreDisplayCallback(interactionObj, resultCode, self->last_num_delete); // this is dupe of below, but we return too fast here...
            }
            return; // we cannot early click it... but we still want to run the score animations...
        }

        if( self->yesClickedFn != nullptr ){
            self->yesClickedFn(self->myTriggeringUiObject, delta);
        }

        if( self->scoreDisplayCallback != nullptr ){
            self->scoreDisplayCallback(interactionObj, resultCode, self->last_num_delete);
        }

        self->hide();
    }

    static void defaultCancelFn(uiObject *interactionObj, uiInteraction *delta){
        Ux* uxInstance = Ux::Singleton();
        uiYesNoChoice* self = ((uiYesNoChoice*)interactionObj->myUiController);
        int resultCode = YES_NO_RESULTS::RESULT_NO;

        if( self->uiObjToAnimate->isAnimating() ){

            /// this is worth 50 points easily though... and can cancel the dialogue?!!
            // the multiplier is based on size in pallete?
            // maybe based on possible points accumlated from tiles in pallete
            // which Ux can tell us~ conveniently....
            // problem being this is generic dialogue
            // and even instances would need configurable callback...

            // score can maybe be a function of how complete the animation is??

            resultCode = YES_NO_RESULTS::RESULT_NO_FAST;

            //uxInstance->uxAnimations->spin_reset(self->no, 15);
            uxInstance->uxAnimations->spin(self->no, 25);

            //return; // allow no to be early clicked, no harm done
        }


        if( self->noClickedFn != nullptr ){
            self->noClickedFn(self->myTriggeringUiObject, delta);
        }

        if( self->scoreDisplayCallback != nullptr ){
            self->scoreDisplayCallback(interactionObj, resultCode, 0);
        }

        self->hide();
    }

    void resize(){
        Ux* uxInstance = Ux::Singleton();

        //uiObjectItself->setBoundaryRect(&boundaries);

        if( uxInstance->widescreen ){
            text_holder->setBoundaryRect( 0.5, 1.0, 0.5, 0.5);

            addSomeMoreHolder->squarifyKeepVt();
        }else{

            float baseWidth = 1.53;
            if( text_length < 3 ){
                baseWidth=1.0;
            }
            text_holder->setBoundaryRect( 0.5, 1.0, baseWidth/text_length, baseWidth/text_length);

            addSomeMoreHolder->squarifyKeepHz();

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
