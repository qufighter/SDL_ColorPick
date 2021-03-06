//
//  ColorPick SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiHueGradient_h
#define ColorPick_iOS_SDL_uiHueGradient_h


struct uiHueGradient{

    uiHueGradient(uiObject* parentObj, Float_Rect boundaries){

        //Ux* uxInstance = Ux::Singleton(); // some useful helper?

        uiObjectItself = new uiObject();

        uiObjectItself->myUiController = this; // this propagates to the other child objects

        

        // 6 chunks of 0-255 rainbow
//        hueGradientHolder = new uiObject();
//        hueGradientHolder->setBoundaryRect(0.0, 0.0, 1.0, 1.0);
//        uiObjectItself->addChild(hueGradientHolder); // add first so controller proagates
        hueGradient = new uiHueGradientScroller(uiObjectItself, Float_Rect(0.0,0.0,1.0,1.0), false); // we dissable scrolling
//        hueGradientHolder = hueGradient->hueGradientHolder;


        huePositionMarker = new uiObject();
        //huePositionMarker->setBoundaryRect(0.0, 0.0, 0.1, 1.0);// whatever we size here does  not matter
        huePositionMarker->doesInFactRender = false;

        huePositionMarker->setAnimationPercCallback(&uiHueGradient::setHueByPercentageBoundaryAnimCb);


        hideHueSlider();
        uiObjectItself->addChild(huePositionMarker);

        uiObject* markerTop = new uiObject();
        //markerTop->setBoundaryRect(0.0, 0.0, 1.0, 0.25);// whatever we size here does  not matter
        //uxInstance->printCharToUiObject(markerTop, CHAR_ARR_DN, DO_NOT_RESIZE_NOW);
        huePositionMarker->addChild(markerTop);

        uiObject* markerBtm = new uiObject();
        //markerBtm->setBoundaryRect(0.0, 1.0-0.25, 1.0, 0.25);// whatever we size here does  not matter
        //uxInstance->printCharToUiObject(markerBtm, CHAR_ARR_UP, DO_NOT_RESIZE_NOW);
        huePositionMarker->addChild(markerBtm);




        uiObjectItself->setInteraction(interactionHueBgClicked);
        uiObjectItself->setInteractionCallback(interactionHueBgClicked);
        uiObjectItself->interactionNonController=true; // its just a click for controllers

        //uiObjectItself->setInteractionCallback(Ux::interactionNoOp);
        parentObj->addChild(uiObjectItself);
        resize(boundaries);
    }

    uiObject* uiObjectItself; // no real inheritance here, this its the uiSqware, I would use self->
    uiHueGradientScroller* hueGradient;
    //uiObject* hueGradientHolder; // the above's hueGradientHolder is hueGradientHolder, there is still hueGradient->uiObjectItself between though...
    uiObject* huePositionMarker;
    bool hueSliderVisible;


    float lastPickPercent = 0.0f;
    bool lockPickerEvent = false;
    static void interactionHueBgClicked(uiObject *interactionObj, uiInteraction *delta){
        uiHueGradient* self = ((uiHueGradient*)interactionObj->myUiController);
        Ux* uxInstance = Ux::Singleton();
        float percent;

        if( uxInstance->widescreen ){
            percent = (delta->py - interactionObj->collisionRect.y) * (1.0/interactionObj->collisionRect.h); // it is arguable delta aleady "know" this?
        }else{
            percent = (delta->px - interactionObj->collisionRect.x) * (1.0/interactionObj->collisionRect.w); // it is arguable delta aleady "know" this?
        }


        if( percent < 0 ) percent = 0;
        else if( percent > 1 ) percent = 1;
        //SDL_Log("HUE PCT %f", percent);


        self->lastPickPercent = percent;
        self->showHueSlider(); // < this is a litlte dupllicate.... but provides much more instant feedback...
        //int clrOffset =  SDL_floorf(self->totalColors * percent); g

        //uxInstance->hueClicked(self->colorForPercent(percent));

        // instead of updating it right now as the above would do.... since it is intense

        self->applyPickedPercent(percent);
    }

    static void setHueByPercentageBoundaryAnimCb(Ux::uiObject *interactionObj, float percent){
        Ux* uxInstance = Ux::Singleton();
        uiHueGradient* self = uxInstance->huePicker;
        self->lastPickPercent = percent;
        self->applyPickedPercent(percent);
    }

    void applyPickedPercent(float percentage){

#ifndef __IPHONEOS__
        if( lockPickerEvent ) return;
        lockPickerEvent = true; // hmm seems like we still need to use a timer if we really want to debounce it...
        SDL_AddTimer(250, my_callbackfunc, this);
#else
        pickerForPercentV(&percentage);
#endif

    }

    static Uint32 my_callbackfunc(Uint32 interval, void* parm){

        Ux* uxInstance = Ux::Singleton();
        uiHueGradient* self = uxInstance->huePicker;
//        if( self->lockPickerEvent ) return 0;


        SDL_Event event;
        SDL_UserEvent userevent;
        userevent.type = SDL_USEREVENT;
        userevent.code = USER_EVENT_ENUM::NEW_HUE_CHOSEN;
        userevent.data1 = (void*)&pickerForPercentV;
        userevent.data2 = &self->lastPickPercent; // &percent; // waste arg
        event.type = SDL_USEREVENT;
        event.user = userevent;
        SDL_PushEvent(&event);

        return 0; // end timer
        //return interval;
    }




    static void pickerForPercentV(float* percent){ // don't use the arg... its out of date!
        Ux* uxInstance = Ux::Singleton();
        uiHueGradient* self = uxInstance->huePicker;

        if( *percent != self->lastPickPercent ){
            // user is not done moving it yet.... lets avoid lag?? (actually this doesn't work since its teh same float always
            return;
        }

        uxInstance->hueClicked(self->colorForPercent(self->lastPickPercent));

        self->showHueSlider();

//        uxInstance->pickerForHuePercentage(*percent);
        self->lockPickerEvent = false;
    }

    SDL_Color* colorForPercent(float percent){
        lastPickPercent = percent;
        return hueGradient->colorForPercent(percent);
    }

    void showHueSlider(){
        hueSliderVisible = true;
        huePositionMarker->showAndAllowInteraction();
        repositionHueSlider(lastPickPercent, false);
        huePositionMarker->doesInFactRender = false; // its just a container, save a render pass :)
    }

    void hideHueSlider(){
        hueSliderVisible = true;
        huePositionMarker->hideAndNoInteraction();
    }

    void repositionHueSlider(float percent, float skipUpdate){
        Ux* uxInstance = Ux::Singleton();
        if( uxInstance->widescreen ){ // widescreen
            huePositionMarker->boundryRect.y = percent - (huePositionMarker->boundryRect.h * 0.5);
        }else{
            huePositionMarker->boundryRect.x = percent - (huePositionMarker->boundryRect.w * 0.5);
        }
        if( !skipUpdate ) huePositionMarker->updateRenderPosition();
    }

    void resize(Float_Rect boundaries){

        uiObjectItself->setBoundaryRect(&boundaries);

        Ux* uxInstance = Ux::Singleton();

        uiObject* markerTop = huePositionMarker->childList[0];
        uiObject* markerBtm = huePositionMarker->childList[1];

        float markerOutset=0.25;
        float markerHeight=0.55 + markerOutset;
        float hueMarkerWidth = 0.08;
        float halfHueMarkerWidth = hueMarkerWidth * 0.5;

        hueGradient->resize(true);

        if( uxInstance->widescreen ){ // widescreen
            markerHeight=0.45 + markerOutset;
            huePositionMarker->setBoundaryRect(0.0, 0.0, 1.0, hueMarkerWidth);
            huePositionMarker->setMovementBoundaryRect( 0.0, -halfHueMarkerWidth, 0.0, 1.0 + hueMarkerWidth);
            huePositionMarker->setInteraction(&Ux::interactionSliderVT);

            markerTop->setBoundaryRect(-markerOutset, 0.0, markerHeight, 1.0);
            markerBtm->setBoundaryRect(1.0-markerHeight+markerOutset, 0.0, markerHeight, 1.0);

            uxInstance->printCharToUiObject(markerTop, CHAR_ARR_RIGHT, DO_NOT_RESIZE_NOW);
            uxInstance->printCharToUiObject(markerBtm, CHAR_ARR_LEFT, DO_NOT_RESIZE_NOW);

        }else{
            huePositionMarker->setBoundaryRect(0.0, 0.0, hueMarkerWidth, 1.0);
            huePositionMarker->setMovementBoundaryRect( -halfHueMarkerWidth, 0, 1.0 + hueMarkerWidth, 0.0);
            huePositionMarker->setInteraction(&Ux::interactionHZ);


            markerTop->setBoundaryRect(0.0, -markerOutset, 1.0, markerHeight);
            markerBtm->setBoundaryRect(0.0, 1.0-markerHeight+markerOutset, 1.0, markerHeight);

            uxInstance->printCharToUiObject(markerTop, CHAR_ARR_DN, DO_NOT_RESIZE_NOW);
            uxInstance->printCharToUiObject(markerBtm, CHAR_ARR_UP, DO_NOT_RESIZE_NOW);
        }

        if( hueSliderVisible ){
            repositionHueSlider(lastPickPercent, true);
        }

        update();
    }

    void update(){ // w/h

        uiObjectItself->updateRenderPosition();

    }


};


#endif
