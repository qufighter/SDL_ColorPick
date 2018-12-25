//
//  uiRunningMan.h
//  ColorPick iOS SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiRunningMan_h
#define ColorPick_iOS_SDL_uiRunningMan_h

#define STRIPE_MIN_LENGTH 0.10
#define STRIPE_MAX_EXTENSION 0.20

struct uiRunningMan{

    uiRunningMan(uiObject* parentObj, Float_Rect boundaries){
        //
        
        Ux* uxInstance = Ux::Singleton();

        uiObjectItself = new uiObject();
        uiObjectItself->myUiController = this;

        uiObjectItself->setBoundaryRect(&boundaries);

        speedStripes = new uiObject();
        speedStripes->doesInFactRender = false; // only children render

        for( int i=3; i<11; i++ ){

            if( i > 5 && i < 8 ) continue;

            float extra = 0;
            if( i > 2 && i < 6 ){
                extra = 0.20;
            }

            uiObject* stripe = new uiObject();
            stripe->hasBackground=true;
            Ux::setColor(&stripe->backgroundColor, 192, 192, 192, 255);
            stripe->setBoundaryRect( i*0.075 + extra, i*0.0855, STRIPE_MIN_LENGTH, 0.015 );
            speedStripes->addChild(stripe);
        }

        runner = new uiObject();

//        runner->hasBackground=true;
//        Ux::setColor(&runner->backgroundColor, 32, 0, 0, 128);
        runner->hasForeground=true;
        Ux::setColor(&runner->foregroundColor, 192, 192, 192, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        uxInstance->printCharToUiObject(runner, CHAR_ZOOM_SPEEDY_GUY, DO_NOT_RESIZE_NOW);
        //no->rotate(180.0f);
        //runner->setClickInteractionCallback(defaultCancelFn);
        runner->setRoundedCorners(0.5);


        // top edge....... or just an edge...  this is double edge

//            top_shadow = new uiObject();
//
//            //top_shadow->setBoundaryRect( 0.0, -0.05, 1.0, 0.1);
//            top_shadow->hasBackground = true;
//            Ux::setColor(&top_shadow->backgroundColor,0, 0, 0, 128);
//            top_shadow->setRoundedCorners(0.5, 0.5, 0, 0);
//            //uiObjectItself->addChild(top_shadow);
//
//            top_shadow2 = new uiObject();
//            //top_shadow2->setBoundaryRect( 0.0, 0.25, 1.0, 0.5);
//            top_shadow2->hasBackground = true;
//            Ux::setColor(&top_shadow2->backgroundColor,255, 255, 255, 128);
//            top_shadow2->setRoundedCorners(0.5, 0.5, 0, 0);
//            top_shadow->addChild(top_shadow2);
//
//            top_shadow2->doesInFactRender = false; //addHighlight
//
        uiObjectItself->addChild(speedStripes);

        uiObjectItself->addChild(runner);

        parentObj->addChild(uiObjectItself);



        resize();

    }

    uiObject* uiObjectItself; // no real inheritance here, this its the uiRunningMan, I would use self->
    uiObject* runner;
    uiObject* speedStripes;

    void hide(){
        uiObjectItself->hide();
    }
    void show(float intensity){
        uiObjectItself->show();
        for( int i=0; i<speedStripes->getChildCount(); i++ ){
            speedStripes->childList[i]->boundryRect.w = STRIPE_MIN_LENGTH + (STRIPE_MAX_EXTENSION * intensity);
        }
        speedStripes->updateRenderPosition();
    }

    void resize(){

    }

    void update(){

    }

};


#endif
