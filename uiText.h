
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

 this is generated from uiScore but doesn't yet do all those things... quite yet - main difference is text scale is handled only once here
 // so it is not yet having a mode where this may be auto adjusted to fit the container

 */

    uiText(uiObject* parentObj, float size){

        hzAlign=TEXT_ALIGN::LEFT;
        vtAlign=TEXT_VERT_ALIGN::VERT_CENTER;

        //Ux* uxInstance = Ux::Singleton(); // some useful helper?

        uiObjectItself = new uiObject();

        //uiObjectItself->matrixInheritedByDescendants = true;

        text_itself = new uiObject();
        text_backgr = new uiObject();
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
        //float w = size;
        //float hw = w * 0.5;

        //no->setBoundaryRect( 0.0+pad, 0.5-hh, w, h);

        text_position->setBoundaryRect( 0.5, 0.5, size, size);

//        uiObjectItself->hasBackground=true;
//        Ux::setColor(&uiObjectItself->backgroundColor, 0, 0, 64, 192);

//        text_position->hasBackground=true;
//        Ux::setColor(&text_position->backgroundColor, 64, 0, 0, 192);

//
//        text_itself->hasBackground=true;
//        Ux::setColor(&text_itself->backgroundColor, 0, 64, 0, 128);

        //uiObjectItself->setModeWhereChildCanCollideAndOwnBoundsIgnored();
        text_position->setModeWhereChildCanCollideAndOwnBoundsIgnored();
        text_position->doesInFactRender = false;

        uiObjectItself->doesInFactRender = false;


        text_itself->setModeWhereChildCanCollideAndOwnBoundsIgnored();

        text_itself->setBoundaryRect( -0.5, -0.5, 1.0, 1.0); // on right


        //text_position->matrixInheritedByDescendants = true;

        uiObjectItself->addChild(text_position);
        text_position->addChild(text_backgr);
        text_position->addChild(text_itself);
        parentObj->addChild(uiObjectItself);

        resize();

        text_backgr->hide();

        text_itself->scale(1.8); // this effects the text scale / letter spacing

        //uxInstance->printStringToUiObject(text_itself, "ok 123", DO_NOT_RESIZE_NOW);


    }

    //anInteractionFn tileClicked=nullptr;

    uiObject *uiObjectItself; // no real inheritance here, this its the uiSqware, I would use self->
    uiObject *text_position;
    uiObject *text_itself;
    uiObject *text_backgr;

    long lastDisplayTxtLen = 0;

    int hzAlign;
    int vtAlign;

    bool textBackground = false;

    uiObject* getTextFirstChar(){
        return text_itself->childList[0];
    }

    uiObject* getTextChar(int index){
        return text_itself->childList[index];
    }

    uiText* print(const char* txtToShow){
        Ux* uxInstance = Ux::Singleton();
        lastDisplayTxtLen = SDL_strlen(txtToShow);
        uxInstance->printStringToUiObject(text_itself, txtToShow, DO_NOT_RESIZE_NOW);
       // text_itself->updateRenderPosition();
        handlePositioning(uiObjectItself);
        return this;
    }

    uiText* autoSize(float maxSize){
        float cmpSize = maxSize;
        if( lastDisplayTxtLen > 0 ){
            cmpSize = 1.0/lastDisplayTxtLen;
        }
        if( cmpSize > maxSize ){
            size(maxSize);
        }else{
            size(cmpSize);
        }
        return this;
    }

    uiText* size(float size){
        text_position->setBoundaryRect( 0.5, 0.5, size, size);
        handlePositioning(uiObjectItself);
        return this;
    }

    uiText* move(float x, float y, float w, float h){
        uiObjectItself->setBoundaryRect( x, y, w, h);
        return this;
    }

    uiText* moveRelative(float x, float y){
        uiObjectItself->setBoundaryRect( uiObjectItself->boundryRect.x + x, uiObjectItself->boundryRect.y + y, uiObjectItself->boundryRect.w, uiObjectItself->boundryRect.h);
        return this;
    }

    uiText* margins(float top, float right, float bottom, float left){
        uiObjectItself->setBoundaryRect( uiObjectItself->boundryRect.x + left,
                                        uiObjectItself->boundryRect.y + top,
                                        uiObjectItself->boundryRect.w - left - right,
                                        uiObjectItself->boundryRect.h - top - bottom);
        return this;
    }

    uiText* marginLeft(float left){
        return margins(0,0,0,left);
    }


    uiText* scale(float sc){
        text_itself->identity();
        text_itself->scale(sc);
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

    uiText* pad(float px, float py){
        text_itself->textPadding.x = px;
        text_itself->textPadding.y = py;
        return this;
    }

    uiText* backgroundColor(SDL_Color* clr){
        return this->backgroundColor(clr->r, clr->g, clr->b, clr->a);
    }

    uiText* backgroundColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a){
        textBackground = true;
        text_backgr->show();
        text_backgr->hasBackground=true;
        Ux::setColor(&text_backgr->backgroundColor,r, g, b, a);
//        text_itself->hasBackground=true;
//        Ux::setColor(&text_itself->backgroundColor,r, g, b, a);
        return this;
    }

    uiText* backgroundRound(float radius){
        text_backgr->setRoundedCorners(radius);
        return this;
    }

    uiText* backgroundTouchBeginCallback(anInteractionFn tileClickedFn){
        text_backgr->setInteractionBegin(tileClickedFn);
        return this;
    }

    uiText* backgroundClickCallback(anInteractionFn tileClickedFn){
        text_backgr->setClickInteractionCallback(tileClickedFn);
        text_backgr->setShouldCeaseInteractionChek(&Ux::bubbleInteractionIfNonClick);
        return this;
    }

    uiText* backgroundClickProxy(uiObject* pobj){
        text_backgr->interactionProxy = pobj;
        return this;
    }

    uiText* color(Uint8 r, Uint8 g, Uint8 b, Uint8 a){
        Ux::setColor(&text_itself->foregroundColor,r, g, b, a);
        //////////////text_itself->propagateTextSettings(); not needed if set before printing!!
        return this;
    }

    void reprint(){
        text_itself->propagateTextSettings();
    }

    long length(){
        return lastDisplayTxtLen;
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
        //Float_Rect* dispRect = &p_dispalyNearUiObject->collisionRect; // this rect has good w/h that we can use (its scaled to boundary space)
//        text_position->setBoundaryRect(dispRect->x + (dispRect->w * 0.5),
//                                        dispRect->y + (dispRect->h * 0.5),
//                                        text_itself_size_scaling,
//                                        text_itself_size_scaling  );




        switch( hzAlign ){

            case TEXT_ALIGN::LEFT:
            {
                float charsThatFit = (1.0/text_position->boundryRect.w);
                text_itself->boundryRect.x = charsThatFit * -0.5;
                break;
            }
            case TEXT_ALIGN::CENTER_LEFTWARD:
            {
                text_itself->boundryRect.x =  -text_length + 0.5; // right aligned (center the rightmost char)
                break;
            }
            case TEXT_ALIGN::CENTER:
            {
                text_itself->boundryRect.x =  text_length * -0.5; // center
                break;
            }
            case TEXT_ALIGN::CENTER_RIGHTWARD:
            {
                text_itself->boundryRect.x = -0.5;
                break;
            }
            case TEXT_ALIGN::RIGHT:
            {
                float charsThatFit = (1.0/text_position->boundryRect.w);
                // TODO: do we apply text length or only text that fit?  depends on if we apply scaling or not based on if hte text fits...
                text_itself->boundryRect.x = (charsThatFit * 0.5) - text_length;
                break;
            }
        }

        text_backgr->boundryRect.w = text_length;
        text_backgr->boundryRect.x = text_itself->boundryRect.x;
        text_backgr->boundryRect.h = (1.0/text_position->boundryRect.h);
        text_backgr->boundryRect.y = text_backgr->boundryRect.h * -0.5;

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
