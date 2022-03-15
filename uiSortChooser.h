//
//  uiHueGradient.h
//  ColorPick SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiSortChooser_h
#define ColorPick_iOS_SDL_uiSortChooser_h


struct uiSortChooser{

    uiSortChooser(uiObject* parentObj, Float_Rect boundaries){

//        Ux* uxInstance = Ux::Singleton(); // some useful helper?

        uiObjectItself = new uiObject();

        uiObjectItself->myUiController = this; // this propagates to the other child objects

//        int counter = 0;
//        int offset = 0;
//        float width = 1.0 / 128;


        historyPreviewContainer = new uiObject();
        historyPreviewContainer->setBoundaryRect(0.0, 0.0, 1.0, 0.25);
        uiObjectItself->addChild(historyPreviewContainer);
        historyPreviewScroller = new uiHistoryPreviewScroller(historyPreviewContainer);

        historySortedPreviewContainer = new uiObject();
        historySortedPreviewContainer->setBoundaryRect(0.0, 0.75, 1.0, 0.25);
        uiObjectItself->addChild(historySortedPreviewContainer);
        historySortedPreviewScroller = new uiHistoryPreviewScroller(historySortedPreviewContainer);



        // 6 chunks of 0-255 rainbow
//        hueGradientHolder = new uiObject();
//        hueGradientHolder->setBoundaryRect(0.0, 0.0, 1.0, 1.0);
//        uiObjectItself->addChild(hueGradientHolder); // add first so controller proagates
        hueGradient = new uiHueGradientScroller(uiObjectItself, Float_Rect(0.0,0.25,1.0,0.5), true);
        hueGradient->addChangeCallback(&theHueGradientChanged);
        hueGradient->hueGradientHolder->setInteractionCallback(&theHueGradientChanged);

//        previousContainer = new uiObject();
//        previousContainer->setBoundaryRect(0.0, 0.0, 1.0, 0.25);
//        uxInstance->printCharToUiObject(previousContainer, CHAR_ARR_DN, DO_NOT_RESIZE_NOW);
//        uiObjectItself->addChild(previousContainer);
//
//        newContainer = new uiObject();
//        newContainer->setBoundaryRect(0.0, 1.0-0.25, 1.0, 0.25);
//        uxInstance->printCharToUiObject(newContainer, CHAR_ARR_UP, DO_NOT_RESIZE_NOW);
//        uiObjectItself->addChild(newContainer);

//        arrowIndicatorL = new uiObject();
//        arrowIndicatorL->setBoundaryRect(0.0, 0.25, 0.15, 0.5);
//        uxInstance->printCharToUiObject(arrowIndicatorL, CHAR_ARR_LEFT, DO_NOT_RESIZE_NOW);
//        uiObjectItself->addChild(arrowIndicatorL);
//
//        arrowIndicatorR = new uiObject();
//        arrowIndicatorR->setBoundaryRect(1.0-0.15, 0.25, 0.15, 0.5);
//        uxInstance->printCharToUiObject(arrowIndicatorR, CHAR_ARR_RIGHT, DO_NOT_RESIZE_NOW);
//        uiObjectItself->addChild(arrowIndicatorR);




        previewColorList = new uiList<ColorList, Uint8>(1);

//        uiObjectItself->setInteraction(interactionHueBgClicked);
//        uiObjectItself->setInteractionCallback(interactionHueBgClicked);

        //uiObjectItself->setInteractionCallback(Ux::interactionNoOp);
        parentObj->addChild(uiObjectItself);

        uiObjectItself->setBoundaryRect(&boundaries);
        resize();
    }

    uiObject* uiObjectItself; // no real inheritance here, this its the uiSqware, I would use self->
    uiHueGradientScroller* hueGradient;

//    uiObject* arrowIndicatorL; // the above's hueGradientHolder is hueGradientHolder, there is still hueGradient->uiObjectItself between though...
//    uiObject* arrowIndicatorR;

    uiObject* previousContainer; // the above's hueGradientHolder is hueGradientHolder, there is still hueGradient->uiObjectItself between though...
    uiObject* newContainer;


    uiObject* historyPreviewContainer;
    uiHistoryPreviewScroller* historyPreviewScroller;

    uiObject* historySortedPreviewContainer;
    uiHistoryPreviewScroller* historySortedPreviewScroller;

    uiList<ColorList, Uint8>* previewColorList;

    void setPreivewListToNewClone(){
        Ux* myUxRef = Ux::Singleton();
        previewColorList->free_list();
        previewColorList = myUxRef->pickHistoryList->clone();
        historySortedPreviewScroller->historyListToShow = previewColorList;
        theHueGradientChanged(nullptr, nullptr);
    }


    float getGradientOffsetPercentage(){ // get amount of scroll of the gradient, and we will make a color out of it...
        return hueGradient->getGradientOffsetPercentage();
    }

    SDL_Color* getGradientOffsetColor(){ // get amount of scroll of the gradient, and we will make a color out of it...
        return hueGradient->getGradientOffsetColor();
    }

//    SDL_Color* colorForPercent(float percent){
//        return hueGradient->colorForPercent(percent);
//    }




    static void theHueGradientChanged(uiObject *interactionObj, uiInteraction *delta){
        /* do not use args, nullptr */

        Ux* myUxRef = Ux::Singleton();
        uiSortChooser* self = myUxRef->historyPalleteEditor->sortChooser;

        //int oldLen =
        self->dedupeHistoryList(self->previewColorList);
        //
        //        float pct = self->sortChooser->getGradientOffsetPercentage();
        //        SDL_Color* clr = self->sortChooser->getGradientOffsetColor();
        //        SDL_Log("THIS IS OUF OFFXET COLOR!!!! %i %i %i, %f", clr->r, clr->g, clr->b, pct);
        // we may wish to simply add this item to our list, then sort the list???? otherwise we have to find which color has the closest hue
        // or otherwise where in the list this color would be positioned.... if it were sorted into the list


        self->offsetHistoryList(self->previewColorList, self->getGradientOffsetColor());

        self->historySortedPreviewScroller->update();
    }

    int dedupeHistoryList(uiList<ColorList, Uint8>* listToSort){
        int oldLen = listToSort->total(); // if we return some scoring info... we could reveal that here??? (should probably return a struct tho)
        listToSort->sort(&compareColorListItems);
        // next: remove dupes
        uiListIterator<uiList<ColorList, Uint8>, ColorList>* pickHistoryIterator = listToSort->iterate();
        ColorList* hist = pickHistoryIterator->nextLast(); // loop in reverse here...
        SDL_Color lastColor = hist->color;
        hist = pickHistoryIterator->nextLast();
        while(hist != nullptr){
            if( colorEquals( &hist->color, &lastColor) ){
                listToSort->remove(pickHistoryIterator->lastIndex+1);
            }else{
                lastColor = hist->color;
            }
            hist = pickHistoryIterator->nextLast();
        }
		FREE_FOR_NEW(pickHistoryIterator);
        return oldLen;
    }

    int offsetHistoryList(uiList<ColorList, Uint8>* listToOffset, SDL_Color* clr_to_seek_and_begin_rainbow_at){
        uiList<ColorList, Uint8>* tempList = new uiList<ColorList, Uint8>(listToOffset->total());
        uiListIterator<uiList<ColorList, Uint8>, ColorList>* pickHistoryIterator = listToOffset->iterate();
        ColorList* hist = pickHistoryIterator->nextLast(); // loop in reverse here...
        while(hist != nullptr){
            if( compareColor( &hist->color, clr_to_seek_and_begin_rainbow_at) > 0 ){
                //SDL_Log("our color is > 0 %i %i %i", hist->color.r, hist->color.g, hist->color.b);
            }else{
                //SDL_Log("our color is <= 0 %i %i %i", hist->color.r, hist->color.g, hist->color.b);
                tempList->add(*hist);
                listToOffset->remove(pickHistoryIterator->lastIndex+1);

            }
            hist = pickHistoryIterator->nextLast();
        }
        SDL_free(pickHistoryIterator);
        // add them back in reverse
        int shiftedItems = tempList->total();
        pickHistoryIterator = tempList->iterate();
        hist = pickHistoryIterator->nextLast();
        while(hist != nullptr){
            listToOffset->add(*hist);
            hist = pickHistoryIterator->nextLast();
        }
		FREE_FOR_NEW(pickHistoryIterator);
		FREE_FOR_NEW(tempList);
        return shiftedItems;
    }








    void resize(){


        Ux* uxInstance = Ux::Singleton();
//
//        uiObject* markerTop = huePositionMarker->childList[0];
//        uiObject* markerBtm = huePositionMarker->childList[1];
//
//        float markerHeight=0.55;
//        float markerOutset=0.25;
//        float hueMarkerWidth = 0.08;

        if( uxInstance->widescreen ){ // widescreen

        }else{
        }


        update();
    }

    void update(){ // w/h
        uiObjectItself->updateRenderPosition();
        historyPreviewScroller->update();
        historySortedPreviewScroller->update();
    }


};


#endif
