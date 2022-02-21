//
//  uiSwatchesGradient.h
//  ColorPick SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiSwatchesGradient_h
#define ColorPick_iOS_SDL_uiSwatchesGradient_h



/*
 aloud: how can this relate to, share API with, otherwise be a descendant of uiViewColor ?  might this be simply another way that a uiViewColor can optionally be represented???
 how much of the same API is really needed...  in any case future version of uiViewColor could do much of what this does, so we will try to keep the api the same.
 */


struct uiSwatchesGradient{



    uiObject* uiObjectItself; // child elements are gradient stripes
    uiGradientLinear* grad1;
    uiGradientLinear* grad2;


    uiSwatchesGradient(uiObject* parentObj, Float_Rect boundaries, bool mixReal){

        Ux* uxInstance = Ux::Singleton();


        uiObjectItself = new uiObject();
        uiObjectItself->myUiController = this;
        uiObjectItself->setBoundaryRect(&boundaries);

        grad1 = new uiGradientLinear(uiObjectItself, Float_Rect(0.0,0.0,0.1,1.0), mixReal);
        grad2 = new uiGradientLinear(uiObjectItself, Float_Rect(0.1,0.0,0.9,1.0), mixReal);

        parentObj->addChild(uiObjectItself);

        resize();
    }


    uiSwatchesGradient* show(){
        uiObjectItself->showAndAllowInteraction();
        return this;
    }

    uiSwatchesGradient* hide(){
        uiObjectItself->hideAndNoInteraction();
        return this;
    }

    void resize(){

    }

    uiSwatchesGradient* update(SDL_Color* destClr1, SDL_Color* destClr2, SDL_Color* mixed){

        // todo, if we omit mixed its computable...

        grad1->clearStops()
            ->addStop(0.0, destClr1)
            ->addStop(1.0, destClr2)
            ->update();

        grad2->clearStops()
            ->addStop(0.2, destClr1)
            ->addStop(0.4, mixed)
            ->addStop(0.6, mixed)
            ->addStop(0.8, destClr2)
            ->update();

        return this;
    }

    void refresh(){
        //update(&last_color);
    }



};


#endif
