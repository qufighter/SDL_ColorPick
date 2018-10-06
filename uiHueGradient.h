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

        Ux* uxInstance = Ux::Singleton(); // some useful helper?

        uiObjectItself = new uiObject();

        uiObjectItself->myUiController = this; // this propagates to the other child objects

        generateHueGradient();

        int counter = 0;
        int offset = 0;
        float width = 1.0 / 128;

        // 6 chunks of 0-255 rainbow
//        uiObject* currentContainer = new uiObject();
//        currentContainer->setBoundaryRect(0.0, 0.0, 1.0, 1.0);
//        uiObjectItself->addChild(currentContainer); // add first so controller proagates

        while( offset < totalColors ){
            uiObject* rt = new uiObject();
            rt->hasBackground=true;
            Ux::setColor(&rt->backgroundColor, &manyColors[offset]);
            //rt->setInteractionCallback(interactionHueClicked); //setClickInteractionCallback
            rt->setBoundaryRect( counter * (width) , 0.0, width+0.01, 1.0);
            //currentContainer->addChild(rt);
            uiObjectItself->addChild(rt);
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

    int totalColors = 1530;
    SDL_Color manyColors[1530];// *totalColorsPer * 6

    float lastPickPercent = 0.0f;
    bool lockPickerEvent = false;
    static void interactionHueBgClicked(uiObject *interactionObj, uiInteraction *delta){
        uiHueGradient* self = ((uiHueGradient*)interactionObj->myUiController);
        Ux* uxInstance = Ux::Singleton();
        float percent;

        if( uxInstance->screenRatio > 1.0 ){ // widescreen
            percent = (delta->py - interactionObj->collisionRect.y) * (1.0/interactionObj->collisionRect.h); // it is arguable delta aleady "know" this?
        }else{
            percent = (delta->px - interactionObj->collisionRect.x) * (1.0/interactionObj->collisionRect.w); // it is arguable delta aleady "know" this?
        }


        if( percent < 0 ) percent = 0;
        else if( percent > 1 ) percent = 1;
        SDL_Log("HUE PCT %f", percent);


        self->lastPickPercent = percent;
        //int clrOffset =  SDL_floorf(self->totalColors * percent); g

        //uxInstance->hueClicked(self->colorForPercent(percent));

        // instead of updating it right now as the above would do.... since it is intense

        if( self->lockPickerEvent ) return;
        self->lockPickerEvent = true; // hmm seems like we still need to use a timer if we really want to debounce it...
        SDL_AddTimer(250, my_callbackfunc, self);
    }

    static Uint32 my_callbackfunc(Uint32 interval, void* parm){

        Ux* uxInstance = Ux::Singleton();
        uiHueGradient* self = uxInstance->huePicker;
//        if( self->lockPickerEvent ) return 0;


        SDL_Event event;
        SDL_UserEvent userevent;
        userevent.type = SDL_USEREVENT;
        userevent.code = 1;
        userevent.data1 = (void*)&pickerForPercentV;
        userevent.data2 = &self->lastPickPercent; // &percent; // waste arg
        event.type = SDL_USEREVENT;
        event.user = userevent;
        SDL_PushEvent(&event);

        return 0;
        //return interval;
    }


    static void pickerForPercentV(float* percent){ // don't use the arg... its out of date!
        Ux* uxInstance = Ux::Singleton();
        uiHueGradient* self = uxInstance->huePicker;
        uxInstance->hueClicked(self->colorForPercent(self->lastPickPercent));
//        uxInstance->pickerForHuePercentage(*percent);
        self->lockPickerEvent = false;
    }


    SDL_Color* colorForPercent(float percent){
        int clrOffset =  SDL_floorf(this->totalColors * percent);
        return &this->manyColors[clrOffset];
    }

//    static void interactionHueClicked(uiObject *interactionObj, uiInteraction *delta){
//       // Ux* uxInstance = Ux::Singleton();
//        uiHueGradient* self = ((uiHueGradient*)interactionObj->myUiController);
//
//        self->tileClicked(interactionObj, delta);
//    }

    void resize(Float_Rect boundaries){

        uiObjectItself->setBoundaryRect(&boundaries);

        Ux* uxInstance = Ux::Singleton();

        if( uxInstance->screenRatio > 1.0 ){ // widescreen
            uiObjectItself->setChildNodeDirection(TEXT_DIR_ENUM::TTB, true);
        }else{
            uiObjectItself->setChildNodeDirection(TEXT_DIR_ENUM::LTR, true);
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
