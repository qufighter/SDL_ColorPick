//
//  uiScrollController.h
//  ColorPick iOS SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiScrollController_h
#define ColorPick_iOS_SDL_uiScrollController_h


struct uiScrollController{

    uiScrollController(){
        Ux* uxInstance = Ux::Singleton();


        uiObjectItself = new uiObject();
        scrollChildContainer = new uiObject();
        scrollBarVertHolder = new uiObject();
        scrollUp = new uiObject();
        scrollVtDragHolder = new uiObject();
        scrollVtDrag = new uiObject();
        scrollDown = new uiObject();

        uiObjectItself->myScrollController = this; // this propagates to the other child objects
        uiObjectItself->addChild(scrollChildContainer);
        uiObjectItself->addChild(scrollBarVertHolder); // we need to add these first here, so the myScrollController propagates to all child elements

        scrollBarVertHolder->addChild(scrollUp);
        scrollBarVertHolder->addChild(scrollDown);
        scrollBarVertHolder->addChild(scrollVtDragHolder);

        scrollVtDragHolder->addChild(scrollVtDrag);

        scrollChildContainer->setBoundaryRect( 0.0, 0.0, 1.0 - SCROLLY_WIDTH, 1.0);

        scrollBarVertHolder->setBoundaryRect( 1.0 - SCROLLY_WIDTH, 0.0, SCROLLY_WIDTH, 1.0);

        scrollUp->hasForeground = true;
        //uxInstance->printCharToUiObject(scrollUp, '^', true);
        uxInstance->printCharToUiObject(scrollUp, CHAR_ARR_UP, true);
        scrollUp->setBoundaryRect( 0.0, 0.0, 1.0, SCROLLY_WIDTH);
        scrollUp->setInteractionCallback(&this->interactionScrollUp);
        scrollUp->hasBackground = true;
        Ux::setColor(&scrollUp->backgroundColor, 255, 255, 255, 50);

        scrollDown->hasForeground = true;
        //uxInstance->printCharToUiObject(scrollDown, 'v', true);
        uxInstance->printCharToUiObject(scrollDown, CHAR_ARR_DN, true);
        scrollDown->setBoundaryRect( 0.0, 1.0 - SCROLLY_WIDTH, 1.0, SCROLLY_WIDTH);
        scrollDown->setInteractionCallback(&this->interactionScrollDown);
        scrollDown->hasBackground = true;
        Ux::setColor(&scrollDown->backgroundColor, 255, 255, 255, 50);

        // TODO scrollVtDrag should become a container, that way it can intercept clicks etc..., then we can do the dynamic part below...

        scrollVtDragHolder->setBoundaryRect( 0.0, SCROLLY_WIDTH, 1.0, 1.0 - SCROLLY_WIDTH - SCROLLY_WIDTH);// 0.23 is the height of the scrolly widget... make it dynamic :!
        scrollVtDragHolder->setInteraction(&this->interactionDragBgClicked);
        scrollVtDragHolder->setInteractionCallback(&this->interactionDragReleased);

        scrollVtDrag->hasForeground = true;
        //uxInstance->printCharToUiObject(scrollVtDrag, '|', true);
        uxInstance->printCharToUiObject(scrollVtDrag, CHAR_BARS, true);
        scrollVtDrag->setBoundaryRect( 0.0, 0.0, 1.0, 0.23); // 0.23 is the height of the scrolly widget... make it dynamic :!
        //scrollVtDrag->setBoundaryRect( 0.0, SCROLLY_WIDTH, 1.0, 0.23); // 0.23 is the height of the scrolly widget... make it dynamic :!
        //Ux::setRect(&scrollVtDrag->movementBoundaryRect, 0.0, SCROLLY_WIDTH, 0.0, 1.0 - SCROLLY_WIDTH - SCROLLY_WIDTH);
        Ux::setRect(&scrollVtDrag->movementBoundaryRect, 0.0, 0.0, 0.0, 1.0);
        scrollVtDrag->setInteraction(&Ux::interactionSliderVT);
        scrollVtDrag->setAnimationPercCallback(setScrollToPercent);


        scrollVtDrag->setInteractionCallback(&this->interactionDragReleased);
        scrollVtDrag->hasBackground = true;
        Ux::setColor(&scrollVtDrag->backgroundColor, 0, 255, 0, 50);


        // IF WE DO NOT ADJUST THE HEIGHT HERE WE WON"T BE ABLE TO CLICK THE BOTTOM ROW OF TILES>>>> AND PERHAPS WE WILL INSTEAD INTERACT WITH A DIFFERENT OBJECT. (we did search for this line)
//        scrollChildContainer->setInteraction(&this->interactionScrollDragVert);
//        scrollChildContainer->setShouldCeaseInteractionChek(&this->testIfObjectCanInteract);
//        scrollChildContainer->setInteractionCallback(&this->interactionDragReleased);
        // so instead this object iself has a sort of catch all for all interactions... hrrrm
        uiObjectItself->setInteraction(&this->interactionScrollDragVert);
        uiObjectItself->setShouldCeaseInteractionChek(&this->testIfObjectCanInteract);
        uiObjectItself->setInteractionCallback(&this->interactionDragReleased);
        scrollChildContainer->setModeWhereChildCanCollideAndOwnBoundsIgnored(); // this should make it really work... since the scroll container is just used to offset the tiles

        scrollx = 0.0;
        scrolly = 0.0;
        allowUp = true; /// if OOB maybe not...
        allowDown = true;

        tileChildObjects = true;
        childObjectsPerRow = 1;
        rowsToShow = 1;
        childObjectsOffset=0;
        getTile=nullptr;

       // allocateChildTiles(); // this should just allocate 2 tiles....  WHY DO WE DO THIS .... its a good deafult but they don't have the click events added....
    }

    typedef bool (*updateTileFunction)(uiObject *tile, int offset);
    typedef int (*getTotalFunction)();


    uiObject* uiObjectItself; // no real inheritance here, this its the uiScrollObject, I would use self->
    uiObject* scrollChildContainer; // contains the tiles
    uiObject* scrollBarVertHolder;
    uiObject* scrollUp;
    uiObject* scrollVtDragHolder;
    uiObject* scrollVtDrag;
    uiObject* scrollDown;


    bool allowUp;
    bool allowDown;

    float scrollx;
    float scrolly;
    int rowsOffset;

    float tileWidth;
    float tileHeight;

    bool tileChildObjects; // enables tiling engine
    bool tileRightToLeft = true; // we just tile rtl for now to match our needs
    int childObjectsPerRow; // this is suppose to help the tiling engine for the scroll container
    int rowsToShow;
    int childObjectsOffset=0;
    int totalChildObjects; // this is for the tiling engine to understand the total items we may represent in the limited squares we allocate
    int totalScrollRows; // is zero when no scrolling possible
    float minimumScrollY;

    updateTileFunction getTile=nullptr;
    getTotalFunction getTotal=nullptr;
    anInteractionFn tileClicked=nullptr;

    void initTilingEngine(int itemsPerRow, int rows, updateTileFunction getTileCb, getTotalFunction getTotalCb, anInteractionFn tileClickedFn){
        childObjectsPerRow = itemsPerRow;
        rowsToShow = rows;
        getTile = getTileCb;
        getTotal = getTotalCb;
        tileClicked = tileClickedFn;

        tileWidth = 1.0 / childObjectsPerRow;
        tileHeight = 1.0 / rowsToShow;

        allocateChildTiles();
    }

    void animateScrollXyRelative(float x, float y){
        Ux* uxInstance = Ux::Singleton();

        uiAminChain* myAnimChain = new uiAminChain();
        myAnimChain->addAnim((new uiAnimation(this->scrollChildContainer))->initialMoveVelocity(x, y)->setAnimationCallback(scrollAnimationUpdaterCb)->setGetBoundsFn(scrollAnimationGetBoundsFn) );
        myAnimChain->addAnim((new uiAnimation(this->scrollChildContainer))->setAnimationReachedCallback(ourAnimationCompleted) );
        this->scrollChildContainer->setAnimation(myAnimChain); // imporrtant to do this before we push it..
        uxInstance->uxAnimations->pushAnimChain(myAnimChain);

    }

    static bool testIfObjectCanInteract(uiObject *interactionObj, uiInteraction *delta){ // return true always, unless the interaction should be dropped and not bubble for some reason....
                                                                                // THIS should return true if the interaciton is still valid, which in all cases should really be YES - unles interaction object is for some reason nullptr reference
        uiScrollController* self = interactionObj->myScrollController;
        if( !self->allowUp && delta->dy > 0 ){ // possibly allowAllowUp or much better configurability of what edge of scrolling might yield the scroll event to another object....
            Ux* uxInstance = Ux::Singleton();
            return uxInstance->bubbleCurrentInteraction(); // *SEEMS * much simploer to call bulbble on the UI object itself, perhaps returning the reference to the new interactionObject instead of bool....
        }

        // this does not work for some reason! possibly known
//        if( !self->allowDown && delta->dy < 0 ){ // possibly allowAllowUp or much better configurability of what edge of scrolling might yield the scroll event to another object....
//            Ux* uxInstance = Ux::Singleton();
//            return uxInstance->bubbleCurrentInteraction();
//        }
        // if object can dismiss from bottom (swip out of view upwards) then we should also return false next...
        return true;
    }

    static void interactionScrollUp(uiObject *interactionObj, uiInteraction *delta){
        uiScrollController* self = interactionObj->myScrollController;
        self->animateScrollXyRelative(0, 0.001);
        //self->scrolly += 0.1; // TODO ANIMATE!
        //self->reflowTiles();
        //self->animConstrainToScrollableRegion();
    }

    static void interactionScrollDown(uiObject *interactionObj, uiInteraction *delta){
        uiScrollController* self = interactionObj->myScrollController;
        self->animateScrollXyRelative(0, -0.001);
        //        self->scrolly -= 0.1; // TODO ANIMATE!
        //        self->reflowTiles();
        //        self->animConstrainToScrollableRegion();
    }

    static void interactionDragReleased(uiObject *interactionObj, uiInteraction *delta){
        uiScrollController* self = interactionObj->myScrollController;
        if( !self->animConstrainToScrollableRegion() ){



            // this is tricky.... we do not want to always react (at all) or at least not the same way to a delta->ry here
            // however we do want to react in some way if this is trully a "throw" release... but depending on the object we interacted with.............. we could act one way or anohter
            // that is if we are not already constraining to a region with a different animation!
            // intertial scroll.... ry is not sufficent to really determine the velocity
            if( interactionObj == self->scrollChildContainer || interactionObj == self->uiObjectItself ){
                self->animateScrollXyRelative(0, delta->vy);
            }else if( interactionObj == self->scrollVtDrag ){
                self->animateScrollXyRelative(0, -delta->vy);
            }
        }
    }

    static void interactionDragBgClicked(uiObject *interactionObj, uiInteraction *delta){
        uiScrollController* self = interactionObj->myScrollController;
        self->scrollChildContainer->cancelCurrentAnimation();
        float percent = (delta->py - interactionObj->collisionRect.y) * (1.0/interactionObj->collisionRect.h);
        self->scrolly = percent * self->minimumScrollY;
        self->reflowTiles(/*updateScrollyThumb=*/true);
    }

    static void interactionScrollDragVert(uiObject *interactionObj, uiInteraction *delta){
        uiScrollController* self = interactionObj->myScrollController;
        self->scrollChildContainer->cancelCurrentAnimation();
        self->scrolly += delta->ry * (1.0/interactionObj->parentObject->collisionRect.h);
        self->reflowTiles();
        self->allowDown = true;
        self->allowUp = true;
    }

    //static for use as UI:: anAnimationPercentCallback callback
    static void setScrollToPercent(uiObject *interactionObj, float percent){
        uiScrollController* self = interactionObj->myScrollController;
        self->scrollChildContainer->cancelCurrentAnimation();
        self->updateTotalScrollRows();
        self->scrolly = percent * self->minimumScrollY;
        self->reflowTiles(/*updateScrollyThumb=*/false); // we do not need to updateScrollProgIndicator though !!!!!
    }

    // this is an instance of animationUpdateCallbackFn
    static void scrollAnimationUpdaterCb(uiAnimation* uiAnim, Float_Rect *newBoundaryRect){
        uiScrollController* self = uiAnim->myUiObject->myScrollController;
        self->scrolly = newBoundaryRect->y;
        self->reflowTiles();
    }

    // this is animationGetRectFn
    static Float_Rect* scrollAnimationGetBoundsFn(uiAnimation* uiAnim){
        uiScrollController* self = uiAnim->myUiObject->myScrollController;
        return new Float_Rect(self->scrollx, self->scrolly, 0, 0);
    }

    // this is animationCallbackFn - related to setAnimationReachedCallback
    static void ourAnimationCompleted(uiAnimation* uiAnim){
        uiScrollController* self = uiAnim->myUiObject->myScrollController;
        self->animConstrainToScrollableRegion(); // careful - ani is updating (though our current animation just completed)
    }

    void updateTotalScrollRows(){
        totalChildObjects = getTotal();
        totalScrollRows = (int(totalChildObjects / childObjectsPerRow) + 1) - rowsToShow;
        if( totalScrollRows < 1 ) totalScrollRows = 0; // sometimes no scrolling necessary, if fewer rows than scrollable
        minimumScrollY = -tileHeight * totalScrollRows;

    }

    bool animConstrainToScrollableRegion(){
        Ux* uxInstance = Ux::Singleton();

        if(scrolly > 0){
            //scrolly = tileHeight-0.0001; // we animate the scrolly instaed
            // I reallyt hink we should not call such a custom 1off fn? moveTo  could pass nullptr though :D (this does cancel existing anim)
            this->scrollChildContainer->setAnimation( uxInstance->uxAnimations->moveTo(this->scrollChildContainer,0.0,0.0, scrollAnimationUpdaterCb, scrollAnimationGetBoundsFn) );

            allowUp = false;
            allowDown = true;

            return true; // short circult hte rest, save some cpus
        }

        updateTotalScrollRows(); // in some cases, this call is redundant since its already updated by reflowTiles() inadvertantly!  I think its tricky to optimize away since we also do not always need to get here

        if( scrolly <= minimumScrollY){
            //scrolly = -tileHeight * totalScrollRows; // we animate the scrolly instaed
            // I reallyt hink we should not call such a custom 1off fn? moveTo  could pass nullptr though :D (this does cancel existing anim)
            this->scrollChildContainer->setAnimation( uxInstance->uxAnimations->moveTo(this->scrollChildContainer,0.0,minimumScrollY, scrollAnimationUpdaterCb, scrollAnimationGetBoundsFn) );

            allowUp = true;
            allowDown = false;

            return true;

        }
        allowDown = true;
        allowUp = true;

        return false; // return true if boundary is reached
    }

    void updateScrollProgIndicator(){
        updateTotalScrollRows();
        scrollVtDrag->boundryRect.h = totalScrollRows < 1 ? 1.0 : ((1.0f * rowsToShow) / (totalScrollRows + rowsToShow));
        scrollVtDrag->updateRenderPosition(); // this update th computed mvmt boundary rect
        scrollVtDrag->updateAnimationPercent(0.0, totalScrollRows < 1 ? 0.0 : (scrolly / minimumScrollY));// this does updateRenderPosition too!

    }

    void reflowTiles(){
        reflowTiles(true);
    }
    void reflowTiles(bool updateScrollyThumb){

        if( getTotal == nullptr) {
            SDL_Log("You forgot to set a total function...");
            return;
        }

        if( updateScrollyThumb ){
            updateScrollProgIndicator(); // THIS CALLS updateTotalScrollRows()
        }

        //Ux* uxInstance = Ux::Singleton();

        //        if( scrolly > 0 ){
        //            uxInstance->uxAnimations->moveTo(scrollChildContainer,0.0,0.0);
        //            scrolly = 0;
        //        }
        // we can't fire this now, but maybe after - there are 2 troubles:
        // 1) scrolly is not updated automatically as the animation progresses so we loose track of where we are
        // 2) etc...
        // lets implement draggy scroll first!  I think that will better detect the boundary conditions
        // 2 types of draggy scroll to go... one may perhaps closely mimmick.....  interactionVert ?
        // the other (scroller) may be more like interactionHZ.... where we just compute a percentage....

        // we allow max 1 tile overscroll
        //        if(scrolly >= tileHeight){
        //            scrolly = tileHeight-0.0001;
        //        }

        //        updateTotalScrollRows();
        //        if( scrolly <= -tileHeight * totalScrollRows){
        //            scrolly = -tileHeight * totalScrollRows;
        //        }

        float rowsOffsetAnd = scrolly / tileHeight;
        int newRowsOffset = int(rowsOffsetAnd);

        scrollChildContainer->boundryRect.y = (rowsOffsetAnd - newRowsOffset) * tileHeight;
        // IF WE DO NOT ADJUST THE HEIGHT HERE WE WON"T BE ABLE TO CLICK THE BOTTOM ROW OF TILES>>>> AND PERHAPS WE WILL INSTEAD INTERACT WITH A DIFFERENT OBJECT. (we did search for this line)
        //scrollChildContainer->boundryRect.h = scrollChildContainer->origBoundryRect.h + scrollChildContainer->boundryRect.y;
        // Instead we just used this: setModeWhereChildCanCollideAndOwnBoundsIgnored - which magically makes the event always collide with any child objects
        //SDL_Log("this is fun %f, %f" , scrollChildContainer->boundryRect.y , scrolly);

        scrollChildContainer->updateRenderPosition();

        if(rowsOffset != newRowsOffset){
            updateTiles();
        }

        rowsOffset = newRowsOffset;
    }

    void updateTiles(){ // we need to call this when 1) rows shift or 2) new history is added
        /// hope you called initTilingEngine!
        if( getTile == nullptr){
            SDL_Log("You forgot to set a tiling function...");
            return;
        }


        // this will be offset based on how far down we are scrolled (rows down times items per row)
        int offset = -int(scrolly / tileHeight) * childObjectsPerRow;

        bool result;
        for( int x=0,l=scrollChildContainer->childListIndex; x<l; x++ ){
            result = getTile(scrollChildContainer->childList[x], offset++); // see updateUiObjectFromHistory
            //if( !result) break; // no point continuing to update them?  there is a point if they have been rendered previously
        }


        updateScrollProgIndicator();
    }



    void allocateChildTiles(){

        //Ux* uxInstance = Ux::Singleton(); // try putting this in the inner loop!

        // TODO unwrap this loop?

        //uiObject* scrollTile = new uiObject();

        //scrollChildContainer->addChild(scrollTile);

        int rowsTotal = rowsToShow + 1; // we allocate an extra row for scrolling
        int rowCtr = -1;
        int tileCounter = 0;
        while( ++rowCtr < rowsTotal ){

            int ctr = childObjectsPerRow; // tileRightToLeft
            while( --ctr >= 0 ){
                uiObject *scrollTile;

                if( tileCounter < scrollChildContainer->childListIndex ){
                    scrollTile = scrollChildContainer->childList[tileCounter];
                }else{
                    scrollTile = new uiObject();
                    scrollTile->hasBackground = true;
                    Ux::setColor(&scrollTile->backgroundColor, 255, 0, 32, 0); // invisible


                    if( tileClicked != nullptr ){
                        scrollTile->setInteractionCallback(tileClicked); // needs more
                        scrollTile->setShouldCeaseInteractionChek(Ux::bubbleInteractionIfNonClick);
                    } // HMM first 2 tiles are not clickable...
                    // why do we do this: this should just allocate 2 tiles.

                    scrollChildContainer->addChild(scrollTile);


//                    // draw letters
//                    scrollTile->hasForeground = true;
//                    uxInstance->printCharToUiObject(scrollTile, 'Z'-tileCounter, true);

                    scrollTile->setCropParent(uiObjectItself); // maybe only needed on first and last 2 rows....
                }
                
                scrollTile->setBoundaryRect( ctr * tileWidth, rowCtr * tileHeight, tileWidth, tileHeight );
                
                tileCounter++;
            }
        }
        

        // we monitor the first tile now
//        if( !is_monitoring ){
//            is_monitoring=true;
//            scrollChildContainer->childList[0]->isDebugObject= true;
//            // setCropParent
//        }

        
    }
    
//    void positionChildTiles(){
//        allocateChildTiles(); // tehehehe (does not update the color values though, since it does not know our scroll position)
//    }
//    

    
};


#endif
