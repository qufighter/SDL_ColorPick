//
//  uiScrollController.h
//  ColorPick iOS SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiViewColor_h
#define ColorPick_iOS_SDL_uiViewColor_h


struct uiViewColor{

    uiViewColor(uiObject* parentObj, Float_Rect boundaries){

        Ux* uxInstance = Ux::Singleton();

        uiObjectItself = new uiObject();
        uiObjectItself->setBoundaryRect(&boundaries);


        hexValueText = new uiObject();


        //NEW rule size the container for the letter size of first letter
        hexValueText->setBoundaryRect( 0.0, 0.0, 0.16666666666667, 0.6);

        //NEW rule 2 - the ohter properties define the text rendering only
        hexValueText->hasForeground = true;
        Ux::setColor(&hexValueText->foregroundColor, 255, 255, 255, 223); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)

        hexValueText->hasBackground = true;
        Ux::setColor(&hexValueText->backgroundColor,0, 0, 0, 0);


        // perhaps properties on container are inherited by text
        // however container itself will no longer render?

        uiObjectItself->addChild(hexValueText);


        sprintf(resultText6char, "000000");
        uxInstance->printStringToUiObject(hexValueText, resultText6char, DO_NOT_RESIZE_NOW);

        // very odd not initializing these...

        rgbRedText = new uiObject();
        //NEW rule size the container for the letter size of first letter
        rgbRedText->setBoundaryRect( 0.0, 0.6, 0.11111111111111, 0.4);
        //NEW rule 2 - the ohter properties define the text rendering only
        rgbRedText->hasForeground = true;
        Ux::setColor(&rgbRedText->foregroundColor, 255, 100, 100, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        rgbRedText->hasBackground = true;
        Ux::setColor(&rgbRedText->backgroundColor, 32, 0, 0, 192);
        uiObjectItself->addChild(rgbRedText);

        sprintf(resultText6char, "  R");
        uxInstance->printStringToUiObject(rgbRedText, resultText6char, DO_NOT_RESIZE_NOW);


        rgbGreenText = new uiObject();
        //NEW rule size the container for the letter size of first letter
        rgbGreenText->setBoundaryRect( 0.33333333333333, 0.6, 0.11111111111111, 0.4);
        //NEW rule 2 - the ohter properties define the text rendering only
        rgbGreenText->hasForeground = true;
        Ux::setColor(&rgbGreenText->foregroundColor, 100, 255, 100, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        rgbGreenText->hasBackground = true;
        Ux::setColor(&rgbGreenText->backgroundColor, 0, 32, 0, 192);
        uiObjectItself->addChild(rgbGreenText);
        sprintf(resultText6char, "  G");
        uxInstance->printStringToUiObject(rgbGreenText, resultText6char, DO_NOT_RESIZE_NOW);


        rgbBlueText = new uiObject();
        //NEW rule size the container for the letter size of first letter
        rgbBlueText->setBoundaryRect( 0.66666666666666, 0.6, 0.11111111111111, 0.4);
        //NEW rule 2 - the ohter properties define the text rendering only
        rgbBlueText->hasForeground = true;
        Ux::setColor(&rgbBlueText->foregroundColor, 100, 100, 255, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        rgbBlueText->hasBackground = true;
        Ux::setColor(&rgbBlueText->backgroundColor, 0, 0, 32, 192);
        uiObjectItself->addChild(rgbBlueText);
        sprintf(resultText6char, "  B");
        uxInstance->printStringToUiObject(rgbBlueText, resultText6char, DO_NOT_RESIZE_NOW);

        parentObj->addChild(uiObjectItself);
    }

    uiObject* uiObjectItself; // no real inheritance here, this its the uiViewColor, I would use self->
    uiObject *hexValueText;
    uiObject *rgbRedText;
    uiObject *rgbGreenText;
    uiObject *rgbBlueText;

    char* resultText6char = (char*)SDL_malloc( 6 ); // seems that we should reuse this and not free it

    void update(SDL_Color* color){
        //char* resultText6char; //leaking memory???
        Ux* uxInstance = Ux::Singleton();

        //char* resultText6char = (char*)SDL_malloc( 6 ); // seems that we should reuse this and not free it


        sprintf(resultText6char, "%02x%02x%02x", color->r, color->g, color->b);
        Ux::setColor(&hexValueText->backgroundColor,color->r, color->g, color->b, 255);
        uxInstance->printStringToUiObject(hexValueText, resultText6char, DO_NOT_RESIZE_NOW);

        sprintf(resultText6char, "%3d", color->r);
        rgbRedText->backgroundColor.a = color->r;
        uxInstance->printStringToUiObject(rgbRedText, resultText6char, DO_NOT_RESIZE_NOW);

        sprintf(resultText6char, "%3d", color->g);
        rgbGreenText->backgroundColor.a = color->g;
        uxInstance->printStringToUiObject(rgbGreenText, resultText6char, DO_NOT_RESIZE_NOW);

        sprintf(resultText6char, "%3d", color->b);
        rgbBlueText->backgroundColor.a = color->b;
        uxInstance->printStringToUiObject(rgbBlueText, resultText6char, DO_NOT_RESIZE_NOW);


        //SDL_free(resultText6char);
    }

};


#endif
