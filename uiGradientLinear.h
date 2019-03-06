//
//  uiGradientLinear.h
//  ColorPick SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiGradientLinear_h
#define ColorPick_iOS_SDL_uiGradientLinear_h



/*
 aloud: how can this relate to, share API with, otherwise be a descendant of uiViewColor ?  might this be simply another way that a uiViewColor can optionally be represented???
 how much of the same API is really needed...  in any case future version of uiViewColor could do much of what this does, so we will try to keep the api the same.
 */


struct uiGradientLinear{

    typedef struct Gradient_Stop
    {
        SDL_Color color;
        float positionPercent;
        Gradient_Stop(float percentage, SDL_Color* clr){
            Ux::setColor(&color, clr);
            positionPercent = percentage;
        }
        Gradient_Stop(float percentage, Uint8 r, Uint8 g, Uint8 b, Uint8 a){
            Ux::setColor(&color, r,g,b,a);
            positionPercent = percentage;
        }

    } Gradient_Stop;

    uiObject* uiObjectItself; // child elements are gradient stripes
    int angle; // we really only support 0 and 90 right now....
    int detail; // how many child objects, max is 128 right now according to uiObject...
    uiList<Gradient_Stop, Uint8>* gradientStops;

    uiGradientLinear(uiObject* parentObj, Float_Rect boundaries){

        Ux* uxInstance = Ux::Singleton();

        gradientStops = new uiList<Gradient_Stop, Uint8>(5); // max 5 stops right now.... plenty...

        uiObjectItself = new uiObject();
        uiObjectItself->myUiController = this;
        //uiObjectItself->matrixInheritedByDescendants=true;

        uiObjectItself->setBoundaryRect(&boundaries);

        detail = 32;
        angle = 0;

        

        parentObj->addChild(uiObjectItself);

        resize();
    }



    uiGradientLinear* addStop(float percentage, Uint8 r, Uint8 g, Uint8 b, Uint8 a){
        gradientStops->add(Gradient_Stop(percentage, r, g, b, a));
        return this;
    }

    void show(){
        uiObjectItself->showAndAllowInteraction();
    }

    void hide(){
        uiObjectItself->hideAndNoInteraction();
    }

    void resize(){

    }

    void print(const char* txtToShow){
        //hexDisplay->print(txtToShow);
    }

    uiGradientLinear* update(){

        // first lets allocate our child objects if they aren't already there...

        gradientStops->sort(&Ux::compareGradientStopItems);
        uiListIterator<uiList<Gradient_Stop, Uint8>, Gradient_Stop>* stopIterator = gradientStops->iterate();
        Gradient_Stop* stop = stopIterator->next();
        Gradient_Stop* prevStop = stop;
        Gradient_Stop* lastStop = stop;

        // should be sorted so lowest percentage is now first....
        // next we will assume 0 means vertial gradient starting from the top, for now....

        float eachRow = 1.0 / detail;
        float prog = 0;

        for( int x=0; x<detail; x++){
            prog = x / (detail + 0.0f);
            uiObject* child;
            if( x < uiObjectItself->childListIndex + 1 ){
                child = new uiObject();
                uiObjectItself->addChild(child);
            }else{
                child = uiObjectItself->childList[x];
            }

            if( prog > stop->positionPercent ){
                prevStop = stop; // gonna advance
            }

            while( stop != nullptr && prog > stop->positionPercent ){
                stop = stopIterator->next();
            }
            if(stop == nullptr){
                stop = lastStop;
            }

            child->setBoundaryRect(0.0, x*eachRow, 1.0, eachRow);

            child->setBackgroundColor(&stop->color);


            float stopDist = stop->positionPercent - prevStop->positionPercent;
            if( stopDist <= 0 ) stopDist = 1.0;
            float multipl = 1.0 / stopDist;

            /// so we know prog is between prevStop->positionPercent and stop->positionPercent
            float stopProg = (prog - prevStop->positionPercent);
            if( stopProg < 0 ) stopProg = 0;

//
//            SDL_Log("gradient stop prev %f %i %i %i", prevStop->positionPercent, prevStop->color.r, prevStop->color.g, prevStop->color.b);
//            SDL_Log("gradient stop curr %f %i %i %i", stop->positionPercent, stop->color.r, stop->color.g, stop->color.b);
//            SDL_Log("overall prog:  %f inter-stop prog %f", prog, stopProg);


            SDL_Color mixed = Ux::mixColors(&prevStop->color, &stop->color, 1.0 - (stopProg * multipl));

            //SDL_Log("result %i %i %i", mixed.r, mixed.g, mixed.b);


            child->setBackgroundColor(&mixed);
            
            lastStop = stop;
        }

        /*
         Ux::mixColors(SDL_Color *a, SDL_Color *b, float progressAB){
         // progress can move from 0.0 (completely A) to 1.0 (completely B)
         */



//        int delCounter=0;
//        while(stop != nullptr){
//            SDL_Log("gradient stop %f %i %i %i", stop->positionPercent, stop->color.r, stop->color.g, stop->color.b);
//
//
//            stop = stopIterator->next();
//        }


        SDL_free(stopIterator);

        return this;
    }

    void refresh(){
        //update(&last_color);
    }



};


#endif
