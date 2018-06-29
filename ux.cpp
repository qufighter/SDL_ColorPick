//
//  ux.cpp
//  ColorPick iOS SDL
//
//  Created by Sam Larison on 8/21/16.
//
//

#include "ux.h"

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

    for( int x=0; x<COLOR_INDEX_MAX; x++ ){
        palleteColorsIndex[x] = -1; // largest Uint..
    }
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
    //printCharToUiObject(bottomBar, '_', DO_NOT_RESIZE_NOW);
    printCharToUiObject(bottomBar, CHAR_ASYMP_SLOPE, DO_NOT_RESIZE_NOW);



    pickSourceBtn = new uiObject();
    pickSourceBtn->hasForeground = true;
    pickSourceBtn->hasBackground = true;
    pickSourceBtn->setInteractionCallback(&Ux::interactionTouchRelease);


    //pickSourceBtn->setInteractionCallback(&Ux::addCurrentToPickHistory);

    pickSourceBtn->setBoundaryRect( 0.8, 0, 0.2, 1);  /// TODO move size components into function to calculate on window rescale bonus points
    Ux::setColor(&pickSourceBtn->backgroundColor, 32, 0, 0, 128);
    Ux::setColor(&pickSourceBtn->foregroundColor, 0, 255, 255, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)

    //printCharToUiObject(pickSourceBtn, '+', DO_NOT_RESIZE_NOW);
    printCharToUiObject(pickSourceBtn, CHAR_CIRCLE_PLUSS, DO_NOT_RESIZE_NOW);

    bottomBar->addChild(pickSourceBtn);




    zoomSlider = new uiObject();
    zoomSlider->hasForeground = true;
    zoomSlider->hasBackground = true;
    Ux::setColor(&zoomSlider->backgroundColor, 32, 0, 0, 128);
    Ux::setColor(&zoomSlider->foregroundColor, 255, 255, 255, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
    zoomSlider->setInteraction(&Ux::interactionHZ);//zoomSlider->canCollide = true;
    
    zoomSlider->setBoundaryRect( 0.0, 0, 0.2, 1);  /// TODO move size components into function to calculate on window rescale bonus points
    Ux::setRect(&zoomSlider->movementBoundaryRect, 0.0, 0, 0.5, 0.0);


    //printCharToUiObject(zoomSlider, '^', DO_NOT_RESIZE_NOW);
    printCharToUiObject(zoomSlider, CHAR_VERTICAL_BAR, DO_NOT_RESIZE_NOW);


    bottomBar->addChild(zoomSlider);



    rootUiObject->addChild(bottomBar);


    historyPalleteHolder = new uiObject();
    historyPalleteHolder->hasBackground = true;
    Ux::setColor(&historyPalleteHolder->backgroundColor, 0, 0, 0, 192);
    historyPalleteHolder->setBoundaryRect( 0.0, 0.04, 1.0, 0.8);

    historyPalleteHolder->setInteractionCallback(&Ux::interactionToggleHistory); // if we dragged and released... it will animate the rest of the way because of this
    historyPalleteHolder->setInteraction(&Ux::interactionVert);
    historyPalleteHolder->setBoundsEnterFunction(&Ux::interactionHistoryEnteredView);


    historyScroller = new uiScrollController();
    historyScroller->initTilingEngine(6, 6, &Ux::updateUiObjectFromHistory, &Ux::getHistoryTotalCount, &Ux::clickHistoryColor);

    newHistoryFullsize = historyScroller->uiObjectItself;

    //newHistoryFullsize->isDebugObject=true;

    newHistoryFullsize->hasBackground = true;
    Ux::setColor(&newHistoryFullsize->backgroundColor, 0, 0, 0, 192);

    newHistoryFullsize->setBoundaryRect( 0.0, 0.0, 1.0, 0.6);


    palleteScroller = new uiScrollController();
    palleteScroller->initTilingEngine(1, 1, &Ux::updateUiObjectFromPallete, &Ux::getPalleteTotalCount, &Ux::clickPalleteColor);

    newHistoryPallete = palleteScroller->uiObjectItself;

    newHistoryPallete->hasBackground = true;
    Ux::setColor(&newHistoryPallete->backgroundColor, 0, 0, 0, 255);

    newHistoryPallete->setBoundaryRect( 0.0, 0.6, 1.0, 0.4);


    //newHistoryFullsize->isDebugObject= true;




//    historyFullsize = new uiObject();
//    historyFullsize->hasBackground = true;
//    Ux::setColor(&historyFullsize->backgroundColor, 0, 0, 0, 192);
//    historyFullsize->setBoundaryRect( 0.0, 0.0, 1.0, 0.5);
//    historyFullsize->canCollide = true; // not stricly needed for setInteractionCallback
//    historyFullsize->setInteractionCallback(&Ux::interactionToggleHistory);
//    historyFullsize->setInteraction(&Ux::interactionVert);
//    historyFullsize->setBoundsEnterFunction(&Ux::interactionHistoryEnteredView);
//
//    //historyFullsize->doesInFactRender = false; // has no BG so nothing to render here
//    historyFullsize->isDebugObject= true;
//
//    int rowsTotal = 7;
//    int rowCtr = -1;
//    while(++rowCtr < rowsTotal ){
//
//        int ctr = SIX_ACROSS;
//        while( --ctr >= 0 ){
//            uiObject * colora = new uiObject();
//            colora->hasBackground = true;
//            Ux::setColor(&colora->backgroundColor, 255, 0, 32, 0);
//
//            //colora->setBoundaryRect( ctr * (1/across), 1.0, 1/across, 1/across * screenRatio );
//            colora->setBoundaryRect( ctr * SIX_ACROSS_RATIO, rowCtr * SIX_ACROSS_RATIO, SIX_ACROSS_RATIO, SIX_ACROSS_RATIO );
//            //colora->setBoundaryRect( 0, 0.0, 1, 1 );
//
//            colora->hasForeground = true;
//            printCharToUiObject(colora, 'E'-ctr, DO_NOT_RESIZE_NOW);
//
//            historyFullsize->addChild(colora);
//        }
//    }



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
        //printCharToUiObject(colora, 'F'-ctr, DO_NOT_RESIZE_NOW);



        historyPreview->addChild(colora);
    }


    //historyPreview->hasForeground = true;
    //printCharToUiObject(historyPreview, '_', DO_NOT_RESIZE_NOW);

    //historyPreview->canCollide = true; // set by setInteractionCallback
    historyPreview->setInteractionCallback(&Ux::interactionToggleHistory);

    historyPreview->setInteraction(&Ux::interactionVert);
    historyPreview->interactionProxy = historyPalleteHolder; // when we drag the preview effect the fullsize sliding into view...

    //printCharToUiObject(historyPreview, 'H', DO_NOT_RESIZE_NOW);

    //bottomBar->addChild(historyPreview);
    rootUiObject->addChild(historyPreview);


    curerntColorPreview = new uiViewColor(rootUiObject, Float_Rect(0.0, 0.04, 1.0, 0.27777777777778));




    //rootUiObject->addChild(curerntColorPreview->uiObjectItself);

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


    //rootUiObject->addChild(historyFullsize); // todo remove all ref to this test obj
    historyPalleteHolder->addChild(newHistoryFullsize);
    palleteSelectionColorPreview = new uiViewColor(historyPalleteHolder, Float_Rect(0.0, 0.5, 1.0, 0.5)); // this rect is reset next...
    historyPalleteHolder->addChild(newHistoryPallete);

    palleteSelectionColorPreview->uiObjectItself->setBoundaryRect(
        newHistoryPallete->boundryRect.x, newHistoryPallete->boundryRect.y - newHistoryPallete->boundryRect.h,
        newHistoryPallete->boundryRect.w, newHistoryPallete->boundryRect.h
    );

    palleteSelectionColorPreview->uiObjectItself->setInteractionCallback(&Ux::interactionTogglePalletePreview); // if we dragged and released... it will animate the rest of the way because of this
    palleteSelectionColorPreview->uiObjectItself->setInteraction(&Ux::interactionVert);
    // palleteSelectionColorPreview->uiObjectItself->setBoundsEnterFunction(&Ux::interactionHistoryEnteredView);


    // lets give palleteSelectionColorPreview a background
    palleteSelectionColorPreview->uiObjectItself->hasBackground = true;
    Ux::setColor(&palleteSelectionColorPreview->uiObjectItself->backgroundColor, 0, 0, 0, 192);


    palleteSelectionColorPreview->uiObjectItself->setCropParentRecursive(historyPalleteHolder); // THIS REALLY DOESN'T WORK! - ALL CHILD OBJECTS NEED TO GET THIS TOO... IT DOES WORK FOR TOP LEVEL OBJ BG ONLY....

    palleteSelectionColorPreview->uiObjectItself->setAnimation( uxAnimations->slideDownFullHeight(palleteSelectionColorPreview->uiObjectItself) ); // returns uiAminChain*



    rootUiObject->addChild(historyPalleteHolder);

    //test cololr history
    for( int q=0; q<36; q++ ){
        currentlyPickedColor = new SDL_Color();
        setColor(currentlyPickedColor, randomInt(0,44), randomInt(0,185), randomInt(0,44), 0);
        addCurrentToPickHistory();
    }


    updateRenderPositions();

    historyScroller->updateTiles();
    palleteScroller->updateTiles();

    return rootUiObject;
}


void Ux::updateRenderPositions(void){
    rootUiObject->updateRenderPosition();
}

void Ux::updateRenderPosition(uiObject *renderObj){
    renderObj->updateRenderPosition();
}








// so should the print functions move into uiObject?
// maybe just be static Ux:: member functions defined the same way as others

void Ux::printStringToUiObject(uiObject* printObj, char* text, bool resizeText){
    // this function will print or update text, adding characters if needed
    // the object must be empy (essentially to become a text container and no loner normal UI object)


    // todo possibly create container on fly if nonempty?
    // see "NEW rule" where it basically defines how to maybe go about this....

    // TODO these are global constants


    int ctr=0;
    int charOffset = 0;
    char * i;
    uiObject* letter;

    int len=sizeof(text) - 1;  // unfortunately sizeof not really work right here

    float letterWidth = 1.0 / len;

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

                // move this right out of the else aboe, since we should fit any len text within, and also needs resize IF text len changed.... !?! easy compute
                letter->setBoundaryRect( (ctr*letterWidth), 0, letterWidth, letterWidth);  /// TODO move size components into function to calculate on window rescale bonus points for suqare?
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

        Ux::printCharOffsetUiObject(letter, charOffset);
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
    printCharToUiObject(letter, (int)character, resizeText);
}

void Ux::printCharToUiObject(uiObject* letter, int character, bool resizeText){
    printCharOffsetUiObject(letter, character - text_firstLetterOffset);
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
    bool childCollides = false;
    // update ???? seems handled by interactionUpdate
    // test collision with object itself, then render all child obj


    // some objects (font) may bypass all collision checks....
    if( renderObj->doesNotCollide ){
        return collides;
    }


    if( renderObj->testChildCollisionIgnoreBounds || pointInRect(&renderObj->collisionRect, x, y) ){
   // if( pointInRect(&renderObj->renderRect, x, y) ){
        if( renderObj->canCollide ){
            collides = true;
            // do something if a thing to do is defined for this collisino
            if( renderObj->hasInteraction || renderObj->hasInteractionCb ){
                // quite arguable we can return here, continuing to seek interaction on child objects may produce odd effects
                interactionObject = renderObj;

                SDL_Log("WE DID FIND ANOTHER MATCH");

                isInteracting = true;
                    // I will argue we want the deepest interaction to tell us if it can be activated currently....
                    // and should not return at once..
                //return collides;
                //collides = true;
            }


        }



        if( renderObj->hasChildren  ){
            // we may transform x and y to local coordinates before sending to child object

            // we need to check the children in reverse, the ones that render last are on top
            for( int ix=renderObj->childListIndex-1; ix>-1; ix-- ){
                // if already has collides and already isInteracting then return it?????
                childCollides = objectCollides(renderObj->childList[ix], x, y);// || collides;

                if( childCollides ) {
                    return childCollides;
                }
            }
        }

    }

    // finally if we perfomed a translation for object position
    // reset that translation

    return collides;
}

// todo investigate dropping this function?  much of this should be split into a different file otherwise
bool Ux::bubbleCurrentInteraction(){ // true if we found a match
    uiObject* anObj = interactionObject->bubbleInteraction();
    if( anObj != nullptr ){
        interactionObject = anObj;
        return true;
    }
    return false;
}

bool Ux::interactionUpdate(uiInteraction *delta){
    // TODO ret bool determine modification?

    uiObject* orig = interactionObject;

    if( isInteracting && (interactionObject->hasInteraction || interactionObject->interactionCallback ) ){

        // IF the object has a 'drop interaction' function and we can instead gift the animation to the parent object if it collides..... we will do so now....
        if( interactionObject->shouldCeaseInteractionChecker == nullptr ||
           interactionObject->shouldCeaseInteractionChecker(interactionObject, delta) ){
            // if we did cease interaction, it is a point of note that interactionObject just changed
            if( orig == interactionObject ){
                // however it is also possible we have the same object here, and also possible we merly have interactionCallback
                if( interactionObject->hasInteraction ){
                    interactionObject->interactionFn(interactionObject, delta);
                }
                return true;
            }else{
                return interactionUpdate(delta); // we allow recursion here in case of double bubble
            }
        }

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
    //self->historyScroller->allowUp = true;
    self->updatePickHistoryPreview();
}

//static
void Ux::interactionNoOp(uiObject *interactionObj, uiInteraction *delta){
    // check delta->dx for movement amount
    // no movement total
}


//static
void Ux::interactionTogglePalletePreview(uiObject *interactionObj, uiInteraction *delta){
    Ux* self = Ux::Singleton();

    uiObject* trueInteractionObj = self->palleteSelectionColorPreview->uiObjectItself;

    /*
     palleteSelectionColorPreview = new uiViewColor(historyPalleteHolder, Float_Rect(0.0, 0.5, 1.0, 0.5));
     historyPalleteHolder->addChild(newHistoryPallete);
     palleteSelectionColorPreview->uiObjectItself->setInteractionCallback(&Ux::interactionTogglePalletePreview); // if we dragged and released... it will animate the rest of the way because of this
     palleteSelectionColorPreview->uiObjectItself->setInteraction(&Ux::interactionVert);
     */

    if( trueInteractionObj->is_being_viewed_state ) {
        trueInteractionObj->setAnimation( self->uxAnimations->slideDownFullHeight(trueInteractionObj) ); // returns uiAminChain*
        trueInteractionObj->is_being_viewed_state =false;
    }else{
        trueInteractionObj->isInBounds = true; // nice hack
        self->updatePickHistoryPreview();
        trueInteractionObj->setAnimation( self->uxAnimations->resetPosition(trueInteractionObj) ); // returns uiAminChain*
        trueInteractionObj->is_being_viewed_state = true;
    }
}


void Ux::clickPalleteColor(uiObject *interactionObj, uiInteraction *delta){

    if( !interactionObj->doesInFactRender ){
        return; // this means our tile is invalid/out of range
    }

    Ux* myUxRef = Ux::Singleton();

    myUxRef->palleteSelectionColorPreview->update(&interactionObj->backgroundColor);

    if( !myUxRef->palleteSelectionColorPreview->uiObjectItself->is_being_viewed_state ) {
        myUxRef->interactionTogglePalletePreview(myUxRef->palleteSelectionColorPreview->uiObjectItself, delta);
    }

}

//static
void Ux::interactionToggleHistory(uiObject *interactionObj, uiInteraction *delta){
    Ux* self = Ux::Singleton();


    //self->newHistoryFullsize->cancelCurrentAnimation();

    if( self->historyPalleteHolder->is_being_viewed_state ) {
        self->historyPalleteHolder->setAnimation( self->uxAnimations->slideDown(self->historyPalleteHolder) ); // returns uiAminChain*
        self->historyPalleteHolder->is_being_viewed_state = false;
    }else{
        self->historyPalleteHolder->isInBounds = true; // nice hack
        self->updatePickHistoryPreview();
        self->historyPalleteHolder->setAnimation( self->uxAnimations->resetPosition(self->historyPalleteHolder) ); // returns uiAminChain*
        self->historyPalleteHolder->is_being_viewed_state = true;
        //self->historyScroller->allowUp = true;
    }
}

void Ux::clickHistoryColor(uiObject *interactionObj, uiInteraction *delta){

    // check delta->dx for movement amount
    // no movement total

    if( !interactionObj->doesInFactRender ){
        return; // this means our tile is invalid/out of range
    }

    Ux* myUxRef = Ux::Singleton();




    // A couple things
    // if image is already in the pallete, we sholud probably not add it twice
    // we should instead scroll to the color and animate it
    // we should always scroll to that index though...

     myUxRef->lastPalleteIndex = myUxRef->palleteIndex;




    int colorIndexOffset = interactionObj->backgroundColor.r * interactionObj->backgroundColor.g * interactionObj->backgroundColor.b;

    if( myUxRef->palleteColorsIndex[colorIndexOffset] > -1 && myUxRef->palleteColorsIndex[colorIndexOffset] < palleteMax ){

        // this color is already taken then


        // pallete offset
        Uint8 palleteOffset = myUxRef->palleteColorsIndex[colorIndexOffset];
        //&myUxRef->palleteColors[palleteOffset]

        myUxRef->palleteScroller->scrollToItemByIndex(palleteOffset);

        uiObject* visibleTile = myUxRef->palleteScroller->getVisibleTileForOffsetOrNull(palleteOffset);

        //uxAnimations->rvbounce(historyPreview);


        if( visibleTile != nullptr ){
            myUxRef->uxAnimations->rvbounce(visibleTile);
        }


        myUxRef->uxAnimations->rvbounce(interactionObj);
        return;
    }

    if( myUxRef->palleteIndex > myUxRef->largestPalleteIndex ) myUxRef->largestPalleteIndex = myUxRef->palleteIndex;

    SDL_Color exi = myUxRef->palleteColors[myUxRef->palleteIndex];
    myUxRef->palleteColorsIndex[exi.r * exi.g * exi.b] = -1; // we are about to overwrite this color... lets reset it from index
    // but is this really biggst value using -1 in this way?


    setColor( &myUxRef->palleteColors[myUxRef->palleteIndex++], &interactionObj->backgroundColor);

    myUxRef->palleteColorsIndex[colorIndexOffset] = myUxRef->lastPalleteIndex;


    if( myUxRef->palleteIndex >= palleteMax ){
        myUxRef->palleteIndex = 0;
    }

    //resize pallete scroller as we add more selections....
    if( myUxRef->largestPalleteIndex > 5 ){
        myUxRef->palleteScroller->resizeTililngEngine(4, 2);
    }else if( myUxRef->largestPalleteIndex > 3 ){
        myUxRef->palleteScroller->resizeTililngEngine(3, 2);
    }else if( myUxRef->largestPalleteIndex > 2 ){
        myUxRef->palleteScroller->resizeTililngEngine(4, 1);
    }else if( myUxRef->largestPalleteIndex > 1 ){
        myUxRef->palleteScroller->resizeTililngEngine(3, 1);
    }else if( myUxRef->largestPalleteIndex > 0 ){
        myUxRef->palleteScroller->resizeTililngEngine(2, 1);
    }


    //if( myUxRef->historyPalleteHolder->isInBounds ){
        myUxRef->palleteScroller->updateTiles(); // calls updateUiObjectFromPallete for each tile
    //}

    myUxRef->palleteScroller->scrollToItemByIndex(myUxRef->lastPalleteIndex);
}


//static // RENAME ME
void Ux::interactionTouchRelease(uiObject *interactionObj, uiInteraction *delta){


    // check delta->dx for movement amount
    // no movement total


    Ux* myUxRef = Ux::Singleton();
    myUxRef->addCurrentToPickHistory();

}



// this overrides the interface fn?
// this function is a bit odd for now, but is planned to be used to drag and throw objects that will otherwise animiate in, or not animate in based on this - Pane
//static
void Ux::interactionVert(uiObject *interactionObj, uiInteraction *delta){
    Ux* self = Ux::Singleton();

    // if we are already panning ....
    interactionObj->cancelCurrentAnimation();
    //self->historyFullsize // right now there is still some history specific stuff here

    if( interactionObj->interactionProxy != nullptr ){
        interactionObj = interactionObj->interactionProxy;
    }

    interactionObj->boundryRect.y += (delta->ry * (1.0/interactionObj->parentObject->collisionRect.h));

    if( delta->ry > 0.003 ){ // moving down
        interactionObj->is_being_viewed_state = true;
    }

    if( delta->ry < -0.003 ){
        interactionObj->is_being_viewed_state = false; // toggle still to be called..... on touch release (interactionCallback)
    }

    self->updateRenderPosition(interactionObj);
}

//static
void Ux::interactionSliderVT(uiObject *interactionObj, uiInteraction *delta){

    interactionObj->boundryRect.y += (delta->ry * (1.0/interactionObj->parentObject->collisionRect.h));
    /// we cannot modify the rect directly like this?

    //computedMovementRect
    if( interactionObj->boundryRect.y < interactionObj->computedMovementRect.y ){
        interactionObj->boundryRect.y = interactionObj->computedMovementRect.y;
        delta->fixY(interactionObj->collisionRect,  interactionObj->parentObject->collisionRect);

    }else if( interactionObj->boundryRect.y > interactionObj->computedMovementRect.y + interactionObj->computedMovementRect.h){
        interactionObj->boundryRect.y = interactionObj->computedMovementRect.y + interactionObj->computedMovementRect.h;
        delta->fixY(interactionObj->collisionRect,  interactionObj->parentObject->collisionRect);
    }


    if( interactionObj->hasAnimCb ){
        interactionObj->animationPercCallback(interactionObj, interactionObj->computedMovementRect.h > 0 ? ((interactionObj->boundryRect.y - interactionObj->computedMovementRect.y) / interactionObj->computedMovementRect.h) : 0.0);
    }

    Ux* myUxRef = Ux::Singleton();
    myUxRef->updateRenderPosition(interactionObj);
}

//static
void Ux::interactionHZ(uiObject *interactionObj, uiInteraction *delta){

    interactionObj->boundryRect.x += (delta->rx * (1.0/interactionObj->parentObject->collisionRect.w));
    /// we cannot modify the rect directly like this?

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
        interactionObj->animationPercCallback(interactionObj, interactionObj->computedMovementRect.w > 0 ? ((interactionObj->boundryRect.x - interactionObj->computedMovementRect.x) / interactionObj->computedMovementRect.w) : 0.0);
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


//static add Ux::
bool Ux::bubbleInteractionIfNonClick(uiObject *interactionObj, uiInteraction *delta){ // return true always, unless the interaction should be dropped and not bubble for some reason....
    // THIS should return true if the interaciton is still valid, which in all cases should really be YES - unles interaction object is for some reason nullptr reference

    Ux* self = Ux::Singleton();
    // see also interactionUpdate

    if( delta->dy != 0 || delta->dx != 0 ){
        return self->bubbleCurrentInteraction(); // *SEEMS * much simploer to call bulbble on the UI object itself, perhaps returning the reference to the new interactionObject instead of bool....
    }

    return true;
}


// **** PALLETTE ****
//static
bool Ux::updateUiObjectFromPallete(uiObject *historyTile, int offset){
    Ux* self = Ux::Singleton();

    if( offset > self->largestPalleteIndex || offset < 0 ){
        historyTile->doesInFactRender = false;
        historyTile->doesRenderChildObjects = false;
        return false;
    }

    offset = offset; // no reverse here

    historyTile->hasBackground = true;

    SDL_Color* clr = &self->palleteColors[offset];
    Ux::setColor(&historyTile->backgroundColor, clr->r, clr->g, clr->b, 255);

    char* resultText = (char*)malloc( 6 ); //leaking memory??
    sprintf(resultText, "%02x%02x%02x", clr->r, clr->g, clr->b);


    if( historyTile->getChildCount() < 1 ){
        uiObject * letterSize = new uiObject();
        letterSize->setBoundaryRect( 0.0, 0.0, 0.16666666666667, 1.0);
        historyTile->addChild(letterSize);

        self->printStringToUiObject(letterSize, resultText, RESIZE_NOW);
    }else{
        self->printStringToUiObject(historyTile->childList[0], resultText, DO_NOT_RESIZE_NOW);
    }

    free(resultText);

    //historyTile->hasForeground = true;
    //self->printCharToUiObject(historyTile, 'Z'-offset, true); // meh>>>?!

    historyTile->doesInFactRender = true;
    historyTile->doesRenderChildObjects = true;

    return true;
}

int Ux::getPalleteTotalCount(){
    Ux* self = Ux::Singleton();
    //return self->palleteIndex - 1;
    return self->largestPalleteIndex;
}



// **** HISTORY ****
//static
bool Ux::updateUiObjectFromHistory(uiObject *historyTile, int offset){
    Ux* self = Ux::Singleton();

    if( offset > self->pickHistoryIndex - 1 || offset < 0 ){
        historyTile->doesInFactRender = false;
        return false;
    }

    // TODO we need to also account for largestPickHistoryIndex to see if we looped!!!!!!!!!
    // if largest is max, then we loop from the current index, but continue looping after reaching the bottom, its a modulous problem
    // really this only applies if offset computed next is less than zero

    offset = self->pickHistoryIndex - offset - 1; // we show them in reverse here

    historyTile->hasBackground = true;

    SDL_Color* clr = &self->pickHistory[offset];
    Ux::setColor(&historyTile->backgroundColor, clr->r, clr->g, clr->b, 255);

    //historyTile->hasForeground = true;
    //self->printCharToUiObject(historyTile, 'Z'-offset, true); // meh>>>?!

    historyTile->doesInFactRender = true;

    return true;
}

int Ux::getHistoryTotalCount(){
    Ux* self = Ux::Singleton();
    return self->pickHistoryIndex - 1;
}




/// eeeh this is really update current color
///   mayhaps this only updates when you stop panning?
///   or otherwise seperate function for addPickHistory from this
void Ux::updateColorValueDisplay(SDL_Color* color){

    curerntColorPreview->update(color);

    currentlyPickedColor = color; // maybe really leaking memory ?
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
    pickHistory[pickHistoryIndex++] = *currentlyPickedColor; // 

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

    if( historyPalleteHolder->isInBounds ){
        historyScroller->updateTiles(); // calls updateUiObjectFromHistory for each tile
        //palleteScroller->updateTiles();
    }

    //if( !historyPalleteHolder->isInBounds ) return;

//    int historyTile = 0;
//    int historyOffset = pickHistoryIndex - 1; // 0;
//    if( histIndex < 0 ){
//        histIndex = largestPickHistoryIndex;
//    }
//    int rowsTotal = 7;
//    int rowCtr = -1;
//    while(++rowCtr < rowsTotal ){
//
//        int ctr = SIX_ACROSS; // redefinition of ctr....
//        while( --ctr >= 0 ){
//
//            //if( historyOffset > largestPickHistoryIndex ) break;
//            if( historyOffset < 0 ) break;
//
//
//            clr = &pickHistory[historyOffset];
//
//
//            colora = historyFullsize->childList[historyTile];
//
//            Ux::setColor(&colora->backgroundColor, clr->r, clr->g, clr->b, 255);
//
//
//            historyTile++;
//            historyOffset--;
//        }
//
//        //if( historyOffset > largestPickHistoryIndex ) break;
//        if( historyOffset < 0 ) break;
//
//
//    }


}


//bool Ux::updateAnimations(float elapsedMs){
//
//    //return uxAnimations->updateAnimations(elapsedMs);
//
//}


int Ux::renderObject(uniformLocationStruct *uniformLocations){
    return renderObjects(uniformLocations, rootUiObject);
}

int Ux::renderObjects(uniformLocationStruct *uniformLocations, uiObject *renderObj){
    // update
    // render object itself, then render all child hobj


    if( !renderObj->isInBounds ) return 1;

    //glm::mat4 uiMatrix = glm::mat4(1.0f);



    if( renderObj->isDebugObject ){
        SDL_Log("DEBUG OBJECT IS BEING RENDERED NOW!");
//        renderObj->boundryRect.y+=0.01; // this proves the boundary tests is working for this object, when out of view they do not render...
//        renderObj->updateRenderPosition();

    }

    //uiMatrix = glm::scale(uiMatrix, glm::vec3(0.9,0.9,1.0));
    //uiMatrix = glm::rotate(uiMatrix,  2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
//

    //renderObj->matrix = glm::scale(renderObj->matrix, glm::vec3(0.998,0.998,1.0));

    glUniformMatrix4fv(uniformLocations->modelMatrixLocation, 1, GL_FALSE, &renderObj->matrix[0][0]); // Send our model matrix to the shader

    //textMatrix = glm::translate(textMatrix, screenToWorldSpace(1000.0,500.0,450.1));  // just try screen coord like -512??


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





        //glUniform1f(uniformLocations->ui_corner_radius, 0.15);
       // glUniform1f(uniformLocations->ui_corner_radius, 0.0);

        glUniform4f(uniformLocations->ui_corner_radius,
                    renderObj->roundedCornersRect.x,
                    renderObj->roundedCornersRect.y,
                    renderObj->roundedCornersRect.w,
                    renderObj->roundedCornersRect.h);


        if( renderObj->hasCropParent ){
//            if( renderObj->isDebugObject ){
//                SDL_Log("DEBUG OBJECT IS BEING UPDATED NOW!");
//
//// crop may need to be parent of parent..... maybe just ask teh scroll controller about it?
//    // TODO: also seems some recursive need where all(or some) children may also share the same crop parent????
////                SDL_Log("child   render rect %f %f %f %f",
////                        renderObj->renderRect.x,
////                        renderObj->renderRect.y,
////                        renderObj->renderRect.w,
////                        renderObj->renderRect.h);
////
////                SDL_Log("parent2 render rect %f %f %f %f",
////                        renderObj->cropParentObject->renderRect.x,
////                        renderObj->cropParentObject->renderRect.y,
////                        renderObj->cropParentObject->renderRect.w,
////                        renderObj->cropParentObject->renderRect.h);
//            }


            glUniform4f(uniformLocations->ui_crop,
                        renderObj->cropParentObject->renderRect.x,
                        -renderObj->cropParentObject->renderRect.y,
                        renderObj->cropParentObject->renderRect.w,
                        renderObj->cropParentObject->renderRect.h);


        }else{
            // also note maybe this should only apply for the first and last 2 rows of tiles (optmimization) see allocateChildTiles and uiShader.vsh
            glUniform4f(uniformLocations->ui_crop,
                        0,
                        0,
                        /*disabled*/0,//1,
                        /*disabled*/0);//1); // 0,0,1,1  is screen crop, but we can skip this logic in vsh
        }

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

        // see updateStageDimension and consider SDL_RenderSetClipRect
        glDrawArrays(GL_TRIANGLES, 0, 6); // hmmm

    }


    if( renderObj->hasChildren && renderObj->doesRenderChildObjects ){
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
