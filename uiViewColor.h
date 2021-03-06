//
//  ColorPick SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiViewColor_h
#define ColorPick_iOS_SDL_uiViewColor_h


struct uiViewColor{

    uiViewColor(uiObject* parentObj, Float_Rect boundaries, bool topShadow){

        resultText6char = (char*)SDL_malloc( sizeof(char) * 8 );
        resultText25char = (char*)SDL_malloc( sizeof(char) * 27 );

        Ux* uxInstance = Ux::Singleton();

        uiObjectItself = new uiObject();
        uiObjectItself->setBoundaryRect(&boundaries);
        uiObjectItself->myUiController = this;


        if( topShadow ){

            myEdgeShadow = new uiEdgeShadow(uiObjectItself, SQUARE_EDGE_ENUM::TOP, 0.0 );
            myEdgeShadow->addHighlight();

            myBottomEdgeShadow = new uiEdgeShadow(uiObjectItself, SQUARE_EDGE_ENUM::BOTTOM, 0.08 );
            //myBottomEdgeShadow->addHighlight();

        }else{
            hueBtn = nullptr;
            myEdgeShadow = nullptr;
        }


        hexBg = new uiObject();
        rgbBg = new uiObject();
        rgbRedBg = new uiObject();
        rgbGreenBg = new uiObject();
        rgbBlueBg = new uiObject();

        uiObjectItself->addChild(hexBg);
        uiObjectItself->addChild(rgbBg); // this isn't really containing the r g b elements just now... its for clicks though!
        uiObjectItself->addChild(rgbRedBg);
        uiObjectItself->addChild(rgbGreenBg);
        uiObjectItself->addChild(rgbBlueBg);


        hexValueText = new uiObject();


        //NEW rule size the container for the letter size of first letter
//        hexValueText->setBoundaryRect( 0.0, 0.0, 0.16666666666667, 0.6);

        //NEW rule 2 - the ohter properties define the text rendering only
        hexValueText->hasForeground = true;
        Ux::setColor(&hexValueText->foregroundColor, 255, 255, 255, 223); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)

        hexBg->hasBackground = true;
        Ux::setColor(&hexBg->backgroundColor,0, 0, 0, 0);

        hexBg->setInteractionCallback(&pickHexValueClicked);
        if( topShadow ){
            hexBg->setShouldCeaseInteractionChek(Ux::bubbleInteractionIfNonClick);
        }else{
            hexBg->setInteraction(&pickHexValueDragged);
            hexBg->interactionNonController = true; // controllers should NOT trigger this interaction...
        }

        // perhaps properties on container are inherited by text
        // however container itself will no longer render?

        uiObjectItself->addChild(hexValueText);

        SDL_snprintf(resultText6char, 7,  "000000");
        uxInstance->printStringToUiObject(hexValueText, resultText6char, DO_NOT_RESIZE_NOW);
        hexValueText->squarifyChildren();


        rgbBg->setInteractionCallback(&pickRgbValueClicked);
        rgbBg->setShouldCeaseInteractionChek(Ux::bubbleInteractionIfNonClick);

        // very odd not initializing these...

        rgbRedText = new uiObject();
        //NEW rule size the container for the letter size of first letter
//        rgbRedText->setBoundaryRect( 0.0, 0.6, 0.11111111111111, 0.4);
        //NEW rule 2 - the ohter properties define the text rendering only
        rgbRedText->hasForeground = true;
        Ux::setColor(&rgbRedText->foregroundColor, 255, 100, 100, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        rgbRedBg->hasBackground = true;
        Ux::setColor(&rgbRedBg->backgroundColor, 32, 0, 0, 192);
        uiObjectItself->addChild(rgbRedText);

        //sprintf(resultText6char, "  R");
        SDL_snprintf(resultText6char, 7,  "  R");
        uxInstance->printStringToUiObject(rgbRedText, resultText6char, DO_NOT_RESIZE_NOW);
        rgbRedText->squarifyChildren();


        rgbGreenText = new uiObject();
        //NEW rule size the container for the letter size of first letter
//        rgbGreenText->setBoundaryRect( 0.33333333333333, 0.6, 0.11111111111111, 0.4);
        //NEW rule 2 - the ohter properties define the text rendering only
        rgbGreenText->hasForeground = true;
        Ux::setColor(&rgbGreenText->foregroundColor, 100, 255, 100, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        rgbGreenBg->hasBackground = true;
        Ux::setColor(&rgbGreenBg->backgroundColor, 0, 32, 0, 192);
        uiObjectItself->addChild(rgbGreenText);
        //sprintf(resultText6char, "  G");
        SDL_snprintf(resultText6char, 7,  "  G");
        uxInstance->printStringToUiObject(rgbGreenText, resultText6char, DO_NOT_RESIZE_NOW);
        rgbGreenText->squarifyChildren();


        rgbBlueText = new uiObject();
        //NEW rule size the container for the letter size of first letter
//        rgbBlueText->setBoundaryRect( 0.66666666666666, 0.6, 0.11111111111111, 0.4);
        //NEW rule 2 - the ohter properties define the text rendering only
        rgbBlueText->hasForeground = true;
        Ux::setColor(&rgbBlueText->foregroundColor, 100, 100, 255, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        rgbBlueBg->hasBackground = true;
        Ux::setColor(&rgbBlueBg->backgroundColor, 0, 0, 32, 192);
        uiObjectItself->addChild(rgbBlueText);
        //sprintf(resultText6char, "  B");
        SDL_snprintf(resultText6char, 7,  "  B");
        uxInstance->printStringToUiObject(rgbBlueText, resultText6char, DO_NOT_RESIZE_NOW);
        rgbBlueText->squarifyChildren();


        if( topShadow ){
            hueBtn = new uiObject();
            hueBtn->setBoundaryRect( 0.5-0.125, 0.5, 0.25, 0.25);
            Ux::setColor(&hueBtn->foregroundColor, 255, 255, 255, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
            uxInstance->printCharToUiObject(hueBtn, CHAR_APP_ICON, DO_NOT_RESIZE_NOW);
            hueBtn->squarifyKeepVt();
            hueBtn->setInteractionBegin(&pickFromHueTouched);
            hueBtn->setClickInteractionCallback(&pickFromHueClicked);
            hueBtn->setShouldCeaseInteractionChek(Ux::bubbleWhenHidden);
            uiObjectItself->addChild(hueBtn);
            hueBtn->myUiController = this;
        }else{
            hueBtn = nullptr;
        }


        parentObj->addChild(uiObjectItself);

        alphaMulitiplier = 1.0;

        Ux::setColor(&maybeGeyedOutBg, 255, 255, 255, 192);
#ifdef COLORPICK_BASIC_MODE
        Ux::setColor(&maybeGeyedOutBg, 255, 255, 255, 64);
#endif

    }

    uiEdgeShadow* myEdgeShadow;
    uiEdgeShadow* myBottomEdgeShadow;


    uiObject* uiObjectItself; // no real inheritance here, this its the uiViewColor, I would use self->
    uiObject *hexValueText;
    uiObject *rgbRedText;
    uiObject *rgbGreenText;
    uiObject *rgbBlueText;

    uiObject *hexBg;
    uiObject *rgbBg;
    uiObject *rgbRedBg;
    uiObject *rgbGreenBg;
    uiObject *rgbBlueBg;

    uiObject *hueBtn;

    SDL_Color maybeGeyedOutBg;


    char* resultText6char; // seems that we should reuse this and not free it
    char* resultText25char;

    SDL_Color last_color;
    float alphaMulitiplier;

    char* getHexString(const char* prefix){
        return getHexString(prefix, false);
    }

    char* getHexString(const char* prefix, bool omitHash){
        const char* hash="#";
        if( omitHash ){
            hash = "";
        }
        SDL_snprintf(resultText25char, 25,  "%s%s%02x%02x%02x", prefix, hash, last_color.r, last_color.g, last_color.b);
        return resultText25char;
    }

    char* getVanillaRgbString(const char* prefix){
        SDL_snprintf(resultText25char, 25,  "%s%d,%d,%d", prefix, last_color.r, last_color.g, last_color.b);
        return resultText25char;
    }

    char* getRgbString(const char* prefix){
        SDL_snprintf(resultText25char, 25,  "%srgb(%d,%d,%d)", prefix, last_color.r, last_color.g, last_color.b);
        return resultText25char;
    }

    static void copyHexValueOmitHashClicked(uiObject *interactionObj, uiInteraction *delta){
        Ux* uxInstance = Ux::Singleton();
        uiViewColor* self = ((uiViewColor*)interactionObj->interactionProxy->myUiController);
        uxInstance->setClipboardText(self->getHexString("", true));
        uxInstance->defaultScoreDisplay->displayExplanation(" Paste Ready ");
        uxInstance->rClickMenu->hide();

    }

    static void copyHexValueClicked(uiObject *interactionObj, uiInteraction *delta){
        Ux* uxInstance = Ux::Singleton();

#ifdef COLORPICK_BASIC_MODE
        uxInstance->rClickMenu->hide();
        uxInstance->showBasicUpgradeMessage();
        return;

        // so if they use a copy that works (not this one)
        // lets start to count how many??
        //after enoough copies lets request a review...
        //OpenGLContext* ogg=OpenGLContext::Singleton();
        //ogg->doRequestReview();

#endif

        uiViewColor* self = ((uiViewColor*)interactionObj->interactionProxy->myUiController);
        //SDL_Log("REACHED COPY HEX CALLBACK");
        uxInstance->setClipboardText(self->getHexString(""));
        uxInstance->defaultScoreDisplay->displayExplanation(" Paste Ready ");
        uxInstance->rClickMenu->hide();
    }

    static void copyVanillaRgbValueClicked(uiObject *interactionObj, uiInteraction *delta){
        Ux* uxInstance = Ux::Singleton();
        uiViewColor* self = ((uiViewColor*)interactionObj->interactionProxy->myUiController);
        //SDL_Log("REACHED COPY RGB CALLBACK");
        uxInstance->setClipboardText(self->getVanillaRgbString(""));
        uxInstance->defaultScoreDisplay->displayExplanation(" Paste Ready ");
        uxInstance->rClickMenu->hide();
    }


    static void copyRgbValueClicked(uiObject *interactionObj, uiInteraction *delta){
        Ux* uxInstance = Ux::Singleton();

#ifdef COLORPICK_BASIC_MODE
        uxInstance->rClickMenu->hide();
        uxInstance->showBasicUpgradeMessage();
        return;
#endif

        uiViewColor* self = ((uiViewColor*)interactionObj->interactionProxy->myUiController);
        //SDL_Log("REACHED COPY RGB CALLBACK");
        uxInstance->setClipboardText(self->getRgbString(""));
        uxInstance->defaultScoreDisplay->displayExplanation(" Paste Ready ");
        uxInstance->rClickMenu->hide();
    }

    static void copyHsvValueClicked(uiObject *interactionObj, uiInteraction *delta){
        SDL_Log("REACHED COPY HSV CALLBACK"); // HSV_Color
    }

    static void showPaperClippy(){
        Ux* uxInstance = Ux::Singleton();
        // last menu item....lastIndex()
        uiText* text = (uiText*)uxInstance->rClickMenu->lastUiObject()->myUiController;
        uxInstance->printCharToUiObject(text->getTextFirstChar()->identity()->rotate(45), CHAR_PAPERCLIP, DO_NOT_RESIZE_NOW);
        uxInstance->printCharToUiObject(text->getTextChar(1)->identity()->rotate(45), CHAR_PAPERCLIP, DO_NOT_RESIZE_NOW);
    }

#define ECOPY_PREFIX "ee"

    static void pickHexValueClicked(uiObject *interactionObj, uiInteraction *delta){
        Ux* uxInstance = Ux::Singleton();
        uiViewColor* self = ((uiViewColor*)interactionObj->myUiController);
        if( delta->wasMenuClick() ){
            //SDL_Log("Double touched color preview......");
            uxInstance->rClickMenu->clearMenuItems();
            uxInstance->rClickMenu->addMenuItem(self->hexBg, self->getHexString(ECOPY_PREFIX"  ", true), &copyHexValueOmitHashClicked, nullptr);
            showPaperClippy();
            uxInstance->rClickMenu->addMenuItem(self->hexBg, self->getHexString(ECOPY_PREFIX" "), &copyHexValueClicked, &self->maybeGeyedOutBg);
            showPaperClippy();

            if( uxInstance->widescreen ){
                uxInstance->rClickMenu->display(self->uiObjectItself);
            }else{
                uxInstance->rClickMenu->display(self->hexBg);
            }
        }else{
            if( self->hueBtn == nullptr ){ // we are tryiing to cancel the modal....
                uxInstance->hideHistoryPalleteIfShowing(); // panning background...
            }
        }
    }

    static void pickRgbValueClicked(uiObject *interactionObj, uiInteraction *delta){
        Ux* uxInstance = Ux::Singleton();
        uiViewColor* self = ((uiViewColor*)interactionObj->myUiController);
        if( delta->wasMenuClick() ){
            //SDL_Log("Double touched rgb color preview......");
            uxInstance->rClickMenu->clearMenuItems();

            uxInstance->rClickMenu->addMenuItem(self->rgbRedBg, self->getVanillaRgbString(ECOPY_PREFIX" "), &copyVanillaRgbValueClicked, nullptr);
            showPaperClippy();

            uxInstance->rClickMenu->addMenuItem(self->rgbRedBg, ECOPY_PREFIX" rgb(,,)" /*self->getRgbString("Copy ")*/, &copyRgbValueClicked, &self->maybeGeyedOutBg);
            showPaperClippy();
            //uxInstance->rClickMenu->addMenuItem("Copy hsv(0,0,0)", &copyHsvValueClicked);
            uxInstance->rClickMenu->addMenuItem(self->rgbRedBg, self->getHexString(ECOPY_PREFIX" "), &copyHexValueClicked, &self->maybeGeyedOutBg);
            showPaperClippy();
            uxInstance->rClickMenu->addMenuItem(self->hexBg, self->getHexString(ECOPY_PREFIX"  ", true), &copyHexValueOmitHashClicked, nullptr);
            showPaperClippy();

            if( uxInstance->widescreen ){
                uxInstance->rClickMenu->display(self->uiObjectItself);
            }else{
                uxInstance->rClickMenu->display(self->rgbRedBg);
            }

        }else{
            if( self->hueBtn == nullptr ){ // we are tryiing to cancel the modal....
                uxInstance->hideHistoryPalleteIfShowing(); // panning background...
            }
        }
    }


    static void pickHexValueDragged(uiObject *interactionObj, uiInteraction *delta){
        Ux* uxInstance = Ux::Singleton();
        uiViewColor* self = ((uiViewColor*)interactionObj->myUiController);
        if( self->hueBtn == nullptr ){ // we are tryiing to cancel the modal....
            uxInstance->hideHistoryPalleteIfShowing(); // panning background...
        }
        delta->interactionNonTap(); // avoid double tap/isSecondInteraction
    }

    static void pickFromHueTouched(uiObject *interactionObj, uiInteraction *delta){
        Ux* uxInstance = Ux::Singleton();
        uxInstance->uxAnimations->scale_bounce(interactionObj, 0.005);
    }

    static void pickFromHueClicked(uiObject *interactionObj, uiInteraction *delta){
        Ux* uxInstance = Ux::Singleton();
        uiViewColor* self = ((uiViewColor*)interactionObj->myUiController);
        //if( !self->uiObjectItself->parentObject->is_being_viewed_state ) return;
        // uxInstance->hueClicked(self->last_color);
        uxInstance->hueClickedPickerHsv(&self->last_color); // < heavy op...
        uxInstance->uxAnimations->scale_bounce(interactionObj, 0.02);
    }

    void resize(Float_Rect boundaries){

        uiObjectItself->setBoundaryRect(&boundaries);
        //hexValueText->setChildNodeDirection(TEXT_DIR_ENUM::TTB, false); // see TEXT_DIR_ENUM
        Ux* uxInstance = Ux::Singleton();

        float hex_size=0.6;
        float rgb_size=0.4;

        if( uxInstance->widescreen ){
            hex_size=0.3;
            float rgb_bg_size=(1.0-hex_size);
            rgb_size=rgb_bg_size / 3.0; // letter width for 3 char...
            hexBg->setBoundaryRect( 0.0, 0.0, hex_size, 1.0);
            //NEW rule size the container for the letter size of first letter
            hexValueText->setBoundaryRect( 0.0, 0.0, hex_size, 0.16666666666667);
            hexValueText->setChildNodeDirection(TEXT_DIR_ENUM::TTB, false);

            rgbBg->setBoundaryRect( hex_size,0.0, rgb_bg_size, 1.0); // not actually container...
            rgbRedBg->setBoundaryRect( hex_size,0.0, rgb_bg_size, 0.333333333333333);
            rgbRedText->setBoundaryRect( hex_size,0.0, rgb_size, 0.333333333333333);
            rgbGreenBg->setBoundaryRect( hex_size, 0.33333333333333, rgb_bg_size, 0.333333333333333);
            rgbGreenText->setBoundaryRect( hex_size, 0.33333333333333, rgb_size, 0.333333333333333);
            rgbBlueBg->setBoundaryRect( hex_size, 0.66666666666666, rgb_bg_size, 0.333333333333333);
            rgbBlueText->setBoundaryRect( hex_size, 0.66666666666666, rgb_size, 0.333333333333333);


            if( myEdgeShadow != nullptr ){
                myEdgeShadow->resize(SQUARE_EDGE_ENUM::LEFT);
                myBottomEdgeShadow->resize(SQUARE_EDGE_ENUM::RIGHT);
            }

            if( hueBtn != nullptr ){
                float h = 0.09;
                float hh = h * 0.5;
                hueBtn->setBoundaryRect(hex_size-0.04-hh, 0.5-hh, h, h);
            }
        }else{
            hexBg->setBoundaryRect( 0.0, 0.0, 1.0, hex_size);
            //NEW rule size the container for the letter size of first letter
            hexValueText->setBoundaryRect( 0.0, 0.0, 0.16666666666667, hex_size);
            hexValueText->setChildNodeDirection(TEXT_DIR_ENUM::LTR, false);

            rgbBg->setBoundaryRect( 0.0, hex_size, 1.0, rgb_size); // not actually container...
            rgbRedBg->setBoundaryRect( 0.0, hex_size, 0.33333333333333, rgb_size);
            rgbRedText->setBoundaryRect( 0.0, hex_size, 0.11111111111111, rgb_size);
            rgbGreenBg->setBoundaryRect( 0.33333333333333, hex_size, 0.33333333333333, rgb_size);
            rgbGreenText->setBoundaryRect( 0.33333333333333, hex_size, 0.11111111111111, rgb_size);
            rgbBlueBg->setBoundaryRect( 0.66666666666666, hex_size, 0.33333333333333, rgb_size);
            rgbBlueText->setBoundaryRect( 0.66666666666666, hex_size, 0.11111111111111, rgb_size);

            if( myEdgeShadow != nullptr ){
                myEdgeShadow->resize(SQUARE_EDGE_ENUM::TOP);
                myBottomEdgeShadow->resize(SQUARE_EDGE_ENUM::BOTTOM);
            }

            if( hueBtn != nullptr ){
                float h = 0.25;
                float hh = h * 0.5;
                hueBtn->setBoundaryRect( 0.5-hh, hex_size-0.05-hh, h, h);
            }
        }
    }

    bool update(SDL_Color* color){

        Ux* uxInstance = Ux::Singleton();

        bool changed = setColorNotifyOfChange(&last_color, color);
        //last_color = color;

        //char* resultText6char = (char*)SDL_malloc( sizeof(char) * 6 ); // seems that we should reuse this and not free it
        //
        //sprintf(resultText6char, "%02x%02x%02x", color->r, color->g, color->b);

        SDL_snprintf(resultText6char, 7,  "%02x%02x%02x", color->r, color->g, color->b);


        Ux::setColor(&hexBg->backgroundColor,color->r, color->g, color->b, 255);
        uxInstance->printStringToUiObject(hexValueText, resultText6char, DO_NOT_RESIZE_NOW);

        //sprintf(resultText6char, "%3d", color->r);
        SDL_snprintf(resultText6char, 7,  "%3d", color->r);

        rgbRedBg->backgroundColor.a = SDL_min(color->r * alphaMulitiplier, 255);
        uxInstance->printStringToUiObject(rgbRedText, resultText6char, DO_NOT_RESIZE_NOW);

        //sprintf(resultText6char, "%3d", color->g);
        SDL_snprintf(resultText6char, 7,  "%3d", color->g);
        rgbGreenBg->backgroundColor.a = SDL_min(color->g * alphaMulitiplier, 255);
        uxInstance->printStringToUiObject(rgbGreenText, resultText6char, DO_NOT_RESIZE_NOW);

        //sprintf(resultText6char, "%3d", color->b);
        SDL_snprintf(resultText6char, 7,  "%3d", color->b);
        rgbBlueBg->backgroundColor.a = SDL_min(color->b * alphaMulitiplier, 255);
        uxInstance->printStringToUiObject(rgbBlueText, resultText6char, DO_NOT_RESIZE_NOW);


        //SDL_free(resultText6char);
        return changed;
    }

};


#endif
