//
//  uiHistoryPalleteEditor.h
//  ColorPick SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiHistoryPalleteEditor_h
#define ColorPick_iOS_SDL_uiHistoryPalleteEditor_h


struct uiHistoryPalleteEditor{  // we will become uxInstance->historyPalleteEditor - and is pretty much singleton....


    uiHistoryPalleteEditor(uiObject* parentObj){
        Ux* uxInstance = Ux::Singleton();

        uiObjectItself = new uiObject();
        historyPalleteHolder = uiObjectItself;

        Uint8 opacity = 220;

        historyPalleteHolder->hasBackground = true;
        Ux::setColor(&historyPalleteHolder->backgroundColor, 0, 0, 0, opacity);
        historyPalleteHolder->setInteractionCallback(&interactionToggleHistory); // if we dragged and released... it will animate the rest of the way because of this
        historyPalleteHolder->setInteraction(&Ux::interactionVert);
        historyPalleteHolder->setBoundsEnterFunction(&Ux::interactionHistoryEnteredView);
        historyPalleteHolder->is_being_viewed_state = true;


        historyScroller = new uiScrollController();
        historyScroller->initTilingEngine(6, 6, &updateUiObjectFromHistory, &getHistoryTotalCount, &clickHistoryColor, &Ux::interactionHorizontal);
        historyScroller->recievedFocus = &historyReceivedFocus;
        newHistoryFullsize = historyScroller->uiObjectItself;


        palleteScroller = new uiScrollController();
        palleteScroller->initTilingEngine(1, 1, &updateUiObjectFromPallete, &getPalleteTotalCount, &clickPalleteColor, Ux::interactionHorizontal);
        newHistoryPallete = palleteScroller->uiObjectItself;
        palleteScrollerEdgeShadow = new uiEdgeShadow(newHistoryPallete, SQUARE_EDGE_ENUM::TOP, 0.0);
        newHistoryPallete->hasBackground = true;
        Ux::setColor(&newHistoryPallete->backgroundColor, 0, 0, 0, 255);


        historyPalleteHolderTlEdgeShadow = new uiEdgeShadow(historyPalleteHolder, SQUARE_EDGE_ENUM::TOP, 0.03);
        historyPalleteHolderBrEdgeShadow = new uiEdgeShadow(historyPalleteHolder, SQUARE_EDGE_ENUM::BOTTOM, 0.03);

        historyPalleteHolderTlEdgeShadow->setOpacity(opacity);
        historyPalleteHolderBrEdgeShadow->setOpacity(opacity);


        palleteSelectionPreviewHolder = new uiObject();
        palleteSelectionPreviewHolder->setShouldCeaseInteractionChek(&bubbleInteractionIfPalletePreviewHidden);
        palleteSelectionPreviewHolder->setInteractionCallback(&interactionTogglePalletePreview); // if we dragged and released... it will animate the rest of the way because of this
        palleteSelectionPreviewHolder->setInteraction(&Ux::interactionVert);
        palleteSelectionPreviewHolder->setCropParentRecursive(historyPalleteHolder);
        palleteSelectionPreviewHolder->is_being_viewed_state = false;
        palleteSelectionPreviewHolder->doesNotCollide = true;
        //palleteSelectionPreviewHolder->setDefaultHidden();

        historyPalleteHolder->addChild(newHistoryFullsize);
        historyPalleteHolder->addChild(palleteSelectionPreviewHolder);
        historyPalleteHolder->addChild(newHistoryPallete);


        //historyPalleteScoreboard=uxInstance->defaultScoreDisplay

        historyPalleteCloseX = new uiObject();
        uxInstance->printCharToUiObject(historyPalleteCloseX, CHAR_CLOSE_ICON, DO_NOT_RESIZE_NOW);
        historyPalleteCloseX->hasForeground = true;
        historyPalleteCloseX->squarify();
        Ux::setColor(&historyPalleteCloseX->foregroundColor, 255, 255, 255, 96); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        historyPalleteHolder->addChild(historyPalleteCloseX);

        historyPalleteScoreboardHolder = new uiObject();
//        historyPalleteScoreboardHolder->hasBackground = true;
//        Ux::setColor(&historyPalleteScoreboardHolder->backgroundColor, 0, 255, 0, 128);
        historyPalleteHolder->addChild(historyPalleteScoreboardHolder);
        

        palleteSelectionColorPreview = new uiViewColor(palleteSelectionPreviewHolder, Float_Rect(0.0, 0.0, 1.0, 1.0), true); // this rect is reset next...


        palleteSelectionColorPreview->uiObjectItself->hasBackground = true;
        Ux::setColor(&palleteSelectionColorPreview->uiObjectItself->backgroundColor, 0, 0, 0, 255);
        palleteSelectionColorPreview->alphaMulitiplier = 255; //always solid bg


        parentObj->addChild(historyPalleteHolder);// aka uiObjectItself);

        uiObject *unusedObject = new uiObject(); // todo - strange convention?
        sortChooser=new uiSortChooser(unusedObject, Float_Rect(0.0, 0.15, 1.0, 0.20));

        historyScroller->updateTiles();
        palleteScroller->updateTiles();
    }

    void addScoreboard(uiObject* scoreboard){
        historyPalleteScoreboardHolder->addChild(scoreboard);
    }


    uiObject* uiObjectItself; // no real inheritance here, this its the uiHistoryPalleteEditor, I would use self->
    uiObject *historyPalleteHolder; // AKA ^ uiObjectItself

    uiEdgeShadow* historyPalleteHolderTlEdgeShadow;
    uiEdgeShadow* historyPalleteHolderBrEdgeShadow;
    uiEdgeShadow* palleteScrollerEdgeShadow;

    uiObject *historyPalleteScoreboardHolder;

    uiObject *historyPalleteCloseX;
    uiScrollController *historyScroller;
    uiScrollController *palleteScroller;

    uiObject *newHistoryFullsize; // just a ref to historyScroller's uiObjectItself
    uiObject *newHistoryPallete;

    uiViewColor *palleteSelectionColorPreview;
    uiObject *palleteSelectionPreviewHolder; // to contain the animation

    uiSortChooser *sortChooser;

    void resize(Float_Rect visible, Float_Rect hidden){
        Ux* uxInstance = Ux::Singleton();

        historyPalleteHolder->setBoundaryRectForAnimState(&visible,&hidden );


        if( uxInstance->widescreen ){


            historyPalleteScoreboardHolder->setBoundaryRect( 1.1, 0.05, 0.5, 0.065);


            historyPalleteHolderTlEdgeShadow->resize(SQUARE_EDGE_ENUM::LEFT);
            historyPalleteHolderBrEdgeShadow->resize(SQUARE_EDGE_ENUM::RIGHT);

            historyPalleteHolder->setInteraction(&Ux::interactionHorizontal);

            historyPalleteCloseX->setBoundaryRect( 1.01, 0.5-0.025, 0.05, 0.05);


            newHistoryFullsize->setBoundaryRect( 0.0, 0.0, 0.6, 1.0);
            // historyScroller->resize()?

            newHistoryPallete->setBoundaryRect( 0.61, 0.0, 0.4 - 0.01, 1.0);
            palleteScrollerEdgeShadow->resize(SQUARE_EDGE_ENUM::LEFT);
            // historyScroller->resize()?

            // we shoudl resize it but it should be in a new container object... that simplifies the update
            // or just palleteSelectionColorPreview->resize() ??
            //            palleteSelectionColorPreview->uiObjectItself->setBoundaryRectForAnimState(
            //              newHistoryPallete->boundryRect.x, newHistoryPallete->boundryRect.y - newHistoryPallete->boundryRect.h,
            //              newHistoryPallete->boundryRect.w, newHistoryPallete->boundryRect.h, // vis and orig
            //              newHistoryPallete->boundryRect.x, newHistoryPallete->boundryRect.y,
            //              newHistoryPallete->boundryRect.w, newHistoryPallete->boundryRect.h // hidden
            //            );
            palleteSelectionPreviewHolder->setBoundaryRectForAnimState(
                                                                       newHistoryPallete->boundryRect.x - newHistoryPallete->boundryRect.w, newHistoryPallete->boundryRect.y,
                                                                       newHistoryPallete->boundryRect.w, newHistoryPallete->boundryRect.h, // vis and orig
                                                                       newHistoryPallete->boundryRect.x, newHistoryPallete->boundryRect.y,
                                                                       newHistoryPallete->boundryRect.w, newHistoryPallete->boundryRect.h // hidden
                                                                       );
            palleteSelectionPreviewHolder->setInteraction(&Ux::interactionHorizontal);
            palleteSelectionColorPreview->resize(Float_Rect(0.0, 0.0, 1.0, 1.0));

        }else{

            historyPalleteScoreboardHolder->setBoundaryRect( 0.0, 1.015, 0.5, 0.075);


            historyPalleteHolderTlEdgeShadow->resize(SQUARE_EDGE_ENUM::TOP);
            historyPalleteHolderBrEdgeShadow->resize(SQUARE_EDGE_ENUM::BOTTOM);
            historyPalleteHolder->setInteraction(&Ux::interactionVert);

            historyPalleteCloseX->setBoundaryRect( 0.5-0.025, 1.01, 0.05, 0.08);


            newHistoryFullsize->setBoundaryRect( 0.0, 0.0, 1.0, 0.6);
            // historyScroller->resize()?

            newHistoryPallete->setBoundaryRect( 0.0, 0.61, 1.0, 0.4 - 0.01);
            palleteScrollerEdgeShadow->resize(SQUARE_EDGE_ENUM::TOP);

            // historyScroller->resize()?

            // or just palleteSelectionColorPreview->resize() ??
            //            palleteSelectionColorPreview->uiObjectItself->setBoundaryRectForAnimState(
            //              newHistoryPallete->boundryRect.x, newHistoryPallete->boundryRect.y - newHistoryPallete->boundryRect.h,
            //              newHistoryPallete->boundryRect.w, newHistoryPallete->boundryRect.h, // vis and orig
            //              newHistoryPallete->boundryRect.x, newHistoryPallete->boundryRect.y,
            //              newHistoryPallete->boundryRect.w, newHistoryPallete->boundryRect.h // hidden
            //            );

            //0.0, 0.5, 1.0, 0.5
            palleteSelectionPreviewHolder->setBoundaryRectForAnimState(
                                                                       newHistoryPallete->boundryRect.x, newHistoryPallete->boundryRect.y - newHistoryPallete->boundryRect.h,
                                                                       newHistoryPallete->boundryRect.w, newHistoryPallete->boundryRect.h, // vis and orig
                                                                       newHistoryPallete->boundryRect.x, newHistoryPallete->boundryRect.y,
                                                                       newHistoryPallete->boundryRect.w, newHistoryPallete->boundryRect.h // hidden
                                                                       );
            palleteSelectionPreviewHolder->setInteraction(&Ux::interactionVert);
            palleteSelectionColorPreview->resize(Float_Rect(0.0, 0.0, 1.0, 1.0));


        }
    }

    void updatePalleteScroller(){
        Ux* uxInstance = Ux::Singleton();

        int palleteIconsTotal = getPalleteTotalCount(nullptr);
        if( uxInstance->widescreen ){
            if( palleteIconsTotal > 5 ){
                palleteScroller->resizeTililngEngine(2, 4);
            }else if( palleteIconsTotal > 4 ){
                palleteScroller->resizeTililngEngine(2, 3);
            }else if( palleteIconsTotal > 3 ){
                palleteScroller->resizeTililngEngine(1, 4);
            }else if( palleteIconsTotal > 2 ){
                palleteScroller->resizeTililngEngine(1, 3);
            }else if( palleteIconsTotal > 1 ){
                palleteScroller->resizeTililngEngine(1, 2);
            }else{
                palleteScroller->resizeTililngEngine(1, 1);
            }
        }else{
            //resize pallete scroller as we add more selections....
            if( palleteIconsTotal > 5 ){
                palleteScroller->resizeTililngEngine(4, 2);
            }else if( palleteIconsTotal > 4 ){
                palleteScroller->resizeTililngEngine(3, 2);
            }else if( palleteIconsTotal > 3 ){
                palleteScroller->resizeTililngEngine(4, 1);
            }else if( palleteIconsTotal > 2 ){
                palleteScroller->resizeTililngEngine(3, 1);
            }else if( palleteIconsTotal > 1 ){
                palleteScroller->resizeTililngEngine(2, 1);
            }else{
                palleteScroller->resizeTililngEngine(1, 1);
            }
        }

        if( historyPalleteHolder->isInBounds ){
            palleteScroller->updateTiles(); // calls updateUiObjectFromPallete for each tile
        }
    }

    bool update(){
        return true;
    }

    // SHARED:
    static void colorTileAddChildObjects(uiObject *historyTile, anInteractionFn removeClickedFn){
        // this adds the common child objects for history and pallete
        if( historyTile->getChildCount() < 2 ){
            Ux* myUxRef = Ux::Singleton();

            historyTile->matrixInheritedByDescendants = true; // animate teh tile to animate anything...

            uiObject* removeButtonHolder = new uiObject();
            uiObject* removeButton = new uiObject();
            removeButtonHolder->setBoundaryRect( 1.0, 0.0, 1.0, 1.0); // ofset to right 1.0
            removeButton->setBoundaryRect( 0.0, 0.0, 1.0, 1.0);

            //        removeButton->setCropParentRecursive(historyTile); // must set before adding child...  not strictly needed unless performing the next opperation below .. or actually this line is dupe since the child is not yet added?
            removeButton->setCropModeOrigPosition(); // influences the rendering of the X while we bounce (duplicate detected)

            removeButtonHolder->setCropParentRecursive(historyTile); // must set before adding child...
            removeButtonHolder->setCropModeOrigPosition(); // influences the rendering of the background color while we bounce (duplicate detected)

            removeButtonHolder->addChild(removeButton);

            historyTile->addChild(removeButtonHolder);


            myUxRef->printCharToUiObject(removeButton, CHAR_CLOSE_ICON, DO_NOT_RESIZE_NOW);
            removeButton->hasForeground = true;
            removeButtonHolder->hasBackground = true;

            removeButton->squarify();
            //removeButton->isDebugObject=true;

            Ux::setColor(&removeButton->foregroundColor, 200, 0, 0, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)

            removeButtonHolder->setInteraction(&Ux::interactionHorizontal);
            removeButtonHolder->setInteractionCallback(removeClickedFn); // cannot be setClickInteractionCallback

            uiObject* buttonIcon = new uiObject();
            buttonIcon->setBoundaryRect( 0.0, 0.0, 1.0, 1.0); // ofset to right 1.0
            buttonIcon->hasForeground = true;
            myUxRef->printCharToUiObject(buttonIcon, CHAR_LIGHTENING_BOLT, DO_NOT_RESIZE_NOW);
            historyTile->addChild(buttonIcon);
            buttonIcon->hide();
            buttonIcon->squarify();
        }
    }

    static void showAdditonalMessageLightenignBolt(uiObject *interactionObj, uiInteraction *delta){
        Ux* myUxRef = Ux::Singleton();
        myUxRef->defaultYesNoChoiceDialogue->displayAdditionalMessage(" * ");
        myUxRef->defaultYesNoChoiceDialogue->showAdditionalMessageCharAtLocation(CHAR_LIGHTENING_BOLT, 1);
        myUxRef->defaultYesNoChoiceDialogue->resize();
    }




























    /*HISTORY */


    // **** HISTORY ****


    static bool updateUiObjectFromHistory(uiScrollController* scroller, uiObject *historyTile, int offset){ // see also clickHistoryColor
        Ux* myUxRef = Ux::Singleton();
        //uiHistoryPalleteEditor* self = myUxRef->historyPalleteEditor;

        colorTileAddChildObjects(historyTile, &clickDeleteHistoryColor);
        uiObject * removeButton = historyTile->childList[0];
        uiObject * iconBtn = historyTile->childList[1];

        if( offset > myUxRef->pickHistoryList->total() - 1 || offset < 0 ){

            removeButton->setBoundaryRect( 1.0, 0.0, 1.0, 1.0); // reset out of view (otherwise it is in the way of clicks!)

            if( myUxRef->pickHistoryList->total() > 0 ){
                if(offset == myUxRef->pickHistoryList->total() + BUTTON_CLEAR_HISTORY ){
                    historyTile->show();
                    removeButton->hide(); // it still collides..... so we set the rect above!
                    iconBtn->show();
                    Ux::setColor(&historyTile->backgroundColor, 0, 0, 0, 255);

                    myUxRef->printCharToUiObject(iconBtn, CHAR_LIGHTENING_BOLT, DO_NOT_RESIZE_NOW);

                    historyTile->myIntegerIndex = BTN_NEGATIVE_START - BUTTON_CLEAR_HISTORY; //awkward but using negative space beyond -1 for codes

                    historyTile->hasInteraction = false; // disable animations which are default for this scroll controller....

                    historyTile->interactionProxy=nullptr;

                    return true;
                }

                //if this is an optional button it should always be LAST
                if(offset == myUxRef->pickHistoryList->total() + BUTTON_ADD_ALL_HISTORY ){
                    historyTile->show();
                    removeButton->hide(); // it still collides..... so we set the rect above!
                    iconBtn->show();
                    Ux::setColor(&historyTile->backgroundColor, 0, 0, 0, 255);

                    myUxRef->printCharToUiObject(iconBtn, CHAR_ADD_ALL_DUDE, DO_NOT_RESIZE_NOW);

                    historyTile->myIntegerIndex = BTN_NEGATIVE_START - BUTTON_ADD_ALL_HISTORY; //awkward but using negative space beyond -1 for codes

                    historyTile->hasInteraction = false; // disable animations which are default for this scroll controller....

                    historyTile->interactionProxy=nullptr;

                    return true;
                }

                //if this is an optional button it should always be LAST
                if(offset == myUxRef->pickHistoryList->total() + BUTTON_SORT_HISTORY ){
                    historyTile->show();
                    removeButton->hide(); // it still collides..... so we set the rect above!
                    iconBtn->show();
                    Ux::setColor(&historyTile->backgroundColor, 0, 0, 0, 255);

                    myUxRef->printCharToUiObject(iconBtn, CHAR_SORT_ORDERING, DO_NOT_RESIZE_NOW);

                    historyTile->myIntegerIndex = BTN_NEGATIVE_START - BUTTON_SORT_HISTORY; //awkward but using negative space beyond -1 for codes

                    historyTile->hasInteraction = false; // disable animations which are default for this scroll controller....

                    historyTile->interactionProxy=nullptr;

                    return true;
                }
            }

            //historyTile->hasInteraction = true; // re enable animations which are default for this scroll controller....
            historyTile->hide();
            historyTile->myIntegerIndex = -1;
            return false;
        }

        offset = myUxRef->pickHistoryList->total() - offset - 1; // we show them in reverse here

        ColorList* listItem = myUxRef->pickHistoryList->get(offset);
        SDL_Color* clr = &listItem->color;

        // TODO we need to also account for largestPickHistoryIndex to see if we looped!!!!!!!!!
        // if largest is max, then we loop from the current index, but continue looping after reaching the bottom, its a modulous problem
        // (((really this only applies if offset computed next (here?) is less than zero))???)
        //(or not)

        // following code is very much shared... (clr, offset, historyTile, removeButton ...)
        historyTile->show();
        removeButton->show();
        iconBtn->hide();
        historyTile->hasInteraction = true; // re enable animations which are default for this scroll controller....

        bool wasBlank = historyTile->myIntegerIndex < 0;
        historyTile->hasBackground = true;
        historyTile->myIntegerIndex = offset;

        Ux::setColor(&historyTile->backgroundColor, clr->r, clr->g, clr->b, 255);

        // so if we are showing removeButton and our color changed, we know that something changed and we should hide our deleteX simple as that....
        bool changed = Ux::setColorNotifyOfChange(&removeButton->backgroundColor, clr->r, clr->g, clr->b, 255) || wasBlank;
        removeButton->myIntegerIndex = offset;
        if( changed ){
            removeButton->cancelCurrentAnimation();// but what if we are currently animating in?  this will just jump to the wrong element...
            removeButton->setBoundaryRect( 1.0, 0.0, 1.0, 1.0); // reset out of view
            if( listItem->is_delete_state ){
                removeButton->boundryRect.x = 0.0; // reset in view
            }
        }
        historyTile->interactionProxy=removeButton; // the tile actually interacts with the delete x....
        return true;
    }

    static int getHistoryTotalCount(uiScrollController* scroller){
        Ux* myUxRef = Ux::Singleton();
        //uiHistoryPalleteEditor* self = myUxRef->historyPalleteEditor;
        int total = myUxRef->pickHistoryList->total();
        if( total > 0 ){
            total+=PICK_HISRTORY_EXTRA_BUTTONS_TOTAL;
        }
        return total;
    }


    static void interactionToggleHistory(uiObject *interactionObj, uiInteraction *delta){
        Ux* myUxRef = Ux::Singleton();
        uiHistoryPalleteEditor* self = myUxRef->historyPalleteEditor;

        // PLEASE NOTE: the args may be nullptr, nullptr - we don't use them here....

        //self->newHistoryFullsize->cancelCurrentAnimation();

        if( self->historyPalleteHolder->is_being_viewed_state ) {
            if( myUxRef->widescreen ){
                self->historyPalleteHolder->setAnimation( myUxRef->uxAnimations->slideRight(self->historyPalleteHolder) ); // returns uiAminChain*
                //self->historyPalleteHolder->setAnimation( self->uxAnimations->slideRightFullWidth(self->historyPalleteHolder) ); // returns uiAminChain*
            }else{
                self->historyPalleteHolder->setAnimation( myUxRef->uxAnimations->slideDown(self->historyPalleteHolder) ); // returns uiAminChain*
            }
            self->historyPalleteHolder->is_being_viewed_state = false;
            myUxRef->endModal(self->historyPalleteHolder);
        }else{
            self->historyPalleteHolder->isInBounds = true; // nice hack
            self->historyPalleteHolder->updateRenderPosition();
            myUxRef->updatePickHistoryPreview();
            self->palleteScroller->updateTiles();
            self->historyPalleteHolder->setAnimation( myUxRef->uxAnimations->resetPosition(self->historyPalleteHolder) ); // returns uiAminChain*
            self->historyPalleteHolder->is_being_viewed_state = true;
            //self->historyScroller->allowUp = true;
            myUxRef->updateModal(self->historyPalleteHolder, &interactionToggleHistory);

            // WE JUST PUSHED OUR MODAL... HOWEVER IF OUR CURRENT STATE SHOWS THE SELECTED COLOR, THAT SHOULD BE THE FIFRST MODAL DISMISSED BY ESC/BACK.... ??
            //        if( self->palleteSelectionPreviewHolder->is_being_viewed_state ) {
            //            self->updateModal(self->palleteSelectionPreviewHolder, &Ux::interactionTogglePalletePreview);
            //        }
        }


#ifdef COLORPICK_BASIC_MODE
        if( self->historyPalleteHolder->is_being_viewed_state ){
            interactionToggleHistory(nullptr, nullptr);
            myUxRef->showBasicUpgradeMessage();
        }
#endif
    }


    static void removeHistoryColor(uiObject *interactionObj, uiInteraction *delta){
        Ux* myUxRef = Ux::Singleton();

        int offset = interactionObj->myIntegerIndex;
        interactionObj->setBoundaryRect( 1.0, 0.0, 1.0, 1.0); // reset  (close x ? ) out of view for next obj to use this tile...
        if( offset < 0 ) return;

        myUxRef->pickHistoryList->remove(offset);

        myUxRef->updatePickHistoryPreview();
        // historyScroller->updateTiles(); // above calls this

    }

    static void removeAllCheckedHistoryColor(uiObject *interactionObj, uiInteraction *delta){
        Ux* myUxRef = Ux::Singleton();
        uiListIterator<uiList<ColorList, Uint8>, ColorList>* pickHistoryIterator = myUxRef->pickHistoryList->iterate();
        ColorList* hist = pickHistoryIterator->nextLast(); // loop in reverse here...
        while(hist != nullptr){
            if( hist->is_delete_state ){
                myUxRef->pickHistoryList->remove(pickHistoryIterator->lastIndex+1);
            }
            hist = pickHistoryIterator->nextLast();
        }
		FREE_FOR_NEW(pickHistoryIterator);
        myUxRef->updatePickHistoryPreview();
    }

    static void clickDeleteHistoryColor(uiObject *interactionObj, uiInteraction *delta){

        if( !interactionObj->doesInFactRender || !interactionObj->parentObject->doesInFactRender ){
            //SDL_Log("CLICKED INVISIBLE REMOVE TILE");
            return; // this means our tile is invalid/out of range.. it is arguable we should not even get the event in this case? // todo (delete last tile and click again - it will prompt again without this or some solution)
        }

        Ux* myUxRef = Ux::Singleton();

        //myUxRef->defaultYesNoChoiceDialogue->uiObjToAnimate == myUxRef->defaultYesNoChoiceHolder

        if( !delta->canceled && delta->dx == 0.0f && myUxRef->defaultYesNoChoiceDialogue->uiObjToAnimate->is_being_viewed_state == false ){
            myUxRef->uxAnimations->scale_bounce(interactionObj->childList[0], 0.001);
            myUxRef->defaultYesNoChoiceDialogue->display(interactionObj, &removeHistoryColor, &clickDeleteHistoryColor); // when no clicked we reach else

            uiListIterator<uiList<ColorList, Uint8>, ColorList>* pickHistoryIterator = myUxRef->pickHistoryList->iterate();
            ColorList* hist = pickHistoryIterator->next();
            int delCounter=0;
            while(hist != nullptr){
                //SDL_Log("%i %i %i", hist->color.r, hist->color.g, hist->color.b);
                if( hist->is_delete_state ){
                    delCounter++;
                }
                hist = pickHistoryIterator->next();
            }
			FREE_FOR_NEW(pickHistoryIterator);
            if( delCounter > 1 ){
                myUxRef->defaultYesNoChoiceDialogue->displayAdditionalAction(&showAdditonalMessageLightenignBolt, &removeAllCheckedHistoryColor, delCounter-1);
            }else{
                myUxRef->defaultYesNoChoiceDialogue->displayAdditionalMessage(" * ");
                myUxRef->defaultYesNoChoiceDialogue->showAdditionalMessageCharAtLocation(CHAR_LIGHTENING_BOLT, 1);
            }

        }else{
            if( !myUxRef->defaultYesNoChoiceDialogue->uiObjToAnimate->is_being_viewed_state || myUxRef->defaultYesNoChoiceDialogue->yesClickedFn != &removeAllCheckedHistoryColor ){
                interactionObj->setAnimation( myUxRef->uxAnimations->resetPosition(interactionObj) ); // returns uiAminChain*
                myUxRef->pickHistoryList->get(interactionObj->myIntegerIndex)->is_delete_state = false;
            }
        }
    }

    static void clickClearHistory(uiObject *interactionObj, uiInteraction *delta){
        Ux* myUxRef = Ux::Singleton();

        myUxRef->pickHistoryList->clear();
        myUxRef->updatePickHistoryPreview();

    }

    static void clickCancelClearHistory(uiObject *interactionObj, uiInteraction *delta){
        Ux* myUxRef = Ux::Singleton();
        uiHistoryPalleteEditor* self = myUxRef->historyPalleteEditor;

        // we should show delete x on all visible tiles....
        for( int x=0,l=self->historyScroller->scrollChildContainer->childListIndex; x<l; x++ ){
            uiObject* childObj = self->historyScroller->scrollChildContainer->childList[x];
            uiObject* removeButton = childObj->childList[0];
            if( childObj->myIntegerIndex >= 0){
                ColorList* listItem = myUxRef->pickHistoryList->get(removeButton->myIntegerIndex);
                if( listItem == nullptr || !listItem->is_delete_state ){
                    removeButton->setAnimation( myUxRef->uxAnimations->resetPosition(removeButton) );
                }
            }
        }


    }

    static void historyReceivedFocus(uiObject *interactionObj, uiInteraction *delta){
        Ux* myUxRef = Ux::Singleton();
        uiHistoryPalleteEditor* self = myUxRef->historyPalleteEditor;
        // hide pallete preview if possible...
        if( self->palleteSelectionPreviewHolder->is_being_viewed_state ){
            self->interactionTogglePalletePreview(self->palleteSelectionPreviewHolder, delta);
        }
    }



    static void addAllScoringHandler(uiObject *interactionObj, int eventCode, int quantityAdded){
        Ux* uxInstance = Ux::Singleton();
        switch(eventCode){
            case YES_NO_RESULTS::RESULT_YES_FAST:
                //uxInstance->defaultScoreDisplay->displayExplanation(" Too Quick ");
                uxInstance->defaultScoreDisplay->display(interactionObj, 5 * quantityAdded, SCORE_EFFECTS::MOVE_UP);
                break;
            case YES_NO_RESULTS::RESULT_YES:
                uxInstance->defaultScoreDisplay->display(interactionObj, 3 * quantityAdded, SCORE_EFFECTS::MOVE_UP);
                // we should consider checking on quantityAdded.... to see if we really effected any change?
                //uxInstance->defaultScoreDisplay->displayExplanation("Wrote History");
                //uxInstance->defaultScoreDisplay->displayAchievement(Ux::uiSettingsScroller::UI_ACHEIVEMENT_REWROTE_HISTORY);
                break;
            case YES_NO_RESULTS::RESULT_NO_FAST:
                uxInstance->defaultScoreDisplay->display(interactionObj, 5, SCORE_EFFECTS::MOVE_UP);
                //uxInstance->defaultScoreDisplay->displayExplanation(" No Thanks ");
                uxInstance->defaultScoreDisplay->displayAchievement(Ux::uiSettingsScroller::UI_ACHEIVEMENT_NO_FAST);

                break;
            case YES_NO_RESULTS::RESULT_NO:
            default:
                uxInstance->defaultScoreDisplay->display(interactionObj, 1, SCORE_EFFECTS::MOVE_UP);
                break;
        }
    }

    void showAddAllConfirmationDialogue(uiObject *interactionObj, uiInteraction *delta){ // broker
        Ux* myUxRef = Ux::Singleton();


        if( myUxRef->palleteList->_out_of_space ){
            myUxRef->defaultScoreDisplay->displayAchievement(Ux::uiSettingsScroller::UI_ACHEIVEMENT_NOSPACE);

            palleteScroller->scrollToItemByIndex( myUxRef->palleteList->total() + BUTTON_CLEAR_PALLETE); // scroll to clear button....
            uiObject* visibleTile = palleteScroller->getVisibleTileForOffsetOrNull(myUxRef->palleteList->total() + BUTTON_CLEAR_PALLETE);
            if( visibleTile != nullptr ){
                myUxRef->uxAnimations->scale_bounce(visibleTile->childList[1], 0.001); // flash clear button....
            }
            return;
        }


        // first things - figure out how many will be added.....
        uiListIterator<uiList<ColorList, Uint8>, ColorList>* pickHistoryIterator = myUxRef->pickHistoryList->iterate();
        ColorList* hist = pickHistoryIterator->nextLast(); // loop in reverse here...
        int addCounter = 0;
        while(hist != nullptr){
            int existingLocation = myUxRef->palleteList->locate(*hist);
            if( existingLocation < 0 ){
                addCounter++;
            }
            hist = pickHistoryIterator->nextLast();
        }
		FREE_FOR_NEW(pickHistoryIterator);

        if( addCounter < 2 ){
            SDL_snprintf(myUxRef->print_here, myUxRef->max_print_here, "+ %i?", addCounter);//"Add All?"

        }else{
            SDL_snprintf(myUxRef->print_here, myUxRef->max_print_here, "+ <%i?", addCounter+1);//"Add All?"
        }

        myUxRef->defaultYesNoChoiceDialogue->display(interactionObj, &clickAddAllHistoryToPallete, &clickCancelNoOp);
        myUxRef->defaultYesNoChoiceDialogue->displayAdditionalMessageBottom(myUxRef->print_here);
        myUxRef->defaultYesNoChoiceDialogue->showAdditionalMessageCharAtLocation(CHAR_ADD_ALL_DUDE, 0);
        myUxRef->defaultYesNoChoiceDialogue->assignScoringProcessor(addAllScoringHandler);
        myUxRef->defaultYesNoChoiceDialogue->allowFastYes();
        //myUxRef->defaultYesNoChoiceDialogue->displayAdditionalUiObject(sortChooser->uiObjectItself);

        if( myUxRef->palleteList->_out_of_space ){
            myUxRef->defaultScoreDisplay->displayAchievement(Ux::uiSettingsScroller::UI_ACHEIVEMENT_NOSPACE);
        }

    }

    static void clickAddAllHistoryToPallete(uiObject *interactionObj, uiInteraction *delta){
        Ux* myUxRef = Ux::Singleton();
        uiHistoryPalleteEditor* self = myUxRef->historyPalleteEditor;
        uiListIterator<uiList<ColorList, Uint8>, ColorList>* pickHistoryIterator = myUxRef->pickHistoryList->iterate();
        ColorList* hist = pickHistoryIterator->nextLast(); // loop in reverse here...
        int addedCounter = 0;
        int foundVis = 0;
        int lastFoundInvisibleOffset = 0;
        while(hist != nullptr){
//            if( hist->is_delete_state ){
//                // we could skip these... right????
//            }
            int existingLocation = myUxRef->palleteList->locate(*hist);
            if( existingLocation > -1 ){
                // its taken... cannot add this to the pallete....
                uiObject* visibleTile = self->palleteScroller->getVisibleTileForOffsetOrNull(existingLocation);
                if( visibleTile != nullptr ){
                    myUxRef->uxAnimations->rvbounce(visibleTile);
                    foundVis++;
                }else{
                    lastFoundInvisibleOffset = existingLocation;
                }
            }else{
                self->addColorToPallete(interactionObj, hist->color, false );
                addedCounter++;
            }
            hist = pickHistoryIterator->nextLast();
        }
        myUxRef->updatePalleteScroller();
        if( addedCounter < 1 && foundVis < 1 ){
            // need of scroll to bounce....
            self->palleteScroller->scrollToItemByIndex(lastFoundInvisibleOffset);
        }else{
            self->palleteScroller->scrollToItemByIndex(myUxRef->palleteList->previousIndex());
            // TODO handle scoring todo note here...
        }
		FREE_FOR_NEW(pickHistoryIterator);
        myUxRef->defaultYesNoChoiceDialogue->updateNumberToEffectWhenYes(addedCounter);
    }

    void showSortConfirmationDialogue(uiObject *interactionObj, uiInteraction *delta){
        Ux* myUxRef = Ux::Singleton();

        // TODO build a uiController that we can init with our new lists, that will have its own uiHueGradientScroller* and will analyze the lists... it can store a property that will tell us the total to be deleted, etc
//        uiList<ColorList, Uint8>* tempList = myUxRef->pickHistoryList->clone();
//        // we can determine what exactly whill occur....
//            tempList->sort(&compareColorListItems); // we could clone it AGAIN now.. :) but we have to walk the list anyway so we can count the dupes as we go...
//        //int oldLen = dedupeHistoryList(tempList);
//        tempList->free();

        sortChooser->setPreivewListToNewClone();


        myUxRef->defaultYesNoChoiceDialogue->display(interactionObj, &clickSortHistory, &clickCancelNoOp);
        myUxRef->defaultYesNoChoiceDialogue->displayAdditionalMessage("Sort?");
        myUxRef->defaultYesNoChoiceDialogue->assignScoringProcessor(sortScoringHandler);
        myUxRef->defaultYesNoChoiceDialogue->displayAdditionalUiObject(sortChooser->uiObjectItself);
        myUxRef->defaultYesNoChoiceDialogue->allowFastYes();

        sortChooser->update();

        //SDL_free(myUxRef->pickHistoryList);
        //myUxRef->pickHistoryList = tempList;
        //myUxRef->updatePickHistoryPreview();

    }


    static void clickSortHistory(uiObject *interactionObj, uiInteraction *delta){
        Ux* myUxRef = Ux::Singleton();
        uiHistoryPalleteEditor* self = myUxRef->historyPalleteEditor;

        if( myUxRef->pickHistoryList->total() < 1 ) return;

        SDL_Color oldFirst = myUxRef->pickHistoryList->get(0)->color;

        int oldLen = self->sortChooser->dedupeHistoryList(myUxRef->pickHistoryList);
//
//        float pct = self->sortChooser->getGradientOffsetPercentage();
//        SDL_Color* clr = self->sortChooser->getGradientOffsetColor();
//        SDL_Log("THIS IS OUF OFFXET COLOR!!!! %i %i %i, %f", clr->r, clr->g, clr->b, pct);
        // we may wish to simply add this item to our list, then sort the list???? otherwise we have to find which color has the closest hue
        // or otherwise where in the list this color would be positioned.... if it were sorted into the list


        int numberRemoved = (oldLen) - myUxRef->pickHistoryList->total();

        self->sortChooser->offsetHistoryList(myUxRef->pickHistoryList, self->sortChooser->getGradientOffsetColor());

        if( numberRemoved < 1 ){
            SDL_Color newFirst = myUxRef->pickHistoryList->get(0)->color;
            if( !colorEquals(&oldFirst, &newFirst) ){
                numberRemoved = 1; // as long as we moved at least one item, lets not reset their great chain multiplier...
            }
        }



        myUxRef->updatePickHistoryPreview(); // also updates teh visible pallete scroller

        // Achievement: rewrote history
        // some custom score bonus???  right now the default one is being applied....
        // todo: scored based on how much effect it had?

        myUxRef->defaultYesNoChoiceDialogue->updateNumberToEffectWhenYes(numberRemoved);
    }

    // default scoreDisplayFn
    static void sortScoringHandler(uiObject *interactionObj, int eventCode, int quantitySelected){
        Ux* uxInstance = Ux::Singleton();
        switch(eventCode){
            case YES_NO_RESULTS::RESULT_YES_FAST:
                //uxInstance->defaultScoreDisplay->displayExplanation(" Too Quick ");
                uxInstance->defaultScoreDisplay->display(interactionObj, 5 * quantitySelected, SCORE_EFFECTS::MOVE_UP);
                break;
            case YES_NO_RESULTS::RESULT_YES:
                uxInstance->defaultScoreDisplay->display(interactionObj, 2 * quantitySelected, SCORE_EFFECTS::MOVE_UP);
                // we should consider checking on quantitySelected.... to see if we really effected any change?
                //uxInstance->defaultScoreDisplay->displayExplanation("Wrote History");
                uxInstance->defaultScoreDisplay->displayAchievement(Ux::uiSettingsScroller::UI_ACHEIVEMENT_REWROTE_HISTORY);


                break;
            case YES_NO_RESULTS::RESULT_NO_FAST:
                uxInstance->defaultScoreDisplay->display(interactionObj, 5, SCORE_EFFECTS::MOVE_UP);
                //uxInstance->defaultScoreDisplay->displayExplanation(" No Thanks ");
                uxInstance->defaultScoreDisplay->displayAchievement(Ux::uiSettingsScroller::UI_ACHEIVEMENT_NO_FAST);

                break;
            case YES_NO_RESULTS::RESULT_NO:
            default:
                uxInstance->defaultScoreDisplay->display(interactionObj, 1, SCORE_EFFECTS::MOVE_UP);
                break;
        }
    }

    static void clickCancelNoOp(uiObject *interactionObj, uiInteraction *delta){
        //        Ux* myUxRef = Ux::Singleton();
        //        uiHistoryPalleteEditor* self = myUxRef->historyPalleteEditor;
    }

    static void clickHistoryColor(uiObject *interactionObj, uiInteraction *delta){ // see also updateUiObjectFromHistory


        Ux* myUxRef = Ux::Singleton();
        uiHistoryPalleteEditor* self = myUxRef->historyPalleteEditor;

        //SDL_Log("clickHistoryColor --------------------- %i", interactionObj->myIntegerIndex);

        if( interactionObj->isInHiddenState() || self->historyPalleteHolder->isAnimating() ){
            return;
        }

        historyReceivedFocus(interactionObj, delta);


        if( !interactionObj->doesInFactRender ){
            interactionObj->resetPosition();

            return; // this means our tile is invalid/out of range
        }



        if( interactionObj->myIntegerIndex < -1 ){

            if( interactionObj->myIntegerIndex == BTN_NEGATIVE_START - BUTTON_CLEAR_HISTORY ){
                //SDL_Log("Clear Button ---------------------");

                if( myUxRef->defaultYesNoChoiceDialogue->isDisplayed ){ return; }

                // ColorList* listItem = myUxRef->pickHistoryList->get(removeButton->myIntegerIndex);

                // we should show delete x on all visible tiles....
                for( int x=0,l=self->historyScroller->scrollChildContainer->childListIndex; x<l; x++ ){
                    uiObject* childObj = self->historyScroller->scrollChildContainer->childList[x];
                    uiObject* removeButton = childObj->childList[0];
                    if( childObj->myIntegerIndex >= 0){
                        removeButton->setAnimation( myUxRef->uxAnimations->slideLeftFullWidth(removeButton) );
                    }
                }

                myUxRef->uxAnimations->scale_bounce(interactionObj->childList[1], 0.001);

                myUxRef->defaultYesNoChoiceDialogue->display(interactionObj, &clickClearHistory, &clickCancelClearHistory, myUxRef->pickHistoryList->total());
                myUxRef->defaultYesNoChoiceDialogue->displayAdditionalMessage(" * ");
                myUxRef->defaultYesNoChoiceDialogue->showAdditionalMessageCharAtLocation(CHAR_LIGHTENING_BOLT, 1);


            }else if( interactionObj->myIntegerIndex == BTN_NEGATIVE_START - BUTTON_ADD_ALL_HISTORY ){

                if( myUxRef->defaultYesNoChoiceDialogue->isDisplayed ){ return; }

                myUxRef->uxAnimations->scale_bounce(interactionObj->childList[1], 0.001);

                self->showAddAllConfirmationDialogue(interactionObj, delta);

            }else if( interactionObj->myIntegerIndex == BTN_NEGATIVE_START - BUTTON_SORT_HISTORY ){

                if( myUxRef->defaultYesNoChoiceDialogue->isDisplayed ){ return; }

                myUxRef->uxAnimations->scale_bounce(interactionObj->childList[1], 0.001);

                self->showSortConfirmationDialogue(interactionObj, delta);
            }

            return;
        }


        uiObject* interactionObjectOrProxy = interactionObj->interactionProxy;

        if( interactionObjectOrProxy == nullptr ){
            interactionObjectOrProxy = interactionObj;
        }


        if( delta->dx < (interactionObj->boundryRect.w * -0.25f) ){ // swipe left
            interactionObjectOrProxy->setAnimation( myUxRef->uxAnimations->slideLeftFullWidth(interactionObjectOrProxy) );
            myUxRef->pickHistoryList->get(interactionObjectOrProxy->myIntegerIndex)->is_delete_state = true;
            return;
        }

        interactionObjectOrProxy->setAnimation( myUxRef->uxAnimations->resetPosition(interactionObjectOrProxy) ); // returns uiAminChain*


        if( fabs(delta->dy) > 0.003 || delta->canceled ){
            return;
        }

        // TEMP debug the clicked color....
//        char* resultText25char = (char*)SDL_malloc( sizeof(char) * 27 );
//        SDL_snprintf(resultText25char, 25,  "(%d,%d,%d)", interactionObj->backgroundColor.r, interactionObj->backgroundColor.g, interactionObj->backgroundColor.b);
//        SDL_free(resultText25char);
//        myUxRef->defaultScoreDisplay->displayExplanation(resultText25char);


        // A couple things
        // if image is already in the pallete, we sholud probably not add it twice √ done
        // we should instead scroll to the color and animate it
        // we should always scroll to that index though...

        ColorList tmpLocate = ColorList(interactionObj->backgroundColor);

        int existingLocation = myUxRef->palleteList->locate(tmpLocate);

        if( existingLocation > -1 ){
            // this color is already taken then, this becomes an error and game over loss :) (handy comment to search for)

            self->palleteScroller->scrollToItemByIndex(existingLocation);

            uiObject* visibleTile = self->palleteScroller->getVisibleTileForOffsetOrNull(existingLocation);

            //uxAnimations->rvbounce(historyPreview);

            if( visibleTile != nullptr ){

                if( myUxRef->widescreen ){
                    myUxRef->uxAnimations->rvbounce(visibleTile,0.005);
                }else{
                    myUxRef->uxAnimations->rvbounce(visibleTile,0.002);
                }

                //historyTile->childList[0]
                //            uiObject * palleteRemoveButton = visibleTile->getChildOrNullPtr(0);
                //            if( palleteRemoveButton != nullptr ){
                //                myUxRef->uxAnimations->rvbounce(palleteRemoveButton);
                //            }
            }

            myUxRef->uxAnimations->rvbounce(interactionObj);
            //        uiObject * removeButton = interactionObj->getChildOrNullPtr(0);
            //        if( removeButton != nullptr ){
            //            myUxRef->uxAnimations->rvbounce(removeButton);
            //        }

            myUxRef->temporarilyDisableControllerCursorForAnimation(); // cursor gets borked by these anim, wait for them to complete :)

            // if in easy mode don't loose :)
            if( self->isGameModeEnabled() && self->isGameModeHardMode() ){
                myUxRef->defaultScoreDisplay->displayExplanation("Hard Mode");
                myUxRef->defaultScoreDisplay->loose(interactionObj, SCORE_EFFECTS::NOMOVE);
            }

            return;
        }

        myUxRef->uxAnimations->scale_bounce(interactionObj, -0.002);
        self->addColorToPallete(interactionObj, interactionObj->backgroundColor, true);
        self->palleteScroller->scrollToItemByIndex(myUxRef->palleteList->previousIndex());

//        uiObject* visibleTile = self->palleteScroller->getVisibleTileForOffsetOrNull(myUxRef->palleteList->previousIndex());
//        if( visibleTile != nullptr ){
//            myUxRef->uxAnimations->scale_bounce(visibleTile, -0.002);
//        }

    }

    bool isGameModeEnabled(){
        Ux* uxInstance = Ux::Singleton();
        return uxInstance->settingsScroller->getBooleanSetting(uiSettingsScroller::UI_SETTING_GAME_ON);
    }

    bool isGameModeHardMode(){
        Ux* uxInstance = Ux::Singleton();
        return !uxInstance->settingsScroller->getBooleanSetting(uiSettingsScroller::UI_SETTING_GAME_EASY_MODE);
    }

    void addColorToPallete(uiObject* interactionObj, SDL_Color color, bool standardScoring){
        Ux* myUxRef = Ux::Singleton();
        myUxRef->palleteList->add(ColorList(color));
        if( myUxRef->palleteList->_out_of_space ){
            if( standardScoring ) myUxRef->defaultScoreDisplay->display(interactionObj, 10, SCORE_EFFECTS::NOMOVE);
            //myUxRef->defaultScoreDisplay->displayExplanation("out of space!");
            if( standardScoring ) myUxRef->defaultScoreDisplay->displayAchievement(Ux::uiSettingsScroller::UI_ACHEIVEMENT_NOSPACE);
        }else{
           if( standardScoring )  myUxRef->defaultScoreDisplay->display(interactionObj, 5, SCORE_EFFECTS::NOMOVE);
        }

        if( standardScoring ) myUxRef->updatePalleteScroller();

        // note we do not perform auto scrolling here... since this might be called in bulk... do not risk running out of animation chains.
    }











































    /*PALLETE */


    // **** PALLETTE ****


    static bool updateUiObjectFromPallete(uiScrollController* scroller, uiObject *historyTile, int offset){  // see also clickPalleteColor
        Ux* myUxRef = Ux::Singleton();
        //uiHistoryPalleteEditor* self = myUxRef->historyPalleteEditor;

        colorTileAddChildObjects(historyTile, &clickDeletePalleteColor);
        uiObject * removeButton = historyTile->childList[0];
        uiObject * iconBtn = historyTile->childList[1];


        if( offset > myUxRef->palleteList->largestIndex() || offset < 0 ){

            removeButton->setBoundaryRect( 1.0, 0.0, 1.0, 1.0); // reset out of view (otherwise it is in the way of clicks!)

            if( myUxRef->palleteList->total() > 0 ){

                if( offset == myUxRef->palleteList->total() + BUTTON_CLEAR_PALLETE ){

                    historyTile->show();
                    removeButton->hide(); // it still collides..... so we set the rect above!
                    iconBtn->show();
                    Ux::setColor(&historyTile->backgroundColor, 0, 0, 0, 255);

                    myUxRef->printCharToUiObject(iconBtn, CHAR_LIGHTENING_BOLT, DO_NOT_RESIZE_NOW);

                    historyTile->myIntegerIndex = BTN_NEGATIVE_START - BUTTON_CLEAR_PALLETE; //awkward but using negative space beyond -1 for codes

                    historyTile->hasInteraction = false; // disable animations which are default for this scroll controller....

                    historyTile->interactionProxy=nullptr;

                    return true;
                }else if( offset == myUxRef->palleteList->total() + BUTTON_SAVE_PALLETE ){

                    historyTile->show();
                    removeButton->hide(); // it still collides..... so we set the rect above!
                    iconBtn->show();
                    Ux::setColor(&historyTile->backgroundColor, 0, 0, 0, 255);

                    myUxRef->printCharToUiObject(iconBtn, CHAR_SAVE_ICON, DO_NOT_RESIZE_NOW);

                    historyTile->myIntegerIndex = BTN_NEGATIVE_START - BUTTON_SAVE_PALLETE; //awkward but using negative space beyond -1 for codes

                    historyTile->hasInteraction = false; // disable animations which are default for this scroll controller....

                    historyTile->interactionProxy=nullptr;

                    return true;
                }
            }else{

                if( offset == myUxRef->palleteList->total() + BUTTON_PALLETE_HELP ){

                    historyTile->show();
                    removeButton->hide(); // it still collides..... so we set the rect above!
                    iconBtn->show();
                    Ux::setColor(&historyTile->backgroundColor, 0, 0, 0, 255);

                    myUxRef->printCharToUiObject(iconBtn, '0', DO_NOT_RESIZE_NOW);

                    historyTile->myIntegerIndex = BTN_NEGATIVE_START - BUTTON_PALLETE_HELP; //awkward but using negative space beyond -1 for codes

                    historyTile->hasInteraction = false; // disable animations which are default for this scroll controller....

                    historyTile->interactionProxy=nullptr;

                    return true;
                }

            }

            historyTile->hide();
            historyTile->myIntegerIndex = -1;
            return false;
        }

        ColorList* listItem = myUxRef->palleteList->get(offset);
        SDL_Color* clr = &listItem->color;

        // following code is very much shared... requires (clr, offset, historyTile, removeButton ...)
        historyTile->show();
        removeButton->show();
        iconBtn->hide();
        historyTile->hasInteraction = true; // re enable animations which are default for this scroll controller....

        bool wasBlank = historyTile->myIntegerIndex < 0;
        historyTile->hasBackground = true;
        historyTile->myIntegerIndex = offset;

        Ux::setColor(&historyTile->backgroundColor, clr->r, clr->g, clr->b, 255);

        // so if we are showing removeButton and our color changed, we know that something changed and we should hide our deleteX simple as that....
        bool changed = Ux::setColorNotifyOfChange(&removeButton->backgroundColor, clr->r, clr->g, clr->b, 255) || wasBlank;
        removeButton->myIntegerIndex = offset;
        if( changed  ){
            removeButton->cancelCurrentAnimation();// but what if we are currently animating in?  this will just jump to the wrong element...
            removeButton->setBoundaryRect( 1.0, 0.0, 1.0, 1.0); // reset out of view
            if( listItem->is_delete_state ){
                removeButton->boundryRect.x = 0.0; // reset in view
            }
        }
        historyTile->interactionProxy=removeButton; // the tile actually interacts with the delete x....
        return true;
    }

    static int getPalleteTotalCount(uiScrollController* scroller){
        Ux* myUxRef = Ux::Singleton();
        //return self->palleteList->largestIndex() + 1;
        int total = myUxRef->palleteList->total();
        if( total > 0 ){
            total+=PALLETE_EXTRA_BUTTONS_TOTAL;
        }else{
            total+=EMPTY_PALLETE_PALLETE_EXTRA_BUTTONS_TOTAL;
        }
        return total;

    }

    // why not just use Ux::bubbleWhenHidden instead of this??/ this works tho obviously...
    static bool bubbleInteractionIfPalletePreviewHidden(uiObject *interactionObj, uiInteraction *delta){
        // return true always, unless the interaction should be dropped and not bubble for some reason....
        Ux* myUxRef = Ux::Singleton();
        uiHistoryPalleteEditor* self = myUxRef->historyPalleteEditor;
        if( !self->historyPalleteHolder->is_being_viewed_state ){
            return myUxRef->bubbleCurrentInteraction(interactionObj, delta);
        }
        return true;
    }


    static void interactionTogglePalletePreview(uiObject *interactionObj, uiInteraction *delta){
        Ux* myUxRef = Ux::Singleton();
        uiHistoryPalleteEditor* self = myUxRef->historyPalleteEditor;

        //uiObject* trueInteractionObj = self->palleteSelectionColorPreview->uiObjectItself;
        uiObject* trueInteractionObj = self->palleteSelectionPreviewHolder;

        /*
         palleteSelectionColorPreview = new uiViewColor(historyPalleteHolder, Float_Rect(0.0, 0.5, 1.0, 0.5));
         historyPalleteHolder->addChild(newHistoryPallete);
         palleteSelectionColorPreview->uiObjectItself->setInteractionCallback(&Ux::interactionTogglePalletePreview); // if we dragged and released... it will animate the rest of the way because of this
         palleteSelectionColorPreview->uiObjectItself->setInteraction(&Ux::interactionVert);
         */

        if( trueInteractionObj->is_being_viewed_state ) {
            if( myUxRef->widescreen ){
                trueInteractionObj->setAnimation( myUxRef->uxAnimations->slideRightFullWidth(trueInteractionObj) );
            }else{
                trueInteractionObj->setAnimation( myUxRef->uxAnimations->slideDownFullHeight(trueInteractionObj) ); // returns uiAminChain*
            }
            trueInteractionObj->is_being_viewed_state =false;
            trueInteractionObj->doesNotCollide = true;
            myUxRef->endModal(trueInteractionObj);
        }else{
            trueInteractionObj->isInBounds = true; // nice hack
            myUxRef->updatePickHistoryPreview();
            trueInteractionObj->setAnimation( myUxRef->uxAnimations->resetPosition(trueInteractionObj) ); // returns uiAminChain*
            trueInteractionObj->is_being_viewed_state = true;
            trueInteractionObj->doesNotCollide = false;
            myUxRef->updateModal(trueInteractionObj, &interactionTogglePalletePreview);
        }
    }

    static void removePalleteColor(uiObject *interactionObj, uiInteraction *delta){

        Ux* myUxRef = Ux::Singleton();
        uiHistoryPalleteEditor* self = myUxRef->historyPalleteEditor;

        int offset = interactionObj->myIntegerIndex;
        interactionObj->setBoundaryRect( 1.0, 0.0, 1.0, 1.0); // reset out of view
        if( offset < 0 ) return;


        if( self->palleteSelectionPreviewHolder->is_being_viewed_state ) {
            SDL_Color clr = myUxRef->palleteList->get(offset)->color;

            if( colorEquals(&self->palleteSelectionColorPreview->last_color, &clr) ){
                // viewing deleted color
                self->interactionTogglePalletePreview(self->palleteSelectionPreviewHolder, delta);
            }
        }

        myUxRef->palleteList->remove(offset);

        self->palleteScroller->updateTiles();
    }

    static void removeAllCheckedPalleteColor(uiObject *interactionObj, uiInteraction *delta){
        Ux* myUxRef = Ux::Singleton();
        uiHistoryPalleteEditor* self = myUxRef->historyPalleteEditor;
        uiListIterator<uiList<ColorList, Uint8>, ColorList>* palleteListIterator = myUxRef->palleteList->iterate();
        ColorList* pallete = palleteListIterator->nextLast(); // loop in reverse here...
        while(pallete != nullptr){
            if( pallete->is_delete_state ){
                myUxRef->palleteList->remove(palleteListIterator->lastIndex+1);
            }
            pallete = palleteListIterator->nextLast();
        }
		FREE_FOR_NEW(palleteListIterator);
        self->palleteScroller->updateTiles();
    }

    static void clickDeletePalleteColor(uiObject *interactionObj, uiInteraction *delta){

        if( !interactionObj->doesInFactRender || !interactionObj->parentObject->doesInFactRender ){
            return; // this means our tile is invalid/out of range.. it is arguable we should not even get the event in this case? // todo (delete last tile and click again - it will prompt again without this or some solution)
        }

        Ux* myUxRef = Ux::Singleton();

        //myUxRef->defaultYesNoChoiceDialogue->uiObjToAnimate == myUxRef->defaultYesNoChoiceHolder

        if( !delta->canceled && delta->dx == 0.0f && myUxRef->defaultYesNoChoiceDialogue->uiObjToAnimate->is_being_viewed_state == false ){
            myUxRef->uxAnimations->scale_bounce(interactionObj->childList[0], 0.001);
            myUxRef->defaultYesNoChoiceDialogue->display(interactionObj, &removePalleteColor, &clickDeletePalleteColor); // when no clicked we reach else

            uiListIterator<uiList<ColorList, Uint8>, ColorList>* palleteListIterator = myUxRef->palleteList->iterate();
            ColorList* pallete = palleteListIterator->next();
            int delCounter=0;
            while(pallete != nullptr){
                //SDL_Log("%i %i %i", hist->color.r, hist->color.g, hist->color.b);
                if( pallete->is_delete_state ){
                    delCounter++;
                }
                pallete = palleteListIterator->next();
            }
			FREE_FOR_NEW(palleteListIterator);
            if( delCounter > 1 ){
                myUxRef->defaultYesNoChoiceDialogue->displayAdditionalAction(&showAdditonalMessageLightenignBolt, &removeAllCheckedPalleteColor, delCounter-1);
            }else{
                myUxRef->defaultYesNoChoiceDialogue->displayAdditionalMessage(" * ");
                myUxRef->defaultYesNoChoiceDialogue->showAdditionalMessageCharAtLocation(CHAR_LIGHTENING_BOLT, 1);
            }

        }else{
            if( !myUxRef->defaultYesNoChoiceDialogue->uiObjToAnimate->is_being_viewed_state || myUxRef->defaultYesNoChoiceDialogue->yesClickedFn != &removeAllCheckedPalleteColor ){
                interactionObj->setAnimation( myUxRef->uxAnimations->resetPosition(interactionObj) ); // returns uiAminChain*
                myUxRef->palleteList->get(interactionObj->myIntegerIndex)->is_delete_state = false;
            }
        }

    }

    static void clickClearPallete(uiObject *interactionObj, uiInteraction *delta){
        Ux* myUxRef = Ux::Singleton();
        uiHistoryPalleteEditor* self = myUxRef->historyPalleteEditor;

        myUxRef->palleteList->clear();
        myUxRef->updatePalleteScroller();


        if( self->palleteSelectionPreviewHolder->is_being_viewed_state ) {
            // viewing deleted color
            self->interactionTogglePalletePreview(self->palleteSelectionPreviewHolder, delta);
        }
    }

    static void clickCancelClearPallete(uiObject *interactionObj, uiInteraction *delta){
        Ux* myUxRef = Ux::Singleton();
        uiHistoryPalleteEditor* self = myUxRef->historyPalleteEditor;
        // we should show delete x on all visible tiles....
        for( int x=0,l=self->palleteScroller->scrollChildContainer->childListIndex; x<l; x++ ){
            uiObject* childObj = self->palleteScroller->scrollChildContainer->childList[x];
            uiObject* removeButton = childObj->childList[0];
            if( childObj->myIntegerIndex >= 0){
                ColorList* listItem = myUxRef->palleteList->get(removeButton->myIntegerIndex);
                if( listItem == nullptr || !listItem->is_delete_state ){
                    removeButton->setAnimation( myUxRef->uxAnimations->resetPosition(removeButton) );
                }
            }
        }


    }



    static void clickPalleteColor(uiObject *interactionObj, uiInteraction *delta){ // see also updateUiObjectFromPallete

        Ux* myUxRef = Ux::Singleton();
        uiHistoryPalleteEditor* self = myUxRef->historyPalleteEditor;

        if( interactionObj->isInHiddenState() || self->historyPalleteHolder->isAnimating() ){
            return;
        }

        if( !interactionObj->doesInFactRender ){

            interactionObj->resetPosition();

            if( self->palleteSelectionPreviewHolder->is_being_viewed_state ) {
                self->interactionTogglePalletePreview(self->palleteSelectionPreviewHolder, delta);
            }

            return; // this means our tile is invalid/out of range
        }


        if( interactionObj->myIntegerIndex < -1 ){

            if( myUxRef->palleteList->total() < 1 ){

                if( interactionObj->myIntegerIndex == BTN_NEGATIVE_START - BUTTON_PALLETE_HELP ){
                    //SDL_Log("PALLETE Help Button ---------------------");

                    if( myUxRef->defaultYesNoChoiceDialogue->isDisplayed ){ return; }

                    myUxRef->uxAnimations->scale_bounce(interactionObj->childList[1], 0.001);

                    if( self->historyScroller->scrollChildContainer->getChildCount() > 0 )
                        myUxRef->uxAnimations->spin_reset(self->historyScroller->scrollChildContainer->childList[0], 32);

                    //myUxRef->defaultYesNoChoiceDialogue->display(interactionObj, &Ux::clickClearPallete, &Ux::clickCancelClearPallete, myUxRef->palleteList->total());

                }

                return;
            }
            if( interactionObj->myIntegerIndex == BTN_NEGATIVE_START - BUTTON_CLEAR_PALLETE ){
                //SDL_Log("PALLETEClear Button ---------------------");

                if( myUxRef->defaultYesNoChoiceDialogue->isDisplayed ){ return; }

                // ColorList* listItem = myUxRef->palleteList->get(removeButton->myIntegerIndex);

                // we should show delete x on all visible tiles....
                for( int x=0,l=self->palleteScroller->scrollChildContainer->childListIndex; x<l; x++ ){
                    uiObject* childObj = self->palleteScroller->scrollChildContainer->childList[x];
                    uiObject* removeButton = childObj->childList[0];
                    if( childObj->myIntegerIndex >= 0){
                        removeButton->setAnimation( myUxRef->uxAnimations->slideLeftFullWidth(removeButton) );
                    }
                }

                myUxRef->uxAnimations->scale_bounce(interactionObj->childList[1], 0.001);

                myUxRef->defaultYesNoChoiceDialogue->display(interactionObj, &clickClearPallete, &clickCancelClearPallete, myUxRef->palleteList->total());
                myUxRef->defaultYesNoChoiceDialogue->displayAdditionalMessage(" * ");
                myUxRef->defaultYesNoChoiceDialogue->showAdditionalMessageCharAtLocation(CHAR_LIGHTENING_BOLT, 1);



            }else if(interactionObj->myIntegerIndex == BTN_NEGATIVE_START - BUTTON_SAVE_PALLETE ){
                //SDL_Log("PALLETE SAVE Button ---------------------");

                //const char* urlBase = "data:text/html;<b>world";
                const char* urlBase = "http://www.vidsbee.com/ColorPick/Pallete#";


                int totalColors = myUxRef->palleteList->total();
                int perColorChars = 7; // 6 plus seperator

                uiListIterator<uiList<ColorList, Uint8>, ColorList>* myIterator = myUxRef->palleteList->iterate();

                long len = (totalColors * perColorChars) + SDL_strlen(urlBase);
                char* clrStr = (char*)SDL_malloc( sizeof(char) * len );
                char* clrStrB = (char*)SDL_malloc( sizeof(char) * len );// linux needs?
                
                SDL_snprintf(clrStr, SDL_strlen(urlBase)+1,  "%s", urlBase);
                SDL_snprintf(clrStrB, len,  "%s", clrStr);// nix needs?
                    
                ColorList *colorItem = myIterator->next();
                while(colorItem != nullptr){
                    SDL_Color* color = &colorItem->color;
                    if( color == nullptr ) continue;
                    // hex is best compression
                    //SDL_Log("%i %i %i", color->r, color->g, color->b);
                    SDL_snprintf(clrStr, len,  "%s%02x%02x%02x,", clrStrB, color->r, color->g, color->b);
                    SDL_snprintf(clrStrB, len,  "%s", clrStr);// nix needs?
                    
                    colorItem = myIterator->next();
                }

                //SDL_snprintf(clrStr, len,  "%s%s", urlBase, clrStr);

                //SDL_Log("%s", clrStr);

                myUxRef->doOpenURL(clrStr);

                SDL_free(clrStr);
                SDL_free(clrStrB);
				FREE_FOR_NEW(myIterator);

                myUxRef->uxAnimations->scale_bounce(interactionObj->childList[1], 0.001);

            }

            return;
        }



        uiObject* interactionObjectOrProxy = interactionObj->interactionProxy;

        if( interactionObjectOrProxy == nullptr ){
            interactionObjectOrProxy = interactionObj;
        }

        if( delta->dx < (interactionObj->boundryRect.w * -0.25f) ){ // swipe left
            interactionObjectOrProxy->setAnimation( myUxRef->uxAnimations->slideLeftFullWidth(interactionObjectOrProxy) );
            myUxRef->palleteList->get(interactionObjectOrProxy->myIntegerIndex)->is_delete_state = true;
            return;
        }

        interactionObjectOrProxy->setAnimation( myUxRef->uxAnimations->resetPosition(interactionObjectOrProxy) ); // returns uiAminChain*

        if( delta->canceled ) return;

        myUxRef->uxAnimations->scale_bounce(interactionObj, -0.002);

        bool changed = self->palleteSelectionColorPreview->update(&interactionObj->backgroundColor);

        if( !self->palleteSelectionPreviewHolder->is_being_viewed_state ) {
            self->interactionTogglePalletePreview(self->palleteSelectionPreviewHolder, delta);
        }else if( !changed ){
            self->interactionTogglePalletePreview(self->palleteSelectionPreviewHolder, delta);
        }


        //    if( trueInteractionObj->is_being_viewed_state ) {
        //        trueInteractionObj->setAnimation( self->uxAnimations->slideDownFullHeight(trueInteractionObj) ); // returns uiAminChain*
        //        trueInteractionObj->is_being_viewed_state =false;
        //    }else{
        //        trueInteractionObj->isInBounds = true; // nice hack
        //        self->updatePickHistoryPreview();
        //        trueInteractionObj->setAnimation( self->uxAnimations->resetPosition(trueInteractionObj) ); // returns uiAminChain*
        //        trueInteractionObj->is_being_viewed_state = true;
        //    }

        //interactionObj->setAnimation( myUxRef->uxAnimations->resetPosition(interactionObj) ); // returns uiAminChain*
    }




};


#endif
