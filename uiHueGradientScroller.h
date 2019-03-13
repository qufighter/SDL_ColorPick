//
//  uiHueGradientScroller.h
//  ColorPick SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiHueGradientScroller_h
#define ColorPick_iOS_SDL_uiHueGradientScroller_h


struct uiHueGradientScroller{

    uiHueGradientScroller(uiObject* parentObj, Float_Rect boundaries, bool allowScrolling){

        changeCallback = nullptr;

        Ux* uxInstance = Ux::Singleton(); // some useful helper?

        uiObjectItself = new uiObject();

        uiObjectItself->myUiController = this; // this propagates to the other child objects  NOTE: this is one case right now where there are really 2 myUiController sometiems depending how this is nested....

        int counter = 0;
        int offset = 0;
        float width = 1.0 / 128;
        int increment = 12;
        int end = uxInstance->hueGradientData->totalColors - 6;
        // 6 chunks of 0-255 rainbow
        hueGradientHolder = new uiObject();
        hueGradientHolder->setBoundaryRect(0.0, 0.0, 1.0, 1.0);
        uiObjectItself->addChild(hueGradientHolder); // add first so controller proagates

        if( allowScrolling ){
            increment = 24;
            end = end * 2;
            width = 1.0 / 256;
            hueGradientHolder->setBoundaryRect(0.0, 0.0, 2.0, 1.0);
            hueGradientHolder->setInteraction(&draggingTheGradient);
            hueGradientHolder->boundryRect.x = -0.281250; // BGYORVI
        }

        while( offset < end ){
            uiObject* rt = new uiObject();
            rt->hasBackground=true;
            Ux::setColor(&rt->backgroundColor, &uxInstance->hueGradientData->manyColors[offset%uxInstance->hueGradientData->totalColors]);
            //rt->setInteractionCallback(interactionHueClicked); //setClickInteractionCallback
            rt->setBoundaryRect( counter * (width) , 0.0, width+0.01, 1.0);
            hueGradientHolder->addChild(rt);
            //uiObjectItself->addChild(rt);
            offset+=increment;
            counter++;
        }

        if( allowScrolling ){
            arrowIndicatorL = new uiObject();
            arrowIndicatorL->setBoundaryRect(0.0, 0.0, 0.15, 1.0);
            uxInstance->printCharToUiObject(arrowIndicatorL, CHAR_ARR_LEFT, DO_NOT_RESIZE_NOW);
            arrowIndicatorL->setClickInteractionCallback(&interactionLClicked);
            uiObjectItself->addChild(arrowIndicatorL);
            arrowIndicatorR = new uiObject();
            arrowIndicatorR->setBoundaryRect(1.0-0.15, 0.0, 0.15, 1.0);
            uxInstance->printCharToUiObject(arrowIndicatorR, CHAR_ARR_RIGHT, DO_NOT_RESIZE_NOW);
            arrowIndicatorR->setClickInteractionCallback(&interactionRClicked);
            uiObjectItself->addChild(arrowIndicatorR);
        }

        //uiObjectItself->setInteractionCallback(Ux::interactionNoOp);
        parentObj->addChild(uiObjectItself);
        resize(boundaries, true);
    }


    uiObject* uiObjectItself; // no real inheritance here, this its the uiSqware, I would use self->
    uiObject* hueGradientHolder;
    bool hueSliderVisible;

    uiObject* arrowIndicatorL; // the above's hueGradientHolder is hueGradientHolder, there is still hueGradient->uiObjectItself between though...
    uiObject* arrowIndicatorR;

    anInteractionFn changeCallback;

    void addChangeCallback(anInteractionFn p_interactionCallback){
        changeCallback = p_interactionCallback;
    }

    static void interactionLClicked(uiObject *interactionObj, uiInteraction *delta){
        Ux* uxInstance = Ux::Singleton();
        uiHueGradientScroller* self = ((uiHueGradientScroller*)interactionObj->myUiController);
        self->hueGradientHolder->boundryRect.x-=1.0 / uxInstance->hueGradientData->totalColors;
        self->handleWrapping();
        self->hueGradientHolder->updateRenderPosition();
        if( self->changeCallback!=nullptr ){
            self->changeCallback(interactionObj, delta);
        }
    }

    static void interactionRClicked(uiObject *interactionObj, uiInteraction *delta){
        Ux* uxInstance = Ux::Singleton();
        uiHueGradientScroller* self = ((uiHueGradientScroller*)interactionObj->myUiController);
        self->hueGradientHolder->boundryRect.x+=1.0 / uxInstance->hueGradientData->totalColors;
        self->handleWrapping();
        self->hueGradientHolder->updateRenderPosition();
        if( self->changeCallback!=nullptr ){
            self->changeCallback(interactionObj, delta);
        }
    }

    void handleWrapping(){
        if( hueGradientHolder->boundryRect.x > 0.0 ){
            hueGradientHolder->boundryRect.x-=1.0;
        }else if( hueGradientHolder->boundryRect.x < -1.0 ){
            hueGradientHolder->boundryRect.x+=1.0;
        }
    }

    static void draggingTheGradient(uiObject *interactionObj, uiInteraction *delta){
        uiHueGradientScroller* self = ((uiHueGradientScroller*)interactionObj->myUiController);
        self->hueGradientHolder->boundryRect.x += delta->rx;
        self->handleWrapping();
        //SDL_Log("self->hueGradientHolder->boundryRect.x %f", self->hueGradientHolder->boundryRect.x);
        self->hueGradientHolder->updateRenderPosition();

        // we could process the events this way, but we need to throttle them....
//        if( self->changeCallback!=nullptr ){
//            self->changeCallback(interactionObj, delta);
//        }
    }

    float getGradientOffsetPercentage(){ // get amount of scroll of the gradient, and we will make a color out of it...
        return SDL_fabsf( hueGradientHolder->boundryRect.x );
    };

    SDL_Color* getGradientOffsetColor(){ // get amount of scroll of the gradient, and we will make a color out of it...
        return colorForPercent(getGradientOffsetPercentage());
    }

    SDL_Color* colorForPercent(float percent){
        Ux* uxInstance = Ux::Singleton(); // some useful helper?
        int clrOffset =  SDL_floorf((uxInstance->hueGradientData->totalColors-1) * percent);
        return &uxInstance->hueGradientData->manyColors[clrOffset];
    }

    void resize(bool skipUpdate){
        Ux* uxInstance = Ux::Singleton();

        if( uxInstance->widescreen ){ // widescreen
            hueGradientHolder->setChildNodeDirection(TEXT_DIR_ENUM::TTB, true);

        }else{
            hueGradientHolder->setChildNodeDirection(TEXT_DIR_ENUM::LTR, true);
        }

        if( !skipUpdate ){
            update();
        }
    }


    void resize(Float_Rect boundaries, bool skipUpdate){

        uiObjectItself->setBoundaryRect(&boundaries);

        resize(skipUpdate);
    }

    void update(){ // w/h

        uiObjectItself->updateRenderPosition();

    }


};


#endif
