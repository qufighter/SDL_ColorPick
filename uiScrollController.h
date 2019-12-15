//
//  ColorPick SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiScrollController_h
#define ColorPick_iOS_SDL_uiScrollController_h


struct uiScrollController{

    uiScrollController(){
        Ux* uxInstance = Ux::Singleton();

        childObjectsToRender=0; // MUST init here too...

        getTile=nullptr;
        getTotal=nullptr;
        tileClicked=nullptr;
        tileDragged=nullptr;
        recievedFocus=nullptr;

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

        uiObjectItself->setInteractionWheel(&this->interactionWheel);


        scrollUp->hasForeground = true;
        //uxInstance->printCharToUiObject(scrollUp, '^', true);
        uxInstance->printCharToUiObject(scrollUp, CHAR_ARR_UP, true);
        scrollUp->setBoundaryRect( 0.0, 0.0, 1.0, SCROLLY_WIDTH);
        scrollUp->setClickInteractionCallback(&this->interactionScrollUp);
        scrollUp->setShouldCeaseInteractionChek(&this->testIfObjectCanInteractBasic);
        //scrollUp->hasBackground = true;
        //Ux::setColor(&scrollUp->backgroundColor, 255, 255, 255, 50);
        Ux::setColor(&scrollUp->foregroundColor, 255, 255, 255, 224);

        scrollDown->hasForeground = true;
        //uxInstance->printCharToUiObject(scrollDown, 'v', true);
        uxInstance->printCharToUiObject(scrollDown, CHAR_ARR_DN, true);
        scrollDown->setBoundaryRect( 0.0, 1.0 - SCROLLY_WIDTH, 1.0, SCROLLY_WIDTH);
        scrollDown->setClickInteractionCallback(&this->interactionScrollDown);
        scrollDown->setShouldCeaseInteractionChek(&this->testIfObjectCanInteractBasic);
        //scrollDown->hasBackground = true;
        //Ux::setColor(&scrollDown->backgroundColor, 255, 255, 255, 50);
        Ux::setColor(&scrollDown->foregroundColor, 255, 255, 255, 224);

#ifdef OMIT_SCROLLBAR_ARROWS
        scrollDown->hideAndNoInteraction();
        scrollUp->hideAndNoInteraction();
        scrollVtDragHolder->setBoundaryRect( 0.0, SCROLLY_MISSING_WIDTH, 1.0, 1.0 - SCROLLY_MISSING_WIDTH - SCROLLY_MISSING_WIDTH);
#else
        scrollVtDragHolder->setBoundaryRect( 0.0, SCROLLY_WIDTH, 1.0, 1.0 - SCROLLY_WIDTH - SCROLLY_WIDTH);
#endif
        scrollVtDragHolder->setInteraction(&this->interactionDragBgClicked);
        scrollVtDragHolder->setInteractionCallback(&this->interactionDragReleased);
        scrollVtDragHolder->setShouldCeaseInteractionChek(&this->testIfObjectCanInteractBasic);

        //scrollVtDrag->hasForeground = true;
        //uxInstance->printCharToUiObject(scrollVtDrag, '|', true);
        //uxInstance->printCharToUiObject(scrollVtDrag, CHAR_BARS, true);
        scrollVtDrag->setBoundaryRect( 0.0, 0.0, 1.0, 0.23); // 0.23 is the height of the scrolly widget... make it dynamic :!
        //scrollVtDrag->setBoundaryRect( 0.0, SCROLLY_WIDTH, 1.0, 0.23); // 0.23 is the height of the scrolly widget... make it dynamic :!
        //scrollVtDrag->setMovementBoundaryRect( 0.0, SCROLLY_WIDTH, 0.0, 1.0 - SCROLLY_WIDTH - SCROLLY_WIDTH);
        scrollVtDrag->setMovementBoundaryRect( 0.0, 0.0, 0.0, 1.0);
        scrollVtDrag->setInteraction(&Ux::interactionSliderVT);
        scrollVtDrag->setAnimationPercCallback(setScrollToPercent);

        scrollVtDrag->setShouldCeaseInteractionChek(&this->testIfObjectCanInteractBasic);
        //scrollVtDrag->setInteractionCallback(&this->interactionDragReleased);
        scrollVtDrag->hasBackground = true;
        Ux::setColor(&scrollVtDrag->backgroundColor, 175, 175, 175, 200);

        scrollVtDrag->matrix = glm::scale(scrollVtDrag->matrix, glm::vec3(0.7,1.0,1.0));

        scrollVtDrag->setRoundedCorners(0.05);

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
        allowUp = false; /// if OOB maybe not...
        allowDown = false;

        tileChildObjects = true;
        childObjectsPerRow = 1;
        rowsToShow = 1;
        childObjectsOffset=0;
        tileRightToLeft = true;
        getTile=nullptr;

       // allocateChildTiles(); // this should just allocate 2 tiles....  WHY DO WE DO THIS .... its a good deafult but they don't have the click events added....
    }

    typedef bool (*updateTileFunction)(uiScrollController* scroller, uiObject *tile, int offset);
    typedef int (*getTotalFunction)(uiScrollController* scroller);


    uiObject* uiObjectItself; // no real inheritance here, this its the uiScrollObject, I would use self->
    uiObject* scrollChildContainer; // contains the tiles
    uiObject* scrollBarVertHolder;
    uiObject* scrollUp;
    uiObject* scrollVtDragHolder;
    uiObject* scrollVtDrag;
    uiObject* scrollDown;

    void* ownerController;

    bool allowUp;
    bool allowDown;

    float scrollx;
    float scrolly;
    int rowsOffset;

    float tileWidth;
    float tileHeight;

    bool tileChildObjects; // enables tiling engine
    bool tileRightToLeft; // we just tile rtl for now to match our needs
    int childObjectsPerRow; // this is suppose to help the tiling engine for the scroll container
    int rowsToShow;
    int childObjectsOffset;
    int totalObjects; // this is for the tiling engine to understand the total items we may represent in the limited squares we allocate
    int totalScrollRows; // is zero when no scrolling possible
    int childObjectsToRender; // in case we allocated extra...
    float minimumScrollY;

    updateTileFunction getTile;
    getTotalFunction getTotal;
    anInteractionFn tileClicked;
    anInteractionFn tileDragged;
    anInteractionFn recievedFocus;

    void weGotFocus(uiObject *interactionObj, uiInteraction *delta){
        if( recievedFocus != nullptr ){
            recievedFocus(interactionObj, delta);
        }
    }

    void initTilingEngine(int itemsPerRow, int rows, updateTileFunction getTileCb, getTotalFunction getTotalCb, anInteractionFn tileClickedFn, anInteractionFn tileDraggedFn){
        getTile = getTileCb;
        getTotal = getTotalCb;
        tileClicked = tileClickedFn;
        tileDragged = tileDraggedFn;
        resizeTililngEngine(itemsPerRow, rows);
    }

    void resizeTililngEngine(int itemsPerRow, int rows){
        childObjectsPerRow = itemsPerRow;
        rowsToShow = rows;
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

    void animateScrollXyAbsolute(float x, float y){
        Ux* uxInstance = Ux::Singleton();

        uiAminChain* myAnimChain = new uiAminChain();
        myAnimChain->addAnim((new uiAnimation(this->scrollChildContainer))->moveTo(x, y)->setAnimationCallback(scrollAnimationUpdaterCb)->setGetBoundsFn(scrollAnimationGetBoundsFn) );
        myAnimChain->addAnim((new uiAnimation(this->scrollChildContainer))->setAnimationReachedCallback(ourAnimationCompleted) );
        this->scrollChildContainer->setAnimation(myAnimChain); // imporrtant to do this before we push it..
        uxInstance->uxAnimations->pushAnimChain(myAnimChain);
        
    }

    static bool testIfObjectCanInteractBasic(uiObject *interactionObj, uiInteraction *delta){ // return true always, unless the interaction should be dropped and not bubble for some reason....
        Ux* uxInstance = Ux::Singleton();
        // THIS should return true if the interaciton is still valid, which in all cases should really be YES - unles interaction object is for some reason nullptr reference
        uiScrollController* self = interactionObj->myScrollController;
        if( self->uiObjectItself->isInHiddenState() /*|| self->uiObjectItself->isInAnimation()*/ ){
            return uxInstance->bubbleCurrentInteraction(interactionObj, delta);
        }
        return true;
    }

    static bool testIfObjectCanInteract(uiObject *interactionObj, uiInteraction *delta){ // return true always, unless the interaction should be dropped and not bubble for some reason....
        Ux* uxInstance = Ux::Singleton();
                                                               // THIS should return true if the interaciton is still valid, which in all cases should really be YES - unles interaction object is for some reason nullptr reference
        uiScrollController* self = interactionObj->myScrollController;

        if( self->uiObjectItself->isInHiddenState() /*|| self->uiObjectItself->isInAnimation()*/ ){
            return uxInstance->bubbleCurrentInteraction(interactionObj, delta);
        }

        if( uxInstance->widescreen ){
           // SDL_Log("00))))0000000000000000000000000x y:%f x:%f",fabs(delta->dy), delta->dx);

            if( fabs(delta->dy) < 0.01f && delta->dx > 0.04f ){
                //SDL_Log("00))))0000000000000000000000000x y:%f x:%f  OK...",fabs(delta->dy), delta->dx);

                //Ux* uxInstance = Ux::Singleton();
                return uxInstance->bubbleCurrentInteraction(interactionObj, delta);
            }

        }else{
            if( !self->allowUp && delta->dy > 0 ){ // possibly allowAllowUp or much better configurability of what edge of scrolling might yield the scroll event to another object....
                //Ux* uxInstance = Ux::Singleton();
                return uxInstance->bubbleCurrentInteraction(interactionObj, delta); // *SEEMS * much simploer to call bulbble on the UI object itself, perhaps returning the reference to the new interactionObject instead of bool....
            }
        }
        // this does not work for some reason! possibly known
//        if( !self->allowDown && delta->dy < 0 ){ // possibly allowAllowUp or much better configurability of what edge of scrolling might yield the scroll event to another object....
//            Ux* uxInstance = Ux::Singleton();
//            return uxInstance->bubbleCurrentInteraction();
//        }
        // if object can dismiss from bottom (swip out of view upwards) then we should also return false next...
        return true;
    }

    static void interactionWheel(uiObject *interactionObj, uiInteraction *delta){
        uiScrollController* self = interactionObj->myScrollController;
        // the "wheel" will be in "lines" as per os configuration...
        // rowsToShow
        float offset = delta->wheel;
        if( SDL_fabsf(delta->wheel) > self->rowsToShow ){
            offset *= self->rowsToShow / SDL_fabsf(delta->wheel);
        }
        self->animateScrollXyAbsolute(0, self->scrolly + (offset * self->tileHeight));
    }

    static void interactionScrollUp(uiObject *interactionObj, uiInteraction *delta){
        uiScrollController* self = interactionObj->myScrollController;
        Ux* uxInstance = Ux::Singleton();
        uiKeyInteractions* keyInteractions = uxInstance->getKeyInteractions(); // TODO: eventually a helper on uxInstance should handle this... computing for either mouse, finger, or key durations and returining the appropriate multipler
        self->animateScrollXyRelative(0, 0.001 * keyInteractions->downDuration(delta->lastUpdate, 6.0, 2.0));
        //self->scrolly += 0.1; // TODO ANIMATE!
        //self->reflowTiles();
        //self->animConstrainToScrollableRegion();
        self->weGotFocus(interactionObj, delta);
    }

    static void interactionScrollDown(uiObject *interactionObj, uiInteraction *delta){
        uiScrollController* self = interactionObj->myScrollController;
        Ux* uxInstance = Ux::Singleton();
        uiKeyInteractions* keyInteractions = uxInstance->getKeyInteractions();
        self->animateScrollXyRelative(0, -0.001 * keyInteractions->downDuration(delta->lastUpdate, 6.0, 2.0));
        //        self->scrolly -= 0.1; // TODO ANIMATE!
        //        self->reflowTiles();
        //        self->animConstrainToScrollableRegion();
        self->weGotFocus(interactionObj, delta);
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
                //self->animateScrollXyRelative(0, -delta->vy);
            }
        }
    }

    static void interactionDragBgClicked(uiObject *interactionObj, uiInteraction *delta){
        uiScrollController* self = interactionObj->myScrollController;
        self->scrollChildContainer->cancelCurrentAnimation();
        float percent = (delta->py - interactionObj->collisionRect.y) * (1.0/interactionObj->collisionRect.h);
        self->scrolly = percent * self->minimumScrollY;
        self->reflowTiles(/*updateScrollyThumb=*/true);

        self->weGotFocus(interactionObj, delta);
    }

    static void interactionScrollDragVert(uiObject *interactionObj, uiInteraction *delta){
        uiScrollController* self = interactionObj->myScrollController;
        self->scrollChildContainer->cancelCurrentAnimation();

        //SDL_Log("%f <--- delta ry %f", delta->ry, (1.0/interactionObj->parentObject->collisionRect.h));
        self->scrolly += delta->ry * (1.0/interactionObj->collisionRect.h);

        self->reflowTiles();
        self->allowDown = true;
        self->allowUp = true;

        self->weGotFocus(interactionObj, delta);
    }

    //static for use as UI:: anAnimationPercentCallback callback
    static void setScrollToPercent(uiObject *interactionObj, float percent){
        uiScrollController* self = interactionObj->myScrollController;
        self->scrollChildContainer->cancelCurrentAnimation();
        self->updateTotalScrollRows();
        self->scrolly = percent * self->minimumScrollY;
        self->reflowTiles(/*updateScrollyThumb=*/false); // we do not need to updateScrollProgIndicator though !!!!!

        Ux* uxInstance = Ux::Singleton();
        self->weGotFocus(interactionObj, &uxInstance->currentInteractions[0]); // todo fix bad hack here... anAnimationPercentCallback should just get the relevant delta even if its unused
    }

    // this is an instance of animationUpdateCallbackFn
    static void scrollAnimationUpdaterCb(uiAnimation* uiAnim, Float_Rect *newBoundaryRect){
        uiScrollController* self = uiAnim->myUiObject->myScrollController;
        self->scrolly = newBoundaryRect->y;
        SDL_free(newBoundaryRect);
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
        totalObjects = getTotal(this);

        totalScrollRows = (int(ceil(totalObjects / (float)childObjectsPerRow))) - rowsToShow;
        if( totalScrollRows < 1 ){
            totalScrollRows = 0; // sometimes no scrolling necessary, if fewer rows than scrollable
        }
        minimumScrollY = -tileHeight * totalScrollRows;
//        if( scrolly < minimumScrollY ){
//            scrolly = minimumScrollY;
//        }

    }

    bool animConstrainToScrollableRegion(){
        return animConstrainToScrollableRegion(false);
    }

    bool animConstrainToScrollableRegion(bool skipUpdate){
        Ux* uxInstance = Ux::Singleton();

        if(scrolly > 0){
            //scrolly = tileHeight-0.0001; // we animate the scrolly instaed
            // I reallyt hink we should not call such a custom 1off fn? moveTo  could pass nullptr though :D (this does cancel existing anim)
            this->scrollChildContainer->setAnimation( uxInstance->uxAnimations->moveTo(this->scrollChildContainer,0.0,0.0, scrollAnimationUpdaterCb, scrollAnimationGetBoundsFn) );

            allowUp = false;
            allowDown = true;

            return true; // short circult hte rest, save some cpus
        }

        if(!skipUpdate)
            updateTotalScrollRows(); // in some cases, this call is redundant since its already updated by reflowTiles() inadvertantly!  I think its tricky to optimize away since we also do not always need to get here

        if( scrolly <= minimumScrollY){
            //scrolly = -tileHeight * totalScrollRows; // we animate the scrolly instaed
            // I reallyt hink we should not call such a custom 1off fn? moveTo  could pass nullptr though :D (this does cancel existing anim)
            this->scrollChildContainer->setAnimation( uxInstance->uxAnimations->moveTo(this->scrollChildContainer,0.0,minimumScrollY, scrollAnimationUpdaterCb, scrollAnimationGetBoundsFn) );


            if( totalObjects > 0 ){
                allowUp = true;
            }else{
                allowUp = false;
            }

            allowDown = false;

            return true;

        }


//        if( totalObjects > 0 ){
//            allowUp = true;
//            allowDown = true;
//        }else{
//            allowUp = false; /// if OOB maybe not...
//            allowDown = false;
//        }

//
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

    uiObject* getVisibleTileForOffsetOrNull(int itemOffset){
        int offset = getTileOffsetFromScroll();
        return scrollChildContainer->getChildOrNullPtr(itemOffset-offset);
    }

    // this is the public interface, the offset for the item in the total set of items
    void scrollToItemByIndex(int itemOffset){
        updateTotalScrollRows();
        if( totalScrollRows < 1 ){ return; }// no scrollable region
        int seekRow = itemOffset / childObjectsPerRow;
        int currentRow = abs((int)(scrolly / tileHeight));
        //        int maxRow = currentRow + rowsToShow;
        //        if( seekRow > currentRow && seekRow < maxRow ){
        //            // row is in full view already
        //            return;
        //        }
        if( seekRow < currentRow ){
            animateScrollXyAbsolute(0, ((seekRow) * -tileHeight) );
            return;

        }else if(seekRow >= (currentRow + rowsToShow)){
            animateScrollXyAbsolute(0, ((seekRow-(rowsToShow-1)) * -tileHeight) );
            return;
        }else{
            // row is in already full view (or is first row in partial view)
            if( seekRow == currentRow ){
                animateScrollXyAbsolute(0, ((seekRow) * -tileHeight) ); // first row partially visible... make it full vis
            }
            //int offset = getTileOffsetFromScroll();
            //Ux* myUxRef = Ux::Singleton();
            //myUxRef->uxAnimations->rvbounce(scrollChildContainer->getChildOrNullPtr(itemOffset-offset));
        }
    }

    int getTileOffsetFromScroll(){ // based on the current scroll this is the offset of the first visible tile.  it may be partially obscured from view
        return -int(scrolly / tileHeight) * childObjectsPerRow;
    }

    void updateTiles(){ // we need to call this when 1) rows shift or 2) new history is added
        /// hope you called initTilingEngine!
        if( getTile == nullptr){
            SDL_Log("You forgot to set a tiling function...");
            return;
        }


        // this will be offset based on how far down we are scrolled (rows down times items per row)
        int offset = getTileOffsetFromScroll();

        bool result;
        for( int x=0,l=childObjectsToRender; x<l; x++ ){
            result = getTile(this, scrollChildContainer->childList[x], offset++); // see updateUiObjectFromHistory
            //if( !result) break; // no point continuing to update them?  there is a point if they have been rendered previously
        }


        updateScrollProgIndicator(); // calls updateTotalScrollRows()

        if( totalScrollRows < 1 ){
            scrollBarVertHolder->boundryRect.w = 0;
            scrollChildContainer->boundryRect.w=1.0;
            uiObjectItself->updateRenderPosition();



        }else{
            scrollBarVertHolder->boundryRect.w = SCROLLY_WIDTH;
            scrollChildContainer->boundryRect.w=1.0 - SCROLLY_WIDTH;
            uiObjectItself->updateRenderPosition();

        }

        if( totalObjects < 1 ){
             uiObjectItself->doesNotCollide = true;
        }else{
            uiObjectItself->doesNotCollide = false;
        }

        // if we are not animating....
        if( !scrollChildContainer->isAnimating()  ){
            animConstrainToScrollableRegion(true);  // also... calls updateTotalScrollRows()
        }
    }



    void allocateChildTiles(){

        //Ux* uxInstance = Ux::Singleton(); // try putting this in the inner loop!

        // TODO unwrap this loop?

        //uiObject* scrollTile = new uiObject();

        //scrollChildContainer->addChild(scrollTile);

        int rowsTotal = rowsToShow + 1; // we allocate an extra row for scrolling
        int rowCtr = -1;
        int tileCounter = 0;
        uiObject *scrollTile;

        while( ++rowCtr < rowsTotal ){

            int ctr = childObjectsPerRow; // tileRightToLeft
            while( --ctr >= 0 ){

                if( tileCounter < scrollChildContainer->childListIndex ){
                    scrollTile = scrollChildContainer->childList[tileCounter];
                }else{
                    scrollTile = new uiObject();
                    scrollTile->hasBackground = true;
                    Ux::setColor(&scrollTile->backgroundColor, 255, 0, 32, 0); // invisible

                    if( tileClicked != nullptr ){
                        scrollTile->setInteractionCallback(tileClicked); // needs more // cannot be setClickInteractionCallback though...
                        scrollTile->setInteraction(tileDragged);

                        scrollTile->setShouldCeaseInteractionChek(Ux::bubbleInteractionIfNonHorozontalMovementScroller);
                    } // HMM first 2 tiles are not clickable...
                    // why do we do this: this should just allocate 2 tiles.

                    scrollChildContainer->addChild(scrollTile);


//                    // draw letters
//                    scrollTile->hasForeground = true;
                    //uxInstance->printCharToUiObject(scrollTile, 'Z'-tileCounter, true);

                    //scrollTile->setRoundedCorners(0.1, 0.2, 0.3, 0.4);
                   // scrollTile->setRoundedCorners(0.1, 0.1, 0.1, 0.1);

                    //scrollTile->setRoundedCorners(0.0, 0.2, 0.0, 0.0);
                    //scrollTile->setRoundedCorners(0.0, 0.0, 0.2, 0.0);
                    //scrollTile->setRoundedCorners(0.0, 0.0, 0.0, 0.4);

                    scrollTile->setCropParent(uiObjectItself); // maybe only needed on first and last 2 rows.... //maths2
                }
                
                scrollTile->setBoundaryRect( ctr * tileWidth, rowCtr * tileHeight, tileWidth, tileHeight );

                scrollTile->allowInteraction();

                // any pending animation needs to reset to this new position now.... hmmm....
                
                tileCounter++;
            }
        }

        childObjectsToRender = SDL_min(tileCounter, scrollChildContainer->childListIndex);

        while( tileCounter < scrollChildContainer->childListIndex ){

//            if(  rowsTotal == 6 ){
//                SDL_Log("we are hiding extra tiles...");
//            }

            // there are MORE tiles allocated than we currently need...
            // we can hide them
            scrollTile = scrollChildContainer->childList[tileCounter];
            scrollTile->hideAndNoInteraction();



            //uxInstance->printCharToUiObject(scrollTile, CHAR_ARR_UP, true);


            tileCounter++;
        }


        // we monitor the first tile now
//        if( !is_monitoring ){
//            is_monitoring=true;
    //        scrollChildContainer->childList[0]->isDebugObject= true;
//            // setCropParent
//        }

        
    }
    
//    void positionChildTiles(){
//        allocateChildTiles(); // tehehehe (does not update the color values though, since it does not know our scroll position)
//    }
//    

    
};


#endif
