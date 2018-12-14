//
//  ux.cpp
//  ColorPick iOS SDL
//
//  Created by Sam Larison on 8/21/16.
//
//

#include "ux.h"


#if __ANDROID__
#include "Platform/Android/AndroidFileChooser.h" // platform specific include!
#else
#include "FileChooser.h" // platform specific include!  this is for ios (and osx...)
#endif
// #include "FileChooser.h" // includes openglContext


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

    lastHue = new HSV_Color();

    isInteracting = false;
    currentInteraction = uiInteraction();

    interactionObject = nullptr;

//    for( int x=0; x<COLOR_INDEX_MAX; x++ ){
//        palleteColorsIndex[x] = palleteMax; //-1; // largest Uint..
//    }

    const char* filename1 = "history.bin";
    const char* filename2 = "pallete.bin";

    char* preferencesPath = SDL_GetPrefPath("vidsbee", "colorpick");
//    SDL_Log("Pref file path: %s", preferencesPath);
//    SDL_Log("Pref file len: %i", SDL_strlen(filename1));
//    SDL_Log("Pref file len: %i", SDL_strlen(preferencesPath));
    size_t len;

    len = SDL_strlen(preferencesPath) + SDL_strlen(filename1) + 4;
    historyPath = (char*)SDL_malloc( sizeof(char) * len );
    SDL_snprintf(historyPath, len, "%s%s", preferencesPath, filename1);

    len = SDL_strlen(preferencesPath) + SDL_strlen(filename2) + 4;
    palletePath = (char*)SDL_malloc( sizeof(char) * len );
    SDL_snprintf(palletePath, len, "%s%s", preferencesPath, filename2);

    //SDL_memset(filePath1, 0, SDL_strlen(filename1) + 1);
    //SDL_memcpy(filePath1, preferencesPath, SDL_strlen(preferencesPath));
    //SDL_memcpy(&filePath1[SDL_strlen(preferencesPath)], filename1, SDL_strlen(filename1));

//    SDL_Log("Pref file path: %s", historyPath);
//    SDL_Log("Pref file len: %i", SDL_strlen(historyPath));
//
//    SDL_Log("Pref file path: %s", palletePath);
//    SDL_Log("Pref file len: %i", SDL_strlen(palletePath));

    SDL_free(preferencesPath);
    

}

void Ux::readInState(char* filepath, void* dest, int destMaxSize, int* readSize){
    SDL_RWops* fileref = SDL_RWFromFile(filepath, "r");
    if( fileref == NULL ) return;

    //int autoFree=1;
    //size_t readDataSize;
    //return SDL_LoadFile_RW(fileref, &readDataSize, autoFree);


    SDL_RWops* memref = SDL_RWFromMem(dest, destMaxSize);

    Sint64 filesize = SDL_RWsize(fileref); // this filesize is in ? units... not a count of u8? (actually it seems to count u8s)

    int currentPosition = 0;
    while( currentPosition < filesize && currentPosition < destMaxSize ){

        SDL_WriteU8(memref, SDL_ReadU8(fileref));

        currentPosition++;
    }

    SDL_RWclose(memref);
    SDL_RWclose(fileref);
    *readSize = currentPosition;
}

void Ux::readInState(void){

    int quantityBytesRead = 0;

    int eachElementSize;
    int memOffset;
    int readOffset;

    SDL_Color* newHistoryList = (SDL_Color*)SDL_malloc( sizeof(SDL_Color) *  pickHistoryList->maxSize );

    readInState(historyPath, newHistoryList, pickHistoryList->maxMemorySize(), &quantityBytesRead);

    pickHistoryList->clear();

    eachElementSize = sizeof(SDL_Color);
    memOffset = 0;
    readOffset = 0;
    while( memOffset < quantityBytesRead ){
        pickHistoryList->add(ColorList(newHistoryList[readOffset++]));
        memOffset+=eachElementSize;
    }
    //pickHistoryList->addAll(newHistoryList, quantityBytesRead);
    SDL_free(newHistoryList);
    updatePickHistoryPreview();



    quantityBytesRead = 0;

    SDL_Color* newPalleteList = (SDL_Color*)SDL_malloc( sizeof(SDL_Color) * palleteList->maxSize );

    readInState(palletePath, newPalleteList, palleteList->maxMemorySize(), &quantityBytesRead);

    palleteList->clear();
    eachElementSize = sizeof(SDL_Color);
    memOffset = 0;
    readOffset = 0;
    while( memOffset < quantityBytesRead ){
        palleteList->add(ColorList(newPalleteList[readOffset++]));
        memOffset+=eachElementSize;
    }
    //palleteList->addAll(newPalleteList, quantityBytesRead);

    SDL_free(newPalleteList);

    updatePalleteScroller();

    //readInState(palletePath, palleteList->listItself, palleteList->maxMemorySize(), &quantityBytesRead);

    // todo pallete index will now be wrong
    // total and last index will now be wrong
    // etc.....
    // maybe its easier to allocate some new memory and then call ->add for each item?

}

void Ux::writeOutState(void){

    SDL_Log("Pref file path: %s", historyPath);
    SDL_Log("Pref file len: %i", SDL_strlen(historyPath));

    SDL_Log("Pref file path: %s", palletePath);
    SDL_Log("Pref file len: %i", SDL_strlen(palletePath));


    int totalUint8s;
    int currentPosition;
    SDL_RWops* memref;
    SDL_RWops* fileref;

    uiListIterator<uiList<ColorList, Uint8>, ColorList>* myIterator;
    SDL_Color *color;

    myIterator = pickHistoryList->iterate();
    fileref = SDL_RWFromFile(historyPath, "w");
    color = &myIterator->next()->color;
    while(color != nullptr){
        //SDL_Log("%i %i %i", color->r, color->g, color->b);
        SDL_WriteU8(fileref, color->r);
        SDL_WriteU8(fileref, color->g);
        SDL_WriteU8(fileref, color->b);
        SDL_WriteU8(fileref, color->a);
        color = &myIterator->next()->color;
    }
    SDL_free(myIterator);
    SDL_RWclose(fileref);

//    totalUint8s = pickHistoryList->memorySize();
//    memref = SDL_RWFromMem(pickHistoryList->listItself, totalUint8s);
//    fileref = SDL_RWFromFile(historyPath, "w");
//    currentPosition = 0;
//    while( currentPosition < totalUint8s ){
//        SDL_WriteU8(fileref, SDL_ReadU8(memref));
//        currentPosition++;
//    }
//    if( memref != NULL ) SDL_RWclose(memref);
//    SDL_RWclose(fileref);


    myIterator = palleteList->iterate();
    fileref = SDL_RWFromFile(palletePath, "w");
    color = &myIterator->next()->color;
    while(color != nullptr){
        //SDL_Log("%i %i %i", color->r, color->g, color->b);
        SDL_WriteU8(fileref, color->r);
        SDL_WriteU8(fileref, color->g);
        SDL_WriteU8(fileref, color->b);
        SDL_WriteU8(fileref, color->a);
        color = &myIterator->next()->color;
    }
    SDL_free(myIterator);
    SDL_RWclose(fileref);

//    totalUint8s = palleteList->memorySize();
//    memref = SDL_RWFromMem(palleteList->listItself, totalUint8s);
//    fileref = SDL_RWFromFile(palletePath, "w");
//    currentPosition = 0;
//    while( currentPosition < totalUint8s ){
//        SDL_WriteU8(fileref, SDL_ReadU8(memref));
//        currentPosition++;
//    }
//    if( memref != NULL ) SDL_RWclose(memref);
//    SDL_RWclose(fileref);

}


void Ux::updateStageDimension(float w, float h){

    screenRatio = w / h;

    if( screenRatio > 1.0 ){
        widescreen = true;
    }else{
        widescreen = false;
    }

    resizeUiElements();

    return;
}

void Ux::resizeUiElements(void){

    // move to constructor?


#ifdef DEVELOPER_TEST_MODE

#else

#ifndef __IPHONEOS__
    clock_bar=0.0f;
#endif

#endif

    //historyPreview->updateRenderPosition();
    // what about elements that have auto sizing properties??? like>  square,

    // todo some maths during resize repeat... this fires more than needed and could just be debounced?

    float temp=0.0;
    float temp1=0.0;

    // watch out for ux components whose update functions will ->updateRenderPosition();
    // all these function should provide means to bypass the update
    //  argubaly the signature of this function should match any update function?

    // optimization - our sizes don't change unless ratio does maybe??? or not...

    float ws_clock=0.0;

    rootUiObject->setBoundaryRect( 0, 0, 1.0, 1.0);
    // we try to indent here in a way that reflects hirearchy under rootUiObject, and attempt to do this in the order of lower (first added) elements first and those that render attop later...
    if( widescreen ){

        bottomBar->setBoundaryRect( 1.0-0.27777777777778+hue_picker, ws_clock, 0.27777777777778-hue_picker-history_preview, 1.0 - ws_clock);

            zoomSliderHolder->setBoundaryRect( 0.0, 0.0, 1.0, 0.25);
                zoomSliderBg->setBoundaryRect( 0.05, 0, 0.88, 1);
                zoomSlider->setBoundaryRect( 0.0, 0, 0.5, 1);
                zoomSlider->setMovementBoundaryRect( 0.0, 0, 1.0, 0.0);

            temp = zoomSliderHolder->boundryRect.h;
            temp1= (1.0-temp)/((float)bottomBarRightStack->getChildCount());
            bottomBarRightStack->setBoundaryRect( 0.0, 1.0-temp1,
                    1.0, temp1); // max size of single stacked right component?
                pickSourceBtn->setBoundaryRect( 0.0, 0, 1.0, 1.0);
                addHistoryBtn->setBoundaryRect( 0.0, 0, 1.0, 1.0);
                pickSourceBtn->stackBottom();
                addHistoryBtn->stackBottom();

        huePicker->resize(Float_Rect(1.0-0.27777777777778, ws_clock, hue_picker, 1.0 - ws_clock ));
        returnToLastImgBtn->setBoundaryRect( 1.0-0.27777777777778 - 0.1, ws_clock, 0.1, 0.1 * screenRatio);
        returnToLastImgBtn->setRoundedCorners(0.5, 0.0, 0.5, 0.5);

        historyPreviewHolder->setBoundaryRect(1.0-history_preview, ws_clock, history_preview, 1.0 - ws_clock);
        historyPreviewHolder->setInteraction(&Ux::interactionHorizontal);
            historyPreview->setChildNodeDirection(TEXT_DIR_ENUM::BTT, true);

        // depending on animation state rect is different....
        historyPalleteHolder->setBoundaryRectForAnimState(
            0.025, ws_clock, 0.95, 1.0 - ws_clock, //visible and orig
            1.0, ws_clock, 0.9, 1.0 - ws_clock ); // hidden
        historyPalleteHolderTlEdgeShadow->resize(SQUARE_EDGE_ENUM::LEFT);
        historyPalleteHolderBrEdgeShadow->resize(SQUARE_EDGE_ENUM::RIGHT);

        historyPalleteHolder->setInteraction(&Ux::interactionHorizontal);

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



        temp = 0.15;
        movementArrows->resize(Float_Rect(0.27777777777778, temp,
            1.0 - 0.27777777777778 - 0.27777777777778, 1.0 - temp - temp));

        curerntColorPreview->resize(Float_Rect(0.0, ws_clock, 0.27777777777778, 1.0 - ws_clock));

        defaultYesNoChoiceHolder->setBoundaryRectForAnimState(0.0, 0.0, 1.0, 1.0, // vis
                                                              0.0, -1.0, 1.0, 1.0); // hid
//        defaultYesNoChoiceDialogue->resize(Float_Rect(0.0, -1.0, 1.0, 1.0));
        defaultYesNoChoiceDialogue->resize();



    }else{ // not widescreen

        bottomBar->setBoundaryRect( 0.0, 0.75, 1.0, 0.15);

            zoomSliderHolder->setBoundaryRect( 0.0, 0, 0.25, 1);
                zoomSliderBg->setBoundaryRect( 0.05, 0, 0.88, 1);
                zoomSlider->setBoundaryRect( 0.0, 0, 0.5, 1);
                zoomSlider->setMovementBoundaryRect( 0.0, 0, 1.0, 0.0);

            temp = (1.0 - zoomSliderHolder->boundryRect.w) / (float)bottomBarRightStack->getChildCount(); // 2 total stack right components
            bottomBarRightStack->setBoundaryRect( 1.0-temp, 0.0, temp, 1.0); // max size of single stacked right component?
                pickSourceBtn->setBoundaryRect( 0.0, 0, 1.0, 1.0);
                addHistoryBtn->setBoundaryRect( 0.0, 0, 1.0, 1.0);
                pickSourceBtn->stackRight();
                addHistoryBtn->stackRight();

        huePicker->resize(Float_Rect(0.0, 0.7, 1.0, hue_picker));
        returnToLastImgBtn->setBoundaryRect( 0.0, 0.7 - 0.1, 0.1 / screenRatio, 0.1);
        returnToLastImgBtn->setRoundedCorners(0.5, 0.5, 0.5, 0.0);

        historyPreviewHolder->setBoundaryRect(0.0, 1.0-history_preview, 1.0, history_preview);
        historyPreviewHolder->setInteraction(&Ux::interactionVert);
            historyPreview->setChildNodeDirection(TEXT_DIR_ENUM::RTL, true);

        // depending on animation state rect is different....
        historyPalleteHolder->setBoundaryRectForAnimState(
            0.0, clock_bar,     1.0, 0.8, //visible and orig
            0.0, clock_bar+1.0, 1.0, 0.8 ); // hidden
        historyPalleteHolderTlEdgeShadow->resize(SQUARE_EDGE_ENUM::TOP);
        historyPalleteHolderBrEdgeShadow->resize(SQUARE_EDGE_ENUM::BOTTOM);
        historyPalleteHolder->setInteraction(&Ux::interactionVert);

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

        temp = 0.27777777777778 + 0.06;
        movementArrows->resize(Float_Rect(0.0, temp, 1.0, 1.0 - temp - temp));
        curerntColorPreview->resize(Float_Rect(0.0, clock_bar, 1.0, 0.27777777777778));


        defaultYesNoChoiceHolder->setBoundaryRectForAnimState(0.0, 0.0, 1.0, 1.0, // vis
                                                              0.0, -1.0, 1.0, 1.0); // hid

//        defaultYesNoChoiceDialogue->resize(Float_Rect(0.0, -1.0, 1.0, 1.0));
        defaultYesNoChoiceDialogue->resize();

    }

    updatePalleteScroller();

    rootUiObject->updateRenderPosition();
    // our "create" caller also updates scrollers at this time??
}

void Ux::updatePalleteScroller(){
    int palleteIconsTotal = getPalleteTotalCount();
    if( widescreen ){
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


int Ux::indexForColor(SDL_Color* c){ // warn if your index components return zero its all zeroes
    return (c->r+1) * (c->g+1) * (c->b+1);
}
int Ux::indexForColor(ColorList* cli){ // warn if your index components return zero its all zeroes
    return indexForColor(&cli->color);
}

// todo - either tons of 1off create functions, or return and define outside
Ux::uiObject* Ux::create(void){


    pickHistoryList = new uiList<ColorList, Uint8>(pickHistoryMax); // WARN - do not enable index if using Uint8 - max Uint8 is far less than pickHistoryMax
    palleteList = new uiList<ColorList, Uint8>(palleteMax);
    palleteList->index(COLOR_INDEX_MAX, indexForColor);

    //pickHistoryListState = new uiList<ColorListState, Uint8>(pickHistoryMax);
    //palleteListState = new uiList<ColorListState, Uint8>(palleteMax);

    uxAnimations = new UxAnim();

    rootUiObject = new uiObject();
    rootUiObject->isRoot = true;

    rootUiObject->hasBackground = true;
    Ux::setColor(&rootUiObject->backgroundColor, 0, 0, 0, 0);

    rootUiObject->hasForeground = true; // render texture
    Ux::setColor(&rootUiObject->foregroundColor, 0, 255, 0, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
    Ux::setColor(&rootUiObject->foregroundColor, 0, 0, 0, 0); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
    rootUiObject->hasForeground = false; // render texture

    // make this color the selected color ! ! ! ! !! ^ ^ ^



    returnToLastImgBtn = new uiObject();
    returnToLastImgBtn->hasForeground = true;
    returnToLastImgBtn->hasBackground = true;
    Ux::setColor(&returnToLastImgBtn->backgroundColor, 0, 0, 0, 50);
    returnToLastImgBtn->setRoundedCorners(0.5); // collision check will be "circular"
    printCharToUiObject(returnToLastImgBtn, CHAR_BACK_ICON, DO_NOT_RESIZE_NOW);
    returnToLastImgBtn->setClickInteractionCallback(&Ux::interactionReturnToPreviousSurface); // TODO rename me
    rootUiObject->addChild(returnToLastImgBtn);
    returnToLastImgBtn->hideAndNoInteraction(); // initially hidden....


    bottomBar = new uiObject();
    bottomBar->hasBackground = true;
    Ux::setColor(&bottomBar->backgroundColor, 0, 0, 0, 198);
    rootUiObject->addChild(bottomBar);
    bottomBar->setInteractionCallback(Ux::interactionNoOp); // cannot click through this bar...

    huePicker = new uiHueGradient(rootUiObject, Float_Rect(0.0, 0.60, 1.0, 0.09), &Ux::hueClicked);
    huePicker->resize(Float_Rect(0.0, 0.7, 1.0, hue_picker));


    pickSourceBtn = new uiObject();
    pickSourceBtn->hasForeground = true;
    Ux::setColor(&pickSourceBtn->foregroundColor, 255, 255, 255, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
//    pickSourceBtn->hasBackground = true;
//    Ux::setColor(&pickSourceBtn->backgroundColor, 32, 0, 0, 128);
    pickSourceBtn->setClickInteractionCallback(&Ux::interactionFileBrowserTime); // TODO rename me
    //printCharToUiObject(pickSourceBtn, '+', DO_NOT_RESIZE_NOW);
    printCharToUiObject(pickSourceBtn, CHAR_OPEN_FILES, DO_NOT_RESIZE_NOW);
    pickSourceBtn->squarify();



    addHistoryBtn = new uiObject();
    addHistoryBtn->hasForeground = true;
    Ux::setColor(&addHistoryBtn->foregroundColor, 255, 255, 255, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
//    addHistoryBtn->hasBackground = true;
//    Ux::setColor(&addHistoryBtn->backgroundColor, 32, 0, 0, 128);
    addHistoryBtn->setClickInteractionCallback(&Ux::interactionAddHistory); // TODO rename me
    printCharToUiObject(addHistoryBtn, CHAR_CIRCLE_PLUSS, DO_NOT_RESIZE_NOW);
    addHistoryBtn->squarify(); // "keep round"
    addHistoryBtn->setRoundedCorners(0.5);



    bottomBarRightStack = new uiObject();
    bottomBarRightStack->testChildCollisionIgnoreBounds = true; // need to check clicks on child objects.......

//    bottomBarRightStack->hasBackground = true;
//    Ux::setColor(&bottomBarRightStack->backgroundColor, 255, 0, 0, 128);


    // stacked bottom bar elements...
    bottomBarRightStack->addChildStackedRight(addHistoryBtn);
    bottomBarRightStack->addChildStackedRight(pickSourceBtn);

    bottomBar->addChild(bottomBarRightStack);


    zoomSliderHolder = new uiObject();
    bottomBar->addChild(zoomSliderHolder);

    zoomSliderBg = new uiObject();
//    zoomSliderBg->hasForeground = true;
  //  zoomSliderBg->hasBackground = true;
   // Ux::setColor(&zoomSliderBg->backgroundColor, 0, 0, 0, 0);
    Ux::setColor(&zoomSliderBg->foregroundColor, 128, 128, 128, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
//    printCharToUiObject(zoomSliderBg, CHAR_VERTICAL_BAR, DO_NOT_RESIZE_NOW);
    printCharToUiObject(zoomSliderBg, CHAR_ASYMP_SLOPE, DO_NOT_RESIZE_NOW);
    zoomSliderHolder->setInteractionBegin(clickZoomSliderBg);
    zoomSliderHolder->setInteraction(clickZoomSliderBg);
    zoomSliderHolder->addChild(zoomSliderBg);





    zoomSlider = new uiObject();
    zoomSlider->hasBackground = true;
    Ux::setColor(&zoomSlider->backgroundColor, 255, 255, 255, 255);
    Ux::setColor(&zoomSlider->foregroundColor, 0, 0, 0, 1); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
    zoomSlider->setInteraction(&Ux::interactionHZ);//zoomSlider->canCollide = true;
    //printCharToUiObject(zoomSlider, '^', DO_NOT_RESIZE_NOW);
    //printCharToUiObject(zoomSlider, CHAR_VERTICAL_BAR_POINTED, DO_NOT_RESIZE_NOW);
    printCharToUiObject(zoomSlider, CHAR_ZOOM_PLUSS, DO_NOT_RESIZE_NOW);
    zoomSlider->setRoundedCorners(0.5);
    zoomSliderHolder->addChild(zoomSlider);






    historyPalleteHolder = new uiObject();
    historyPalleteHolder->hasBackground = true;
    Ux::setColor(&historyPalleteHolder->backgroundColor, 0, 0, 0, 192);

    historyPalleteHolder->setInteractionCallback(&Ux::interactionToggleHistory); // if we dragged and released... it will animate the rest of the way because of this
    historyPalleteHolder->setInteraction(&Ux::interactionVert);
    historyPalleteHolder->setBoundsEnterFunction(&Ux::interactionHistoryEnteredView);
    historyPalleteHolder->is_being_viewed_state = true;

    //historyPalleteHolder->setAnimation( uxAnimations->slideDown(historyPalleteHolder) ); // returns uiAminChain*


    historyScroller = new uiScrollController();
                                   // 3  2
    historyScroller->initTilingEngine(6, 6, &Ux::updateUiObjectFromHistory, &Ux::getHistoryTotalCount, &Ux::clickHistoryColor, &Ux::interactionHorizontal);
    historyScroller->recievedFocus = &Ux::historyReceivedFocus;

    newHistoryFullsize = historyScroller->uiObjectItself;

    //newHistoryFullsize->isDebugObject=true;

    newHistoryFullsize->hasBackground = true;
    Ux::setColor(&newHistoryFullsize->backgroundColor, 0, 0, 0, 192);



    palleteScroller = new uiScrollController();
    palleteScroller->initTilingEngine(1, 1, &Ux::updateUiObjectFromPallete, &Ux::getPalleteTotalCount, &Ux::clickPalleteColor, Ux::interactionHorizontal);

    newHistoryPallete = palleteScroller->uiObjectItself;

    palleteScrollerEdgeShadow = new uiEdgeShadow(newHistoryPallete, SQUARE_EDGE_ENUM::TOP, 0.0);

    newHistoryPallete->hasBackground = true;
    Ux::setColor(&newHistoryPallete->backgroundColor, 0, 0, 0, 255);


    historyPalleteHolderTlEdgeShadow = new uiEdgeShadow(historyPalleteHolder, SQUARE_EDGE_ENUM::TOP, 0.03);
    historyPalleteHolderBrEdgeShadow = new uiEdgeShadow(historyPalleteHolder, SQUARE_EDGE_ENUM::BOTTOM, 0.03);


    historyPreviewHolder = new uiObject();
    historyPreviewHolder->hasBackground = true;
    Ux::setColor(&historyPreviewHolder->backgroundColor, 0, 0, 0, 192);


    historyPreview = new uiObject();
    //historyPreview->hasForeground = true;
//    historyPreview->hasBackground = true;
//    Ux::setColor(&historyPreview->backgroundColor, 0, 0, 0, 192);
    //Ux::setColor(&historyPreview->foregroundColor, 255, 255, 255, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
    //historyPreview->doesInFactRender = false;




    //historyPreview->setInteraction(&Ux::interactionHZ);
    //historyPreview->canCollide = true;






    // we should another way to set rects for animation support?


    int ctr = SIX_ACROSS;
    while( --ctr >= 0 ){
        uiObject * colora = new uiObject();
        colora->hasBackground = true;
        Ux::setColor(&colora->backgroundColor, 255, 0, 32, 0);

        //colora->setBoundaryRect( ctr * (1/across), 1.0, 1/across, 1/across * screenRatio );
        //colora->setBoundaryRect( ctr * SIX_ACROSS_RATIO, 0.0, SIX_ACROSS_RATIO, 1.0 );
        //colora->setBoundaryRect( 0, 0.0, 1, 1 );

        //colora->hasForeground = true;
        //printCharToUiObject(colora, 'F'-ctr, DO_NOT_RESIZE_NOW);

        historyPreview->addChild(colora);
    }


    //historyPreview->hasForeground = true;
    //printCharToUiObject(historyPreview, '_', DO_NOT_RESIZE_NOW);

    //historyPreview->canCollide = true; // set by setInteractionCallback
    historyPreviewHolder->setInteractionCallback(&Ux::interactionToggleHistory);

    historyPreviewHolder->setInteraction(&Ux::interactionVert);
    historyPreviewHolder->interactionProxy = historyPalleteHolder; // when we drag the preview effect the fullsize sliding into view...

    //printCharToUiObject(historyPreview, 'H', DO_NOT_RESIZE_NOW);

    //bottomBar->addChild(historyPreview);
    historyPreviewHolder->addChild(historyPreview);
    rootUiObject->addChild(historyPreviewHolder);




    movementArrows = new uiNavArrows(rootUiObject, Float_Rect(0.0, clock_bar + 0.27777777777778, 1.0, 0.38), &Ux::interactionDirectionalArrowClicked);

    curerntColorPreview = new uiViewColor(rootUiObject, Float_Rect(0.0, clock_bar, 1.0, 0.27777777777778), false);




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

    palleteSelectionPreviewHolder = new uiObject();
    palleteSelectionPreviewHolder->setInteractionCallback(&Ux::interactionTogglePalletePreview); // if we dragged and released... it will animate the rest of the way because of this
    palleteSelectionPreviewHolder->setInteraction(&Ux::interactionVert);
    palleteSelectionPreviewHolder->setCropParentRecursive(historyPalleteHolder);
    palleteSelectionPreviewHolder->is_being_viewed_state = false;
    //palleteSelectionPreviewHolder->setDefaultHidden();

    historyPalleteHolder->addChild(newHistoryFullsize);
    historyPalleteHolder->addChild(palleteSelectionPreviewHolder);
    historyPalleteHolder->addChild(newHistoryPallete);



    palleteSelectionColorPreview = new uiViewColor(palleteSelectionPreviewHolder, Float_Rect(0.0, 0.0, 1.0, 1.0), true); // this rect is reset next...

    //palleteSelectionColorPreview->uiObjectItself->is_being_viewed_state = false;
    //palleteSelectionColorPreview->uiObjectItself->setDefaultHidden();
    //setDefaultHidden

//    palleteSelectionColorPreview->uiObjectItself->setBoundaryRect(
//        newHistoryPallete->boundryRect.x, newHistoryPallete->boundryRect.y - newHistoryPallete->boundryRect.h,
//        newHistoryPallete->boundryRect.w, newHistoryPallete->boundryRect.h
//    );




//    palleteSelectionColorPreview->uiObjectItself->setInteractionCallback(&Ux::interactionTogglePalletePreview); // if we dragged and released... it will animate the rest of the way because of this
//    palleteSelectionColorPreview->uiObjectItself->setInteraction(&Ux::interactionVert);
    // palleteSelectionColorPreview->uiObjectItself->setBoundsEnterFunction(&Ux::interactionHistoryEnteredView);


    // lets give palleteSelectionColorPreview a background
    palleteSelectionColorPreview->uiObjectItself->hasBackground = true;
    Ux::setColor(&palleteSelectionColorPreview->uiObjectItself->backgroundColor, 0, 0, 0, 255);
    palleteSelectionColorPreview->alphaMulitiplier = 255; //always solid bg

    //palleteSelectionColorPreview->uiObjectItself->setCropParentRecursive(historyPalleteHolder);






    rootUiObject->addChild(historyPalleteHolder);


    defaultYesNoChoiceHolder = new uiObject();
    //defaultYesNoChoiceHolder->setBoundaryRect(0.0, -1.0, 1.0, 1.0);
    defaultYesNoChoiceHolder->setDefaultHidden();
    rootUiObject->addChild(defaultYesNoChoiceHolder);

    // top most objects
    defaultYesNoChoiceDialogue = new uiYesNoChoice(defaultYesNoChoiceHolder, Float_Rect(0.0, 0.0, 1.0, 1.0), true);

    defaultYesNoChoiceDialogue->uiObjectItself->hasBackground = true;
    Ux::setColor(&defaultYesNoChoiceDialogue->uiObjectItself->backgroundColor, 98, 0, 98, 192);

    //defaultYesNoChoiceDialogue->uiObjectItself->setAnimation( uxAnimations->slideUp(defaultYesNoChoiceDialogue->uiObjectItself) ); // returns uiAminChain*



    updateRenderPositions();

    historyScroller->updateTiles();
    palleteScroller->updateTiles();


    readInState(); // reads saved state into lists


    //test cololr history
    for( int q=0; q<64; q++ ){
        currentlyPickedColor = new SDL_Color();
        setColor(currentlyPickedColor, randomInt(0,44), randomInt(0,185), randomInt(0,44), 0);
        addCurrentToPickHistory();
    }


    //writeOutState(); // testing


    // for awful reasons during init some things should not be called....

    //palleteSelectionColorPreview->uiObjectItself->setAnimation( uxAnimations->slideDownFullHeight(palleteSelectionColorPreview->uiObjectItself) ); // returns uiAminChain*
    // todo we should just chain a interactionTogglePalletePreview
    //interactionTogglePalletePreview(nullptr, nullptr);
    interactionToggleHistory(nullptr, nullptr);

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

                // LTR default
                // move this right out of the else aboe, since we should fit any len text within, and also needs resize IF text len changed.... !?! easy compute
                letter->setBoundaryRect( (ctr*letterWidth), 0, letterWidth, letterWidth);  /// TODO move size components into function to calculate on window rescale bonus points for suqare?
            }else{

                //letter->squarify();
                // LTR default - after text is printed you can change this!
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

    // we just printed a shorter string than we did previously, hide any allocated letteres beyond current str len
    // it is also possible extra letters need to be set to offeset 0 to render space?
    for( int l=printObj->childListIndex; ctr<l; ctr++ ){
        printObj->childList[ctr]->doesInFactRender = false;

//        Ux::setRect(&printObj->childList[ctr]->textureCropRect,
//                    0, // space is asci 32 0,0
//                    0,
//                    xStep,yStep);
    }

    //        hexValueText->setChildNodeDirection(TEXT_DIR_ENUM::RTL); // see TEXT_DIR_ENUM


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

    letter->hasForeground = true;

    int row = (int)(charOffset / text_stride);
    charOffset = charOffset % text_stride;

    Ux::setRect(&letter->textureCropRect,
                charOffset * text_xStep, row * text_yStep,
                text_xStep,              text_yStep);
}

bool Ux::triggerInteraction(bool isStart){ // mouseup, mouse didn't move

    bool hasInteraction = objectCollides(currentInteraction.px, currentInteraction.py);

    //interactionObject  currentInteraction
    if( hasInteraction && isStart ){
        if( lastInteractionObject == interactionObject ){
            // second click
            currentInteraction.isSecondInteraction=true;
        }
        if( interactionObject->interactionBeginCallback != nullptr ){
            interactionObject->interactionBeginCallback(interactionObject, &currentInteraction);
        }
    }else{
        lastInteractionObject = interactionObject;
    }
    return hasInteraction;
}

// by default, its the start of interaction when this is called...
bool Ux::triggerInteraction(void){ // mouseup, mouse didn't move
    return triggerInteraction(true);
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


    if( renderObj->testChildCollisionIgnoreBounds || renderObj->collidesWithPoint(x, y) ){
   // if( pointInRect(&renderObj->renderRect, x, y) ){
        if( renderObj->canCollide ){
            collides = true;
            // do something if a thing to do is defined for this collisino
            if( renderObj->hasInteraction || renderObj->hasInteractionCb ){
                // quite arguable we can return here, continuing to seek interaction on child objects may produce odd effects

                interactionObject = renderObj;

                //SDL_Log("WE DID FIND ANOTHER MATCH");

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

    uiObject* origInteractionObj = interactionObject;
    interactionObject = nullptr;
    triggerInteraction(false); // interaction obj might change!!!!

    if( !interactionObject || origInteractionObj != interactionObject ){
        if( !origInteractionObj || origInteractionObj->interactionCallbackTypeClick ){
            return false; // if interaction type is click and we moved objects; or if we never had an origional click object
        }else{
            /// otherwise reset it
            interactionObject = origInteractionObj;
        }
    }

    // really we should consider storing 2 seperate callbacks - interaction complete is really quite different from click callback
    // and in some cases we may need both....

    // so in some cases, the interaction is complete even if the object changed!?!

    if( isInteracting && interactionObject->hasInteractionCb ){
        isInteracting = false;
        interactionObject->interactionCallback(interactionObject, delta);
        return true;
    }

    return false;
}


//static
void Ux::interactionHistoryEnteredView(uiObject *interactionObj){

    Ux* self = Ux::Singleton();
    //self->historyScroller->allowUp = true;
    self->updatePickHistoryPreview();  /// this is too much update right?
    self->palleteScroller->updateTiles();
}

//static
void Ux::interactionNoOp(uiObject *interactionObj, uiInteraction *delta){
    //SDL_Log("INERACTION NOOP REACHED");

}


//static
void Ux::interactionTogglePalletePreview(uiObject *interactionObj, uiInteraction *delta){
    Ux* self = Ux::Singleton();

    //uiObject* trueInteractionObj = self->palleteSelectionColorPreview->uiObjectItself;
    uiObject* trueInteractionObj = self->palleteSelectionPreviewHolder;

    /*
     palleteSelectionColorPreview = new uiViewColor(historyPalleteHolder, Float_Rect(0.0, 0.5, 1.0, 0.5));
     historyPalleteHolder->addChild(newHistoryPallete);
     palleteSelectionColorPreview->uiObjectItself->setInteractionCallback(&Ux::interactionTogglePalletePreview); // if we dragged and released... it will animate the rest of the way because of this
     palleteSelectionColorPreview->uiObjectItself->setInteraction(&Ux::interactionVert);
     */

    if( trueInteractionObj->is_being_viewed_state ) {
        if( self->widescreen ){
            trueInteractionObj->setAnimation( self->uxAnimations->slideRightFullWidth(trueInteractionObj) );
        }else{
            trueInteractionObj->setAnimation( self->uxAnimations->slideDownFullHeight(trueInteractionObj) ); // returns uiAminChain*
        }
        trueInteractionObj->is_being_viewed_state =false;
        trueInteractionObj->doesNotCollide = true;
    }else{
        trueInteractionObj->isInBounds = true; // nice hack
        self->updatePickHistoryPreview();
        trueInteractionObj->setAnimation( self->uxAnimations->resetPosition(trueInteractionObj) ); // returns uiAminChain*
        trueInteractionObj->is_being_viewed_state = true;
        trueInteractionObj->doesNotCollide = false;

    }
}

void Ux::removePalleteColor(uiObject *interactionObj, uiInteraction *delta){

    Ux* myUxRef = Ux::Singleton();

    int offset = interactionObj->myIntegerIndex;
    interactionObj->setBoundaryRect( 1.0, 0.0, 1.0, 1.0); // reset out of view
    if( offset < 0 ) return;


    if( myUxRef->palleteSelectionPreviewHolder->is_being_viewed_state ) {
        SDL_Color clr = myUxRef->palleteList->get(offset)->color;

        if( colorEquals(&myUxRef->palleteSelectionColorPreview->last_color, &clr) ){
            // viewing deleted color
            myUxRef->interactionTogglePalletePreview(myUxRef->palleteSelectionPreviewHolder, delta);
        }
    }

    myUxRef->palleteList->remove(offset);

    myUxRef->palleteScroller->updateTiles();
}

void Ux::clickDeletePalleteColor(uiObject *interactionObj, uiInteraction *delta){

    if( !interactionObj->doesInFactRender || !interactionObj->parentObject->doesInFactRender ){
        return; // this means our tile is invalid/out of range.. it is arguable we should not even get the event in this case? // todo (delete last tile and click again - it will prompt again without this or some solution)
    }

    Ux* myUxRef = Ux::Singleton();

    //myUxRef->defaultYesNoChoiceDialogue->uiObjToAnimate == myUxRef->defaultYesNoChoiceHolder

    if( delta->dx == 0.0f && myUxRef->defaultYesNoChoiceDialogue->uiObjToAnimate->is_being_viewed_state == false ){
        myUxRef->uxAnimations->scale_bounce(interactionObj->childList[0], 0.001);
        myUxRef->defaultYesNoChoiceDialogue->display(interactionObj, &Ux::removePalleteColor, &Ux::clickDeletePalleteColor); // when no clicked we reach else
    }else{
        interactionObj->setAnimation( myUxRef->uxAnimations->resetPosition(interactionObj) ); // returns uiAminChain*
        myUxRef->palleteList->get(interactionObj->myIntegerIndex)->is_delete_state = false;
    }

}

void Ux::clickClearPallete(uiObject *interactionObj, uiInteraction *delta){
    Ux* myUxRef = Ux::Singleton();

    myUxRef->palleteList->clear();
    myUxRef->updatePalleteScroller();


    if( myUxRef->palleteSelectionPreviewHolder->is_being_viewed_state ) {
        // viewing deleted color
        myUxRef->interactionTogglePalletePreview(myUxRef->palleteSelectionPreviewHolder, delta);
    }
}

void Ux::clickCancelClearPallete(uiObject *interactionObj, uiInteraction *delta){
    Ux* myUxRef = Ux::Singleton();
    // we should show delete x on all visible tiles....
    for( int x=0,l=myUxRef->palleteScroller->scrollChildContainer->childListIndex; x<l; x++ ){
        uiObject* removeButton = myUxRef->palleteScroller->scrollChildContainer->childList[x]->childList[0];
        ColorList* listItem = myUxRef->palleteList->get(removeButton->myIntegerIndex);
        if( listItem == nullptr || !listItem->is_delete_state ){
            removeButton->setAnimation( myUxRef->uxAnimations->resetPosition(removeButton) );
        }
    }
    
    
}



void Ux::clickPalleteColor(uiObject *interactionObj, uiInteraction *delta){ // see also updateUiObjectFromPallete

    Ux* myUxRef = Ux::Singleton();

    if( !interactionObj->doesInFactRender ){

        interactionObj->resetPosition();

        if( myUxRef->palleteSelectionPreviewHolder->is_being_viewed_state ) {
            myUxRef->interactionTogglePalletePreview(myUxRef->palleteSelectionPreviewHolder, delta);
        }

        return; // this means our tile is invalid/out of range
    }


    if( interactionObj->myIntegerIndex < -1 ){

        if( interactionObj->myIntegerIndex == BTN_NEGATIVE_START - BUTTON_CLEAR_PALLETE ){
            SDL_Log("PALLETEClear Button ---------------------");

            // we should show delete x on all visible tiles....
            for( int x=0,l=myUxRef->palleteScroller->scrollChildContainer->childListIndex; x<l; x++ ){
                uiObject* removeButton = myUxRef->palleteScroller->scrollChildContainer->childList[x]->childList[0];
                removeButton->setAnimation( myUxRef->uxAnimations->slideLeftFullWidth(removeButton) );
            }

            myUxRef->uxAnimations->scale_bounce(interactionObj->childList[1], 0.001);

            myUxRef->defaultYesNoChoiceDialogue->display(interactionObj, &Ux::clickClearPallete, &Ux::clickCancelClearPallete, myUxRef->palleteList->total());


        }else if(interactionObj->myIntegerIndex == BTN_NEGATIVE_START - BUTTON_SAVE_PALLETE ){
            SDL_Log("PALLETE SAVE Button ---------------------");

            //const char* urlBase = "data:text/html;<b>world";
            const char* urlBase = "http://www.vidsbee.com/ColorPick/Pallete#";
            

            int totalColors = myUxRef->palleteList->total();
            int perColorChars = 7; // 6 plus seperator

            uiListIterator<uiList<ColorList, Uint8>, ColorList>* myIterator = myUxRef->palleteList->iterate();

            int len = (totalColors * perColorChars) + SDL_strlen(urlBase);
            char* clrStr = (char*)SDL_malloc( sizeof(char) * len );

            SDL_snprintf(clrStr, SDL_strlen(urlBase)+1,  "%s", urlBase);


            ColorList *colorItem = myIterator->next();
            while(colorItem != nullptr){
                SDL_Color* color = &colorItem->color;
                if( color == nullptr ) continue;
                // hex is best compression
                //SDL_Log("%i %i %i", color->r, color->g, color->b);
                SDL_snprintf(clrStr, len,  "%s%02x%02x%02x,", clrStr, color->r, color->g, color->b);
                colorItem = myIterator->next();
            }

            //SDL_snprintf(clrStr, len,  "%s%s", urlBase, clrStr);

            SDL_Log("%s", clrStr);

            openURL(clrStr);

            SDL_free(clrStr);
            SDL_free(myIterator); // does free recurse

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


    myUxRef->palleteSelectionColorPreview->update(&interactionObj->backgroundColor);

    if( !myUxRef->palleteSelectionPreviewHolder->is_being_viewed_state ) {
        myUxRef->interactionTogglePalletePreview(myUxRef->palleteSelectionPreviewHolder, delta);
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
    interactionObj->setAnimation( myUxRef->uxAnimations->resetPosition(interactionObj) ); // returns uiAminChain*

}

void Ux::hideHistoryPalleteIfShowing(){
    if( historyPalleteHolder->is_being_viewed_state ) {
        interactionToggleHistory(nullptr, nullptr);
    }
}

//static
void Ux::interactionToggleHistory(uiObject *interactionObj, uiInteraction *delta){
    Ux* self = Ux::Singleton();

    // PLEASE NOTE: the args may be nullptr, nullptr - we don't use them here....

    //self->newHistoryFullsize->cancelCurrentAnimation();

    if( self->historyPalleteHolder->is_being_viewed_state ) {
        if( self->widescreen ){
            self->historyPalleteHolder->setAnimation( self->uxAnimations->slideRight(self->historyPalleteHolder) ); // returns uiAminChain*
            //self->historyPalleteHolder->setAnimation( self->uxAnimations->slideRightFullWidth(self->historyPalleteHolder) ); // returns uiAminChain*
        }else{
            self->historyPalleteHolder->setAnimation( self->uxAnimations->slideDown(self->historyPalleteHolder) ); // returns uiAminChain*
        }
        self->historyPalleteHolder->is_being_viewed_state = false;
    }else{
        self->historyPalleteHolder->isInBounds = true; // nice hack
        self->updatePickHistoryPreview();
        self->palleteScroller->updateTiles();
        self->historyPalleteHolder->setAnimation( self->uxAnimations->resetPosition(self->historyPalleteHolder) ); // returns uiAminChain*
        self->historyPalleteHolder->is_being_viewed_state = true;
        //self->historyScroller->allowUp = true;
    }
}

void Ux::removeHistoryColor(uiObject *interactionObj, uiInteraction *delta){
    Ux* myUxRef = Ux::Singleton();

    int offset = interactionObj->myIntegerIndex;
    interactionObj->setBoundaryRect( 1.0, 0.0, 1.0, 1.0); // reset  (close x ? ) out of view for next obj to use this tile...
    if( offset < 0 ) return;

    myUxRef->pickHistoryList->remove(offset);

    myUxRef->updatePickHistoryPreview();
   // historyScroller->updateTiles(); // above calls this

}

void Ux::clickDeleteHistoryColor(uiObject *interactionObj, uiInteraction *delta){

    if( !interactionObj->doesInFactRender || !interactionObj->parentObject->doesInFactRender ){
        SDL_Log("CLICKED INVISIBLE REMOVE TILE");
        return; // this means our tile is invalid/out of range.. it is arguable we should not even get the event in this case? // todo (delete last tile and click again - it will prompt again without this or some solution)
    }

    Ux* myUxRef = Ux::Singleton();

    //myUxRef->defaultYesNoChoiceDialogue->uiObjToAnimate == myUxRef->defaultYesNoChoiceHolder

    if( delta->dx == 0.0f && myUxRef->defaultYesNoChoiceDialogue->uiObjToAnimate->is_being_viewed_state == false ){
        myUxRef->uxAnimations->scale_bounce(interactionObj->childList[0], 0.001);
        myUxRef->defaultYesNoChoiceDialogue->display(interactionObj, &Ux::removeHistoryColor, &Ux::clickDeleteHistoryColor); // when no clicked we reach else
    }else{
        interactionObj->setAnimation( myUxRef->uxAnimations->resetPosition(interactionObj) ); // returns uiAminChain*
        myUxRef->pickHistoryList->get(interactionObj->myIntegerIndex)->is_delete_state = false;
    }
}

void Ux::clickClearHistory(uiObject *interactionObj, uiInteraction *delta){
    Ux* myUxRef = Ux::Singleton();

    myUxRef->pickHistoryList->clear();
    myUxRef->updatePickHistoryPreview();

}

void Ux::clickCancelClearHistory(uiObject *interactionObj, uiInteraction *delta){
    Ux* myUxRef = Ux::Singleton();

    // we should show delete x on all visible tiles....
    for( int x=0,l=myUxRef->historyScroller->scrollChildContainer->childListIndex; x<l; x++ ){
        uiObject* removeButton = myUxRef->historyScroller->scrollChildContainer->childList[x]->childList[0];
        ColorList* listItem = myUxRef->pickHistoryList->get(removeButton->myIntegerIndex);
        if( listItem == nullptr || !listItem->is_delete_state ){
            removeButton->setAnimation( myUxRef->uxAnimations->resetPosition(removeButton) );
        }
    }


}

void Ux::historyReceivedFocus(uiObject *interactionObj, uiInteraction *delta){
    Ux* myUxRef = Ux::Singleton();
    // hide pallete preview if possible...
    if( myUxRef->palleteSelectionPreviewHolder->is_being_viewed_state ){
        myUxRef->interactionTogglePalletePreview(myUxRef->palleteSelectionPreviewHolder, delta);
    }
}

void Ux::clickHistoryColor(uiObject *interactionObj, uiInteraction *delta){ // see also updateUiObjectFromHistory



    //SDL_Log("clickHistoryColor --------------------- %i", interactionObj->myIntegerIndex);

    historyReceivedFocus(interactionObj, delta);


    if( !interactionObj->doesInFactRender ){
        interactionObj->resetPosition();

        return; // this means our tile is invalid/out of range
    }

    Ux* myUxRef = Ux::Singleton();


    if( interactionObj->myIntegerIndex < -1 ){

        if( interactionObj->myIntegerIndex == BTN_NEGATIVE_START - BUTTON_CLEAR_HISTORY ){
            SDL_Log("Clear Button ---------------------");

            // we should show delete x on all visible tiles....
            for( int x=0,l=myUxRef->historyScroller->scrollChildContainer->childListIndex; x<l; x++ ){
                uiObject* removeButton = myUxRef->historyScroller->scrollChildContainer->childList[x]->childList[0];
                removeButton->setAnimation( myUxRef->uxAnimations->slideLeftFullWidth(removeButton) );
            }

            myUxRef->uxAnimations->scale_bounce(interactionObj->childList[1], 0.001);

            myUxRef->defaultYesNoChoiceDialogue->display(interactionObj, &Ux::clickClearHistory, &Ux::clickCancelClearHistory, myUxRef->pickHistoryList->total());


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


    if( fabs(delta->dy) > 0.003 ){
        return;
    }

    // A couple things
    // if image is already in the pallete, we sholud probably not add it twice √ done
    // we should instead scroll to the color and animate it
    // we should always scroll to that index though...

    ColorList tmpLocate = ColorList(interactionObj->backgroundColor);

    int existingLocation = myUxRef->palleteList->locate(tmpLocate);

    if( existingLocation > -1 ){
        // this color is already taken then
        Uint8 palleteOffset = existingLocation; // pointless var

        myUxRef->palleteScroller->scrollToItemByIndex(palleteOffset);

        uiObject* visibleTile = myUxRef->palleteScroller->getVisibleTileForOffsetOrNull(palleteOffset);

        //uxAnimations->rvbounce(historyPreview);

        if( visibleTile != nullptr ){
            myUxRef->uxAnimations->rvbounce(visibleTile);
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
        return;
    }

    myUxRef->palleteList->add(ColorList(interactionObj->backgroundColor));

    myUxRef->updatePalleteScroller();

    myUxRef->palleteScroller->scrollToItemByIndex(myUxRef->palleteList->previousIndex());
}

//void Ux::resizePalleteBasedOnNumberOfColors(){
//
//}
//static
void Ux::interactionReturnToPreviousSurface(uiObject *interactionObj, uiInteraction *delta){

    Ux* myUxRef = Ux::Singleton();
    OpenGLContext* ogg=OpenGLContext::Singleton();
    ogg->restoreLastSurface();
    myUxRef->returnToLastImgBtn->hideAndNoInteraction(); // this should have aleady occured, best be safe though, above does return false wehn we need to do this...
}

void Ux::interactionFileBrowserTime(uiObject *interactionObj, uiInteraction *delta){
    Ux* myUxRef = Ux::Singleton();
    myUxRef->uxAnimations->scale_bounce(interactionObj, 0.001);
    OpenGLContext* ogg=OpenGLContext::Singleton();
    ogg->chooseFile();
}


//static
void Ux::interactionAddHistory(uiObject *interactionObj, uiInteraction *delta){
    Ux* myUxRef = Ux::Singleton();
    myUxRef->uxAnimations->scale_bounce(interactionObj, 0.001);
    myUxRef->addCurrentToPickHistory();

    // in the popup, if we were viewing a color, we should hide it next time we open this up.... esp for widescreen
    if( myUxRef->palleteSelectionPreviewHolder->is_being_viewed_state ) {
        myUxRef->interactionTogglePalletePreview(myUxRef->palleteSelectionPreviewHolder, delta);
    }
}

// possibly unused now...
void Ux::hueClicked(uiObject *interactionObj, uiInteraction *delta){
    // seeingly unused...
    hueClicked(&interactionObj->backgroundColor);
}

void Ux::hueClicked(SDL_Color* c){
    OpenGLContext* ogg=OpenGLContext::Singleton();
    ogg->pickerForHue(c);
}

void Ux::hueClickedPickerHsv(SDL_Color* c){
    OpenGLContext* ogg=OpenGLContext::Singleton();
    Ux* myUxRef = Ux::Singleton();
    myUxRef->lastHue->fromColor(c);
    ogg->pickerForHue(myUxRef->lastHue, c);
    interactionToggleHistory(myUxRef->interactionObject, &myUxRef->currentInteraction);
    //interactionToggleHistory(nullptr, nullptr);
}

// unused...
void Ux::pickerForHuePercentage(float percent){
    OpenGLContext* ogg=OpenGLContext::Singleton();
    Ux* uxInstance = Ux::Singleton();
    ogg->pickerForHue(uxInstance->huePicker->colorForPercent(percent));
}

void Ux::interactionDirectionalArrowClicked(uiObject *interactionObj, uiInteraction *delta){
    //SDL_Log("directionalllly");
    if( delta->py < 0 ){
        colorPickState->mmovey+=1;
    }else if( delta->py > 0 ){
        colorPickState->mmovey-=1;
    }else if( delta->px < 0 ){
        colorPickState->mmovex-=1;
    }else if( delta->px > 0 ){
        colorPickState->mmovex+=1;
    }
}



// this overrides the interface fn?
// this function is a bit odd for now, but is planned to be used to drag and throw objects that will otherwise animiate in, or not animate in based on this - Pane
//static
void Ux::interactionHorizontal(uiObject *interactionObj, uiInteraction *delta){
    Ux* self = Ux::Singleton();

    //self->historyFullsize // right now there is still some history specific stuff here

    if( interactionObj->interactionProxy != nullptr ){
        interactionObj = interactionObj->interactionProxy;
    }
    // if we are already panning ....
    interactionObj->cancelCurrentAnimation();

    interactionObj->boundryRect.x += (delta->rx * (1.0/interactionObj->parentObject->collisionRect.w));

    if( delta->rx > 0.003 ){ // moving --down--
        interactionObj->is_being_viewed_state = true;
    }

    if( delta->rx < -0.003 ){
        interactionObj->is_being_viewed_state = false; // toggle still to be called..... on touch release (interactionCallback)
    }

    self->updateRenderPosition(interactionObj);
}

// this overrides the interface fn?
// this function is a bit odd for now, but is planned to be used to drag and throw objects that will otherwise animiate in, or not animate in based on this - Pane
//static
void Ux::interactionVert(uiObject *interactionObj, uiInteraction *delta){
    Ux* self = Ux::Singleton();

    //self->historyFullsize // right now there is still some history specific stuff here

    if( interactionObj->interactionProxy != nullptr ){
        interactionObj = interactionObj->interactionProxy;
    }
    // if we are already panning ....
    interactionObj->cancelCurrentAnimation();

    interactionObj->boundryRect.y += (delta->ry * (1.0/interactionObj->parentObject->collisionRect.h));

    if( delta->ry > 0.003 ){ // moving down
        interactionObj->is_being_viewed_state = true;
    }

    if( delta->ry < -0.003 ){
        interactionObj->is_being_viewed_state = false; // toggle still to be called..... on touch release (interactionCallback)
    }

    self->updateRenderPosition(interactionObj);
}

//static   // must have move boundary rect....
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

void Ux::clickZoomSliderBg(uiObject *interactionObj, uiInteraction *delta){
    float percent = (delta->px - interactionObj->collisionRect.x) * (1.0/interactionObj->collisionRect.w); // it is arguable delta aleady "know" this?
    if( percent < 0 ) percent = 0;
    else if( percent > 1 ) percent = 1;
    OpenGLContext* ogg=OpenGLContext::Singleton();
    ogg->setFishScalePerentage(interactionObj, percent);
    Ux* myUxRef = Ux::Singleton();
    myUxRef->zoomSlider->updateAnimationPercent(percent, 0.0);
}

//static // this seems like interactionSliderHZ ? // must have move boundary rect....
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

    //OpenGLContext* ogg=OpenGLContext::Singleton();

    Ux* myUxRef = Ux::Singleton();
    //myUxRef->updateRenderPositions();
    myUxRef->updateRenderPosition(interactionObj);


//    SDL_Log(" (((((((((((((((((((((((((((((((((((((((((((((((((((((((((((( FIX ME");
//    myUxRef->bottomBar->updateRenderPosition();


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

// this really means when swiping left far enough we disable scrolling
// other conditions we should bubble (if it seems like scrolling)
// this is also becomming rather specific to scroller tiles, and probably belongs in the scroller....
bool Ux::bubbleInteractionIfNonHorozontalMovement(uiObject *interactionObj, uiInteraction *delta){ // return true always, unless the interaction should be dropped and not bubble for some reason....
    // THIS should return true if the interaciton is still valid, which in all cases should really be YES - unles interaction object is for some reason nullptr reference

    Ux* self = Ux::Singleton();
    // see also interactionUpdate

    // interactionObj is the tile itself basically... it contains 2 child objects
    // -x is leftward movement
    // -y is upward movement

    //SDL_Log("00))))0000000000000000000000000 y:%f x:%f",delta->dy, delta->dx);
    SDL_Log("00))))0000000000000000000000000 y:%f x:%f",fabs(delta->dy), fabs(0.0f-delta->dx));
    //SDL_Log("00))))0000000000000000000000000 x:%f 0x:%f",interactionObj->boundryRect.x, interactionObj->origBoundryRect.x);


    // todo: for widescreen mode.... if we swipe right (delta->dx > 0.04f)? and we are not showing the close X
    // this means we should also bubble...

    if( !interactionObj->doesInFactRender
       || (self->widescreen && delta->dx > 0.01f )
       || ( fabs(delta->dy) > 0.01f && delta->dx > -0.04f) ){ // or is approx 0

        // since we were animating and its about to get lost, lets reset
        if( interactionObj->interactionProxy != nullptr ){
            interactionObj->interactionProxy->setAnimation( self->uxAnimations->resetPosition(interactionObj->interactionProxy) );
        }

        //SDL_Log("00))))0000000000000000000000000 y:%i x:%f",delta ->dy, fabs(0.0f-delta->dx));
        return self->bubbleCurrentInteraction(); // *SEEMS * much simploer to call bulbble on the UI object itself, perhaps returning the reference to the new interactionObject instead of bool....

    }

    return true;
}


// **** PALLETTE ****
//static
bool Ux::updateUiObjectFromPallete(uiObject *historyTile, int offset){  // see also clickPalleteColor
    Ux* self = Ux::Singleton();

    self->colorTileAddChildObjects(historyTile, &Ux::clickDeletePalleteColor);
    uiObject * removeButton = historyTile->childList[0];
    uiObject * iconBtn = historyTile->childList[1];


    if( offset > self->palleteList->largestIndex() || offset < 0 ){

        removeButton->setBoundaryRect( 1.0, 0.0, 1.0, 1.0); // reset out of view (otherwise it is in the way of clicks!)

        if( self->palleteList->total() > 0 ){

            if( offset == self->palleteList->total() + BUTTON_CLEAR_PALLETE ){

                historyTile->show();
                removeButton->hide(); // it still collides..... so we set the rect above!
                iconBtn->show();
                Ux::setColor(&historyTile->backgroundColor, 0, 0, 0, 255);

                self->printCharToUiObject(iconBtn, CHAR_LIGHTENING_BOLT, DO_NOT_RESIZE_NOW);

                historyTile->myIntegerIndex = BTN_NEGATIVE_START - BUTTON_CLEAR_PALLETE; //awkward but using negative space beyond -1 for codes

                historyTile->hasInteraction = false; // disable animations which are default for this scroll controller....

                historyTile->interactionProxy=nullptr;
                
                return true;
            }else if( offset == self->palleteList->total() + BUTTON_SAVE_PALLETE ){

                historyTile->show();
                removeButton->hide(); // it still collides..... so we set the rect above!
                iconBtn->show();
                Ux::setColor(&historyTile->backgroundColor, 0, 0, 0, 255);

                self->printCharToUiObject(iconBtn, CHAR_SAVE_ICON, DO_NOT_RESIZE_NOW);

                historyTile->myIntegerIndex = BTN_NEGATIVE_START - BUTTON_SAVE_PALLETE; //awkward but using negative space beyond -1 for codes

                historyTile->hasInteraction = false; // disable animations which are default for this scroll controller....

                historyTile->interactionProxy=nullptr;

                return true;
            }
        }

        historyTile->hide();
        historyTile->myIntegerIndex = -1;
        return false;
    }

    ColorList* listItem = self->palleteList->get(offset);
    SDL_Color* clr = &listItem->color;
    bool is_delete_state = listItem->is_delete_state;

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

int Ux::getPalleteTotalCount(){
    Ux* self = Ux::Singleton();
    //return self->palleteList->largestIndex() + 1;

    int total = self->palleteList->total();
    if( total > 0 ){
        total+=PALLETE_EXTRA_BUTTONS_TOTAL;
    }
    return total;

}



// **** HISTORY ****
//static
bool Ux::updateUiObjectFromHistory(uiObject *historyTile, int offset){ // see also clickHistoryColor
    Ux* self = Ux::Singleton();

    self->colorTileAddChildObjects(historyTile, &Ux::clickDeleteHistoryColor);
    uiObject * removeButton = historyTile->childList[0];
    uiObject * iconBtn = historyTile->childList[1];

    if( offset > self->pickHistoryList->total() - 1 || offset < 0 ){

        removeButton->setBoundaryRect( 1.0, 0.0, 1.0, 1.0); // reset out of view (otherwise it is in the way of clicks!)

        if( self->pickHistoryList->total() > 0 ){
            if(offset == self->pickHistoryList->total() + BUTTON_CLEAR_HISTORY ){
                historyTile->show();
                removeButton->hide(); // it still collides..... so we set the rect above!
                iconBtn->show();
                Ux::setColor(&historyTile->backgroundColor, 0, 0, 0, 255);

                historyTile->myIntegerIndex = BTN_NEGATIVE_START - BUTTON_CLEAR_HISTORY; //awkward but using negative space beyond -1 for codes

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

    offset = self->pickHistoryList->total() - offset - 1; // we show them in reverse here

    ColorList* listItem = self->pickHistoryList->get(offset);
    SDL_Color* clr = &listItem->color;
    bool is_delete_state = listItem->is_delete_state;

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

int Ux::getHistoryTotalCount(){
    Ux* self = Ux::Singleton();
    int total = self->pickHistoryList->total();
    if( total > 0 ){
        total+=PICK_HISRTORY_EXTRA_BUTTONS_TOTAL;
    }
    return total;
}


void Ux::colorTileAddChildObjects(uiObject *historyTile, anInteractionFn removeClickedFn){
    // this adds the common child objects for history and pallete

    // todo - even if this tile is not reached yet, its still a possible tile to render, so we could or might as well allocate
    // any nececesssary child objects upfront, possibly with the use of a helper function which could be common between this and
    // the pallet tile updater function....  then each child tile index may have a special purpose of its own and be enabled/disable easily as needed

    if( historyTile->getChildCount() < 1 ){



        uiObject* removeButtonHolder = new uiObject();
        uiObject* removeButton = new uiObject();
        removeButtonHolder->setBoundaryRect( 1.0, 0.0, 1.0, 1.0); // ofset to right 1.0
        removeButton->setBoundaryRect( 0.0, 0.0, 1.0, 1.0);

//        removeButton->setCropParentRecursive(historyTile); // must set before adding child...  not strictly needed unless performing the next opperation below .. or actually this line is dupe since the child is not yet added?
        removeButton->setCropModeOrigPosition(); // influences the rendering of the X while we bounce (duplicate detected)

        removeButtonHolder->setCropParentRecursive(historyTile); // must set before adding child...
        removeButtonHolder->setCropModeOrigPosition(); // influences the rendering of the background color while we bounce (duplicate detected)



        removeButtonHolder->addChild(removeButton);

//

        historyTile->addChild(removeButtonHolder);


        printCharToUiObject(removeButton, CHAR_CLOSE_ICON, DO_NOT_RESIZE_NOW);
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
        printCharToUiObject(buttonIcon, CHAR_LIGHTENING_BOLT, DO_NOT_RESIZE_NOW);
        historyTile->addChild(buttonIcon);
        buttonIcon->hide();
        buttonIcon->squarify();



    }




}


/// eeeh this is really update current color
///   mayhaps this only updates when you stop panning?
///   or otherwise seperate function for addPickHistory from this
void Ux::updateColorValueDisplay(SDL_Color* color){

    curerntColorPreview->update(color);

    //currentlyPickedColor = color; // maybe we should copy it instead?
    setColor(currentlyPickedColor, color);
    //addCurrentToPickHistory();
}



void Ux::addCurrentToPickHistory(){

//  TODO consider eliminating lastPickHistoryIndex instead
    /// i.e pickHistoryIndex = lastPickHistoryIndex
    // we can increment and bound check before using the new index
    // this should simplify the checks used elsewhere

    // IMPORTANT remember ot text with pickHistoryMax = <5
    // IMPORTANT remember to check more than 10 colors


    float bounceIntensity = -0.001;

    // if we have a lastPickHistoryIndex
    if( pickHistoryList->total() > 0 || pickHistoryList->largestIndex() > 0 ){
        // the new color must be unique
        if( colorEquals(&pickHistoryList->getLast()->color, currentlyPickedColor) ){

            // we already had this color added to the end of the history... indicate this with an effect and do not continue

            uiObject * colora = historyPreview->childList[0];
            //colora->boundryRect.y -= 0.1;
            //colora->updateRenderPosition();
            // openglContext->
            bounceIntensity = -0.01;
            uxAnimations->emphasizedBounce(colora, widescreen?bounceIntensity:0, widescreen?0:bounceIntensity);
            return;
        }else{
            uxAnimations->softBounce(historyPreview, widescreen?0:bounceIntensity, widescreen?bounceIntensity:0);
        }
    }else{
        uxAnimations->softBounce(historyPreview, widescreen?0:bounceIntensity, widescreen?bounceIntensity:0);
    }

    pickHistoryList->add(ColorList(*currentlyPickedColor));

    updatePickHistoryPreview();

    if( historyPalleteHolder->isInBounds ){
        historyScroller->scrollToItemByIndex(0);
    }else{
        historyScroller->scrolly = 0;
    }

}


void Ux::updatePickHistoryPreview(){


    // err - add 10 to loose add button!

    if( pickHistoryList->total() < 0 ) return; //no history yet, and this means we may need to keep lastPickHistoryIndex around?

    int ctr = 0;
    int histIndex = pickHistoryList->total() - 1;
    if( histIndex < 0 ){
        histIndex = pickHistoryList->largestIndex();
    }
    int startIndex = histIndex;
    uiObject * colora;
    SDL_Color* clr;
    while( ctr < SIX_ACROSS  ){

        colora = historyPreview->childList[ctr];


        if( histIndex >= 0 ){


            colora->doesInFactRender = true;

            //clr = &pickHistory[histIndex];
            clr = &pickHistoryList->get(histIndex)->color;

            Ux::setColor(&colora->backgroundColor, clr->r, clr->g, clr->b, 255);



            // call itoa 10 times and we will suddenly loose a few UI elements....
           // SDL_itoa(histIndex, resultText, 10);
            //colora->containText =true;
           // printStringToUiObject(colora, "A", true);


            //colora->hasForeground = false;
            // printCharToUiObject(colora, 'Z'+ctr, true);
            //historyPreview->addChild(colora);

        }else{
            colora->doesInFactRender = false;
        }


        histIndex--;
        ctr++;

        if( histIndex < 0 && pickHistoryList->largestIndex() > startIndex ){
            histIndex = pickHistoryList->largestIndex();
        }

        //return;

    }


    // somertimes upate the full histoury preview too
    if( historyPalleteHolder->isInBounds ){
        historyScroller->updateTiles(); // calls updateUiObjectFromHistory for each tile
        //palleteScroller->updateTiles();
    }
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



//    if( renderObj->isDebugObject ){
//        SDL_Log("DEBUG OBJECT IS BEING RENDERED NOW!");
////        renderObj->boundryRect.y+=0.01; // this proves the boundary tests is working for this object, when out of view they do not render...
////        renderObj->updateRenderPosition();
//
//    }

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




            if( renderObj->cropParentObject->hasCropParent ){


                glUniform4f(uniformLocations->ui_crop2,
                            renderObj->cropParentObject->cropParentObject->renderRect.x,
                            -renderObj->cropParentObject->cropParentObject->renderRect.y,
                            renderObj->cropParentObject->cropParentObject->renderRect.w,
                            renderObj->cropParentObject->cropParentObject->renderRect.h);

            }else{
                glUniform4f(uniformLocations->ui_crop2,
                            0,
                            0,
                            /*disabled*/0,//1,
                            /*disabled*/0);//1); // 0,0,1,1  is screen crop, but we can skip this logic in vsh
            }

            if( renderObj->useCropParentOrig ){
                glUniform4f(uniformLocations->ui_crop,
                            renderObj->cropParentObject->origRenderRect.x,
                            -renderObj->cropParentObject->origRenderRect.y,
                            renderObj->cropParentObject->origRenderRect.w,
                            renderObj->cropParentObject->origRenderRect.h);
            }else{

            glUniform4f(uniformLocations->ui_crop,
                        renderObj->cropParentObject->renderRect.x,
                        -renderObj->cropParentObject->renderRect.y,
                        renderObj->cropParentObject->renderRect.w,
                        renderObj->cropParentObject->renderRect.h);

            }
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
                        renderObj->backgroundColor.r/255.0, // maths can be avoided (or moved to shader?)
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
