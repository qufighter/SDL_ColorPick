//
//  ColorPick SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiNavArrows_h
#define ColorPick_iOS_SDL_uiNavArrows_h


struct uiNavArrows{

    uiNavArrows(uiObject* parentObj, Float_Rect boundaries, anInteractionFn tileClickedFn){


        tileClicked = tileClickedFn;

        Ux* uxInstance = Ux::Singleton(); // some useful helper?

        uiObjectItself = new uiObject();
        up = new uiObject();
        dn = new uiObject();
        lf = new uiObject();
        rt = new uiObject();
        //
        //        uiObjectItself->addChild(round_tl);
        //        uiObjectItself->addChild(middle);
        //        uiObjectItself->addChild(round_br);


        uiObjectItself->myUiController = this; // this propagates to the other child objects


        //uiObjectItself->setInteractionCallback(tileClickedFn);



        //up->hasBackground=true;
        up->hasForeground=true;
        //Ux::setColor(&up->backgroundColor, 32, 0, 0, 128);
        Ux::setColor(&up->foregroundColor, 255, 255, 255, 192); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        uxInstance->printCharToUiObject(up, CHAR_ARR_UP, DO_NOT_RESIZE_NOW);
        up->setInteractionBegin(interactionDirectionalArrowClickBegin);
        up->setClickInteractionCallback(interactionDirectionalArrowClicked);
        up->forceDelta = new uiInteraction();
        up->forceDelta->begin(0, -1.0);
        up->squarifyKeepHz();
        //up->rotate(90.0f);

        //dn->hasBackground=true;
        dn->hasForeground=true;
        //Ux::setColor(&dn->backgroundColor, 32, 0, 0, 128);
        Ux::setColor(&dn->foregroundColor, 255, 255, 255, 192); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        uxInstance->printCharToUiObject(dn, CHAR_ARR_DN, DO_NOT_RESIZE_NOW);
        //dn->rotate(180.0f);
        dn->setInteractionBegin(interactionDirectionalArrowClickBegin);
        dn->setClickInteractionCallback(interactionDirectionalArrowClicked);
        dn->forceDelta = new uiInteraction();
        dn->forceDelta->begin(0, 1.0);
        dn->squarifyKeepHz();
        //dn->rotate(90.0f);


        //lf->hasBackground=true;
        lf->hasForeground=true;
        //Ux::setColor(&lf->backgroundColor, 32, 0, 0, 128);
        Ux::setColor(&lf->foregroundColor, 255, 255, 255, 192); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        uxInstance->printCharToUiObject(lf, CHAR_ARR_LEFT, DO_NOT_RESIZE_NOW);
        //lf->rotate(90.0f);
        lf->setInteractionBegin(interactionDirectionalArrowClickBegin);
        lf->setClickInteractionCallback(interactionDirectionalArrowClicked);
        lf->forceDelta = new uiInteraction();
        lf->forceDelta->begin(1.0, 0.0);
        lf->squarifyKeepHz();


        //rt->hasBackground=true;
        rt->hasForeground=true;
        //Ux::setColor(&rt->backgroundColor, 32, 0, 0, 128);
        Ux::setColor(&rt->foregroundColor, 255, 255, 255, 192); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        uxInstance->printCharToUiObject(rt, CHAR_ARR_RIGHT, DO_NOT_RESIZE_NOW);
        //rt->rotate(-90.0f);
        rt->setInteractionBegin(interactionDirectionalArrowClickBegin);
        rt->setClickInteractionCallback(interactionDirectionalArrowClicked);
        rt->forceDelta = new uiInteraction();
        rt->forceDelta->begin(-1.0, 0.0);
        rt->squarifyKeepHz();

        uiObjectItself->addChild(up);
        uiObjectItself->addChild(dn);
        uiObjectItself->addChild(lf);
        uiObjectItself->addChild(rt);

        parentObj->addChild(uiObjectItself);

        resize(boundaries);
    }

    anInteractionFn tileClicked=nullptr;

    uiObject* uiObjectItself; // no real inheritance here, this its the uiSqware, I would use self->
    uiObject *up;
    uiObject *dn;
    uiObject *lf;
    uiObject *rt;

    static void interactionDirectionalArrowClickBegin(uiObject *interactionObj, uiInteraction *delta){
        Ux* uxInstance = Ux::Singleton();
        //uiNavArrows* self = ((uiNavArrows*)interactionObj->myUiController);
        interactionObj->identity();
        interactionObj->setAnimation( uxInstance->uxAnimations->scale_bounce(interactionObj) );
    }

    static void interactionDirectionalArrowClicked(uiObject *interactionObj, uiInteraction *delta){
       // Ux* uxInstance = Ux::Singleton();
        uiNavArrows* self = ((uiNavArrows*)interactionObj->myUiController);


        self->tileClicked(interactionObj, interactionObj->forceDelta);


    }

    void displayOrHideBasedOnSettings(){
        if( isSettingNavArrowDisplayed() ){
            uiObjectItself->showAndAllowInteraction();
        }else{
            uiObjectItself->hideAndNoInteraction();
        }
    }

    bool isSettingNavArrowDisplayed(){
        Ux* uxInstance = Ux::Singleton();
        return uxInstance->settingsScroller->getBooleanSetting(uiSettingsScroller::UI_SETTING_SHOW_NAV_ARROWS);
    }

    void indicateVelocity(float vx, float vy){
        if( !uiObjectItself->doesRenderChildObjects ) return;  // we are hidden, no cycles wasted
        Ux* uxInstance = Ux::Singleton();
        float scaler=0.0001;
        float cap = 50;
        lf->identity();
        rt->identity();
        up->identity();
        dn->identity();
        if( vx > 0 ){
            if( vx > cap ){vx = cap;}
            lf->setAnimation( uxInstance->uxAnimations->scale_bounce(lf, (scaler*vx)));
        }else if( vx < 0 ){
            if( vx < -cap ){vx = -cap;}
            rt->setAnimation( uxInstance->uxAnimations->scale_bounce(rt, -(scaler*vx)));
        }
        if( vy > 0 ){
            if( vy > cap ){vy = cap;}
            up->setAnimation( uxInstance->uxAnimations->scale_bounce(up, (scaler*vy)));
        }else if( vy < 0 ){
            if( vy < -cap ){vy = -cap;}
            dn->setAnimation( uxInstance->uxAnimations->scale_bounce(dn, -(scaler*vy)));
        }
    }

    void resize(Float_Rect boundaries){

        uiObjectItself->setBoundaryRect(&boundaries);

        float w = 0.1;
        float h = w;
        float hh = h * 0.5;
        float hw = w * 0.5;

        up->setBoundaryRect( 0.5 - hw, 0.0, w, h);
        dn->setBoundaryRect( 0.5 - hw, 1.0 - h, w, h);
        lf->setBoundaryRect( 0.0, 0.5-hh, w, h);
        rt->setBoundaryRect( 1.0-w, 0.5-hh, w, h);

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
