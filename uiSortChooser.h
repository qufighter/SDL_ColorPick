//
//  uiHueGradient.h
//  ColorPick SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiSortChooser_h
#define ColorPick_iOS_SDL_uiSortChooser_h


struct uiSortChooser{

    uiSortChooser(uiObject* parentObj, Float_Rect boundaries){

        Ux* uxInstance = Ux::Singleton(); // some useful helper?

        uiObjectItself = new uiObject();

        uiObjectItself->myUiController = this; // this propagates to the other child objects

        int counter = 0;
        int offset = 0;
        float width = 1.0 / 128;

        // 6 chunks of 0-255 rainbow
//        hueGradientHolder = new uiObject();
//        hueGradientHolder->setBoundaryRect(0.0, 0.0, 1.0, 1.0);
//        uiObjectItself->addChild(hueGradientHolder); // add first so controller proagates
        hueGradient = new uiHueGradientScroller(uiObjectItself, Float_Rect(0.0,0.0,1.0,1.0), true);


        previousContainer = new uiObject();
        previousContainer->setBoundaryRect(0.0, 0.0, 1.0, 0.25);// whatever we size here does  not matter
        uxInstance->printCharToUiObject(previousContainer, CHAR_ARR_DN, DO_NOT_RESIZE_NOW);
        uiObjectItself->addChild(previousContainer);

        uiObject* newContainer = new uiObject();
        newContainer->setBoundaryRect(0.0, 1.0-0.25, 1.0, 0.25);// whatever we size here does  not matter
        uxInstance->printCharToUiObject(newContainer, CHAR_ARR_UP, DO_NOT_RESIZE_NOW);
        uiObjectItself->addChild(newContainer);



//        uiObjectItself->setInteraction(interactionHueBgClicked);
//        uiObjectItself->setInteractionCallback(interactionHueBgClicked);

        //uiObjectItself->setInteractionCallback(Ux::interactionNoOp);
        parentObj->addChild(uiObjectItself);

        uiObjectItself->setBoundaryRect(&boundaries);
        resize();
    }

    uiObject* uiObjectItself; // no real inheritance here, this its the uiSqware, I would use self->
    uiHueGradientScroller* hueGradient;

    uiObject* previousContainer; // the above's hueGradientHolder is hueGradientHolder, there is still hueGradient->uiObjectItself between though...
    uiObject* newContainer;

    SDL_Color* getGradientOffsetColor(){ // get amount of scroll of the gradient, and we will make a color out of it...
        return hueGradient->getGradientOffsetColor();
    }

//    SDL_Color* colorForPercent(float percent){
//        return hueGradient->colorForPercent(percent);
//    }



    void resize(){


        Ux* uxInstance = Ux::Singleton();
//
//        uiObject* markerTop = huePositionMarker->childList[0];
//        uiObject* markerBtm = huePositionMarker->childList[1];
//
//        float markerHeight=0.55;
//        float markerOutset=0.25;
//        float hueMarkerWidth = 0.08;

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
