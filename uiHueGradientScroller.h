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



        //uiObjectItself->setInteractionCallback(Ux::interactionNoOp);
        parentObj->addChild(uiObjectItself);
        resize(boundaries);
    }


    uiObject* uiObjectItself; // no real inheritance here, this its the uiSqware, I would use self->
    uiObject* hueGradientHolder;
    bool hueSliderVisible;


    float lastPickPercent = 0.0f;

    static void draggingTheGradient(uiObject *interactionObj, uiInteraction *delta){
        uiHueGradientScroller* self = ((uiHueGradientScroller*)interactionObj->myUiController);
        self->hueGradientHolder->boundryRect.x += delta->rx;


        if( self->hueGradientHolder->boundryRect.x > 0.0 ){
            self->hueGradientHolder->boundryRect.x-=1.0;
        }else if( self->hueGradientHolder->boundryRect.x < -1.0 ){
            self->hueGradientHolder->boundryRect.x+=1.0;
        }

        //SDL_Log("self->hueGradientHolder->boundryRect.x %f", self->hueGradientHolder->boundryRect.x);

        self->hueGradientHolder->updateRenderPosition();
    }

    SDL_Color* colorForPercent(float percent){
        Ux* uxInstance = Ux::Singleton(); // some useful helper?
        lastPickPercent = percent;
        int clrOffset =  SDL_floorf((uxInstance->hueGradientData->totalColors-1) * percent);
        return &uxInstance->hueGradientData->manyColors[clrOffset];
    }


    void resize(Float_Rect boundaries){

        uiObjectItself->setBoundaryRect(&boundaries);

        Ux* uxInstance = Ux::Singleton();

        if( uxInstance->widescreen ){ // widescreen
            hueGradientHolder->setChildNodeDirection(TEXT_DIR_ENUM::TTB, true);

        }else{
            hueGradientHolder->setChildNodeDirection(TEXT_DIR_ENUM::LTR, true);
        }

        update();
    }

    void update(){ // w/h

        uiObjectItself->updateRenderPosition();

    }


};


#endif
