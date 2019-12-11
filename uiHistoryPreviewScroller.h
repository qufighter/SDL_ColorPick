//
//  uiHistoryPreviewScroller.h
//  ColorPick SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiHistoryPreviewScroller_h
#define ColorPick_iOS_SDL_uiHistoryPreviewScroller_h


// we made this generic - maybe its just color preview scroller??? any uiList<ColorList, Uint8> can work...
struct uiHistoryPreviewScroller{

    const static int tileRowLength = 50; // rename this to tileRowLengthDefault and allow modifications...

    uiHistoryPreviewScroller(uiObject* parentObj){
        Ux* uxInstance = Ux::Singleton();

        uiObjectItself = new uiObject();

        previewScroller = new uiScrollController();

        previewScroller->uiObjectItself->doesNotCollide = true; // this scroller won't scroll no more :) (this is unfortunately overriden in the uiScrollController)
        previewScroller->uiObjectItself->canCollide = false;

        previewScroller->initTilingEngine(10, 1, &updateUiObjectFromHistory, &getHistoryTotalCount, nullptr,  nullptr);
        previewScrollerItself = previewScroller->uiObjectItself;

        previewScroller->ownerController = this;

        uiObjectItself->addChild(previewScrollerItself);

        parentObj->addChild(uiObjectItself);// aka uiObjectItself);

        historyListToShow = uxInstance->pickHistoryList;
    }


    uiObject* uiObjectItself; // no real inheritance here, this its the uiHistoryPreviewScroller, I would use self->

    uiScrollController *previewScroller;

    uiObject *previewScrollerItself; // just a ref to historyScroller's uiObjectItself

    uiText *scoreText;
    uiText *highScoreText;
    uiText *scoreMultiplierText;


    uiList<ColorList, Uint8>* historyListToShow;





    static bool updateUiObjectFromHistory(uiScrollController* scroller, uiObject *historyTile, int offset){ // see also clickHistoryColor
        //Ux* myUxRef = Ux::Singleton();
        uiHistoryPreviewScroller* self = (uiHistoryPreviewScroller*)scroller->ownerController;

        int totalInList = self->historyListToShow->total();


//        int origOffset = offset;

        if( totalInList > tileRowLength ){

            int halfRowLen = SDL_floorf(tileRowLength * 0.5);

            //
            if( offset == halfRowLen || offset == halfRowLen+1 ){
                offset = totalInList;

            }else if( offset > halfRowLen ){ // the last 25 positions...
                offset = (totalInList) - (tileRowLength - offset);
                //SDL_Log("RIGHT OFFSETS... %i %i %i", origOffset, offset, totalInList);
            }
            // todo add ellipsis at center position...
        }

        if( offset > totalInList - 1 || offset < 0 ){
            historyTile->hide();
            historyTile->myIntegerIndex = -1;
            return false;
        }



        //offset = totalInList - offset - 1; // we show them in reverse here

        ColorList* listItem = self->historyListToShow->get(offset);
        SDL_Color* clr = &listItem->color;

        historyTile->show();
        //historyTile->hasInteraction = true; // re enable animations which are default for this scroll controller....

        //bool wasBlank = historyTile->myIntegerIndex < 0;
        historyTile->hasBackground = true;
        historyTile->myIntegerIndex = offset;

        Ux::setColor(&historyTile->backgroundColor, clr->r, clr->g, clr->b, 255);


        return true;
    }

    static int getHistoryTotalCount(uiScrollController* scroller){
        //Ux* myUxRef = Ux::Singleton();
        uiHistoryPreviewScroller* self = (uiHistoryPreviewScroller*)scroller->ownerController;

        int total = self->historyListToShow->total();
        if( total > tileRowLength ){
            total = tileRowLength;
        }
        return total;
    }





//    static void interactionClickGoToHistoryBtn(uiObject *interactionObj, uiInteraction *delta){
//
//        Ux* myUxRef = Ux::Singleton();
//
//        interactionToggleSettings(nullptr, nullptr);
//
//        myUxRef->historyPalleteEditor->interactionToggleHistory(nullptr, nullptr);
//
//    }


//
//    static void interactionResetAchievements(uiObject *interactionObj, uiInteraction *delta){
//        Ux* myUxRef = Ux::Singleton();
//        uiHistoryPreviewScroller* self = myUxRef->settingsScroller;
//
//    }
//



























    void resize(){
        Ux* uxInstance = Ux::Singleton();

        if( uxInstance->widescreen ){
//            previewScrollerItself->setBoundaryRect( 0.0, 0.0, 1.0, 1.0);
//            previewScroller->resizeTililngEngine(1, 5);

        }else{
//            previewScrollerItself->setBoundaryRect( 0.0, 0.0, 1.0, 1.0);
//            previewScroller->resizeTililngEngine(1, 10);

        }

        previewScroller->updateTiles();
    }


    bool update(){
        previewScroller->resizeTililngEngine(SDL_min(getHistoryTotalCount(previewScroller), tileRowLength), 1);
        previewScroller->updateTiles();
        return true;
    }











};


#endif
