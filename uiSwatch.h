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

    char* resultText6char;

    uiSwatch(uiObject* parentObj, Float_Rect boundaries){

        resultText6char = (char*)SDL_malloc( sizeof(char) * 8 );

        Ux* uxInstance = Ux::Singleton();

        uiObjectItself = new uiObject();
        uiObjectItself->myUiController = this;
        //uiObjectItself->matrixInheritedByDescendants=true; (when you need it)

        uiObjectItself->setBoundaryRect(&boundaries);


        uiObjectItself->hasBackground=true;
        Ux::setColor(&uiObjectItself->backgroundColor, 128, 128, 128, 255);


        uiObjectItself->squarify();

        swatchItself = new uiObject();
        // todo: scale border dynamically!!!! or statically><

        uiObjectItself->setRoundedCorners(0.23);
        swatchItself->setRoundedCorners(0.24);

        float border_w = 0.025;
        swatchItself->setBoundaryRect(border_w,border_w,1.0-border_w-border_w,1.0-border_w-border_w);
        swatchItself->hasBackground=true;
        Ux::setColor(&swatchItself->backgroundColor, 0, 0, 0, 192);
        uiObjectItself->addChild(swatchItself);

//        swatchItself->hasForeground=true;
//        Ux::setColor(&swatchItself->foregroundColor, 192, 192, 192, 255);
//        uxInstance->printCharToUiObject(swatchItself, CHAR_ZOOM_SPEEDY_GUY, DO_NOT_RESIZE_NOW);


        hexDisplay = (new Ux::uiText(uiObjectItself, 1.0/6))->pad(0.0,0.0)->margins(0.0,0.05,0.0,0.05)->print("");

        parentObj->addChild(uiObjectItself);

        resize();

        displayHexOn=false;
    }

    uiObject* uiObjectItself; //BTW: this is the border too...  // no real inheritance here, this its the uiSwatch, I would use self->
    uiObject* swatchItself;
    Ux::uiText* hexDisplay;

    SDL_Color last_color;

    bool displayHexOn;

    uiSwatch* displayHex(){
        displayHexOn = true;
        return this;
    }

    void resize(){

    }

    bool update(SDL_Color* color){

        //Ux* uxInstance = Ux::Singleton();

        bool changed = setColorNotifyOfChange(&last_color, color);

        Ux::setColor(&swatchItself->backgroundColor, color->r, color->g, color->b, 255);

        if( displayHexOn ){
            SDL_snprintf(resultText6char, 7,  "%02x%02x%02x", color->r, color->g, color->b);
            hexDisplay->print(resultText6char);
        }

        return changed;
    }


};


#endif
