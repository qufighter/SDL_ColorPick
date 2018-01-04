//
//  ux.cpp
//  ColorPick iOS SDL
//
//  Created by Sam Larison on 8/21/16.
//
//

#include "ux.h"
#include "ux-anim.h"

bool Ux::ms_bInstanceCreated = false;
Ux* Ux::pInstance = NULL;

Ux* Ux::Singleton() {
    if(!ms_bInstanceCreated){
        pInstance = new Ux();
        ms_bInstanceCreated=true;
    }
    return pInstance;
}


/**
 Destructor
 */
Ux::~Ux(void) {

}

/**
 Default constructor
 */
Ux::Ux(void) {
    isInteracting = false;
    currentInteraction = uiInteraction();
    fullPickHistoryAnimation = nullptr;
}

void Ux::updateStageDimension(float w, float h){

    screenRatio = w / h;

    if( screenRatio > 1 ){
        widescreen = true;
    }else{
        widescreen = false;
    }

    resizeUiElements();

    return;
}

void Ux::resizeUiElements(void){

    //historyPreview->setBoundaryRect( 0.5, 0.5, 0.2, 0.2 * screenRatio);  /// TODO move size components into function to calculate on window rescale bonus points
    //historyPreview->updateRenderPosition();


    // what about elements that have auto sizing properties??? like>  square,
}

// todo - either tons of 1off create functions, or return and define outside
Ux::uiObject* Ux::create(void){

    uxAnimations = new UxAnim();

    char* resultText; //leaking memory???
    bool resizeLater = false; // assert resizeLater === false
    // todo use a ?

    rootUiObject = new uiObject();
    rootUiObject->isRoot = true;
    rootUiObject->setBoundaryRect( 0, 0, 1.0, 1.0);

    rootUiObject->hasBackground = true;
    Ux::setColor(&rootUiObject->backgroundColor, 0, 0, 0, 0);

    rootUiObject->hasForeground = true; // render texture
    Ux::setColor(&rootUiObject->foregroundColor, 0, 255, 0, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
    Ux::setColor(&rootUiObject->foregroundColor, 0, 0, 0, 0); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
    rootUiObject->hasForeground = false; // render texture

    // make this color the selected color ! ! ! ! !! ^ ^ ^

    bottomBar = new uiObject();
    bottomBar->hasBackground = true;
    //bottomBar.setInteraction(&Ux::interactionHZ);
    bottomBar->setBoundaryRect( 0.1, 0.7, 0.8, 0.2);
    bottomBar->setBoundaryRect( 0.0, 0.7, 1.0, 0.2);
    //bottomBar->setBoundaryRect( 0.0, 0.7, 0.5, 0.2);

    Ux::setColor(&bottomBar->backgroundColor, 0, 0, 0, 198);


    bottomBar->hasForeground = true;
    printCharToUiObject(bottomBar, '_', resizeLater);




    pickSourceBtn = new uiObject();
    pickSourceBtn->hasForeground = true;
    pickSourceBtn->hasBackground = true;
    pickSourceBtn->setInteractionCallback(&Ux::interactionTouchRelease);

    //pickSourceBtn->setInteractionCallback(&Ux::addCurrentToPickHistory);

    pickSourceBtn->setBoundaryRect( 0.8, 0, 0.2, 1);  /// TODO move size components into function to calculate on window rescale bonus points
    Ux::setColor(&pickSourceBtn->backgroundColor, 32, 0, 0, 128);
    Ux::setColor(&pickSourceBtn->foregroundColor, 0, 255, 255, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
    pickSourceBtn->canCollide = true;

    printCharToUiObject(pickSourceBtn, '+', resizeLater);

    bottomBar->addChild(pickSourceBtn);




    zoomSlider = new uiObject();
    zoomSlider->hasForeground = true;
    zoomSlider->hasBackground = true;
    Ux::setColor(&zoomSlider->backgroundColor, 32, 0, 0, 128);
    Ux::setColor(&zoomSlider->foregroundColor, 255, 255, 255, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
    zoomSlider->setInteraction(&Ux::interactionHZ);
    zoomSlider->canCollide = true;
    zoomSlider->setBoundaryRect( 0.0, 0, 0.2, 1);  /// TODO move size components into function to calculate on window rescale bonus points
    Ux::setRect(&zoomSlider->movementBoundaryRect, 0.0, 0, 0.5, 0.0);

    printCharToUiObject(zoomSlider, '^', resizeLater);

    bottomBar->addChild(zoomSlider);



    rootUiObject->addChild(bottomBar);

    viewing_full_history = true;
    historyFullsize = new uiObject();
    historyFullsize->hasBackground = true;
    Ux::setColor(&historyFullsize->backgroundColor, 0, 0, 0, 192);
    historyFullsize->setBoundaryRect( 0.0, 0.0, 1.0, 1.0);
    historyFullsize->canCollide = true;
    historyFullsize->setInteractionCallback(&Ux::interactionToggleHistory);
    historyFullsize->setInteraction(&Ux::interactionVert);
    historyFullsize->setBoundsEnterFunction(&Ux::interactionHistoryEnteredView);

    //historyFullsize->doesInFactRender = false; // has no BG so nothing to render here
    historyFullsize->isDebugObject= true;

    int rowsTotal = 7;
    int rowCtr = -1;
    while(++rowCtr < rowsTotal ){

        int ctr = SIX_ACROSS;
        while( --ctr >= 0 ){
            uiObject * colora = new uiObject();
            colora->hasBackground = true;
            Ux::setColor(&colora->backgroundColor, 255, 0, 32, 0);

            //colora->setBoundaryRect( ctr * (1/across), 1.0, 1/across, 1/across * screenRatio );
            colora->setBoundaryRect( ctr * SIX_ACROSS_RATIO, rowCtr * SIX_ACROSS_RATIO, SIX_ACROSS_RATIO, SIX_ACROSS_RATIO );
            //colora->setBoundaryRect( 0, 0.0, 1, 1 );

            colora->hasForeground = true;
            printCharToUiObject(colora, 'E'-ctr, resizeLater);

            historyFullsize->addChild(colora);
        }
    }



    historyPreview = new uiObject();
    //historyPreview->hasForeground = true;
    historyPreview->hasBackground = true;
    Ux::setColor(&historyPreview->backgroundColor, 0, 0, 0, 192);
    //Ux::setColor(&historyPreview->foregroundColor, 255, 255, 255, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
    //historyPreview->doesInFactRender = false;




    //historyPreview->setInteraction(&Ux::interactionHZ);
    //historyPreview->setBoundaryRect( 0.5, 0.5, 0.2, 0.2 * screenRatio);  /// TODO move size components into function to calculate on window rescale bonus points
    //Ux::setRect(&historyPreview->movementBoundaryRect, 0.0, 0, 0.0, 0.0);
    //historyPreview->canCollide = true;






    historyPreview->setBoundaryRect(0.0, 0.9, 1.0, 0.1);

    // we should another way to set rects for animation support?


    int ctr = SIX_ACROSS;
    while( --ctr >= 0 ){
        uiObject * colora = new uiObject();
        colora->hasBackground = true;
        Ux::setColor(&colora->backgroundColor, 255, 0, 32, 0);

        //colora->setBoundaryRect( ctr * (1/across), 1.0, 1/across, 1/across * screenRatio );
        colora->setBoundaryRect( ctr * SIX_ACROSS_RATIO, 0.0, SIX_ACROSS_RATIO, 1.0 );
        //colora->setBoundaryRect( 0, 0.0, 1, 1 );

        //colora->hasForeground = true;
        //printCharToUiObject(colora, 'F'-ctr, resizeLater);



        historyPreview->addChild(colora);
    }


    //historyPreview->hasForeground = true;
    //printCharToUiObject(historyPreview, '_', resizeLater);
    historyPreview->canCollide = true;
    historyPreview->setInteractionCallback(&Ux::interactionToggleHistory);

    historyPreview->setInteraction(&Ux::interactionVert);
    historyPreview->interactionProxy = historyFullsize; // when we drag the preview effect the fullsize sliding into view...

    //printCharToUiObject(historyPreview, 'H', resizeLater);

    //bottomBar->addChild(historyPreview);
    rootUiObject->addChild(historyPreview);


    hexValueText = new uiObject();
    

    //NEW rule size the container for the letter size of first letter
    hexValueText->setBoundaryRect( 0.0, 0.04, 0.16666666666667, 0.16666666666667);

    //NEW rule 2 - the ohter properties define the text rendering only
    hexValueText->hasForeground = true;
    Ux::setColor(&hexValueText->foregroundColor, 255, 255, 255, 223); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)

    hexValueText->hasBackground = true;
    Ux::setColor(&hexValueText->backgroundColor,0, 0, 0, 0);


    // perhaps properties on container are inherited by text
    // however container itself will no longer render?

    rootUiObject->addChild(hexValueText);

    //sprintf(resultText, "000000");
   // printStringToUiObject(hexValueText, resultText, resizeLater); // this method wont work on desktop , malloc? 
    printStringToUiObject(hexValueText, "000000", resizeLater);

    // very odd not initializing these...

    rgbRedText = new uiObject();
    //NEW rule size the container for the letter size of first letter
    rgbRedText->setBoundaryRect( 0.0, 0.04+0.16666666666667, 0.11111111111111, 0.11111111111111);
    //NEW rule 2 - the ohter properties define the text rendering only
    rgbRedText->hasForeground = true;
    Ux::setColor(&rgbRedText->foregroundColor, 255, 100, 100, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
    rgbRedText->hasBackground = true;
    Ux::setColor(&rgbRedText->backgroundColor, 32, 0, 0, 192);
    rootUiObject->addChild(rgbRedText);
    printStringToUiObject(rgbRedText, "  R", resizeLater);


    rgbGreenText = new uiObject();
    //NEW rule size the container for the letter size of first letter
    rgbGreenText->setBoundaryRect( 0.33333333333333, 0.04+0.16666666666667, 0.11111111111111, 0.11111111111111);
    //NEW rule 2 - the ohter properties define the text rendering only
    rgbGreenText->hasForeground = true;
    Ux::setColor(&rgbGreenText->foregroundColor, 100, 255, 100, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
    rgbGreenText->hasBackground = true;
    Ux::setColor(&rgbGreenText->backgroundColor, 0, 32, 0, 192);
    rootUiObject->addChild(rgbGreenText);
    printStringToUiObject(rgbGreenText, "  G", resizeLater);


    rgbBlueText = new uiObject();
    //NEW rule size the container for the letter size of first letter
    rgbBlueText->setBoundaryRect( 0.66666666666666, 0.04+0.16666666666667, 0.11111111111111, 0.11111111111111);
    //NEW rule 2 - the ohter properties define the text rendering only
    rgbBlueText->hasForeground = true;
    Ux::setColor(&rgbBlueText->foregroundColor, 100, 100, 255, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
    rgbBlueText->hasBackground = true;
    Ux::setColor(&rgbBlueText->backgroundColor, 0, 0, 32, 192);
    rootUiObject->addChild(rgbBlueText);
    printStringToUiObject(rgbBlueText, "  B", resizeLater);


//    uiObject childObj = uiObject();
//    childObj.hasForeground = true;
//    childObj.setInteraction(&Ux::interactionHZ);
//    rootUiObject.addChild(childObj);
//
//
//    uiObject childObj2 = uiObject();
//    childObj2.hasForeground = true;
//    childObj.addChild(childObj2);



    //rootUiObject.childUiObjects->push_back(&childObj);
    
    // the add child function on the struct may be needed (to specify parent object ref on child object)
    // and it may expand the boundary rect automatically ?
    // only some items will accept collisions
    // will all boundary rect be global coordinate or within
    // the parent coordinate space? for render purposes, later seems nice


    rootUiObject->addChild(historyFullsize);


    updateRenderPositions();

    return rootUiObject;
}


void Ux::updateRenderPositions(void){
    rootUiObject->updateRenderPosition();
}

void Ux::updateRenderPosition(uiObject *renderObj){
    renderObj->updateRenderPosition();
}








// so should the print functions move into uiObject?

void Ux::printStringToUiObject(uiObject* printObj, char* text, bool resizeText){
    // this function will print or update text, adding characters if needed
    // the object must be empy (essentially to become a text container and no loner normal UI object)


    // TODO these are global constants


    int ctr=0;
    int charOffset = 0;
    char * i;
    uiObject* letter;

    int len=sizeof(text) - 1;  // unfortunately sizeof not really work right here

    printObj->doesNotCollide = true;
    printObj->doesInFactRender = printObj->containText; // the container is never the size to render.. unless contains text?!

    for (i=text; *i; i++) {
        //SDL_Log("%c %i", *i, *i);

        charOffset = (int)*i - text_firstLetterOffset;

        // there is a chance the UI object already has this many letters
        // and that we don't need a new letter just to update existing one
        if( ctr < printObj->childListIndex ){
            letter = printObj->childList[ctr];
        }else{
            letter = new uiObject();

            if( printObj->containText == true ){
                float w = 1.0 / len ; // todo move

                // move this right out of the else aboe, since we should fit any len text within, and also needs resize IF text len changed.... !?! easy compute
                letter->setBoundaryRect( (ctr*w), 0, w, w);  /// TODO move size components into function to calculate on window rescale bonus points for suqare?
            }else{
                letter->setBoundaryRect( (ctr*1.0), 0, 1, 1);  /// TODO move size components into function to calculate on window rescale bonus points
            }


            printObj->addChild(letter);
        }

        letter->hasForeground = true; // printObj->hasForeground;
        letter->hasBackground = printObj->hasBackground;
        Ux::setColor(&letter->backgroundColor, &printObj->backgroundColor);
        letter->lastBackgroundColor = letter->backgroundColor; // initializations
        Ux::setColor(&letter->foregroundColor, &printObj->foregroundColor);
        letter->doesInFactRender = true;

        //  // we should very likely inherit styles from parent (printObj)
        ///  whenever text changes

        printCharOffsetUiObject(letter, charOffset);
        //        renderedletters[renderedLettersCtr++] = letter;
        ctr++;
    }

    // it is also possible extra letters need to be set to offeset 0 to render space?
    for( int l=printObj->childListIndex; ctr<l; ctr++ ){
        printObj->childList[ctr]->doesInFactRender = false;

//        Ux::setRect(&printObj->childList[ctr]->textureCropRect,
//                    0, // space is asci 32 0,0
//                    0,
//                    xStep,yStep);
    }

    if( resizeText ){
        // we must rebuild this object otherwise strange effects
        // unless it has been printed before at this length
        // not needed during first init where we build once at end...
        // also not needed during normal course of printing a second time the same text length....
        // updateRenderPositions();
        updateRenderPosition(printObj);

    }
}

void Ux::printCharToUiObject(uiObject* letter, char character, bool resizeText){
    printCharOffsetUiObject(letter, (int)character - text_firstLetterOffset);
    if( resizeText ){
        // we must rebuild this object otherwise strange effects
        // not needed during first init where we build once at end...
        // also not needed during normal course of printing a second time the same text length....
        // updateRenderPositions();
        updateRenderPosition(letter->parentObject);
    }
}

void Ux::printCharOffsetUiObject(uiObject* letter, int charOffset){
//    if( charOffset == 0 ){
//        letter->lastBackgroundColor = letter->backgroundColor;
//        Ux::setColor(&letter->backgroundColor, 0, 0, 0, 0);
//    }else{
//        letter->backgroundColor = letter->lastBackgroundColor;
//    }


    int row = (int)(charOffset / text_stride);
    charOffset = charOffset % text_stride;

    Ux::setRect(&letter->textureCropRect,
                charOffset * text_xStep, row * text_yStep,
                text_xStep,              text_yStep);
}









bool Ux::triggerInteraction(void){ // mouseup, mouse didn't move

    return objectCollides(currentInteraction.px, currentInteraction.py);
}

bool Ux::objectCollides(float x, float  y){
    return objectCollides(rootUiObject, x, y);
}

bool Ux::objectCollides(uiObject* renderObj, float x, float y){

    bool collides = false;

    // update ???? seems handled by interactionUpdate
    // test collision with object itself, then render all child obj


    // some objects (font) may bypass all collision checks....
    if( renderObj->doesNotCollide ){
        return collides;
    }


    if( pointInRect(&renderObj->collisionRect, x, y) ){
   // if( pointInRect(&renderObj->renderRect, x, y) ){
        if( renderObj->canCollide ){
            collides = true;
            // do something if a thing to do is defined for this collisino
            if( renderObj->hasInteraction || renderObj->hasInteractionCb ){
                // quite arguable we can return here, continuing to seek interaction on child objects may produce odd effects
                interactionObject = renderObj;
                isInteracting = true;
                return collides;
            }


        }



        if( renderObj->hasChildren  ){
            // we may transform x and y to local coordinates before sending to child object

            // we need to check the children in reverse, the ones that render last are on top
            for( int ix=renderObj->childListIndex-1; ix>-1; ix-- ){
                // if already has collides and already isInteracting then return it?????
                collides = objectCollides(renderObj->childList[ix], x, y);// || collides;

                if( collides ) {
                    return collides;
                }
            }
        }

    }

    // finally if we perfomed a translation for object position
    // reset that translation

    return collides;
}

bool Ux::interactionUpdate(uiInteraction *delta){
    // TODO ret bool determine modification?

    if( isInteracting && interactionObject->hasInteraction ){
        interactionObject->interactionFn(interactionObject, delta);
        return true;
    }

    return false;
}

bool Ux::interactionComplete(uiInteraction *delta){

    if( isInteracting && interactionObject->hasInteractionCb ){
        isInteracting = false;
        interactionObject->interactionCallback(interactionObject, delta);
        return true;
    }

    return false;
}


//static
void Ux::interactionHistoryEnteredView(uiObject *interactionObj){
    // check delta->dx for movement amount
    // no movement total
    Ux* self = Ux::Singleton();
    self->updatePickHistoryPreview();
}

//static
void Ux::interactionNoOp(uiObject *interactionObj, uiInteraction *delta){
    // check delta->dx for movement amount
    // no movement total
}

//static
void Ux::interactionToggleHistory(uiObject *interactionObj, uiInteraction *delta){
    Ux* self = Ux::Singleton();

    // we should store a ref to the last animation and cancel it....
    // the problem is that ref we have is being updated from another thread
    // and also may already be free'd by the time we need to use it!
    // a) we can't mark it without first checkign it
    // b) we can't clear it without possibly breaking the update thread.
    // this may be another good argument to process all animation updates from the main thread.

    // we can mark some chain to not auto free, but if we do his we should find a way to reset that chain too (to truly reuse it)

    if( self->fullPickHistoryAnimation != nullptr ){
        self->fullPickHistoryAnimation->endAnimation(); // thread safe
        self->fullPickHistoryAnimation = nullptr;
    }

    if( self->viewing_full_history ) {
        self->fullPickHistoryAnimation = self->uxAnimations->slideDown(self->historyFullsize)->preserveReference(); // returns uiAminChain*
        self->viewing_full_history=false;
    }else{
        self->historyFullsize->isInBounds = true; // nice hack
        self->updatePickHistoryPreview();
        self->fullPickHistoryAnimation = self->uxAnimations->slideUp(self->historyFullsize)->preserveReference(); // returns uiAminChain*
        self->viewing_full_history=true;
    }
}

//static
void Ux::interactionTouchRelease(uiObject *interactionObj, uiInteraction *delta){


    // check delta->dx for movement amount
    // no movement total


    Ux* myUxRef = Ux::Singleton();
    myUxRef->addCurrentToPickHistory();

}

// this function is a bit odd for now, but is planned to be used to drag and throw objects that will otherwise animiate in, or not animate in based on this
//static
void Ux::interactionVert(uiObject *interactionObj, uiInteraction *delta){
    Ux* self = Ux::Singleton();

    // if we are already panning .... this is the second thing thats history specific in this function... this and what to do when direciton changes
    // this function is really a "pan in/out" sort of function for now and not at all like HZ
    if( self->fullPickHistoryAnimation != nullptr ){
        self->fullPickHistoryAnimation->endAnimation(); // thread safe
        self->fullPickHistoryAnimation = nullptr;
        // so uiObject cannot currently have references to uiAminChain since they must be defined after uiAnimation and uiObject
        // we could define something after that could hold references to both though
        // and then we could track that thing instead of uiObject
    }

    if( interactionObj->interactionProxy != nullptr ){
        interactionObj = interactionObj->interactionProxy;
    }

    interactionObj->boundryRect.y += (delta->ry * (1.0/interactionObj->parentObject->collisionRect.h));

    // first todo: will mouseup always trigger the toggle pick history fn - current not (drag up mouse up on ctrl that has its own interactionCb)
    // now if the control supports drop then maybe the existing way is good
    if( delta->ry > 0.003 ){ // moving down
        self->viewing_full_history = true; // toggle still to be called..... ?
        // and once thats certain, we can more easily modify self->viewing_full_history as needed based on which dir this drag is going!
    }

    if( delta->ry < -0.003 ){
        self->viewing_full_history = false; // toggle still to be called.....
    }

    self->updateRenderPosition(interactionObj);
}

//static
void Ux::interactionHZ(uiObject *interactionObj, uiInteraction *delta){

    interactionObj->boundryRect.x += (delta->rx * (1.0/interactionObj->parentObject->collisionRect.w));
    /// we cannot modify the rect directly like this

//computedMovementRect
    if( interactionObj->boundryRect.x < interactionObj->computedMovementRect.x ){
        interactionObj->boundryRect.x = interactionObj->computedMovementRect.x;

        delta->fixX(interactionObj->collisionRect,  interactionObj->parentObject->collisionRect);


    }else if( interactionObj->boundryRect.x > interactionObj->computedMovementRect.x + interactionObj->computedMovementRect.w){
        interactionObj->boundryRect.x = interactionObj->computedMovementRect.x + interactionObj->computedMovementRect.w;

        delta->fixX(interactionObj->collisionRect,  interactionObj->parentObject->collisionRect);
    }


// now why do we set the volume?
    //setVolume


    if( interactionObj->hasAnimCb ){
        interactionObj->animationPercCallback(interactionObj->boundryRect.x / interactionObj->computedMovementRect.w);
    }

    //updateRenderPositions();  // WHY NOT MOVE HERE
    // we need to make a singleton ux and then we can get instance
    // and call member functions
    // see OpenGLContext::Singleton

    Ux* myUxRef = Ux::Singleton();
    //myUxRef->updateRenderPositions();
    myUxRef->updateRenderPosition(interactionObj);

    // currently does not consider width of the object, since given a width it could remain within boundaries, but this is not usefl
    // for sliders where the center point of the object is all that is considered for positioning (bool isCentered) <-- ? untrue ? planing stage problems
}
//
//
//void Ux::setVolume(uiObject *interactionObj, uiInteraction *delta){
//
//}




/// eeeh this is really update current color
///   mayhaps this only updates when you stop panning?
///   or otherwise seperate function for addPickHistory from this
void Ux::updateColorValueDisplay(SDL_Color* color){

    char* resultText; //leaking memory???
    bool doNotResizeText = false;  // assert doNotResizeText == false;
    bool doResizeText = true; // assert doResizeText == true;

    sprintf(resultText, "%02x%02x%02x", color->r, color->g, color->b);
    Ux::setColor(&hexValueText->backgroundColor,color->r, color->g, color->b, 255);
    printStringToUiObject(hexValueText, resultText, doNotResizeText);

    sprintf(resultText, "%3d", color->r);
    rgbRedText->backgroundColor.a = color->r;
    printStringToUiObject(rgbRedText, resultText, doNotResizeText);

    sprintf(resultText, "%3d", color->g);
    rgbGreenText->backgroundColor.a = color->g;
    printStringToUiObject(rgbGreenText, resultText, doNotResizeText);

    sprintf(resultText, "%3d", color->b);
    rgbBlueText->backgroundColor.a = color->b;
    printStringToUiObject(rgbBlueText, resultText, doNotResizeText);

    currentlyPickedColor = color;
    //addCurrentToPickHistory();
}

void Ux::addCurrentToPickHistory(){

//  TODO consider eliminating lastPickHistoryIndex instead
    /// i.e pickHistoryIndex = lastPickHistoryIndex
    // we can increment and bound check before using the new index
    // this should simplify the checks used elsewhere

    // IMPORTANT remember ot text with pickHistoryMax = <5
    // IMPORTANT remember to check more than 10 colors

    // if we have a lastPickHistoryIndex
    if( pickHistoryIndex > 0 || largestPickHistoryIndex > 0 ){
        // the new color must be unique
        if( pickHistory[lastPickHistoryIndex].r == currentlyPickedColor->r &&
            pickHistory[lastPickHistoryIndex].g == currentlyPickedColor->g &&
            pickHistory[lastPickHistoryIndex].b == currentlyPickedColor->b ){
            // we already had this color added to the end of the history... indicate this with an effect and do not continue

            uiObject * colora = historyPreview->childList[0];
            //colora->boundryRect.y -= 0.1;
            //colora->updateRenderPosition();
            // openglContext->
            uxAnimations->bounce(colora);
            return;
        }else{
            uxAnimations->rvbounce(historyPreview);
        }
    }else{
        uxAnimations->rvbounce(historyPreview);
    }



    lastPickHistoryIndex = pickHistoryIndex;
    if( pickHistoryIndex > largestPickHistoryIndex ) largestPickHistoryIndex = pickHistoryIndex;
    pickHistory[pickHistoryIndex++] = *currentlyPickedColor;

    if( pickHistoryIndex >= pickHistoryMax ){
        pickHistoryIndex = 0;
    }

    updatePickHistoryPreview();
}


void Ux::updatePickHistoryPreview(){


    // err - add 10 to loose add button!

    if( lastPickHistoryIndex < 0 ) return; //no history yet, and this means we may need to keep lastPickHistoryIndex around?

    int ctr = 0;
    int histIndex = pickHistoryIndex - 1;
    if( histIndex < 0 ){
        histIndex = largestPickHistoryIndex;
    }
    int startIndex = histIndex;
    uiObject * colora;
    SDL_Color* clr;
    while( ctr < SIX_ACROSS && histIndex >= 0 ){

        clr = &pickHistory[histIndex];

        colora = historyPreview->childList[ctr];

        Ux::setColor(&colora->backgroundColor, clr->r, clr->g, clr->b, 255);

        //char* resultText; //leaking memory???

        // call itoa 10 times and we will suddenly loose a few UI elements....
       // SDL_itoa(histIndex, resultText, 10);
        //colora->containText =true;
       // printStringToUiObject(colora, "A", true);


        //colora->hasForeground = false;
        // printCharToUiObject(colora, 'Z'+ctr, true);
        //historyPreview->addChild(colora);

        histIndex--;
        ctr++;

        if( histIndex < 0 && largestPickHistoryIndex > startIndex ){
            histIndex = largestPickHistoryIndex;
        }

        //return;

    }


    // somertimes upate the full histoury preview too



    if( !historyFullsize->isInBounds ) return;

    int historyTile = 0;
    int historyOffset = pickHistoryIndex - 1; // 0;
    if( histIndex < 0 ){
        histIndex = largestPickHistoryIndex;
    }
    int rowsTotal = 7;
    int rowCtr = -1;
    while(++rowCtr < rowsTotal ){

        int ctr = SIX_ACROSS; // redefinition of ctr....
        while( --ctr >= 0 ){

            //if( historyOffset > largestPickHistoryIndex ) break;
            if( historyOffset < 0 ) break;


            clr = &pickHistory[historyOffset];


            colora = historyFullsize->childList[historyTile];

            Ux::setColor(&colora->backgroundColor, clr->r, clr->g, clr->b, 255);


            historyTile++;
            historyOffset--;
        }

        //if( historyOffset > largestPickHistoryIndex ) break;
        if( historyOffset < 0 ) break;


    }


}

//bool Ux::updateAnimations(float elapsedMs){
//
//    //return uxAnimations->updateAnimations(elapsedMs);
//
//}


void Ux::renderFullPickHistory(){
    // so we might dynamically allocate a bunch of UI object if we don't find existing ones in the positon
    /// this could get interesting..

    // so we recompute positon
    // and must notify our parent object if our boundaries exceed our parent rect bounds?

    /// but what about the root object/

    // maybe we keep reporting thisup the tree until we reach an object taht has scroollll barsz enabled
    // and the root UI object must alwasy jus in case something else does not capture hte scrol// and it works gud' ? no


    //// sooo - whjen do we detect that we left the render bounds
    /// and defer adding more ojbects
    // or avoid rendering objects....
    // or avoid recomputing positions
    // or ??
    /// so its really got to be updated based on scroll X though we still render from 0
    // at least to create empty objects to take ups pace
    // only if the child object size is predictable asi t will be inthis case
    /// such taht and given scroll offset has a known object offset to begin the rendering gud"

}

int Ux::renderObject(uniformLocationStruct *uniformLocations){
    return renderObjects(uniformLocations, rootUiObject);
}

int Ux::renderObjects(uniformLocationStruct *uniformLocations, uiObject *renderObj){
    // update
    // render object itself, then render all child hobj


    if( !renderObj->isInBounds ) return 1;

    if( renderObj->isDebugObject ){
        SDL_Log("DEBUG OBJECT IS BEING RENDERED NOW!");
//        renderObj->boundryRect.y+=0.01; // this proves the boundary tests is working for this object, when out of view they do not render...
//        renderObj->updateRenderPosition();
    }

    if( renderObj->doesInFactRender ){
        glUniform4f(uniformLocations->ui_position,
                    renderObj->renderRect.x,
                    -renderObj->renderRect.y,
                    0.0,
                    0.0);

    //    glUniform4f(uniformLocations->ui_position,
    //                 0.2,
    //                0.2,
    //                0.0,
    //                0.0);

    //    glUniform3f(uniformLocations->ui_scale,
    //                renderObj.boundryRect,
    //                renderObj.boundryRect.y,
    //                0.0);

        glUniform4f(uniformLocations->ui_scale,
                    renderObj->renderRect.w,
                    renderObj->renderRect.h,
                    1.0,
                    1.0);

    //
    //    glUniform4f(uniformLocations->ui_scale,
    //                1.0,
    //                1.0,
    //                1.0,
    //                1.0);


        glUniform4f(uniformLocations->texture_crop,
                    renderObj->textureCropRect.x,
                    renderObj->textureCropRect.y,
                    renderObj->textureCropRect.w,
                    renderObj->textureCropRect.h);


        if( renderObj->hasBackground ){
            glUniform4f(uniformLocations->ui_color,
                        renderObj->backgroundColor.r/255.0, // maths can be avoided
                        renderObj->backgroundColor.g/255.0,
                        renderObj->backgroundColor.b/255.0,
                        renderObj->backgroundColor.a/255.0
            );
        }else{
            glUniform4f(uniformLocations->ui_color, 1.0,1.0,1.0,0.0);
        }

        if( renderObj->hasForeground ){

            glUniform4f(uniformLocations->ui_foreground_color,
                        renderObj->foregroundColor.r/255.0,
                        renderObj->foregroundColor.g/255.0,
                        renderObj->foregroundColor.b/255.0,
                        renderObj->foregroundColor.a/255.0
            );


        }else{
            glUniform4f(uniformLocations->ui_foreground_color, 0.0,0.0,0.0,0.0);

        }

        glDrawArrays(GL_TRIANGLES, 0, 6);

    }


    if( renderObj->hasChildren  ){
        //for( int x=0, l=(int)renderObj->childUiObjects.size(); x<l; x++ ){

//
        for( int x=0,l=renderObj->childListIndex; x<l; x++ ){
            renderObjects(uniformLocations, renderObj->childList[x]);
        }

        // renderChildrenInReverse

//        for( int x=renderObj->childListIndex-1; x>-1; x-- ){
//            renderObjects(uniformLocations, renderObj->childList[x]);
//        }
    }


    return 0;
}
