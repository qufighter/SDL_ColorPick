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

void Ux::updateModal(uiObject *newModal, anInteractionFn pModalDismissal){
    if( currentModal == newModal ) return;
    if( currentModal == nullptr ){
        currentModal = rootUiObject;
    }
    newModal->modalParent = currentModal;
    newModal->modalDismissal = pModalDismissal;
    currentModal = newModal;
}
void Ux::endModal(uiObject *oldModal){
    if( currentModal == oldModal ){
        currentModal = oldModal->modalParent;
    }else{
        if( oldModal->modalParent && currentModal == oldModal->modalParent ){
            return; // preserve modal
        }
        currentModal = nullptr;
    }
}
void Ux::endCurrentModal(){
    if( currentModal != rootUiObject && currentModal != nullptr ){
        // NOTE: just using  currentInteraction here could have SIDE EFFECTS (key presses could reset those???)
        currentModal->modalDismissal(currentModal, &currentInteraction);
        // endModal(currentModal); // the modal dismissal SHOULD call end modal automatically!  if not you probably did it wrong
    }
}


void Ux::GetPrefPath(char* preferencesPath, const char* filename, char** resultDest){
    size_t len = SDL_strlen(preferencesPath) + SDL_strlen(filename) + 4;
    *resultDest = (char*)SDL_malloc( sizeof(char) * len );
    SDL_snprintf(*resultDest, len, "%s%s", preferencesPath, filename);
}

/**
 Default constructor
 */
Ux::Ux(void) {

    lastHue = new HSV_Color();
    currentlyPickedColor = new SDL_Color();

    isInteracting = false;
    currentInteraction = uiInteraction();

    interactionObject = nullptr;
    currentModal = nullptr;
//    for( int x=0; x<COLOR_INDEX_MAX; x++ ){
//        palleteColorsIndex[x] = palleteMax; //-1; // largest Uint..
//    }

    char* preferencesPath = SDL_GetPrefPath("vidsbee", "colorpick");

    GetPrefPath(preferencesPath, "history.bin", &historyPath);
    GetPrefPath(preferencesPath, "pallete.bin", &palletePath);
    GetPrefPath(preferencesPath, "records.bin", &scoresPath);

    SDL_free(preferencesPath);
}

void Ux::readInState(char* filepath, void* dest, int destMaxSize, int* readSize){
    SDL_RWops* fileref = SDL_RWFromFile(filepath, "r");
    if( fileref == NULL ) return;

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
    int eachElementSize = sizeof(SDL_Color);
    int memOffset;
    int readOffset;

    SDL_Color* newHistoryList = (SDL_Color*)SDL_malloc( sizeof(SDL_Color) *  pickHistoryList->maxSize );
    readInState(historyPath, newHistoryList, pickHistoryList->maxSize * eachElementSize, &quantityBytesRead);
    pickHistoryList->clear();

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
    readInState(palletePath, newPalleteList, palleteList->maxSize * eachElementSize, &quantityBytesRead);
    palleteList->clear();

    memOffset = 0;
    readOffset = 0;
    while( memOffset < quantityBytesRead ){
        palleteList->add(ColorList(newPalleteList[readOffset++]));
        memOffset+=eachElementSize;
    }
    //palleteList->addAll(newPalleteList, quantityBytesRead);
    SDL_free(newPalleteList);
    updatePalleteScroller();


    eachElementSize = sizeof(Sint32);
    int maxReads = 2 * eachElementSize;
    Sint32 tempRead = 0;
    SDL_RWops* fileref = SDL_RWFromFile(scoresPath, "r");
    if( fileref == NULL ) return;
    Sint64 filesize = SDL_RWsize(fileref); // this filesize is in ? units... not a count of u8? (actually it seems to count u8s)
    int currentPosition = 0;
    if( currentPosition < filesize && currentPosition < maxReads ){
        defaultScoreDisplay->int_max_score = SDL_ReadBE32(fileref);
        currentPosition++;
    }
    if( currentPosition < filesize && currentPosition < maxReads ){
        tempRead = SDL_ReadLE32(fileref);
        currentPosition++;
    }
    if( defaultScoreDisplay->int_max_score != tempRead ){
        defaultScoreDisplay->int_max_score = 0; // corrupted file!
        SDL_Log("File appears to be corrupted, resetting high score");
    }

    if( currentPosition < filesize && currentPosition < maxReads ){
        defaultScoreDisplay->int_score = SDL_ReadBE32(fileref);
        currentPosition++;
    }
    if( currentPosition < filesize && currentPosition < maxReads ){
        tempRead = SDL_ReadLE32(fileref);
        currentPosition++;
    }
    if( defaultScoreDisplay->int_score != tempRead ){
        defaultScoreDisplay->int_score = 0; // corrupted file!
        SDL_Log("File appears to be corrupted, resetting score");
    }

    //defaultScoreDisplay->int_score = SDL_MAX_SINT32;
    //defaultScoreDisplay->int_max_score = 0;

    SDL_RWclose(fileref);
    //*readSize = currentPosition;
}

void Ux::writeOutState(void){
//    SDL_Log("Pref file path: %s", historyPath);
//    SDL_Log("Pref file len: %i", SDL_strlen(historyPath));
//    SDL_Log("Pref file path: %s", palletePath);
//    SDL_Log("Pref file len: %i", SDL_strlen(palletePath));
//    int totalUint8s;
//    int currentPosition;
//    SDL_RWops* memref;
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

    fileref = SDL_RWFromFile(scoresPath, "w");
    SDL_WriteBE32(fileref, defaultScoreDisplay->int_max_score);
    SDL_WriteLE32(fileref, defaultScoreDisplay->int_max_score);
    SDL_WriteBE32(fileref, defaultScoreDisplay->int_score);
    SDL_WriteLE32(fileref, defaultScoreDisplay->int_score);
    SDL_RWclose(fileref);

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

//#ifndef __IPHONEOS__
    clock_bar=0.0f;
//#endif

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

            zoomSliderHolder->setBoundaryRect( 0.0, 0.01, 1.0, 0.25);
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
        historyPalleteEditor->resize(
            Float_Rect(0.025, ws_clock, 0.92, 1.0 - ws_clock), // visible and orig
            Float_Rect(1.0, ws_clock, 0.92, 1.0 - ws_clock) // hidden
        );


        temp = 0.15;
#ifndef OMIT_SCROLLY_ARROWS
        movementArrows->resize(Float_Rect(0.27777777777778, 0.0,
            1.0 - 0.27777777777778 - 0.27777777777778, 1.0));
#endif
        curerntColorPreview->resize(Float_Rect(0.0, ws_clock, 0.27777777777778, 1.0 - ws_clock));

        defaultYesNoChoiceHolder->setBoundaryRectForAnimState(0.0, 0.0, 1.0, 1.0, // vis
                                                              0.0, -1.0, 1.0, 1.0); // hid
//        defaultYesNoChoiceDialogue->resize(Float_Rect(0.0, -1.0, 1.0, 1.0));
        defaultYesNoChoiceDialogue->resize();



    }else{ // not widescreen

        bottomBar->setBoundaryRect( 0.0, 0.75, 1.0, 0.15);

            zoomSliderHolder->setBoundaryRect( 0.0, 0.05, 0.25, 0.95);
                zoomSliderBg->setBoundaryRect( 0.05, 0, 0.88, 1);
                zoomSlider->setBoundaryRect( 0.0, 0, 0.5, 1);
                zoomSlider->setMovementBoundaryRect( 0.0, 0, 1.0, 0.0);
                    //runner->resize(Float_Rect(0.0, 0.7, 1.0, hue_picker));

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

        float clocky_space = 0.032;

        historyPalleteEditor->resize(
            Float_Rect(0.0, clock_bar+clocky_space,     1.0, 0.92-clock_bar-clocky_space), // visible and orig
            Float_Rect(0.0, clock_bar+clocky_space+1.0, 1.0, 0.92-clock_bar-clocky_space) // hidden
        );


        temp = 0.27777777777778 + 0.04;
#ifndef OMIT_SCROLLY_ARROWS
        movementArrows->resize(Float_Rect(0.0, temp, 1.0, 1.0 - temp - temp));
#endif
        curerntColorPreview->resize(Float_Rect(0.0, clock_bar, 1.0, 0.27777777777778));


        defaultYesNoChoiceHolder->setBoundaryRectForAnimState(0.0, 0.0, 1.0, 1.0, // vis
                                                              0.0, -1.0, 1.0, 1.0); // hid

//        defaultYesNoChoiceDialogue->resize(Float_Rect(0.0, -1.0, 1.0, 1.0));
        defaultYesNoChoiceDialogue->resize();

    }

    OpenGLContext* ogg=OpenGLContext::Singleton();
    ogg->updateFishScaleSlider();

    updatePalleteScroller();

    rootUiObject->updateRenderPosition();
    // our "create" caller also updates scrollers at this time??
}

void Ux::updatePalleteScroller(){
    historyPalleteEditor->updatePalleteScroller();
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
    returnToLastImgBtn->setInteractionBegin(&Ux::interactionBeginReturnToPreviousSurface);
    returnToLastImgBtn->setClickInteractionCallback(&Ux::interactionReturnToPreviousSurface);
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
    Ux::setColor(&zoomSlider->foregroundColor, 0, 0, 0, 50); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
    zoomSlider->setInteraction(&Ux::interactionHZ);//zoomSlider->canCollide = true;
    //printCharToUiObject(zoomSlider, '^', DO_NOT_RESIZE_NOW);
    //printCharToUiObject(zoomSlider, CHAR_VERTICAL_BAR_POINTED, DO_NOT_RESIZE_NOW);
    printCharToUiObject(zoomSlider, CHAR_ZOOM_PLUSS, DO_NOT_RESIZE_NOW);
    zoomSlider->setRoundedCorners(0.486);
    zoomSliderHolder->addChild(zoomSlider);


    runner = new uiRunningMan(zoomSlider, Float_Rect(1.0,0.0,1.0,1.0));



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
        historyPreview->addChild(colora);
    }

    //historyPreview->hasForeground = true;
    //printCharToUiObject(historyPreview, '_', DO_NOT_RESIZE_NOW);
    //historyPreview->canCollide = true; // set by setInteractionCallback
    historyPreviewHolder->setInteraction(&Ux::interactionVert);
    //historyPreviewHolder->interactionProxy = historyPalleteEditor->historyPalleteHolder; // when we drag the preview effect the fullsize sliding into view...
    //printCharToUiObject(historyPreview, 'H', DO_NOT_RESIZE_NOW);
    //bottomBar->addChild(historyPreview);
    historyPreviewHolder->addChild(historyPreview);
    rootUiObject->addChild(historyPreviewHolder);



#ifndef OMIT_SCROLLY_ARROWS
    movementArrows = new uiNavArrows(rootUiObject, Float_Rect(0.0, clock_bar + 0.27777777777778, 1.0, 0.38), &Ux::interactionDirectionalArrowClicked);
#endif
    curerntColorPreview = new uiViewColor(rootUiObject, Float_Rect(0.0, clock_bar, 1.0, 0.27777777777778), false);



    historyPalleteEditor = new uiHistoryPalleteEditor(rootUiObject);

    // late setup of these, we have historyPalleteEditor now....
    historyPreviewHolder->interactionProxy = historyPalleteEditor->historyPalleteHolder; // when we drag the preview effect the fullsize sliding into view...
    historyPreviewHolder->setInteractionCallback(&historyPalleteEditor->interactionToggleHistory);


    defaultYesNoChoiceHolder = new uiObject();
    //defaultYesNoChoiceHolder->setBoundaryRect(0.0, -1.0, 1.0, 1.0);
    defaultYesNoChoiceHolder->setDefaultHidden();
    rootUiObject->addChild(defaultYesNoChoiceHolder);

    // top most objects
    defaultYesNoChoiceDialogue = new uiYesNoChoice(defaultYesNoChoiceHolder, Float_Rect(0.0, 0.0, 1.0, 1.0), true);

    defaultYesNoChoiceDialogue->uiObjectItself->hasBackground = true;
    Ux::setColor(&defaultYesNoChoiceDialogue->uiObjectItself->backgroundColor, 98, 0, 98, 192);

    //defaultYesNoChoiceDialogue->uiObjectItself->setAnimation( uxAnimations->slideUp(defaultYesNoChoiceDialogue->uiObjectItself) ); // returns uiAminChain*


    defaultScoreDisplay = new uiScore(rootUiObject);

    updateRenderPositions();

    readInState(); // reads saved state into lists


    //test cololr history
#ifdef DEVELOPER_TEST_MODE
    for( int q=0; q<64; q++ ){
        currentlyPickedColor = new SDL_Color();
        setColor(currentlyPickedColor, randomInt(0,44), randomInt(0,185), randomInt(0,44), 0);
        addCurrentToPickHistory();
    }
#endif

    //writeOutState(); // testing
    // for awful reasons during init some things should not be called....
    //palleteSelectionColorPreview->uiObjectItself->setAnimation( uxAnimations->slideDownFullHeight(palleteSelectionColorPreview->uiObjectItself) ); // returns uiAminChain*
    // todo we should just chain a interactionTogglePalletePreview
    //interactionTogglePalletePreview(nullptr, nullptr);
    historyPalleteEditor->interactionToggleHistory(nullptr, nullptr);

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

    int len=SDL_strlen(text); // sizeof(text) - 1;  // unfortunately sizeof not really work right here


    float letterWidth = 1.0 / len;
    float vertOffset = 0.5 - (letterWidth*0.5);

    printObj->doesNotCollide = !printObj->containText;
    printObj->doesInFactRender = printObj->containText; // the container is never the size to render.. unless contains text?!

    printObj->matrixInheritedByDescendants = true;

    for (i=text; *i; i++) {
        //SDL_Log("%c %i", *i, *i);

        charOffset = (int)*i - text_firstLetterOffset;

        // there is a chance the UI object already has this many letters
        // and that we don't need a new letter just to update existing one
        if( ctr < printObj->childListIndex ){
            letter = printObj->childList[ctr];
        }else{
            letter = new uiObject();

            if( !printObj->containText ){
                //letter->squarify();
                // LTR default - after text is printed you can change this!
                letter->setBoundaryRect( (ctr*1.0), 0, 1, 1);  /// TODO move size components into function to calculate on window rescale bonus points
            }


            printObj->addChild(letter);
        }

        if( printObj->containText == true ){
            // LTR default
            // move this right out of the else aboe, since we should fit any len text within, and also needs resize IF text len changed.... !?! easy compute
            letter->setBoundaryRect( (ctr*letterWidth), vertOffset, letterWidth, letterWidth);  /// TODO move size components into function to calculate on window rescale bonus points for suqare?
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
    self->historyPalleteEditor->palleteScroller->updateTiles();
}

//static
void Ux::interactionNoOp(uiObject *interactionObj, uiInteraction *delta){
    //SDL_Log("INERACTION NOOP REACHED");

}

void Ux::openURL(char* url){
    openURL(url); // uses platform specific version from FileChooser.h
}

void Ux::hideHistoryPalleteIfShowing(){
    if( historyPalleteEditor->historyPalleteHolder->is_being_viewed_state ) {
        historyPalleteEditor->interactionToggleHistory(nullptr, nullptr);
    }
}

//static
void Ux::interactionReturnToPreviousSurface(uiObject *interactionObj, uiInteraction *delta){

    Ux* myUxRef = Ux::Singleton();
    OpenGLContext* ogg=OpenGLContext::Singleton();
    ogg->restoreLastSurface();
    myUxRef->returnToLastImgBtn->hideAndNoInteraction(); // this should have aleady occured, best be safe though, above does return false wehn we need to do this...
}

void Ux::interactionBeginReturnToPreviousSurface(uiObject *interactionObj, uiInteraction *delta){
    Ux* myUxRef = Ux::Singleton();
    myUxRef->uxAnimations->scale_bounce(myUxRef->returnToLastImgBtn, 0.006);
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
    if( myUxRef->historyPalleteEditor->palleteSelectionPreviewHolder->is_being_viewed_state ) {
        myUxRef->historyPalleteEditor->interactionTogglePalletePreview(myUxRef->historyPalleteEditor->palleteSelectionPreviewHolder, delta);
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
    myUxRef->historyPalleteEditor->interactionToggleHistory(myUxRef->interactionObject, &myUxRef->currentInteraction);
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


//static add Ux:: // UNUSED FUNCTION
bool Ux::bubbleInteractionIfNonClickOrHiddenPalletePreview(uiObject *interactionObj, uiInteraction *delta){ // return true always, unless the interaction should be dropped and not bubble for some reason....
    // THIS should return true if the interaciton is still valid, which in all cases should really be YES - unles interaction object is for some reason nullptr reference

    Ux* self = Ux::Singleton();
    // see also interactionUpdate

    if( !self->historyPalleteEditor->palleteSelectionPreviewHolder->is_being_viewed_state || delta->dy != 0 || delta->dx != 0 ){
        return self->bubbleCurrentInteraction(); // *SEEMS * much simploer to call bulbble on the UI object itself, perhaps returning the reference to the new interactionObject instead of bool....
    }

    return true;
}


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
    //SDL_Log("00))))0000000000000000000000000 y:%f x:%f",fabs(delta->dy), fabs(0.0f-delta->dx));
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
            defaultScoreDisplay->loose(historyPreview->childList[0]);

            return;
        }else{
            uxAnimations->softBounce(historyPreview, widescreen?0:bounceIntensity, widescreen?bounceIntensity:0);
        }
    }else{
        uxAnimations->softBounce(historyPreview, widescreen?0:bounceIntensity, widescreen?bounceIntensity:0);
    }

    pickHistoryList->add(ColorList(*currentlyPickedColor));
    if( pickHistoryList->_out_of_space ){
        defaultScoreDisplay->displayExplanation("out of space!");
        defaultScoreDisplay->display(historyPreview->childList[0], 2);
    }else{
        defaultScoreDisplay->display(historyPreview->childList[0], 1);
    }


    updatePickHistoryPreview();

    if( historyPalleteEditor->historyPalleteHolder->isInBounds ){
        historyPalleteEditor->historyScroller->scrollToItemByIndex(0);
    }else{
        historyPalleteEditor->historyScroller->scrolly = 0;
    }

}


void Ux::updatePickHistoryPreview(){

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
            clr = &pickHistoryList->get(histIndex)->color;
            Ux::setColor(&colora->backgroundColor, clr->r, clr->g, clr->b, 255);
        }else{
            colora->doesInFactRender = false;
        }

        histIndex--;
        ctr++;

        if( histIndex < 0 && pickHistoryList->largestIndex() > startIndex ){
            histIndex = pickHistoryList->largestIndex();
        }
    }

    // somertimes upate the full histoury preview too
    if( historyPalleteEditor->historyPalleteHolder->isInBounds ){
        historyPalleteEditor->historyScroller->updateTiles(); // calls updateUiObjectFromHistory for each tile
        //palleteScroller->updateTiles();
    }
}








int Ux::renderObject(uniformLocationStruct *uniformLocations){
    return renderObjects(uniformLocations, rootUiObject, glm::mat4(1.0f));
}

int Ux::renderObjects(uniformLocationStruct *uniformLocations, uiObject *renderObj, glm::mat4 inheritMat){
    // update
    // render object itself, then render all child hobj


    if( !renderObj->isInBounds ) return 1;

    glm::mat4 resolvedRenderObjMat = renderObj->matrix; // copy struct

    if( renderObj->matrixInheritedByDescendants ){
        resolvedRenderObjMat *= inheritMat; // we limit the matrix math except where needed this way....
    }

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

    glUniformMatrix4fv(uniformLocations->modelMatrixLocation, 1, GL_FALSE, &resolvedRenderObjMat[0][0]); // Send our model matrix to the shader

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
            renderObjects(uniformLocations, renderObj->childList[x], resolvedRenderObjMat);
        }

        // renderChildrenInReverse

//        for( int x=renderObj->childListIndex-1; x>-1; x-- ){
//            renderObjects(uniformLocations, renderObj->childList[x]);
//        }
    }


    return 0;
}
