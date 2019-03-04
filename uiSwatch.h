//
//  uiSwatch.h
//  ColorPick SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiSwatch_h
#define ColorPick_iOS_SDL_uiSwatch_h

#define STRIPE_MIN_LENGTH 0.10
#define STRIPE_MAX_EXTENSION 0.20

/*
 aloud: how can this relate to, share API with, otherwise be a descendant of uiViewColor ?  might this be simply another way that a uiViewColor can optionally be represented???
 how much of the same API is really needed...  in any case future version of uiViewColor could do much of what this does, so we will try to keep the api the same.
 */

struct uiSwatch{

    uiSwatch(uiObject* parentObj, Float_Rect boundaries){

        Ux* uxInstance = Ux::Singleton();

        uiObjectItself = new uiObject();
        uiObjectItself->myUiController = this;
        uiObjectItself->matrixInheritedByDescendants=true;

        uiObjectItself->setBoundaryRect(&boundaries);

        uiObjectItself->hasBackground=true;
        Ux::setColor(&uiObjectItself->backgroundColor, 128, 128, 128, 255);

        //uiObjectItself->squarify();
        uiObjectItself->squarifyKeepVt(); //make dynamic??

        swatchItself = new uiObject();
        // todo: scale border dynamically!!!! or statically><

        uiObjectItself->setRoundedCorners(0.23);
        swatchItself->setRoundedCorners(0.24);

        float border_w = 0.025;
        swatchItself->setBoundaryRect(border_w,border_w,1.0-border_w-border_w,1.0-border_w-border_w);
        swatchItself->hasBackground=true;
        Ux::setColor(&swatchItself->backgroundColor, 0, 0, 0, 255);
        uiObjectItself->addChild(swatchItself);

//        swatchItself->hasForeground=true;
//        Ux::setColor(&swatchItself->foregroundColor, 192, 192, 192, 255);
//        uxInstance->printCharToUiObject(swatchItself, CHAR_ZOOM_SPEEDY_GUY, DO_NOT_RESIZE_NOW);

        parentObj->addChild(uiObjectItself);

        
        hexDisplay = (new Ux::uiText(uiObjectItself, 1.0/6))->pad(0.0,0.0)->margins(0.0,0.05,0.0,0.05)->print("");


        resize();

        displayHexOn=false;
        displayBgOn=true;
    }

    uiObject* uiObjectItself; //BTW: this is the border too...  // no real inheritance here, this its the uiSwatch, I would use self->
    uiObject* swatchItself;
    Ux::uiText* hexDisplay;

    SDL_Color last_color;

    bool displayHexOn;
    bool displayBgOn;

    uiSwatch* displayHex(){
        displayHexOn = true;
        return this;
    }

    uiSwatch* hideHex(){
        displayHexOn = false;
        return this;
    }

    uiSwatch* displayBg(){
        displayBgOn = true;
        return this;
    }

    uiSwatch* hideBg(){
        displayBgOn = false;
        return this;
    }

    bool isFlipped(){
        return displayHexOn && displayBgOn;
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
        hexDisplay->print(txtToShow);
    }

    bool update(SDL_Color* color){

        Ux* uxInstance = Ux::Singleton();

        bool changed = setColorNotifyOfChange(&last_color, color);

        if( displayBgOn ){
            Ux::setColor(&swatchItself->backgroundColor, color->r, color->g, color->b, 255);
        }else{
            Ux::setColor(&swatchItself->backgroundColor, 0, 0, 0, 255);
        }

        if( ((int)swatchItself->backgroundColor.r + (int)swatchItself->backgroundColor.g + (int)swatchItself->backgroundColor.b) > 382 ){
            hexDisplay->color(0, 0, 0, 255);
        }else{
            hexDisplay->color(255, 255, 255, 255);
        }

        if( displayHexOn ){
            SDL_snprintf(uxInstance->print_here, 7,  "%02x%02x%02x", color->r, color->g, color->b);
            hexDisplay->print(uxInstance->print_here);
        }else{
            hexDisplay->print("");
        }

        return changed;
    }

    void refresh(){
        update(&last_color);
    }



};


#endif
