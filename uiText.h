
#ifndef ColorPick_iOS_SDL_uiText_h
#define ColorPick_iOS_SDL_uiText_h

struct uiText{

    // see also
    // TEXT_ALIGN TEXT_VERT_ALIGN

    typedef enum  {
        LEFT,
        CENTER_LEFTWARD,
        CENTER,
        CENTER_RIGHTWARD,
        RIGHT
    } TEXT_ALIGN;

    typedef enum  {
        TOP,
        VERT_CENTER,
        BOTTOM, // uiText::BOTTOM
    } TEXT_VERT_ALIGN;


/*

 SO PRELIM

 right now its squarified and sized based on HZ percent size starting at a provided fractional size

 */

    uiText(uiObject* parentObj, float size){

        hzAlign=TEXT_ALIGN::LEFT;
        vtAlign=TEXT_VERT_ALIGN::VERT_CENTER;

        Ux* uxInstance = Ux::Singleton(); // some useful helper?

        uiObjectItself = new uiObject();

        text_itself = new uiObject();
        text_position = new uiObject();

        uiObjectItself->myUiController = this; // this propagates to the other child objects


        //uiObjectItself->setInteractionCallback(tileClickedFn);


        //deleteColorPreview = new uiViewColor(uiObjectItself, Float_Rect(0.0, 0.0, 1.0, 0.27777777777778), false);



//        text_itself->hasBackground=true;
//        Ux::setColor(&text_itself->backgroundColor, 32, 0, 0, 128);
        text_itself->hasForeground=true;
        Ux::setColor(&text_itself->foregroundColor, 255, 255, 255, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        //uxInstance->printCharToUiObject(text_itself, CHAR_CHECKMARK_ICON, DO_NOT_RESIZE_NOW);
        //uxInstance->printStringToUiObject(text_itself, "xAll", DO_NOT_RESIZE_NOW);

        //text_itself->squarify();
        text_itself->squarifyKeepHz();

        //maths
        float w = size;
        float hw = w * 0.5;

        //no->setBoundaryRect( 0.0+pad, 0.5-hh, w, h);

        text_position->setBoundaryRect( 0.5, 0.5, w, w);

//        text_position->hasBackground=true;
//        Ux::setColor(&text_position->backgroundColor, 32, 0, 0, 128);
//
//        text_itself->hasBackground=true;
//        Ux::setColor(&text_itself->backgroundColor, 0, 32, 0, 128);


        text_itself->setBoundaryRect( -0.5, -0.5, 1.0, 1.0); // on right

        
        uiObjectItself->addChild(text_position);
        text_position->addChild(text_itself);
        parentObj->addChild(uiObjectItself);

        resize();

        text_itself->scale(1.8); // this effects the text scale / letter spacing

        //uxInstance->printStringToUiObject(text_itself, "ok 123", DO_NOT_RESIZE_NOW);


    }

    //anInteractionFn tileClicked=nullptr;

    uiObject *uiObjectItself; // no real inheritance here, this its the uiSqware, I would use self->
    uiObject *text_position;
    uiObject *text_itself;

    int lastDisplayTxtLen = 0;

    int hzAlign;
    int vtAlign;

    uiText* print(const char* txtToShow){
        Ux* uxInstance = Ux::Singleton();
        lastDisplayTxtLen = SDL_strlen(txtToShow);
        uxInstance->printStringToUiObject(text_itself, txtToShow, DO_NOT_RESIZE_NOW);
       // text_itself->updateRenderPosition();
        handlePositioning(uiObjectItself);
        return this;
    }

   /*The followign have no effect here - this text is simply set to "FLL" and "CENTER" on arbitrary UI object provided*/
    

    uiText* align(int hz){
        hzAlign = hz;
        return this;
    }
    uiText* align(int hz, int vert){
        hzAlign = hz;
        vtAlign = vert;
        return this;
    }


    /*

     the text will always ultimately shrink to fit
     the text has an initial size which is not lost

     */

    void handlePositioning(uiObject *p_dispalyNearUiObject){
        // we need to handle positioning of the main "text_position" element before animation starts, but after we know the text...
        float text_length = (float)lastDisplayTxtLen;

        float text_itself_size_scaling = 1.0/text_length;
        if( text_itself_size_scaling > text_position->origBoundryRect.w ){ // enforce "max" (default) size:
            text_itself_size_scaling=text_position->origBoundryRect.w;
        }
        Float_Rect* dispRect = &p_dispalyNearUiObject->collisionRect; // this rect has good w/h that we can use (its scaled to boundary space)
//        text_position->setBoundaryRect(dispRect->x + (dispRect->w * 0.5),
//                                        dispRect->y + (dispRect->h * 0.5),
//                                        text_itself_size_scaling,
//                                        text_itself_size_scaling  );



        switch( hzAlign ){

            case TEXT_ALIGN::LEFT:
            {
                float charsThatFit = (1.0/text_position->boundryRect.w);
                text_itself->boundryRect.x = charsThatFit * -0.5;
            }
                break;
            case TEXT_ALIGN::CENTER_LEFTWARD:
                    text_itself->boundryRect.x =  -text_length + 0.5; // right aligned (center the rightmost char)
                break;

            case TEXT_ALIGN::CENTER:
                    text_itself->boundryRect.x =  text_length * -0.5; // center
                break;

            case TEXT_ALIGN::CENTER_RIGHTWARD:
                text_itself->boundryRect.x = -0.5;
                break;

            case TEXT_ALIGN::RIGHT:

                break;
        }

        //

        //if( text_itself->boundryRect.x + -0.5 + text_position->boundryRect.x < 0 ) text_position->boundryRect.x += text_itself->boundryRect.x + -0.5 + text_position->boundryRect.x; // keep "on screen"

        float hidLeftAmt = text_position->boundryRect.x + (text_itself_size_scaling * text_itself->boundryRect.x);
        if( hidLeftAmt < 0 ){
            text_position->boundryRect.x -= hidLeftAmt; // minus a negative
        }


        text_position->updateRenderPosition();

    }

    void resize(){
        update();
    }

    void update(){
        uiObjectItself->updateRenderPosition();
    }

};


#endif