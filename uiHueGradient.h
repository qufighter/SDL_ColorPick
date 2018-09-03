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
        uiObject* currentContainer = new uiObject();
        currentContainer->setBoundaryRect(0.0, 0.0, 1.0, 1.0);
        uiObjectItself->addChild(currentContainer); // add first so controller proagates

        while( offset < totalColors ){
            uiObject* rt = new uiObject();
            rt->hasBackground=true;
            Ux::setColor(&rt->backgroundColor, &manyColors[offset]);
            rt->setInteractionCallback(interactionHueClicked); //setClickInteractionCallback
            rt->setBoundaryRect( counter * (width) , 0.0, width+0.01, 1.0);
            currentContainer->addChild(rt);
            offset+=12;
            counter++;
        }


        //uiObjectItself->setInteractionCallback(Ux::interactionNoOp);
        parentObj->addChild(uiObjectItself);
        resize(boundaries);
    }

    anInteractionFn tileClicked=nullptr;

    uiObject* uiObjectItself; // no real inheritance here, this its the uiSqware, I would use self->

    int totalColors = 1530;
    SDL_Color manyColors[1530];// *totalColorsPer * 6

    static void interactionHueClicked(uiObject *interactionObj, uiInteraction *delta){
       // Ux* uxInstance = Ux::Singleton();
        uiHueGradient* self = ((uiHueGradient*)interactionObj->myUiController);

        self->tileClicked(interactionObj, delta);
    }

    void resize(Float_Rect boundaries){

        uiObjectItself->setBoundaryRect(&boundaries);

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
