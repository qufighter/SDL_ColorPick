//
//  uiScrollController.h
//  ColorPick iOS SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiHueGradient_h
#define ColorPick_iOS_SDL_uiHueGradient_h


struct uiHueGradient{

    uiHueGradient(uiObject* parentObj, Float_Rect boundaries, anInteractionFn tileClickedFn){

        tileClicked = tileClickedFn;

        //Ux* uxInstance = Ux::Singleton(); // some useful helper?

        uiObjectItself = new uiObject();

        uiObjectItself->myUiController = this; // this propagates to the other child objects

        generateHueGradient();

        int counter = 0;
        int offset = 0;
        float width = 1.0 / 128;

        // 6 chunks of 0-255 rainbow
        hueGradientHolder = new uiObject();
        hueGradientHolder->setBoundaryRect(0.0, 0.0, 1.0, 1.0);
        uiObjectItself->addChild(hueGradientHolder); // add first so controller proagates


        huePositionMarker = new uiObject();
        //huePositionMarker->setBoundaryRect(0.0, 0.0, 0.1, 1.0);// whatever we size here does  not matter
        huePositionMarker->doesInFactRender = false;
        huePositionMarker->doesRenderChildObjects = false;
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

        while( offset < totalColors - 6 ){
            uiObject* rt = new uiObject();
            rt->hasBackground=true;
            Ux::setColor(&rt->backgroundColor, &manyColors[offset]);
            //rt->setInteractionCallback(interactionHueClicked); //setClickInteractionCallback
            rt->setBoundaryRect( counter * (width) , 0.0, width+0.01, 1.0);
            hueGradientHolder->addChild(rt);
            //uiObjectItself->addChild(rt);
            offset+=12;
            counter++;
        }

        uiObjectItself->setInteraction(interactionHueBgClicked);
        uiObjectItself->setInteractionCallback(interactionHueBgClicked);

        //uiObjectItself->setInteractionCallback(Ux::interactionNoOp);
        parentObj->addChild(uiObjectItself);
        resize(boundaries);
    }

    anInteractionFn tileClicked=nullptr;

    uiObject* uiObjectItself; // no real inheritance here, this its the uiSqware, I would use self->
    uiObject* hueGradientHolder;
    uiObject* huePositionMarker;
    bool hueSliderVisible;

    int totalColors = 1530;
    SDL_Color manyColors[1530];// *totalColorsPer * 6

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
        //int clrOffset =  SDL_floorf(self->totalColors * percent); g

        //uxInstance->hueClicked(self->colorForPercent(percent));

        // instead of updating it right now as the above would do.... since it is intense

#ifndef __IPHONEOS__
        if( self->lockPickerEvent ) return;
        self->lockPickerEvent = true; // hmm seems like we still need to use a timer if we really want to debounce it...
        SDL_AddTimer(250, my_callbackfunc, self);
#else
        pickerForPercentV(&percent);
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
        uxInstance->hueClicked(self->colorForPercent(self->lastPickPercent));

        self->showHueSlider();

//        uxInstance->pickerForHuePercentage(*percent);
        self->lockPickerEvent = false;
    }


    SDL_Color* colorForPercent(float percent){
        lastPickPercent = percent;
        int clrOffset =  SDL_floorf((this->totalColors-1) * percent);
        return &this->manyColors[clrOffset];
    }

//    static void interactionHueClicked(uiObject *interactionObj, uiInteraction *delta){
//       // Ux* uxInstance = Ux::Singleton();
//        uiHueGradient* self = ((uiHueGradient*)interactionObj->myUiController);
//
//        self->tileClicked(interactionObj, delta);
//    }

    void showHueSlider(){
        hueSliderVisible = true;
        huePositionMarker->doesRenderChildObjects = true;
        repositionHueSlider(lastPickPercent, false);
    }

    void hideHueSlider(){
        hueSliderVisible = true;
        huePositionMarker->doesRenderChildObjects = false;
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

        float markerHeight=0.55;
        float markerOutset=0.25;
        float hueMarkerWidth = 0.08;

        if( uxInstance->widescreen ){ // widescreen
            markerHeight=0.45;
            hueGradientHolder->setChildNodeDirection(TEXT_DIR_ENUM::TTB, true);
            huePositionMarker->setBoundaryRect(-markerOutset, 0.0, 1.0+markerOutset+markerOutset, hueMarkerWidth);
            markerTop->setBoundaryRect(0.0, 0.0, markerHeight, 1.0);
            markerBtm->setBoundaryRect(1.0-markerHeight, 0.0, markerHeight, 1.0);

            uxInstance->printCharToUiObject(markerTop, CHAR_ARR_RIGHT, DO_NOT_RESIZE_NOW);
            uxInstance->printCharToUiObject(markerBtm, CHAR_ARR_LEFT, DO_NOT_RESIZE_NOW);

        }else{
            hueGradientHolder->setChildNodeDirection(TEXT_DIR_ENUM::LTR, true);
            huePositionMarker->setBoundaryRect(0.0, -markerOutset, hueMarkerWidth, 1.0+markerOutset+markerOutset);
            markerTop->setBoundaryRect(0.0, 0.0, 1.0, markerHeight);
            markerBtm->setBoundaryRect(0.0, 1.0-markerHeight, 1.0, markerHeight);

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

    void generateHueGradient(){

        int totalColorsPer = 255;
        int stopPoint = totalColorsPer;
        int counter = 0;
        int current_r = 255;
        int current_g = 0;
        int current_b = 0;

        // 6 chunks of 0-255 rainbow

        SDL_Log("SPLIT %i %i %i", current_r, current_g, current_b);

        while( counter < totalColorsPer ){
            Ux::setColor(&manyColors[counter], current_r, current_g, current_b, 255);
            current_b+=1;
            counter++;
        }
        SDL_Log("SPLIT %i %i %i", current_r, current_g, current_b);

        stopPoint = totalColorsPer * 2;
        while( counter < stopPoint ){
            Ux::setColor(&manyColors[counter], current_r, current_g, current_b, 255);
            current_r-=1;
            counter++;
        }
        SDL_Log("SPLIT %i %i %i", current_r, current_g, current_b);

        stopPoint = totalColorsPer * 3;
        while( counter < stopPoint ){
            Ux::setColor(&manyColors[counter], current_r, current_g, current_b, 255);
            current_g+=1;
            counter++;
        }
        SDL_Log("SPLIT %i %i %i", current_r, current_g, current_b);

        stopPoint = totalColorsPer * 4;
        while( counter < stopPoint ){
            Ux::setColor(&manyColors[counter], current_r, current_g, current_b, 255);
            current_b-=1;
            counter++;
        }
        SDL_Log("SPLIT %i %i %i", current_r, current_g, current_b);

        stopPoint = totalColorsPer * 5;
        while( counter < stopPoint ){
            Ux::setColor(&manyColors[counter], current_r, current_g, current_b, 255);
            current_r+=1;
            counter++;
        }
        SDL_Log("SPLIT %i %i %i", current_r, current_g, current_b);

        stopPoint = totalColorsPer * 6;
        while( counter < stopPoint ){
            Ux::setColor(&manyColors[counter], current_r, current_g, current_b, 255);
            current_g-=1;
            counter++;
        }
        SDL_Log("SPLIT %i %i %i", current_r, current_g, current_b);
    }

};


#endif
