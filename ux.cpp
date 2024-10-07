//
//  ux.cpp
//  ColorPick SDL
//
//  Created by Sam Larison on 8/21/16.
//
//

#include "ux.h"


#include "FileChooserInclude.h"


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

#if defined(__EMSCRIPTEN__) 
#if defined(COLORPICK_BUILD_FOR_EXT)

EM_JS(void, em_add_current_to_history, (int r, int g, int b), {
    addColorToHistory(r, g, b);
});

EM_JS(const char*, em_history_loaded, (int numLoaded), {
    return historyLoaded(numLoaded);
});

EM_JS(void, em_launch_extension_options, (), {
    setTimeout(function(){
        launchExtensionOptions();
    }, 500); // we give the time for our animation engine to catch up... the first part of scale UP needs to complete BEFORE time starts accumulating.....
});
#else // __EMSCRIPTEN__ NOT COLORPICK_BUILD_FOR_EXT


// EM_JS(void, em_save_settings, (const char* settingsBinName, const char* encodedSettings), {    
//     localStorage['color_pick_settings_'+settingsBinName+'_bin_previous'] = localStorage['color_pick_settings_'+settingsBinName+'_bin_current']];
//     localStorage['color_pick_settings_'+settingsBinName+'_bin_current'] = encodedSettings;
// });


#endif
#endif


void Ux::updateModal(uiObject *newModal, anInteractionFn pModalDismissal){
    if( currentModal == newModal ) return;
    if( currentModal == nullptr ){
        currentModal = rootUiObject;
    }
    newModal->modalParent = currentModal;
    newModal->modalDismissal = pModalDismissal;
    currentModal = newModal;
    temporarilyDisableControllerCursorForAnimation();
}
void Ux::endModal(uiObject *oldModal){
    if( currentModal == oldModal ){
        currentModal = oldModal->modalParent;
    }else{
        if( oldModal->modalParent && currentModal == oldModal->modalParent ){
            //SDL_Log("Well there's your problem right there, not calling temporarilyDisableControllerCursorForAnimation here !!!!!!!!!!!!!!!!!!!!!!!!!"); //< unsure this would or will ever be the problem....
            return; // preserve modal
        }
        currentModal = nullptr;
    }
    temporarilyDisableControllerCursorForAnimation();
}
void Ux::endCurrentModal(){
    if( hasCurrentModal() ){
        // NOTE: just using  currentInteraction here could have SIDE EFFECTS (key presses could reset those???)
        currentModal->modalDismissal(currentModal, &currentInteractions[0]); // NOTE this BAD hack... lucky most modal dismissals dont' care much about the interaction details......
        // endModal(currentModal); // the modal dismissal SHOULD call end modal automatically!  if not you probably did it wrong
    }
}

bool Ux::hasCurrentModal(){
    return currentModal != nullptr && currentModal != rootUiObject;
}


bool Ux::modalWasFalseModal(){
    return currentModal != nullptr && currentModal == returnToLastImgBtn;
}

uiKeyInteractions* Ux::getKeyInteractions(){
    OpenGLContext* ogg=OpenGLContext::Singleton();
    return &ogg->keyInteractions;
}// TODO: eventually a helper on uxInstance should handle this... computing for either mouse, finger, or key durations and returining the appropriate multipler

char* Ux::GetPrefPath(){
    char* preferencesPath = SDL_GetPrefPath("vidsbee", "colorpick");

#if defined(__EMSCRIPTEN__) && !defined(COLORPICK_BUILD_FOR_EXT)

    if( preferencesPath != nullptr ){
        SDL_free(preferencesPath); // we better free this before we re-assign
    }

    preferencesPath = (char*)((const char*)"/vidsbee.colorpick.store/"); // wow this works even after we free the return value!  added casting to justify why it maybe works but it's not needed... guess it copies!?  wierd.  dissatisfied I wanted heap corruptin and program crash at second call here, and instaed all I have is questions... so it's a smart compiler is my guess, and it does a memcpy behind the scenes to a new allocation.

#endif

    if( preferencesPath == nullptr ){
        preferencesPath = (char*)"";
    }
    SDL_Log("Preferences Path: %s", preferencesPath);

    return preferencesPath;
}
void Ux::CreatePrefPath(char* preferencesPath, const char* filename, char** resultDest){
    size_t len = SDL_strlen(preferencesPath) + SDL_strlen(filename) + 4;
    *resultDest = (char*)SDL_malloc( sizeof(char) * len );
    SDL_snprintf(*resultDest, len, "%s%s", preferencesPath, filename);
}
/**
 Default constructor
 */
Ux::Ux(void) {

#ifndef COLORPICK_DEBUG_MODE
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_ERROR);
    // this will be set again in main()
#endif

    pickHistoryList = new uiList<ColorList, Uint8>(pickHistoryMax); // WARN - do not enable index if using Uint8 - max Uint8 is far less than pickHistoryMax
    palleteList = new uiList<ColorList, Uint8>(palleteMax);
    palleteList->index(COLOR_INDEX_MAX, indexForColor);

    minigameColorList = new uiList<ColorList, Uint8>(minigameColorListMax);
    minigameColorList->index(COLOR_INDEX_MAX, indexForColor);

 //    pickHistoryListState = new uiList<ColorListState, Uint8>(pickHistoryMax);
 //    palleteListState = new uiList<ColorListState, Uint8>(palleteMax);

    uxAnimations = new UxAnim();

//    rootUiObject->hasForeground = true; // render texture
//    Ux::setColor(&rootUiObject->foregroundColor, 0, 255, 0, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
//    Ux::setColor(&rootUiObject->foregroundColor, 0, 0, 0, 0); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
//    rootUiObject->hasForeground = false; // render texture
 //     make this color the selected color ! ! ! ! !! ^ ^ ^

    // moved here to support loading screen being possible...
    rootUiObject = new uiObject();
    rootUiObject->isRoot = true;
    rootUiObject->setBackgroundColor(0, 0, 0, 0); // transparent 0

    controllerCursorModeEnabled = false;
    controllerCursorLockedToObject=false;
    controllerCursorTemporarilyDisabledForAnimatedChange=false;
    controllerCursorIndex = 0;
    controllerCursorLastMovedAtTimestamp = 0;

    isMinigameMode = false;

    print_here = (char*)SDL_malloc( sizeof(char) * max_print_here );

    hueGradientData = new uxHueGradientData();

    lastHue = new HSV_Color();
    currentlyPickedColor = new SDL_Color();

    //nextFingerIndex = 0; // next "unknown" finger...
    for( int f=0; f<MAX_SUPPORTED_FINGERS_MICE; f++ ){
        currentInteractions[f] = uiInteraction();
    }

    currentModal = nullptr;
//    for( int x=0; x<COLOR_INDEX_MAX; x++ ){
//        palleteColorsIndex[x] = palleteMax; //-1; // largest Uint..
//    }

    char* preferencesPath = GetPrefPath();



    CreatePrefPath(preferencesPath, "history.bin", &historyPath);
    CreatePrefPath(preferencesPath, "pallete.bin", &palletePath);
    CreatePrefPath(preferencesPath, "setting.bin", &settingPath);
    CreatePrefPath(preferencesPath, "records.bin", &scoresPath);


    SDL_Log("Preferences Path: %s", historyPath);
    SDL_Log("Preferences Path: %s", palletePath);
    SDL_Log("Preferences Path: %s", settingPath);
    SDL_Log("Preferences Path: %s", scoresPath);


//    CreatePrefPath(preferencesPath, "shistory.bin", &historyPath);
//    CreatePrefPath(preferencesPath, "spallete.bin", &palletePath);
//    CreatePrefPath(preferencesPath, "ssetting.bin", &settingPath);
//    CreatePrefPath(preferencesPath, "srecords.bin", &scoresPath);

    SDL_free(preferencesPath);
}

// this wrapper helps us get something POSSIBLY usable on emscripten side...
SDL_RWops* Ux::GET_SDL_RWFromFile(const char *file, const char *mode){
    SDL_RWops* possibly_fileref = SDL_RWFromFile(file, mode);

#if defined(__EMSCRIPTEN__)
    if( possibly_fileref == NULL ){
        SDL_Log("Problem reading file... %s", file);
    }
#endif

    return possibly_fileref;
}

void Ux::PERFORM_SDL_RWclose(SDL_RWops* fileref){
    // ONLY call this on "files" that we want to persist to local storage (on emscripten).... before we close them!
    // so for EG only during WRITE operation...
    #if defined(__EMSCRIPTEN__)
        // actually we are using IDBFS, {autoPersist: true}
    #endif
    SDL_RWclose(fileref);
}

void Ux::readInState(char* filepath, void* dest, int destMaxSize, int* readSize){
    SDL_Log("READING: %s", filepath);
    SDL_RWops* fileref = GET_SDL_RWFromFile(filepath, "r");
    if( fileref == NULL ){
        SDL_Log("ERR READING: %s", SDL_GetError());
        return;
    }
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
    // EMSCRIPTEN NOTE: with current version of dependencies, READ seems to potentially require double path to reach the data by key?

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
 //    pickHistoryList->addAll(newHistoryList, quantityBytesRead);
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
 //    palleteList->addAll(newPalleteList, quantityBytesRead);
    SDL_free(newPalleteList);
    updatePalleteScroller();



    quantityBytesRead = 0;
    eachElementSize = sizeof(uiSettingsScroller::SettingsRwObject);
    int maxSettingsStorageSize = 255;
    uiSettingsScroller::SettingsRwObject* newSettingsList = (uiSettingsScroller::SettingsRwObject*)SDL_malloc( eachElementSize * maxSettingsStorageSize );
    readInState(settingPath, newSettingsList, maxSettingsStorageSize * eachElementSize, &quantityBytesRead);
    memOffset = 0;
    readOffset = 0;
    while( memOffset < quantityBytesRead ){
        settingsScroller->applyReadSettingsState(newSettingsList[readOffset++]);
        memOffset+=eachElementSize;
    }
    SDL_free(newSettingsList);




    eachElementSize = sizeof(Sint32);
    int maxReads = 6 * eachElementSize; // TOTAL SETTINGS NUMBER!
    Sint32 tempRead = 0;
    SDL_RWops* fileref = GET_SDL_RWFromFile(scoresPath, "r");
    if( fileref == NULL ){
        SDL_Log("ERR READING: %s", SDL_GetError());
        return;
    }
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

    tempRead = 0;
    if( currentPosition < filesize && currentPosition < maxReads ){
        defaultScoreDisplay->int_score = SDL_ReadBE32(fileref);
        currentPosition+=eachElementSize;
    }
    if( currentPosition < filesize && currentPosition < maxReads ){
        tempRead = SDL_ReadLE32(fileref);
        currentPosition+=eachElementSize;
    }
    if( defaultScoreDisplay->int_score != tempRead ){
        defaultScoreDisplay->int_score = 0; // corrupted file!
        SDL_Log("File appears to be corrupted, resetting score");
    }

    tempRead = 0;
    if( currentPosition < filesize && currentPosition < maxReads ){
        defaultScoreDisplay->combo_chain = SDL_ReadBE32(fileref);
        currentPosition+=eachElementSize;
    }
    if( currentPosition < filesize && currentPosition < maxReads ){
        tempRead = SDL_ReadLE32(fileref);
        currentPosition+=eachElementSize;
    }
    if( defaultScoreDisplay->combo_chain != tempRead ){
        defaultScoreDisplay->combo_chain = 0; // corrupted file!
        SDL_Log("File appears to be corrupted, resetting chain multiplier");
    }


 //    defaultScoreDisplay->int_score = 1000000000 - 1;
 //    defaultScoreDisplay->int_score = SDL_MAX_SINT32; // 2147483647  = 26,214 full history (8192x10 "out of space!") "Much Risk" to reach.... with max chain its as low as 262.14
 //    defaultScoreDisplay->int_max_score = 0;

    SDL_RWclose(fileref);
 //    *readSize = currentPosition;


}


void Ux::writeOutState(void){

    OpenGLContext* ogg=OpenGLContext::Singleton();
    if( !ogg->isProgramBooted() ){
        SDL_Log("Skipping preference write, program not booted yet...");
        return;
    }else{
        SDL_Log("Saving preferences...");
    }
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
    fileref = GET_SDL_RWFromFile(historyPath, "w");
    color = &myIterator->next()->color;
    while(color != nullptr){
        //SDL_Log("%i %i %i", color->r, color->g, color->b);
        SDL_WriteU8(fileref, color->r);
        SDL_WriteU8(fileref, color->g);
        SDL_WriteU8(fileref, color->b);
        SDL_WriteU8(fileref, color->a);
        color = &myIterator->next()->color;
    }
	FREE_FOR_NEW(myIterator); // again, free with use of new operator on windows causing issues!  we should use a macro
    PERFORM_SDL_RWclose(fileref);

//    totalUint8s = pickHistoryList->memorySize();
//    memref = SDL_RWFromMem(pickHistoryList->listItself, totalUint8s);
//    fileref = GET_SDL_RWFromFile(historyPath, "w");
//    currentPosition = 0;
//    while( currentPosition < totalUint8s ){
//        SDL_WriteU8(fileref, SDL_ReadU8(memref));
//        currentPosition++;
//    }
//    if( memref != NULL ) SDL_RWclose(memref);
//    PERFORM_SDL_RWclose(fileref);


    myIterator = palleteList->iterate();
    fileref = GET_SDL_RWFromFile(palletePath, "w");
    color = &myIterator->next()->color;
    while(color != nullptr){
        //SDL_Log("%i %i %i", color->r, color->g, color->b);
        SDL_WriteU8(fileref, color->r);
        SDL_WriteU8(fileref, color->g);
        SDL_WriteU8(fileref, color->b);
        SDL_WriteU8(fileref, color->a);
        color = &myIterator->next()->color;
    }
	FREE_FOR_NEW(myIterator); // free with new op causing issues on windows
    PERFORM_SDL_RWclose(fileref);

    
//    totalUint8s = palleteList->memorySize();
//    memref = SDL_RWFromMem(palleteList->listItself, totalUint8s);
//    fileref = GET_SDL_RWFromFile(palletePath, "w");
//    currentPosition = 0;
//    while( currentPosition < totalUint8s ){
//        SDL_WriteU8(fileref, SDL_ReadU8(memref));
//        currentPosition++;
//    }
//    if( memref != NULL ) SDL_RWclose(memref);
//    PERFORM_SDL_RWclose(fileref);

    fileref = GET_SDL_RWFromFile(settingPath, "w");
    settingsScroller->writeSettingsToFile(fileref);
    PERFORM_SDL_RWclose(fileref);


    fileref = GET_SDL_RWFromFile(scoresPath, "w");
    SDL_WriteBE32(fileref, defaultScoreDisplay->int_max_score);
    SDL_WriteLE32(fileref, defaultScoreDisplay->int_max_score);
    SDL_WriteBE32(fileref, defaultScoreDisplay->int_score);
    SDL_WriteLE32(fileref, defaultScoreDisplay->int_score);
    SDL_WriteBE32(fileref, defaultScoreDisplay->combo_chain);
    SDL_WriteLE32(fileref, defaultScoreDisplay->combo_chain);
    PERFORM_SDL_RWclose(fileref);
}


void Ux::updateStageDimension(float w, float h){

    if( h < 1 ){ h=1.0;}

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
    SDL_Log("Resize ui elements called, %i", widescreen);

    OpenGLContext* ogg=OpenGLContext::Singleton();

    if( isMinigameMode ){

        minigamesUiContainer->setBoundaryRect(0.0, 0.0, 1.0, 1.0);
        mainUiContainer->setBoundaryRect(5000.0, 5000.0, 1.0, 1.0);

        // resize the minigame... and thats it
        ogg->minigames->resize();

        // we COULD resize everything and then it will be ready for us, but game will run faster if we defer that.... and then resize when leaving the game?
        //rootUiObject->updateRenderPosition();
        //return;

// actually we only skip the set rect below if
        //if( hasChildren && isInBounds ){
// check remains in place in updateRenderPosition

    }else{
        mainUiContainer->setBoundaryRect(0.0, 0.0, 1.0, 1.0);
        minigamesUiContainer->setBoundaryRect(5000.0, 5000.0, 1.0, 1.0);







#ifdef DEVELOPER_TEST_MODE

#else

 //    #ifndef __IPHONEOS__
        clock_bar=0.0f;
 //    #endif

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

        //rootUiObject->setBoundaryRect( 0, 0, 1.0, 1.0); // default rect...
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
 //                    pickSourceBtn->setBoundaryRect( 0.0, 0, 1.0, 1.0);
 //                    addHistoryBtn->setBoundaryRect( 0.0, 0, 1.0, 1.0);
 //                    optionsGearBtn->setBoundaryRect( 0.0, 0, 1.0, 1.0);
#ifdef PICK_FROM_SCREEN_ENABLED
                    pickScreenBtn->stackBottom();
#endif
                    pickSourceBtn->stackBottom();
                    addHistoryBtn->stackBottom();
                    optionsGearBtn->stackBottom();

                    optionsGearBtn->setInteraction(&Ux::interactionHorizontal);

            huePicker->resize(Float_Rect(1.0-0.27777777777778, ws_clock, hue_picker, 1.0 - ws_clock ));
            returnToLastImgBtn->setBoundaryRect( 1.0-0.27777777777778 - 0.1, ws_clock, 0.1, 0.1 * screenRatio);
            returnToLastImgBtn->setRoundedCorners(0.5, 0.0, 0.5, 0.5);

            historyPreviewHolder->setBoundaryRect(1.0-history_preview, ws_clock, history_preview, 1.0 - ws_clock);
            historyPreviewHolder->setInteraction(&Ux::interactionHorizontal);
                historyPreview->setChildNodeDirection(TEXT_DIR_ENUM::BTT, true);

            //previewMessage->setChildNodeDirection(TEXT_DIR_ENUM::TTB, true);


            // depending on animation state rect is different....
            Float_Rect visibleModal = Float_Rect(0.025, ws_clock, 0.92, 1.0 - ws_clock);
            Float_Rect hiddenModal = Float_Rect(1.0, ws_clock, 0.92, 1.0 - ws_clock);

            historyPalleteEditor->resize(visibleModal, hiddenModal);
            settingsScroller->resize(visibleModal, hiddenModal);



            temp = 0.15;
#ifndef OMIT_SCROLLY_ARROWS
            movementArrows->resize(Float_Rect(0.27777777777778, 0.05,
                1.0 - 0.27777777777778 - 0.27777777777778, 1.0-0.1));
#endif
            curerntColorPreview->resize(Float_Rect(0.0, ws_clock, 0.27777777777778, 1.0 - ws_clock));

            defaultYesNoChoiceHolder->setBoundaryRectForAnimState(0.0, 0.0, 1.0, 1.0, // vis
                                                                  0.0, -1.0, 1.0, 1.0); // hid
 //            defaultYesNoChoiceDialogue->resize(Float_Rect(0.0, -1.0, 1.0, 1.0));
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
 //                    pickSourceBtn->setBoundaryRect( 0.0, 0, 1.0, 1.0);
 //                    addHistoryBtn->setBoundaryRect( 0.0, 0, 1.0, 1.0);
 //                    optionsGearBtn->setBoundaryRect( 0.0, 0, 1.0, 1.0);
#ifdef PICK_FROM_SCREEN_ENABLED
                    pickScreenBtn->stackRight();
#endif
                    pickSourceBtn->stackRight();
                    addHistoryBtn->stackRight();
                    optionsGearBtn->stackRight();

                    optionsGearBtn->setInteraction(&Ux::interactionVert);

            huePicker->resize(Float_Rect(0.0, 0.7, 1.0, hue_picker));
            returnToLastImgBtn->setBoundaryRect( 0.0, 0.7 - 0.1, 0.1 / screenRatio, 0.1);
            returnToLastImgBtn->setRoundedCorners(0.5, 0.5, 0.5, 0.0);

            historyPreviewHolder->setBoundaryRect(0.0, 1.0-history_preview, 1.0, history_preview);
            historyPreviewHolder->setInteraction(&Ux::interactionVert);
                historyPreview->setChildNodeDirection(TEXT_DIR_ENUM::RTL, true);

            //previewMessage->setChildNodeDirection(TEXT_DIR_ENUM::LTR, true);

            // depending on animation state rect is different....

            float clocky_space = 0.032;

            Float_Rect visibleModal = Float_Rect(0.0, clock_bar+clocky_space,     1.0, 0.92-clock_bar-clocky_space);
            Float_Rect hiddenModal = Float_Rect(0.0, clock_bar+clocky_space+1.0, 1.0, 0.92-clock_bar-clocky_space);

            historyPalleteEditor->resize(visibleModal, hiddenModal);
            settingsScroller->resize(visibleModal, hiddenModal);


            temp = 0.27777777777778 + 0.04;
#ifndef OMIT_SCROLLY_ARROWS
            movementArrows->resize(Float_Rect(0.0, temp, 1.0, 1.0 - temp - temp));
#endif
            curerntColorPreview->resize(Float_Rect(0.0, clock_bar, 1.0, 0.27777777777778));


            defaultYesNoChoiceHolder->setBoundaryRectForAnimState(0.0, 0.0, 1.0, 1.0, // vis
                                                                  0.0, -1.0, 1.0, 1.0); // hid

 //            defaultYesNoChoiceDialogue->resize(Float_Rect(0.0, -1.0, 1.0, 1.0));
            defaultYesNoChoiceDialogue->resize();

        }

        ogg->updateFishScaleSlider();

        updatePalleteScroller();

    }

    updateRenderPositions();

    //  we have an optimization where OOB elements aren't updated by the above, so lets manually update the important ones now....
    // untrue... boot in widescreen though to see teh issue with these:
//    historyPalleteEditor->uiObjectItself->testChildCollisionIgnoreBounds=true;
//    historyPalleteEditor->uiObjectItself->updateRenderPosition();
//    settingsScroller->uiObjectItself->testChildCollisionIgnoreBounds=true;
//    settingsScroller->uiObjectItself->updateRenderPosition();


 //     our "create" caller also updates scrollers at this time??
}

void Ux::updatePalleteScroller(){
    historyPalleteEditor->updatePalleteScroller();
}


int Ux::indexForColor(SDL_Color* c){
//     //here is an index test, albeit in javascript:
//    console.log('test beginning ... ... ... ... ...');
//    function getIndex(r,g,b){
//        return (r*65536) + (g*256) + (b);
//    }
//    var found=[];
//    for( var r=0;r<256;r++ ){
//        for( var g=0;g<256;g++ ){
//            for( var b=0;b<256;b++ ){
//                var idx=getIndex(r,g,b);
//                if( found[idx] ){
//                    console.error('index is found', idx, found[idx]);
//                }else{
//                    found[idx] = {r:r,g:g,b:b};
//                }
//            }
//        }
//    }
//    if( found.length != 16777216 ){
//        console.error('length of index appears odd, maybe JS engine...');
//    }
//    for( var i=0,l=found.length; i<l; i++ ){
//        if(!found[i]){
//            console.error('index is not tight, there is space at index', i);
//        }
//    }
//    console.log('test concluded, any errors above?');
    return (c->r*65536) + (c->g*256) + (c->b);
}
int Ux::indexForColor(ColorList* cli){
    return indexForColor(&cli->color);
}

Ux::uiObject* Ux::create_loading_screen(void){
    loadingScreen = new uiObject();
    loadingScreen->hasForeground = true;
    //loadingScreen->setBackgroundColor(0, 255, 0, 50); // this renders but we did not set hasBackground.... interesting?/
    loadingScreen->setRoundedCorners(0.5); // collision check will be "circular"
    printCharToUiObject(loadingScreen, CHAR_APP_ICON, DO_NOT_RESIZE_NOW); // the shadow on the icon is typically hidden by the black background... 
    rootUiObject->addChild(loadingScreen);
    loadingScreen->storeControllerCursor = false; // this modal is indistinguishable from root ???
    loadingScreen->setBoundaryRect(0.4, 0.4, 0.2, 0.2);
    loadingScreen->squarify(); // don't skew icon center it in the space "defubed"

    loadingScreen->matrixInheritedByDescendants = true; // allows hacky override matrix way, we should instead just translate the matrix on the object... and then we don't need a seperate render function for loading screen...
    rootUiObject->matrixInheritedByDescendants = true; // allows hacky override matrix way

    updateRenderPositions();
    currentModal = rootUiObject;// used for cusror tracking.... we could remove some nullptr checks elsewhere with this here...
    return rootUiObject;
}

// todo - either tons of 1off create functions, or return and define outside
Ux::uiObject* Ux::create(void){

    rootUiObject->empty();
    rootUiObject->matrixInheritedByDescendants = false;


    screenRenderQuadObj = new uiObject(); // an orphan, it is a root of sorts...
    screenRenderQuadObj->setForegroundColor(255,255,255,128);
    Ux::setRect(&screenRenderQuadObj->textureCropRect,0, 0,1, -1.0);


    mainUiContainer = new uiObject();

    minigamesUiContainer = new uiObject();
    minigamesUiContainer->setBackgroundColor(0, 0, 0, 255);
    minigamesUiContainer->setBoundaryRect(5000.0, 5000.0, 1.0, 1.0);


    minigamesUiContainer->addChild(new uiObject());// this is sort of a hack... index zero is unused in games, but this lets our child index match our game index!
    minigamesUiContainer->childList[0]->hideAndNoInteraction();

    rootUiObject->addChild(mainUiContainer);
    rootUiObject->addChild(minigamesUiContainer);


    minigameCounterText = new uiObject(); // a true orphan, own root
    minigameCounterText->containText=true; // we'd rather not contain text, but we'd have to render all the OOB child objects too...

    returnToLastImgBtn = new uiObject();
    returnToLastImgBtn->hasForeground = true;
    returnToLastImgBtn->setBackgroundColor(0, 0, 0, 50);
    returnToLastImgBtn->setRoundedCorners(0.5); // collision check will be "circular"
    printCharToUiObject(returnToLastImgBtn, CHAR_BACK_ICON, DO_NOT_RESIZE_NOW);
    returnToLastImgBtn->setInteractionBegin(&Ux::interactionBeginReturnToPreviousSurface);
    returnToLastImgBtn->setClickInteractionCallback(&Ux::interactionReturnToPreviousSurface);
    mainUiContainer->addChild(returnToLastImgBtn);
    returnToLastImgBtn->hideAndNoInteraction(); // initially hidden....
    returnToLastImgBtn->storeControllerCursor = false; // this modal is indistinguishable from root ???

    bottomBar = new uiObject();
    bottomBar->setBackgroundColor(0, 0, 0, 198);
    mainUiContainer->addChild(bottomBar);
    bottomBar->setInteractionCallback(Ux::interactionNoOp); // cannot click through this bar...

    huePicker = new uiHueGradient(mainUiContainer, Float_Rect(0.0, 0.60, 1.0, 0.09));
    huePicker->resize(Float_Rect(0.0, 0.7, 1.0, hue_picker));


    pickSourceBtn = new uiObject();
    pickSourceBtn->setForegroundColor(255, 255, 255, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
//    pickSourceBtn->hasBackground = true;
//    Ux::setColor(&pickSourceBtn->backgroundColor, 32, 0, 0, 128);
    pickSourceBtn->setClickInteractionCallback(&Ux::interactionFileBrowserTime); // TODO rename me
 //    printCharToUiObject(pickSourceBtn, '+', DO_NOT_RESIZE_NOW);
    printCharToUiObject(pickSourceBtn, CHAR_OPEN_FILES, DO_NOT_RESIZE_NOW);
    pickSourceBtn->squarify();


    pickScreenBtn = new uiObject();
#ifdef PICK_FROM_SCREEN_ENABLED
    pickScreenBtn->setForegroundColor(255, 255, 255, 255);
    pickScreenBtn->setClickInteractionCallback(&Ux::interactionPickFromScreen); // TODO rename me
# ifdef __EMSCRIPTEN__
     printCharToUiObject(pickScreenBtn, '@', DO_NOT_RESIZE_NOW); // totall different than pick from fullscreen mode, but triggers closest thing achievable...
# else
     printCharToUiObject(pickScreenBtn, '+', DO_NOT_RESIZE_NOW);
# endif
 //   printCharToUiObject(pickScreenBtn, CHAR_OPEN_FILES, DO_NOT_RESIZE_NOW);
    pickScreenBtn->squarify();

    uiObject* pickScreenBtnSub = new uiObject();
    pickScreenBtnSub->setForegroundColor(255, 255, 255, 255);
    //pickScreenBtnSub->setBackgroundColor(255, 0, 0, 128);
    printCharToUiObject(pickScreenBtnSub, CHAR_MOUSE_CURSOR, DO_NOT_RESIZE_NOW);
    pickScreenBtnSub->setBoundaryRect(0.42, 0.42, 0.5, 0.5);
    pickScreenBtn->addChild(pickScreenBtnSub);
#endif

    addHistoryBtn = new uiObject();
    addHistoryBtn->setForegroundColor(255, 255, 255, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
//    addHistoryBtn->hasBackground = true;
//    Ux::setColor(&addHistoryBtn->backgroundColor, 32, 0, 0, 128);
    addHistoryBtn->setClickInteractionCallback(&Ux::interactionAddHistory); // TODO rename me
    printCharToUiObject(addHistoryBtn, CHAR_CIRCLE_PLUSS, DO_NOT_RESIZE_NOW);
    addHistoryBtn->squarify(); // "keep round"
    addHistoryBtn->setRoundedCorners(0.5);
    addHistoryBtn->controllerInteractionKeyupOnly = true;


    optionsGearBtn = new uiObject();
    optionsGearBtn->setForegroundColor(240, 240, 240, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
 //        optionsGearBtn->hasBackground = true;
 //        Ux::setColor(&optionsGearBtn->backgroundColor, 32, 0, 0, 128);
 //    optionsGearBtn->setClickInteractionCallback(&Ux::interactionAddHistory); // TODO rename me
    printCharToUiObject(optionsGearBtn, CHAR_GEAR_ICON, DO_NOT_RESIZE_NOW);
    optionsGearBtn->squarify(); // "keep round"
    optionsGearBtn->setRoundedCorners(0.5);


    bottomBarRightStack = new uiObject();
    bottomBarRightStack->testChildCollisionIgnoreBounds = true; // need to check clicks on child objects.......

//    bottomBarRightStack->hasBackground = true;
//    Ux::setColor(&bottomBarRightStack->backgroundColor, 255, 0, 0, 128);


 //     stacked bottom bar elements...
    bottomBarRightStack->addChildStackedRight(addHistoryBtn);
#ifdef PICK_FROM_SCREEN_ENABLED
    bottomBarRightStack->addChildStackedRight(pickScreenBtn);
#endif
    bottomBarRightStack->addChildStackedRight(pickSourceBtn);
    bottomBarRightStack->addChildStackedRight(optionsGearBtn);

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
    //zoomSlider->isDebugObject=true;
    zoomSlider->setBackgroundColor(255, 255, 255, 255);
    Ux::setColor(&zoomSlider->foregroundColor, 0, 0, 0, 50); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
    zoomSlider->setInteraction(&Ux::interactionHZ);//zoomSlider->canCollide = true;
 //    printCharToUiObject(zoomSlider, '^', DO_NOT_RESIZE_NOW);
 //    printCharToUiObject(zoomSlider, CHAR_VERTICAL_BAR_POINTED, DO_NOT_RESIZE_NOW);
    printCharToUiObject(zoomSlider, CHAR_ZOOM_PLUSS, DO_NOT_RESIZE_NOW);
    zoomSlider->setRoundedCorners(0.486);
    zoomSlider->setAnimationPercCallback(&OpenGLContext::setFishScalePerentage);
    zoomSliderHolder->addChild(zoomSlider);


    runner = new uiRunningMan(zoomSlider, Float_Rect(1.0,-0.05,1.0,1.0));



    historyPreviewHolder = new uiObject();
    historyPreviewHolder->setBackgroundColor( 0, 0, 0, 192);


    historyPreview = new uiObject();
 //    historyPreview->hasForeground = true;
//    historyPreview->hasBackground = true;
//    Ux::setColor(&historyPreview->backgroundColor, 0, 0, 0, 192);
 //    Ux::setColor(&historyPreview->foregroundColor, 255, 255, 255, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
 //    historyPreview->doesInFactRender = false;
 //    historyPreview->setInteraction(&Ux::interactionHZ);
 //    historyPreview->canCollide = true;

 //     we should another way to set rects for animation support?

    int ctr = SIX_ACROSS;
    while( --ctr >= 0 ){
        uiObject * colora = new uiObject();
        colora->hasBackground = true;
        Ux::setColor(&colora->backgroundColor, 255, 0, 32, 0);
        historyPreview->addChild(colora);
    }

 //    historyPreview->hasForeground = true;
 //    printCharToUiObject(historyPreview, '_', DO_NOT_RESIZE_NOW);
 //    historyPreview->canCollide = true; // set by setInteractionCallback
    historyPreviewHolder->setInteraction(&Ux::interactionVert);
    historyPreviewHolder->interactionNonController=true;

 //    historyPreviewHolder->interactionProxy = historyPalleteEditor->historyPalleteHolder; // when we drag the preview effect the fullsize sliding into view...
 //    printCharToUiObject(historyPreview, 'H', DO_NOT_RESIZE_NOW);
 //    bottomBar->addChild(historyPreview);
    historyPreviewHolder->addChild(historyPreview);
    mainUiContainer->addChild(historyPreviewHolder);


    //historyPreviewHolder->addChild(previewMessage);
    //previewMessage->setChildNodeDirection(TEXT_DIR_ENUM::LTR, true); // this superceedes containText in so many ways... but text won't rotate (did it ever?)


//    historyPreviewHolder->setBoundaryRect(0.0, 1.0-history_preview, 1.0, history_preview);
//    historyPreviewHolder->setInteraction(&Ux::interactionVert);
//        historyPreview->setChildNodeDirection(TEXT_DIR_ENUM::RTL, true);





#ifndef OMIT_SCROLLY_ARROWS
    movementArrows = new uiNavArrows(mainUiContainer, Float_Rect(0.0, clock_bar + 0.27777777777778, 1.0, 0.38), &Ux::interactionDirectionalArrowClicked);
#endif
    curerntColorPreview = new uiViewColor(mainUiContainer, Float_Rect(0.0, clock_bar, 1.0, 0.27777777777778), false);



    historyPalleteEditor = new uiHistoryPalleteEditor(mainUiContainer);

 //     late setup of these, we have historyPalleteEditor now....
    historyPreviewHolder->interactionProxy = historyPalleteEditor->historyPalleteHolder; // when we drag the preview effect the fullsize sliding into view...
    historyPreviewHolder->setInteractionCallback(&historyPalleteEditor->interactionToggleHistory);


    previewMessageOver = new uiObject();

#if defined(__EMSCRIPTEN__) && defined(COLORPICK_BUILD_FOR_EXT)
    uiObject* previewMessage = new uiObject();
    //previewMessage->setBoundaryRect(0.0, 0.0, 0.9 * 0.5, 0.1);
    //previewMessage->setBoundaryRect(0.0, 0.0, 0.9 * 0.25, 0.1);// sizing for uiText so strange... 1/4 size same result!  wait whats .1425 for??
    previewMessage->setBoundaryRect(0.0, 0.0, 0.9 * 0.30, 0.1);

    //printStringToUiObject(previewMessage, "Tablet Edition", DO_NOT_RESIZE_NOW);
    //previewMessage->setChildNodeDirection(TEXT_DIR_ENUM::LTR, true); // this superceedes containText in so many ways... but text won't rotate (did it ever?)
    uiText * previewMessageText = new uiText(previewMessage, 0.1425);
    previewMessageText->print(" Web Tablet Edition*");

    SDL_memcpy(previewMessageOver, previewMessage, sizeof(uiObject)); // we shallow clone a UI object this way, nify :)
    // the child nodes (text) are not distinct though, and they cannot render at different locations (no update is called mid render to move them based on a parent obj boundary change)

    uiObject* previewMessage2 = new uiObject();
    previewMessage2->setBoundaryRect(0.0, 0.1, 0.9 * 0.30, 0.1);
    uiText * previewMessage2Text = new uiText(previewMessage2, 0.1425);
    previewMessage2Text->print("* Changes made in this"
                               " history scroller or  "
                               " palete editor below  "
                               " will not be saved in "
                               " this version.        "
                               " See base extension."); // this method also exceeds 128 child objects
    previewMessage2Text->text_itself->textRowsAfter = 22; /// needs to be set before calling setChildNodeDirection
    previewMessage2Text->text_itself->setChildNodeDirection(previewMessage2Text->text_itself->textDirection); /// needs to be after, multi line printing wont' work without this.... either

    historyPalleteEditor->historyScroller->uiObjectItself->addChild(previewMessageOver);

    previewMessageOver->hide();

    historyPalleteEditor->historyScroller->uiObjectItself->addBottomChild(previewMessage);

    historyPalleteEditor->historyScroller->uiObjectItself->addBottomChild(previewMessage2);
#endif

    settingsScroller = new uiSettingsScroller(mainUiContainer);
//    optionsGearBtn->setClickInteractionCallback(&Ux::interactionVisitSettings);
 //     to make this dragable.... you disable the above, and use this instead....
    optionsGearBtn->setInteractionCallback(&Ux::interactionVisitSettings);
    optionsGearBtn->setInteraction(&Ux::interactionVert); // todo: not responsive
    optionsGearBtn->interactionProxy = settingsScroller->uiObjectItself;
    optionsGearBtn->interactionNonController=true;


    defaultYesNoChoiceHolder = new uiObject();
 //    defaultYesNoChoiceHolder->setBoundaryRect(0.0, -1.0, 1.0, 1.0);
    defaultYesNoChoiceHolder->setDefaultHidden();
    /*mainUiContainer*/ rootUiObject->addChild(defaultYesNoChoiceHolder);

 //     top most objects
    defaultYesNoChoiceDialogue = new uiYesNoChoice(defaultYesNoChoiceHolder, Float_Rect(0.0, 0.0, 1.0, 1.0), true);


 //    defaultYesNoChoiceDialogue->uiObjectItself->setAnimation( uxAnimations->slideUp(defaultYesNoChoiceDialogue->uiObjectItself) ); // returns uiAminChain*

    rClickMenu = new uiToolMenu(rootUiObject /*mainUiContainer*/);

    defaultScoreDisplay = new uiScore(rootUiObject /*mainUiContainer*/); // score and some others are always available....

    historyPalleteEditor->addScoreboard(defaultScoreDisplay->buildScoreDisplay());


 //    uiHueGradientScroller* gradTest = new uiHueGradientScroller(mainUiContainer, Float_Rect(0.0,0.0,1.0,0.035), true);

 //    uiText::BOTTOM;



    // FINALLY !!!!!!!!!!!!!!
    controllerCursor = new uiObject();
//    controllerCursor->hasBackground = true;
//    Ux::setColor(&controllerCursor->backgroundColor, 255, 0, 0, 128);
//    controllerCursor->setRoundedCorners(0.486);
//    printCharToUiObject(controllerCursor, CHAR_CIRCLE_PLAIN, DO_NOT_RESIZE_NOW);

    rootUiObject->addChild(controllerCursor);
    controllerCursor->matrixInheritedByDescendants = true; // Must be set AFTER addChild or it just auto inherits this value :)


    float outsetAmount = 0.1;
    uiObject* controllerCursorOutset = new uiObject();
    controllerCursorOutset->setBoundaryRect(-outsetAmount, -outsetAmount, 1.0+outsetAmount+outsetAmount, 1.0+outsetAmount+outsetAmount);
    controllerCursor->addChild(controllerCursorOutset);

    outsetAmount = 0.05;
    uiObject* controllerCursorBorder = new uiObject();
    controllerCursorOutset->setBoundaryRect(-outsetAmount, -outsetAmount, 1.0+outsetAmount+outsetAmount, 1.0+outsetAmount+outsetAmount);
    controllerCursor->addChild(controllerCursorBorder);

	SDL_Color tmp;
	Ux::setColor(&tmp, 0, 0, 0, 128);
    addBordersToObject(controllerCursorOutset, tmp, 0.15);
	Ux::setColor(&tmp, 255, 0, 0, 192);
	addBordersToObject(controllerCursorBorder, tmp, 0.1);

    // CURSOR HINTS: 1) if your cursor is getting all messed up, and you are triggerin an animation which is messing it up, just call:
    //                  myUxRef->temporarilyDisableControllerCursorForAnimation(); where you trigger the animation, your cursor should then fix itself momentarily

    // CUSOR BUGS: TODO
    // 0)
    // CURSOR IMPROVEMENTS:
    // 1) hints and indicators about direction may be optionally stored on and displayed from some objects when entering controllerCursorLockedToObject and hidden otherwise
    //     a) these hints should be dynamic: i.e you can only move to the left initially, but may move either way after you have stated moving ???
    //     b) may be nice to have these auto hide ??  once user starts they can figure it out ?? (simler than having it fully dynamic)
    //     c) may be confusing (user thinks only choise is to slide, when they can also just press enter without movign for a different result...)


    controllerCursorObjects = new uiList<uiObject*, Uint8>(controllerCursorObjectsMax); // controller cursor limit
    disableControllerCursor(); // just hides it


    updateRenderPositions();


    readInState(); // reads saved state into lists, requires historyPalleteEditor


#if defined(__EMSCRIPTEN__) && defined(COLORPICK_BUILD_FOR_EXT)
    // Javascript will return the color history string, or empty string
    const char* result = em_history_loaded(pickHistoryList->total());
    int total = SDL_strlen(result);
    int c = 0;
    int co = 0;
    int charsCtr = 0;
    SDL_Color colorRead;
    if( total > 0 ){
        pickHistoryList->clear();
        char* twoChars = (char*)SDL_malloc( sizeof(char) * 2 );
        for( c=0; c<total; c++ ){
            if( result[c] == '#' ){
                co = 0; // reset color offset...
                charsCtr = 0; // reset chars ctr...
            }else{
                twoChars[charsCtr++] = result[c];
                if( charsCtr > 1 ){ // every two chars...
                    charsCtr = 0;
                    long conversion = SDL_strtoul(twoChars, nullptr, 16);
                    if( co == 0 ){
                        colorRead.r = (Uint8)conversion;co++;
                    }else if( co == 1){
                        colorRead.g = (Uint8)conversion;co++;
                    }else if (co == 2){
                        colorRead.b = (Uint8)conversion;co++;
                    }
                    if( co > 2 ){
                        co = 0;
                        pickHistoryList->add(ColorList(colorRead));
                    }
                }
            }
        }
        updatePickHistoryPreview();
        SDL_free(twoChars);
    }
#endif


    defaultScoreDisplay->updateScoreDisplay();


 //    test cololr history
//#ifdef DEVELOPER_TEST_MODE
//    for( int q=0; q<8192; q++ ){
//        currentlyPickedColor = new SDL_Color();
//        setColor(currentlyPickedColor, randomInt(0,44), randomInt(0,185), randomInt(0,44), 0);
//        addCurrentToPickHistory();
//    }
//#endif

 //    writeOutState(); // testing
 //     for awful reasons during init some things should not be called....
 //    palleteSelectionColorPreview->uiObjectItself->setAnimation( uxAnimations->slideDownFullHeight(palleteSelectionColorPreview->uiObjectItself) ); // returns uiAminChain*
 //     todo we should just chain a interactionTogglePalletePreview
 //    interactionTogglePalletePreview(nullptr, nullptr);
    historyPalleteEditor->interactionToggleHistory(nullptr, nullptr);

    settingsScroller->interactionToggleSettings(nullptr, nullptr);


/*
     typedef struct
     {
     Uint32 flags;                       /// ::SDL_MessageBoxFlags
    SDL_Window *window;                 ///< Parent window, can be NULL
    const char *title;                 // / UTF-8 title
    const char *message;                /// UTF-8 message text

    int numbuttons;
    const SDL_MessageBoxButtonData *buttons;

    const SDL_MessageBoxColorScheme *colorScheme;   //< ::SDL_MessageBoxColorScheme, can be NULL to use system settings
} SDL_MessageBoxData;
*/

//    OpenGLContext* ogg=OpenGLContext::Singleton();
//    int selected;
//    SDL_MessageBoxData messagebox;
//    SDL_MessageBoxButtonData buttons[] = {
//        {   0,  SDL_ASSERTION_RETRY,            "Retry" },
//        {   0,  SDL_ASSERTION_BREAK,            "Break" },
//        {   0,  SDL_ASSERTION_ABORT,            "Abort" },
//        {   SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT,
//            SDL_ASSERTION_IGNORE,           "Ignore" },
//        {   SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT,
//            SDL_ASSERTION_ALWAYS_IGNORE,    "Always Ignore" }
//    };
//
//    SDL_zero(messagebox);
//    messagebox.flags = SDL_MESSAGEBOX_WARNING;
//    messagebox.window = ogg->sdlWindow;
//    messagebox.title = "Assertion Failed";
//    messagebox.message = "test message1";
//    messagebox.numbuttons = SDL_arraysize(buttons);
//    messagebox.buttons = buttons;
//    if (SDL_ShowMessageBox(&messagebox, &selected) == 0) {
//        SDL_Log("itz zero pretty much no matter what");
//        SDL_Log("--- itz %d", selected);
//    }


 //    SDL_ShowMessageBox(const SDL_MessageBoxData *messageboxdata, int *buttonid);

    //SDL_Log("ui create done");

    currentModal = rootUiObject;// used for cusror tracking.... we could remove some nullptr checks elsewhere with this here...
    return rootUiObject;
}

// these borders are not sized dynamically, borderThickness is proportional to the shape of the object
void Ux::addBordersToObject(uiObject* parent, SDL_Color borderColor, float borderThickness){

    // parent->matrixInheritedByDescendants = true;  // ASSERT THIS IS TRUE.... but also if we wish to transform any parent... it has to be true all the way up to that parent...  but as long as it was set on the parent BEFORE children were added, it will be inherited by all child objects

    uiObject* cursor_part;
    glm::mat4 cursor_part_mat; // we use matrix for these so that the size is inherited properly ???

    float cursor_scale = borderThickness;
    float cursor_scale2 = cursor_scale + cursor_scale;

    // top
    cursor_part = new uiObject();
    cursor_part->hasBackground = true;
    Ux::setColor(&cursor_part->backgroundColor, &borderColor);
    cursor_part_mat = glm::mat4(1.0);
    cursor_part_mat = glm::translate(cursor_part_mat, glm::vec3(0.0, 1.0 - cursor_scale, 0.0) );
    cursor_part_mat = glm::scale(cursor_part_mat, glm::vec3(1.0 - cursor_scale2, cursor_scale, 1.0) );
    cursor_part->matrix = cursor_part_mat;
//    printCharToUiObject(cursor_part, '-', DO_NOT_RESIZE_NOW);
    parent->addChild(cursor_part);

    // btm
    cursor_part = new uiObject();
    cursor_part->hasBackground = true;
    Ux::setColor(&cursor_part->backgroundColor, &borderColor);
    cursor_part_mat = glm::mat4(1.0);
    cursor_part_mat = glm::translate(cursor_part_mat, glm::vec3(0.0, -1.0 + cursor_scale, 0.0) );
    cursor_part_mat = glm::scale(cursor_part_mat, glm::vec3(1.0 - cursor_scale2, cursor_scale, 1.0) );
    cursor_part->matrix = cursor_part_mat;
//    printCharToUiObject(cursor_part, '-', DO_NOT_RESIZE_NOW);
    parent->addChild(cursor_part);

    // left
    cursor_part = new uiObject();
    cursor_part->hasBackground = true;
    Ux::setColor(&cursor_part->backgroundColor, &borderColor);
    cursor_part_mat = glm::mat4(1.0);
    cursor_part_mat = glm::translate(cursor_part_mat, glm::vec3(-1.0 + cursor_scale, 0.0, 0.0) );
    cursor_part_mat = glm::scale(cursor_part_mat, glm::vec3(cursor_scale, 1.0, 1.0) );
    cursor_part->matrix = cursor_part_mat;
//    printCharToUiObject(cursor_part, '|', DO_NOT_RESIZE_NOW);
    parent->addChild(cursor_part);

    // right
    cursor_part = new uiObject();
    cursor_part->hasBackground = true;
    Ux::setColor(&cursor_part->backgroundColor, &borderColor);
    cursor_part_mat = glm::mat4(1.0);
    cursor_part_mat = glm::translate(cursor_part_mat, glm::vec3(1.0 - cursor_scale, 0.0, 0.0) );
    cursor_part_mat = glm::scale(cursor_part_mat, glm::vec3(cursor_scale, 1.0, 1.0) );
    cursor_part->matrix = cursor_part_mat;
//    printCharToUiObject(cursor_part, '|', DO_NOT_RESIZE_NOW);
    parent->addChild(cursor_part);

}


void Ux::updateRenderPositions(void){
    rootUiObject->updateRenderPosition();
    //seekAllControllerCursorObjects();
}

void Ux::updateRenderPositions(uiObject *renderObj){
    renderObj->updateRenderPosition();
}



void Ux::seekAllControllerCursorObjects(){
    controllerCursorObjects->clear();

    int scannedObjects = 0;
    rootUiObject->seekControllerCursorObjects(&scannedObjects);

    // next: sort these objects ;) (its done later :)
    //controllerCursorObjects->sort(Ux::compareUiObjectsYpos);

    SDL_Log("Cursor Objects Found: %i", controllerCursorObjects->total());

}

void Ux::storeControllerCursorToModal(){
    uiObject* curModal = currentModal != nullptr ? currentModal : rootUiObject;
    if( curModal->storeControllerCursor ){
        curModal->myModalControllerCursor = *controllerCursorObjects->get(controllerCursorIndex);
    }
}

bool Ux::seekObjectInCursorObjects(uiObject* curObj){
    int found = controllerCursorObjects->locate(curObj);
    if( found >= 0 ){
        controllerCursorIndex = found;
        return true;
    }
    return false;
}

void Ux::refreshControllerCursorObjects(){
    controllerCursorLastMovedAtTimestamp = SDL_GetTicks();

    uiObject* curObj = nullptr;
    uiObject* curModal = currentModal != nullptr ? currentModal : rootUiObject;

    if( curModal->myModalControllerCursor != nullptr ){
        curObj = curModal->myModalControllerCursor;
    }else if( controllerCursorObjects->total() > 0 ){
        curObj = *controllerCursorObjects->get(controllerCursorIndex);
    }

    seekAllControllerCursorObjects();
    if( controllerCursorObjects->total() > 0 ){

        if( curObj != nullptr ){
            if( !seekObjectInCursorObjects(curObj) ){
                if( !seekObjectInCursorObjects(curObj->interactionProxy) ){
                    if( !seekObjectInCursorObjects(curObj->parentObject) ){
                        int index = 0;
                        int total = curObj->getChildCount();
                        while( index < total && !seekObjectInCursorObjects(curObj->childList[index]) ){
                            index++; // still didn't find it!
                        }
    //                    if( index == total ){
    //                        // still didn't find it!
    //                    }
                    }
                }
            }
        }

        // mayhaps we should just reset the index to zero? (when entering modal and not in this mode?)   thts what this does someitimes
        controllerCursorIndex = controllerCursorObjects->validateIndexLooping(controllerCursorIndex);

        storeControllerCursorToModal();
    }
}
void Ux::toggleControllerCursor(){
    if(controllerCursorModeEnabled){
        disableControllerCursor();
    }else{
        enableControllerCursor();
    }
}
void Ux::enableControllerCursor(){
    refreshControllerCursorObjects();
    controllerCursorTemporarilyDisabledForAnimatedChange=false;
    if( controllerCursorObjects->total() > 0 ){
        controllerCursorModeEnabled = true;
        controllerCursorLockedToObject = false;
        controllerCursor->show();
    }
}
void Ux::temporarilyDisableControllerCursorForAnimation(){
    // maybe we only want to be notified abut a specific anim?? we wait for all of them though :)
    // this will even ADD a 1s animation if NONE are found, so ADD YOUR ANIM FIRST
    if( controllerCursorModeEnabled ){
        if( !uxAnimations->hasAnimations() ){
            // we can push a dummy animation or something... a simple 1 second animation???
            uxAnimations->scale_bounce(rootUiObject);
            // this looks scary, but the matrix of the root is not inherited, and root doens't render
            // so it looks like it has an effect, but the above is "completely free" currently (sans matrix maths....)....
            // maybe we can pick another object or different animation to debug/detect when this branch is triggered though!
            // except for when exiting the minigame I'd consider reachng this code to be a bug, this is just a workaround
        }
        controllerCursorTemporarilyDisabledForAnimatedChange = true;
        disableControllerCursor();
    }
}
void Ux::disableControllerCursor(){
    if( controllerCursorLockedToObject ){
        currentInteractions[0].canceled = true; // helps click handlers (and other interactionCallbaks) know this was NOT a click
        selectCurrentControllerCursor(0); // exists this mode and releases the object.... this zero may have no effect in this locked mode...
    }
    controllerCursorModeEnabled = false;

    controllerCursor->hide();
}
void Ux::navigateControllerCursor(int x, int y){

    if( controllerCursorLockedToObject ){ // TODO also test our currentInteractions[0] not done????
        Uint32 ticks = SDL_GetTicks();
        float ageMulti = currentInteractions[0].ageMultiplier(ticks);

        //we are just fiddling with a given object now.... fun stuffs...
        currentInteractions[0].update(ticks,
                                      currentInteractions[0].px - (colorPickState->convertToScreenXCoord(x) * ageMulti),
                                      currentInteractions[0].py - (colorPickState->convertToScreenYCoord(y) * ageMulti)
                                      );
        moveLockedControllerCursor();
        return;
    }
    controllerCursorLastMovedAtTimestamp = SDL_GetTicks();
    bool isY = y != 0;
    uiObject* curObj = *controllerCursorObjects->get(controllerCursorIndex);

    controllerCursorObjects->sort(isY ? Ux::compareUiObjectsYpos : Ux::compareUiObjectsXpos);

    controllerCursorIndex = controllerCursorObjects->locate(curObj);

    int startControllerCursorIndex = controllerCursorIndex;
    uiObject* newObj = curObj;

    // y or x will be zero hence + (y+x)  In any case at it's core this function need only call this, and even defining newObj is pointless - except to get the BEST POSSIBLE match
#define whatToDoToGetNextTestObj \
    controllerCursorIndex = controllerCursorObjects->validateIndexLooping(controllerCursorIndex + (y+x)); \
    newObj = *controllerCursorObjects->get(controllerCursorIndex);

    // finding an object that actually advances in desired direction....
    while( (isY && curObj->collisionRect.centerY() == newObj->collisionRect.centerY()) || (!isY && curObj->collisionRect.centerX() == newObj->collisionRect.centerX()) ){
        whatToDoToGetNextTestObj
        if( newObj == curObj ){ // hmm didn't use startControllerCursorIndex ??
            whatToDoToGetNextTestObj
            break; // we could alternatively just check there is more than one obejct at differnt x or y coordinate... point being we should move to another object and we should not wrap back around... and stop
        }
    }

    // find the best matching object out of the objects perpendicular to the desired direction; that is within; say '0.2f' of the closest directional match
    if( controllerCursorObjects->total() > 2 ){
        float startPoint = isY ? curObj->collisionRect.centerY() : curObj->collisionRect.centerX();
        float startDimension = isY ? curObj->collisionRect.h : curObj->collisionRect.w;
        float stopPoint = isY ? newObj->collisionRect.centerY() : newObj->collisionRect.centerX();
        float objectDistance = SDL_fabs(startPoint - stopPoint);
        if( objectDistance < startDimension ){
            objectDistance = startDimension + 0.01; // make sure we search at LEAST our start dimension...
        }
        //SDL_Log("Object distance as compared with 0.2 : %f ", objectDistance);

        startControllerCursorIndex = controllerCursorIndex;
        float bestDist = 999999.0f;
        int bestIndex = controllerCursorIndex;
        float nextDistance;
#define computeNextDistance\
        nextDistance = SDL_fabs( isY ? startPoint - newObj->collisionRect.centerY() : startPoint - newObj->collisionRect.centerX() );

        computeNextDistance

        while( nextDistance <= objectDistance /*0.2f*/ ){ // NOTE this is how far we would go to stay on the same plane we are moving in vs jumping laterally to a different object....

        // anyway rename dist to otherDimensionDist we need to clarify varible names to trace this better..

        // indeed its still bad, eg moving laterally X to X that Y is selected even when NOT on bottom row...
        // X   X
        // X   X
        // X   X
        //   Y

        // so in the origional issue
        //   Y   Y
        //     X
        //   Y   Y
        // so next imagien it gets even WORSER to detect...
        //   Y               Y
        //           X
        //   Y               Y

        // anyway the plan is one of two:
        // somehow allow a diagonal move mode OR give some alternate mode when moving right or down so it works differently than left or up, such that X is selectable in
        // conditions when surrounded by Y's
        // and create a completely seperate minigame that can be used to stage these scenarios, to help verify them.... along with a way to trigger the minigame at launch?
        // *(maybe some scenarios need a custom game to produce the intended testing difficulties...)

        // act

            if( newObj != curObj ){ // we looped, so this mode is fruitless ?
                float dist = SDL_fabs( isY ? curObj->collisionRect.centerX() - newObj->collisionRect.centerX() : curObj->collisionRect.centerY() - newObj->collisionRect.centerY() );
                if( dist < bestDist ){
                    bestDist = dist;
                    bestIndex = controllerCursorIndex;
                }
            }else{
                //bestIndex = startControllerCursorIndex;
                //SDL_Log("TOO FAR"); // except we probably advanced columsn /rows here
                break;
            }

            whatToDoToGetNextTestObj
            computeNextDistance

        }
        controllerCursorIndex = bestIndex;
    }

    storeControllerCursorToModal();
}


void Ux::moveLockedControllerCursor(){

    uiObject* curObj = *controllerCursorObjects->get(controllerCursorIndex);


    if(curObj->hasInteraction){

        interactionUpdate(&currentInteractions[0]);

        // OR just call interactionUpdate ???

    }
}

void Ux::selectCurrentControllerCursor(Uint32 down_timestamp){
    // this is to either "activate and enter" soem fine control choices for the selection (movement mode)
    // OR simply to click it... depends on the object and interactions contained!

    // First lets get teh simple click working!
    uiObject* curObj = *controllerCursorObjects->get(controllerCursorIndex);

    if( !controllerCursorLockedToObject ){

        if( down_timestamp > 0 ){
            if( controllerCursorLastMovedAtTimestamp >= down_timestamp ){
                return; // since enter was pressed, well it got canceled... by a cursor movement or rescan event
            }
            if( curObj->controllerInteractionKeyupOnly ){
                return; // skip keydown
            }
        }else{ // keyup event (typically we ignore these, since we trigger the event on keydown)
            if( !curObj->controllerInteractionKeyupOnly ){
                refreshControllerCursorObjects(); // we can still refresh the view now, it may have changed,
                // TODO there are other caess for the keyup only objects, where we may too wish to rescan the view!
                return;  // this isn't our special key up event object... ignore it
            }
        }

        currentInteractions[0].begin(SDL_GetTicks(), curObj->collisionRect.x + (curObj->collisionRect.w * 0.5), curObj->collisionRect.y + (curObj->collisionRect.h * 0.5));

        // NOT sure why we really call this, but it does set up some things....
        // we MIGHT AS WELL trust currentInteractions[0] to be a finger though (and not a controller...)
        triggerInteraction(&currentInteractions[0]);
        if( currentInteractions[0].interactionObject != curObj ){
            SDL_Log("SEEMS LIKE AN ERROR< we didn't collide with our object!");
            // THIS PROBALBY MEANS we have an interaction proxy :)! ??
        }

        currentInteractions[0].isSecondInteraction = true; // AWKWARD TO DOUBLE TAP...
    }else{

    }

    // NEED: to determine - should some of these be ignored in keyboard mode (YES FOR THE LOVE OF GOD)
    if( curObj->hasInteraction && !curObj->interactionNonController ){

        controllerCursorLockedToObject = !controllerCursorLockedToObject;

        if( !controllerCursorLockedToObject ){

            // should we still call interaction update?? probably..... or ohterwise slow it down velocity wize....
            currentInteractions[0].update(SDL_GetTicks());

            bool result = interactionComplete(&currentInteractions[0]); // better for delegating the interactionProxy ?

            //enableControllerCursor(); // rescan... should defer this ? hasAnimCb ? (not all will)

            //controllerCursor->cancelCurrentAnimation(); // doesn't end gracefully...

            
        }else{
            newCursorAnimation();
        }



        // changes apperance ???

        // OR just call interactionUpdate ??? (now done elsewhere)

    }else{

        bool result = interactionComplete(&currentInteractions[0]); // better for delegating the interactionProxy ?
        //if( down_timestamp == 0 && !controllerCursorLockedToObject ) refreshControllerCursorObjects(); <- for keyup only objects, we probably want to do this now?
        // problem being, the arg down_timestamp could be two different things... if we are exiting controllerCursorLockedToObject we SHOULD never be in this else, but you never know right?

        //enableControllerCursor(); // rescan... should defer this ? hasAnimCb ? (not all will)

    }
    // 2x above, in hasInteractionCb
    //disableControllerCursor(); // seems it depends MUCH on what we just did, and IF/WHEN the action is complete...
    // some knowings :  1) if our action changes a modal, we should just refresh when that modal is fully shown...
    //                  2) if our action were to disableControllerCursor.... we would not enable it again I htink....
    //}
}

void Ux::newCursorAnimation(){

    if( controllerCursorLockedToObject ){

        uiAminChain* myAnimChain = uxAnimations->scale_bounce(controllerCursor);

        //myAnimChain->lastAnimation()->setAnimationCallback(Ux::CursorAnimationCompleted);

        myAnimChain->addAnim((new Ux::uiAnimation(controllerCursor))->setAnimationReachedCallback(Ux::CursorAnimationCompleted) );


        controllerCursor->setAnimation(myAnimChain);
    }

}

//static
void Ux::CursorAnimationCompleted(uiAnimation* uiAnim){
    // loops
    Ux* myUxRef = Ux::Singleton();
    myUxRef->newCursorAnimation();
    //uiAnim->myUiObject
    //uiScrollController* self = uiAnim->myUiObject->myScrollController;
    //self->animConstrainToScrollableRegion(); // careful - ani is updating (though our current animation just completed)
}

void Ux::updateControllerCursorPosition(bool animationsJustCompleted){

    if( animationsJustCompleted ){
        if( controllerCursorTemporarilyDisabledForAnimatedChange ){
            enableControllerCursor();
        }else if( controllerCursorModeEnabled ){
            //refreshControllerCursorObjects(); //  <- should be pretty safe to call whenever objects are added.... messes up key repeats... better to call later...
        }
    }

    //TODO: we could just ALWAS set this rect before rendering.... if in this mdoe anyway... but we may still need to know when to rescan....
    if(controllerCursorModeEnabled && controllerCursorObjects->total() > controllerCursorIndex ){

        uiObject* curObj = *controllerCursorObjects->get(controllerCursorIndex);
        controllerCursor->boundryRect.setRectConstrainedToUnit(&curObj->collisionRect);
        controllerCursor->origBoundryRect.setRect(&controllerCursor->boundryRect);
        //Ux::setRect(&controllerCursor->renderRect, &curObj->renderRect);
        controllerCursor->updateRenderPosition();
        // we we haev child objects... might as well just set the first two and call update no?
    }
}

void Ux::DebugPrintAllCursorPositions(uniformLocationStruct *uniformLocations){
    // the following debug code renders ALL the cursor positions
    if( controllerCursorModeEnabled && controllerCursorObjects->total() > 0 ){
        int previousControllerCursorIndex = controllerCursorIndex;
        for( int i=0; i<controllerCursorObjects->total(); i++ ){
            controllerCursorIndex = i;
            updateControllerCursorPosition(false);
            printCharToUiObject(controllerCursor, (char)(48+(i%10)), DO_NOT_RESIZE_NOW);
            renderObjects(uniformLocations, controllerCursor, glm::mat4(1.0f));
        }
        controllerCursorIndex = previousControllerCursorIndex;
        printCharToUiObject(controllerCursor, CHAR_CIRCLE_PLAIN, DO_NOT_RESIZE_NOW);
    }
}

// so should the print functions move into uiObject?
// maybe just be static Ux:: member functions defined the same way as others

void Ux::printStringToUiObject(uiObject* printObj, const char* text, bool resizeText){
 //     this function will print or update text, adding characters if needed
 //     the object must be empy (essentially to become a text container and no loner normal UI object)


 //     todo possibly create container on fly if nonempty?
 //     see "NEW rule" where it basically defines how to maybe go about this....

 //     TODO these are global constants


    int ctr=0;
    int charOffset = 0;
    const char * i;
    uiObject* letter;

    long len=SDL_strlen(text);

    float letterWidth = ((1.0 - printObj->containTextPadding - printObj->containTextPadding) / len);// + printObj->textSpacing;
 //    float letterWidth = ((1.0 - printObj->containTextPadding - printObj->containTextPadding - printObj->textSpacing) / len);// + printObj->textSpacing;
    float letterSpacing = letterWidth; //((1.0 - printObj->containTextPadding - printObj->containTextPadding + printObj->textSpacing) / len);
    float firstOffset = printObj->containTextPadding;// - ((printObj->textSpacing * 0.5));
    float vertOffset = 0.5 - (letterWidth*0.5);
    float letterHeight = letterWidth;

//    if( letterSpacing > SDL_fabsf(printObj->textSpacing) ){
//        letterSpacing += printObj->textSpacing;
//    }

    if( !printObj->containText ){
        letterWidth = ((1.0 * len) - (printObj->textPadding.x * 2.0)) / len;

        firstOffset = printObj->textPadding.x;

        vertOffset = printObj->textPadding.y;

        // we want to keep width/height the same so we keep our square (if we are already square)

        letterHeight = 1.0 - (printObj->textPadding.y * 2);

        if( letterHeight < letterWidth ){
//            float scaler = letterHeight / letterWidth;
//            letterWidth *= scaler; // this really just made teh width equal the height...
            letterWidth = letterHeight;
            firstOffset = ((1.0 * len) - (letterWidth*len)) * 0.5;

        }else if(letterHeight > letterWidth){
            letterHeight = letterWidth;
            vertOffset = (1.0 - letterHeight) * 0.5;
        }

        letterSpacing = letterWidth;

//        if( printObj->textPadding.x > 0 ){
//            SDL_Log("Leter width %f %f", letterWidth, letterHeight);
//        }
    }



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
                letter->setBoundaryRect( firstOffset+(ctr*letterSpacing), vertOffset, letterWidth, letterHeight);  /// TODO move size components into function to calculate on window rescale bonus points
            }

            printObj->addChild(letter);
        }

        if( printObj->containText == true ){
            // LTR default, see below - maybe we can check if we aren't TEXT_DIR_ENUM::NO_TEXT
            // move this right out of the else aboe, since we should fit any len text within, and also needs resize IF text len changed.... !?! easy compute

            letter->setBoundaryRect( firstOffset+(ctr*letterSpacing), vertOffset, letterWidth, letterHeight);  /// TODO move size components into function to calculate on window rescale bonus points for suqare?


        }

        letter->hasForeground = true; // printObj->hasForeground;
        letter->hasBackground = printObj->hasBackground;
        Ux::setColor(&letter->backgroundColor, &printObj->backgroundColor);
        //letter->lastBackgroundColor = letter->backgroundColor; // initializations
        Ux::setColor(&letter->foregroundColor, &printObj->foregroundColor);
        letter->doesInFactRender = true;

        //  // we should very likely inherit styles from parent (printObj)
        ///  whenever text changes

        Ux::printCharOffsetUiObject(letter, charOffset);
        //        renderedletters[renderedLettersCtr++] = letter;
        ctr++;
    }

    printObj->isTextParentOfTextLength = ctr; // this influences how we compute the collision rect...
    if( printObj->textDirection == TEXT_DIR_ENUM::NO_TEXT ){
        printObj->textDirection = TEXT_DIR_ENUM::LTR;
    }
    //printObj->setChildNodeDirection(printObj->textDirection); // some printings should not do this.... breaks things in confirmation dialogue..

 //     we just printed a shorter string than we did previously, hide any allocated letteres beyond current str len
 //     it is also possible extra letters need to be set to offeset 0 to render space?
    for( int l=printObj->childListIndex; ctr<l; ctr++ ){
        printObj->childList[ctr]->doesInFactRender = false;

//        Ux::setRect(&printObj->childList[ctr]->textureCropRect,
//                    0, // space is asci 32 0,0
//                    0,
//                    xStep,yStep);
    }

 //            hexValueText->setChildNodeDirection(TEXT_DIR_ENUM::RTL); // see TEXT_DIR_ENUM


    if( resizeText ){
        // we must rebuild this object otherwise strange effects
        // unless it has been printed before at this length
        // not needed during first init where we build once at end...
        // also not needed during normal course of printing a second time the same text length....
        // updateRenderPositions();
        updateRenderPositions(printObj);

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
        updateRenderPositions(letter->parentObject);
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



uiInteraction* Ux::interactionForPointerEvent(SDL_Event* event){

    // it may not be a pointer event afterall... so watch out!
    // for mouse, we may look at     Uint32 which;       /**< The mouse instance id, or SDL_TOUCH_MOUSEID */
    // to support multi mouse
    // but for multi touch......

    int fingerDevice = 0; // SDL supports 64 bits of finger devices (and 32 bits of mouse devices...), we support maybe.. 10 ?

    int touchIndex = -1;

    if( event->type == SDL_FINGERDOWN || event->type == SDL_FINGERMOTION || event->type == SDL_FINGERUP ){
         fingerDevice = (int)event->tfinger.fingerId;
    }// else if its mouse, look at which (see above) for multi mouse support :)

    // origionally we used this, and it worked on android...
    // problems include: fingerDevice could be negative ( we had no guard )
    // ios fingerId's are all over the place
//
//    SDL_Log("Finger device: %i", fingerDevice);
//
//    if( fingerDevice >= MAX_SUPPORTED_FINGERS_MICE ){
//        //fingerDevice = 0;// MAX_SUPPORTED_FINGERS_MICE - 1;
//        fingerDevice = MAX_SUPPORTED_FINGERS_MICE - 1;
//    }
//
//    if( fingerDevice < 0 ){
//        fingerDevice = 0;
//    }
//    return &currentInteractions[fingerDevice];

    for( int f=0; f<MAX_SUPPORTED_FINGERS_MICE; f++ ){
        // search for finger...
        if( currentInteractions[f].fingerId == fingerDevice ){
            touchIndex = f;
            break;
        }
    }

    if( touchIndex < 0 ){
        // guess it's a new finger...

        for( int f=0; f<MAX_SUPPORTED_FINGERS_MICE; f++ ){
            // for a new interaction??
            if( !currentInteractions[f].fingerStateDown ){
                currentInteractions[f].fingerId = fingerDevice;
                touchIndex = f;
                break;
            }
        }

        if( touchIndex < 0 ){
            //SDL_Log("too many fingers?");
            touchIndex = 0;
        }
    }


    //SDL_Log("Finger id: %i", touchIndex);

    return &currentInteractions[touchIndex];

    // NOTE; if the interaction already has fingerStateDown then a buttion is ALREADY pressed....... (handled elsewhere but good to know)
}

bool Ux::triggerInteraction(uiInteraction* which, bool isStart){ // mouseup, mouse didn't move

 //SDL_Log("trigger interaction reached for %d", which->fingerId);

    bool hasInteraction = objectCollides(which);

    uiObject* iinteractionObject = (uiObject*)which->interactionObject;

 //    interactionObject  currentInteraction
    if( hasInteraction && isStart && iinteractionObject != nullptr ){
        if( which->lastInteractionObject == iinteractionObject ){
            // second click
            which->isSecondInteraction=true;
        }
        if( iinteractionObject->interactionBeginCallback != nullptr ){
            iinteractionObject->interactionBeginCallback(iinteractionObject, which);
        }


    }
//    else{
//        which->lastInteractionObject = iinteractionObject;
//    }

    which->didCollideWithObject = hasInteraction;

    return hasInteraction;
}

// by default, its the start of interaction when this is called...
bool Ux::triggerInteraction(uiInteraction* which){ // mouseup, mouse didn't move
    return triggerInteraction(which, true);
}

bool Ux::objectCollides(uiInteraction* which){
    return objectCollides(rootUiObject, which);
}

bool Ux::objectCollides(uiObject* renderObj, uiInteraction* which){

    float x = which->px;
    float y = which->py;

    bool collides = false;
    bool childCollides = false;
 //     update ???? seems handled by interactionUpdate
 //     test collision with object itself, then render all child obj


 //     some objects (font container) may bypass all collision checks....
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

                    // I will argue we want the deepest interaction to tell us if it can be activated currently....
                    // and should not return at once..
                //return collides;
                //collides = true;

                which->isInteracting = true;
                which->interactionObject = renderObj;

            }
        }



        if( renderObj->hasChildren  ){
            // we may transform x and y to local coordinates before sending to child object

            // we need to check the children in reverse, the ones that render last are on top
            for( int ix=renderObj->childListIndex-1; ix>-1; ix-- ){
                // if already has collides and already isInteracting then return it?????
                childCollides = objectCollides(renderObj->childList[ix], which);// || collides;

                if( childCollides ) {
                    return childCollides;
                }
            }
        }

    }

 //     finally if we perfomed a translation for object position
 //     reset that translation

    return collides;
}

// TODO this needs uiInteraction *delta arg now.... which is jsut better than float all around anyway right?.0
// we should split this out, this is for wheeel interaction only....
void Ux::wheelOrPinchInteraction(uiInteraction *delta, float wheeldelta){
    uiObject* iinteractionObject = (uiObject*)delta->interactionObject;
    if( iinteractionObject == nullptr ) return;
    uiObject* wheelIntObj = iinteractionObject->findWheelOrPinchInteractionObject();
    if( wheelIntObj != nullptr ){
        delta->wheel = wheeldelta; // arguably begin should have alreay set this for us.......
        wheelIntObj->interactionWheelCallback(wheelIntObj, delta);
    }
}

// todo investigate dropping this function?  much of this should be split into a different file otherwise
bool Ux::bubbleCurrentInteraction(uiObject *interactionObj, uiInteraction *delta){ // true if we found a match
    uiObject* anObj = interactionObj->bubbleInteraction();
    //uiObject* anObj = ((uiObject*)(delta->interactionObject))->bubbleInteraction(); // note, maybe the first arg is misguided - it may not match anyway depending on where we call and which obj we provide... we could use this instead...
    if( anObj != nullptr ){
        delta->interactionObject = anObj; // this event's object changed...
        return true;
    }
    return false;
}

bool Ux::interactionUpdate(uiInteraction *delta){
 //     TODO ret bool determine modification?

    uiObject* iinteractionObject = (uiObject*)delta->interactionObject;
    uiObject* orig = iinteractionObject;

    if( delta->isInteracting && (iinteractionObject->hasInteraction || iinteractionObject->interactionCallback ) ){

        // IF the object has a 'drop interaction' function and we can instead gift the animation to the parent object if it collides..... we will do so now....
        if( iinteractionObject->shouldCeaseInteractionChecker == nullptr ||
           iinteractionObject->shouldCeaseInteractionChecker(iinteractionObject, delta) ){
            // if we did cease interaction, it is a point of note that interactionObject (on delta) just changed
            if( orig == (uiObject*)delta->interactionObject ){
                // however it is also possible we have the same object here, and also possible we merly have interactionCallback
                if( iinteractionObject->hasInteraction ){
                    iinteractionObject->interactionFn(iinteractionObject, delta);
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

    uiObject* origInteractionObj = (uiObject*)delta->interactionObject;
    delta->interactionObject = nullptr;
    triggerInteraction(delta, false); // interaction obj might change!!!!

    if( !delta->interactionObject || origInteractionObj != (uiObject*)delta->interactionObject ){

        // to avoid teh mouseup on object becomming the lastInteractionObject (and appear to be a real click) we reset this
        delta->interactionObject = origInteractionObj;

        if( !origInteractionObj || origInteractionObj->interactionCallbackTypeClick ){
            return false; // if interaction type is click and we moved objects; or if we never had an origional click object
        }
    }

 //     really we should consider storing 2 seperate callbacks - interaction complete is really quite different from click callback
 //     and in some cases we may need both....

 //     so in some cases, the interaction is complete even if the object changed!?!

// hte above ramblings..... argue for somethign akin to setInteractionBegin
// right now the setInteractionCallback could mean click OR end but not both
    // if it was click and our object changed we won't reach this code either....

    uiObject* iinteractionObject = (uiObject*)delta->interactionObject;

    if( delta->isInteracting && iinteractionObject->hasInteractionCb && iinteractionObject->interactionCallback != nullptr ){
        delta->isInteracting = false;
        iinteractionObject->interactionCallback(iinteractionObject, delta);
        return true;
    }

    return false;
}


//static
void Ux::interactionHistoryEnteredView(uiObject *interactionObj){

    Ux* self = Ux::Singleton();
 //    self->historyScroller->allowUp = true;
    self->updatePickHistoryPreview();  /// this is too much update right?
    self->historyPalleteEditor->palleteScroller->updateTiles();
}

//static
void Ux::interactionNoOp(uiObject *interactionObj, uiInteraction *delta){
 //    SDL_Log("INERACTION NOOP REACHED");

}

void Ux::doOpenURL(char* url){ // note spaces and newlines should be auto replaced, but other whitespace may break the URL opening... (platform dependent?)
    OpenGLContext* ogg=OpenGLContext::Singleton();
    ogg->doOpenURL(url);
    //openURL(url); // uses platform specific version from FileChooser.h
}

void Ux::setClipboardText(char* txt){
#ifdef __EMSCRIPTEN__
    emscripen_copy_to_clipboard(txt);
#else
    SDL_SetClipboardText(txt);
#endif
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
    myUxRef->endModal(myUxRef->returnToLastImgBtn);
}

void Ux::interactionBeginReturnToPreviousSurface(uiObject *interactionObj, uiInteraction *delta){
    Ux* myUxRef = Ux::Singleton();
    myUxRef->uxAnimations->scale_bounce(myUxRef->returnToLastImgBtn, 0.006);
}


void Ux::loadTestImageByIndex(int index){
 //    Ux* myUxRef = Ux::Singleton();
    OpenGLContext* ogg=OpenGLContext::Singleton();
    ogg->loadNextTestImageByIndex(index);
}

void Ux::interactionPickFromScreen(uiObject *interactionObj, uiInteraction *delta){
    Ux* myUxRef = Ux::Singleton();
    myUxRef->uxAnimations->scale_bounce(interactionObj, 0.008);
    OpenGLContext* ogg=OpenGLContext::Singleton();
    ogg->choosePickFromScreen();
}


void Ux::interactionFileBrowserTime(uiObject *interactionObj, uiInteraction *delta){
    Ux* myUxRef = Ux::Singleton();
    myUxRef->uxAnimations->scale_bounce(interactionObj, 0.001);
    OpenGLContext* ogg=OpenGLContext::Singleton();
    ogg->chooseFile();
}

//static
void Ux::interactionVisitSettings(uiObject *interactionObj, uiInteraction *delta){
    Ux* myUxRef = Ux::Singleton();
    myUxRef->uxAnimations->scale_bounce(interactionObj, 0.001);
    myUxRef->settingsScroller->interactionToggleSettings(interactionObj, delta);
}

//static
void Ux::interactionUpgradeFromBasic(uiObject *interactionObj, uiInteraction *delta){
    Ux* myUxRef = Ux::Singleton();
#if __ANDROID__
    myUxRef->doOpenURL((char*)"https://play.google.com/store/apps/details?id=com.vidsbee.colorpicksdl");
#else
    myUxRef->doOpenURL((char*)"https://itunes.apple.com/us/app/colorpick-eyedropper/id1455143862");
#endif
}

//static
void Ux::interactionAddHistory(uiObject *interactionObj, uiInteraction *delta){
    Ux* myUxRef = Ux::Singleton();
    myUxRef->uxAnimations->scale_bounce(interactionObj, 0.001);
    myUxRef->addCurrentToPickHistory();

 //     in the popup, if we were viewing a color, we should hide it next time we open this up.... esp for widescreen
    if( myUxRef->historyPalleteEditor->palleteSelectionPreviewHolder->is_being_viewed_state ) {
        myUxRef->historyPalleteEditor->interactionTogglePalletePreview(myUxRef->historyPalleteEditor->palleteSelectionPreviewHolder, delta);
    }
}

void Ux::hueClicked(SDL_Color* c){
    OpenGLContext* ogg=OpenGLContext::Singleton();
    ogg->pickerForHue(c);
}

void Ux::hueClickedPickerHsv(SDL_Color* c){
    OpenGLContext* ogg=OpenGLContext::Singleton();
    Ux* myUxRef = Ux::Singleton();
    myUxRef->lastHue->fromColor(c);
    //myUxRef->historyPalleteEditor->interactionToggleHistory(myUxRef->interactionObject, &myUxRef->currentInteraction);
    myUxRef->historyPalleteEditor->interactionToggleHistory(nullptr, nullptr);
    ogg->pickerForHue(myUxRef->lastHue, c);
}

// unused...
void Ux::pickerForHuePercentage(float percent){
    OpenGLContext* ogg=OpenGLContext::Singleton();
    Ux* uxInstance = Ux::Singleton();
    ogg->pickerForHue(uxInstance->huePicker->colorForPercent(percent));
}

void Ux::interactionDirectionalArrowClicked(uiObject *interactionObj, uiInteraction *delta){
 //    SDL_Log("directionalllly");
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

void Ux::interactionDragMove(uiObject *interactionObj, uiInteraction *delta){

    interactionObj->cancelCurrentAnimation();

    interactionObj->boundryRect.x += (delta->rx * (1.0/interactionObj->parentObject->collisionRect.w));
    interactionObj->boundryRect.y += (delta->ry * (1.0/interactionObj->parentObject->collisionRect.h));

    interactionObj->updateRenderPosition();

}
//interactionDragReleased
void Ux::interactionDragMoveConstrain(uiObject *interactionObj, uiInteraction *delta){
    Ux* uxInstance = Ux::Singleton();
    uxInstance->interactionDragMoveConstrain(interactionObj, delta, &interactionConstrainToParentObject); // just usign the default callback here...
}

void Ux::interactionDragMoveConstrain(uiObject *interactionObj, uiInteraction *delta, animationCallbackFn customVelocityAnimationAppliedFn){
    Ux* uxInstance = Ux::Singleton();

    uiAminChain* myAnimChain = new uiAminChain();
    myAnimChain->addAnim((new uiAnimation(interactionObj))->initialMoveVelocity(delta->vx, delta->vy) /*->setAnimationCallback(scrollAnimationUpdaterCb)->setGetBoundsFn(scrollAnimationGetBoundsFn)*/ );
    myAnimChain->addAnim((new uiAnimation(interactionObj))->setAnimationReachedCallback(customVelocityAnimationAppliedFn) );
    interactionObj->setAnimation(myAnimChain); // imporrtant to do this before we push it..
    uxInstance->uxAnimations->pushAnimChain(myAnimChain);
}

void Ux::interactionConstrainToParentObject(uiAnimation* uiAnim){
    Ux* uxInstance = Ux::Singleton();
    uxInstance->interactionConstrainToParentObject(uiAnim, nullptr);
}

void Ux::interactionConstrainToParentObject(uiAnimation* uiAnim, animationCallbackFn customViewportConstraintAnimationAppliedFn){
    Ux* uxInstance = Ux::Singleton();
    Ux::uiObject* interactionObj = uiAnim->myUiObject;
    bool needsAnim = false;
    float xDest = interactionObj->boundryRect.x;
    float yDest = interactionObj->boundryRect.y;
    if( interactionObj->boundryRect.x < 0 ){
        xDest = 0.0; needsAnim=true;
    }else if( interactionObj->boundryRect.x + interactionObj->boundryRect.w > 1.0 ){
        xDest = 1.0 - interactionObj->boundryRect.w; needsAnim=true;
    }
    if( interactionObj->boundryRect.y < 0 ){
        yDest = 0.0; needsAnim=true;
    }else if( interactionObj->boundryRect.y + interactionObj->boundryRect.h > 1.0 ){
        yDest = 1.0 - interactionObj->boundryRect.h; needsAnim=true;
    }
    if( needsAnim ){
        uiAminChain* myAnimChain = uxInstance->uxAnimations->moveTo(interactionObj,xDest,yDest, nullptr, nullptr);
        if( customViewportConstraintAnimationAppliedFn != nullptr ){
            myAnimChain->addAnim((new uiAnimation(interactionObj))->setAnimationReachedCallback(customViewportConstraintAnimationAppliedFn) );
        }
        interactionObj->setAnimation(myAnimChain);
    }
}


// this overrides the interface fn?
// this function is a bit odd for now, but is planned to be used to drag and throw objects that will otherwise animiate in, or not animate in based on this - Pane
//static
void Ux::interactionHorizontal(uiObject *interactionObj, uiInteraction *delta){
    Ux* self = Ux::Singleton();

 //    self->historyFullsize // right now there is still some history specific stuff here

    if( interactionObj->interactionProxy != nullptr ){
        interactionObj = interactionObj->interactionProxy;
    }
 //     if we are already panning ....
    interactionObj->cancelCurrentAnimation();

    interactionObj->boundryRect.x += (delta->rx * (1.0/interactionObj->parentObject->collisionRect.w));

    if( delta->rx > 0.003 ){ // moving --down--
        interactionObj->is_being_viewed_state = true;
    }

    if( delta->rx < -0.003 ){
        interactionObj->is_being_viewed_state = false; // toggle still to be called..... on touch release (interactionCallback)
    }

    self->updateRenderPositions(interactionObj);
}

// this overrides the interface fn?
// this function is a bit odd for now, but is planned to be used to drag and throw objects that will otherwise animiate in, or not animate in based on this - Pane
//static
void Ux::interactionVert(uiObject *interactionObj, uiInteraction *delta){
    Ux* self = Ux::Singleton();

 //    self->historyFullsize // right now there is still some history specific stuff here

    if( interactionObj->interactionProxy != nullptr ){
        interactionObj = interactionObj->interactionProxy;
    }
 //     if we are already panning ....
    interactionObj->cancelCurrentAnimation();

    interactionObj->boundryRect.y += (delta->ry * (1.0/interactionObj->parentObject->collisionRect.h));

    if( delta->ry > 0.003 ){ // moving down
        interactionObj->is_being_viewed_state = true;
    }

    if( delta->ry < -0.003 ){
        interactionObj->is_being_viewed_state = false; // toggle still to be called..... on touch release (interactionCallback)
    }

    self->updateRenderPositions(interactionObj);
}

//static   // must have move boundary rect....
void Ux::interactionSliderVT(uiObject *interactionObj, uiInteraction *delta){

    interactionObj->boundryRect.y += (delta->ry * (1.0/interactionObj->parentObject->collisionRect.h));
 //    / we cannot modify the rect directly like this?

 //    computedMovementRect
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
    myUxRef->updateRenderPositions(interactionObj);
}

//static // this seems like interactionSliderHZ ? // must have move boundary rect....
void Ux::interactionHZ(uiObject *interactionObj, uiInteraction *delta){

    interactionObj->cancelCurrentAnimation();

    interactionObj->boundryRect.x += (delta->rx * (1.0/interactionObj->parentObject->collisionRect.w));
 //    / we cannot modify the rect directly like this?

//computedMovementRect
    if( interactionObj->boundryRect.x < interactionObj->computedMovementRect.x ){
        interactionObj->boundryRect.x = interactionObj->computedMovementRect.x;

        delta->fixX(interactionObj->collisionRect,  interactionObj->parentObject->collisionRect);


    }else if( interactionObj->boundryRect.x > interactionObj->computedMovementRect.x + interactionObj->computedMovementRect.w){
        interactionObj->boundryRect.x = interactionObj->computedMovementRect.x + interactionObj->computedMovementRect.w;

        delta->fixX(interactionObj->collisionRect,  interactionObj->parentObject->collisionRect);
    }


// now why do we set the volume?
 //    setVolume

    if( interactionObj->hasAnimCb ){
        interactionObj->animationPercCallback(interactionObj, interactionObj->computedMovementRect.w > 0 ? ((interactionObj->boundryRect.x - interactionObj->computedMovementRect.x) / interactionObj->computedMovementRect.w) : 0.0);
    }

 //    updateRenderPositions();  // WHY NOT MOVE HERE
 //     we need to make a singleton ux and then we can get instance
 //     and call member functions
 //     see OpenGLContext::Singleton

 //    OpenGLContext* ogg=OpenGLContext::Singleton();

    Ux* myUxRef = Ux::Singleton();
 //    myUxRef->updateRenderPositions();
    myUxRef->updateRenderPositions(interactionObj);


//    SDL_Log(" (((((((((((((((((((((((((((((((((((((((((((((((((((((((((((( FIX ME");
//    myUxRef->bottomBar->updateRenderPosition();


 //     currently does not consider width of the object, since given a width it could remain within boundaries, but this is not usefl
 //     for sliders where the center point of the object is all that is considered for positioning (bool isCentered) <-- ? untrue ? planing stage problems
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


//
//
//void Ux::setVolume(uiObject *interactionObj, uiInteraction *delta){
//
//}


//static add Ux:: // UNUSED FUNCTION
bool Ux::bubbleInteractionIfNonClickOrHiddenPalletePreview(uiObject *interactionObj, uiInteraction *delta){ // return true always, unless the interaction should be dropped and not bubble for some reason....
 //     THIS should return true if the interaciton is still valid, which in all cases should really be YES - unles interaction object is for some reason nullptr reference

    Ux* self = Ux::Singleton();
 //     see also interactionUpdate

    if( !self->historyPalleteEditor->palleteSelectionPreviewHolder->is_being_viewed_state || delta->dy != 0 || delta->dx != 0 ){
        return self->bubbleCurrentInteraction(interactionObj, delta); // *SEEMS * much simploer to call bulbble on the UI object itself, perhaps returning the reference to the new interactionObject instead of bool....
    }

    return true;
}


//static add Ux::
bool Ux::bubbleInteractionIfNonClick(uiObject *interactionObj, uiInteraction *delta){ // return true always, unless the interaction should be dropped and not bubble for some reason....
 //     THIS should return true if the interaciton is still valid, which in all cases should really be YES - unles interaction object is for some reason nullptr reference

    delta->interactionNonTap(); // avoid double tap/isSecondInteraction

    Ux* self = Ux::Singleton();
 //     see also interactionUpdate

    if( delta->dy != 0 || delta->dx != 0 ){
        return self->bubbleCurrentInteraction(interactionObj, delta); // *SEEMS * much simploer to call bulbble on the UI object itself, perhaps returning the reference to the new interactionObject instead of bool....
    }


    return true;
}

bool Ux::bubbleWhenHidden(uiObject *interactionObj, uiInteraction *delta){ // return true always, unless the interaction should be dropped and not bubble for some reason....
    Ux* self = Ux::Singleton();
    if( interactionObj->isInHiddenState() /*|| interactionObj->isInAnimation()*/ ){
        return self->bubbleCurrentInteraction(interactionObj, delta);
    }
    return true;
}

// if we are moving up or down we want to scroll instead
bool Ux::bubbleInteractionIfNonHorozontalMovement(uiObject *interactionObj, uiInteraction *delta){ // return true always, unless the interaction should be dropped and not bubble for some reason....
 //     THIS should return true if the interaciton is still valid, which in all cases should really be YES - unles interaction object is for some reason nullptr reference
    Ux* self = Ux::Singleton();

    if( interactionObj->isInHiddenState() ){
        return self->bubbleCurrentInteraction(interactionObj, delta);
    }
 //    SDL_Log("we are looking at the fabs %f %f ", fabs(delta->dy), fabs(delta->dx) );
    if( !interactionObj->doesInFactRender
       || ( fabs(delta->dy) > 0.01f && fabs(delta->dx) < 0.01f) ){ // or is approx 0

        if( interactionObj->hasInteractionCb ){
            // in some cases, we can pretend the interaction DID complete (even though the mouse is not released) since the handler will reset the position automagically and is otherwise no-op
            // this is utilized currently in uiControlBooleanToggle
            interactionObj->interactionCallback(interactionObj, delta);
        }

        return self->bubbleCurrentInteraction(interactionObj, delta);
    }

    return true;
}

// this really means when swiping left far enough we disable scrolling
// other conditions we should bubble (if it seems like scrolling)
// this is also becomming rather specific to scroller tiles, and probably belongs in the scroller....
bool Ux::bubbleInteractionIfNonHorozontalMovementScroller(uiObject *interactionObj, uiInteraction *delta){ // return true always, unless the interaction should be dropped and not bubble for some reason....
 //     THIS should return true if the interaciton is still valid, which in all cases should really be YES - unles interaction object is for some reason nullptr reference

    Ux* self = Ux::Singleton();
 //     see also interactionUpdate

 //     interactionObj is the tile itself basically... it contains 2 child objects
 //     -x is leftward movement
 //     -y is upward movement

 //    SDL_Log("00))))0000000000000000000000000 y:%f x:%f",delta->dy, delta->dx);
 //    SDL_Log("00))))0000000000000000000000000 y:%f x:%f",fabs(delta->dy), fabs(0.0f-delta->dx));
 //    SDL_Log("00))))0000000000000000000000000 x:%f 0x:%f",interactionObj->boundryRect.x, interactionObj->origBoundryRect.x);


 //     todo: for widescreen mode.... if we swipe right (delta->dx > 0.04f)? and we are not showing the close X
 //     this means we should also bubble...

    if( interactionObj->isInHiddenState() /*|| interactionObj->isInAnimation()*/ ){
        return self->bubbleCurrentInteraction(interactionObj, delta);
    }

    if( !interactionObj->doesInFactRender
       || (self->widescreen && delta->dx > 0.01f )
       || ( fabs(delta->dy) > 0.01f && delta->dx > -0.04f) ){ // or is approx 0

        // since we were animating and its about to get lost, lets reset
        if( interactionObj->interactionProxy != nullptr ){
            interactionObj->interactionProxy->setAnimation( self->uxAnimations->resetPosition(interactionObj->interactionProxy) );
        }

        //SDL_Log("00))))0000000000000000000000000 y:%i x:%f",delta ->dy, fabs(0.0f-delta->dx));
        return self->bubbleCurrentInteraction(interactionObj, delta); // *SEEMS * much simploer to call bulbble on the UI object itself, perhaps returning the reference to the new interactionObject instead of bool....
    }

    return true;
}


/// eeeh this is really update current color
///   mayhaps this only updates when you stop panning?
///   or otherwise seperate function for addPickHistory from this
void Ux::updateColorValueDisplay(SDL_Color* color){
    curerntColorPreview->update(color);
 //    currentlyPickedColor = color; // maybe we should copy it instead?
    setColor(currentlyPickedColor, color);
 //    addCurrentToPickHistory();
}

bool Ux::wouldLooseIfColorAdded(){
    return pickHistoryList->total() > 0 && colorEquals(&pickHistoryList->getLast()->color, currentlyPickedColor);
}


void Ux::showBasicUpgradeMessage(){
    defaultYesNoChoiceDialogue->display(addHistoryBtn, &interactionUpgradeFromBasic, nullptr);
    defaultYesNoChoiceDialogue->displayAdditionalMessage("Basic:Upgrade?");
    defaultYesNoChoiceDialogue->assignScoringProcessor(nullptr);
    defaultYesNoChoiceDialogue->allowFastYes();
}

void Ux::visitExtensionOptions(){
#if defined(__EMSCRIPTEN__) && defined(COLORPICK_BUILD_FOR_EXT)
    em_launch_extension_options();
#endif
}

void Ux::addCurrentToPickHistory(){


//  TODO consider eliminating lastPickHistoryIndex instead
 //    / i.e pickHistoryIndex = lastPickHistoryIndex
 //     we can increment and bound check before using the new index
 //     this should simplify the checks used elsewhere

 //     IMPORTANT remember ot text with pickHistoryMax = <5
 //     IMPORTANT remember to check more than 10 colors

#if defined(__EMSCRIPTEN__) && defined(COLORPICK_BUILD_FOR_EXT)
    em_add_current_to_history(currentlyPickedColor->r, currentlyPickedColor->g, currentlyPickedColor->b );
#endif

    ColorList anEntry = ColorList(*currentlyPickedColor);
    bool historyEnabled = true;
    float bounceIntensity = -0.001;

#ifdef COLORPICK_BASIC_MODE
    showBasicUpgradeMessage();
    historyEnabled = false;
#endif


    OpenGLContext* ogg=OpenGLContext::Singleton();

    if( historyEnabled ){
     //     if we have a lastPickHistoryIndex
        if( pickHistoryList->total() > 0  ){
            // the new color must be unique
            if( wouldLooseIfColorAdded() ){

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

        pickHistoryList->add(anEntry);
        if( pickHistoryList->_out_of_space ){
            defaultScoreDisplay->display(historyPreview->childList[0], 2);
            //defaultScoreDisplay->displayExplanation("out of space!");
            defaultScoreDisplay->displayAchievement(Ux::uiSettingsScroller::UI_ACHEIVEMENT_GREATER_NOSPACE);
        }else{
            defaultScoreDisplay->display(historyPreview->childList[0], 1);
        }
        updatePickHistoryPreview();
        //     move logic into historyPalleteEditor
        if( historyPalleteEditor->historyPalleteHolder->isInBounds ){
            historyPalleteEditor->historyScroller->scrollToItemByIndex(0);
        }else{
            historyPalleteEditor->historyScroller->scrolly = 0;
        }
    }

    ogg->begin3dDropperAnimation(OpenGLContext::ANIMATION_ADD_COLOR, currentlyPickedColor);

 //     now we also update our minigame list....
    if( ogg->minigames->minigamesEnabled() ){
        Ux::setColor(&ogg->generalUx->minigameCounterText->foregroundColor, 255, 255, 255, 255);

		ColorList anEntryB = ColorList(*currentlyPickedColor);

        int existingLocation = minigameColorList->locate(anEntryB);
        if( existingLocation < 0 ){
            // other checks - minigame colors can't be too dark, too light, or too grey....
            int total = (int)currentlyPickedColor->r + (int)currentlyPickedColor->g + (int)currentlyPickedColor->b;
            if( total < 715 && total > 50  ){ // 765 - 64 // not too brigt
                int avg = total / 3;
                int variance =  SDL_abs(avg-currentlyPickedColor->r) + SDL_abs(avg-currentlyPickedColor->g) + SDL_abs(avg-currentlyPickedColor->b);
                if( variance > 49 ){
                    minigameColorList->add(anEntryB);
                    if( minigameColorList->_out_of_space || minigameColorList->total() >= ogg->minigames->requiredSwatchesForMinigames() ){
                        // is this the condition to tirgger the minigame????
                        ogg->begin3dDropperAnimation(OpenGLContext::ANIMATION_ZOOM_INTO_BULB, currentlyPickedColor);
                    }
                }else{
                    //SDL_Log("omit color from minigame: too grey %i", variance);

                    Ux::setColor(&ogg->generalUx->minigameCounterText->foregroundColor, 128, 128, 128, 255);

                }
            }else{
                //SDL_Log("omit color from minigame: too bright/dark %i", total);

            }
        }else{
            // SDL_Log("this color is already in the minigame...");
        }
        SDL_snprintf(print_here, max_print_here, "%i", minigameColorList->total());//"Add All?"
        printStringToUiObject(minigameCounterText, print_here, DO_NOT_RESIZE_NOW);
        minigameCounterText->updateRenderPosition();
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


    if( pickHistoryList->total() >= SIX_ACROSS){
        previewMessageOver->show();
    }else{
        previewMessageOver->hide();
    }


 //     somertimes upate the full histoury preview too
    if( historyPalleteEditor->historyPalleteHolder->isInBounds ){
        historyPalleteEditor->historyScroller->updateTiles(); // calls updateUiObjectFromHistory for each tile
        //palleteScroller->updateTiles();
    }
}



//
//void Ux::setUniformsForRenderObject(uniformLocationStruct *uniformLocations, uiObject *renderObj){
//
//}
//


int Ux::renderObject(uniformLocationStruct *uniformLocations){
    bool animationsJustCompleted = uxAnimations->animationsJustCompleted(); // this should only be true for 1 render pass, could be stored in more global render state, or passed in from above...
    updateControllerCursorPosition(animationsJustCompleted);
    /*return */renderObjects(uniformLocations, rootUiObject, glm::mat4(1.0f));

//    DebugPrintAllCursorPositions(uniformLocations);
    return 0;
}

int Ux::renderObjects(uniformLocationStruct *uniformLocations, uiObject *renderObj, glm::mat4 inheritMat){
 //     update
 //     render object itself, then render all child hobj


    if( !renderObj->isInBounds ) return 1;

    glm::mat4 resolvedRenderObjMat = renderObj->matrix; // copy struct

    if( renderObj->matrixInheritedByDescendants ){
//        resolvedRenderObjMat *= inheritMat; // we limit the matrix math except where needed this way....
        resolvedRenderObjMat = inheritMat * resolvedRenderObjMat; // we limit the matrix math except where needed this way....

    }

//    if( renderObj->isDebugObject ){
//        SDL_Log("DEBUG OBJECT IS BEING RENDERED NOW!");
////        renderObj->boundryRect.y+=0.01; // this proves the boundary tests is working for this object, when out of view they do not render...
////        renderObj->updateRenderPosition();
//
//    }

    if( renderObj->doesInFactRender && (renderObj->hasBackground || renderObj->hasForeground) ){
        // TODO count renders and see, is the following condition any better/different ?????
//  if( renderObj->doesInFactRender && ((renderObj->hasBackground && renderObj->backgroundColor.a > 254) || (renderObj->hasForeground && renderObj->foregroundColor.a > 0 )) ){

        glUniformMatrix4fv(uniformLocations->ui_modelMatrix, 1, GL_FALSE, &resolvedRenderObjMat[0][0]); // Send our model matrix to the shader

        //    glUniformMatrix4fv(uniformLocations->ui_modelMatrix, 1, GL_FALSE, &glm::mat4(1.0f)[0][0]);

        renderObj->setUniformsForRender(uniformLocations);

//        if( !renderObj->hasBackground && !renderObj->hasForeground ){
//            SDL_Log("This object is being rendered but it has no foreground or background!!!");
//        }

        //glFlush()
        //glFlush();
        //SDL_Delay(66);

        // see updateStageDimension and consider SDL_RenderSetClipRect
        glDrawArrays(GL_TRIANGLES, 0, 6); // hmmm
//        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT, ogg->squareTriangleIndicies);
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL); // if using GL_ELEMENT_ARRAY_BUFFER, nify macro makes NULL go away, see elsewhere.... actually we should use triangle strip anyway :shrug: not much benefit eitehr way :)

        //glFlush();
        //SDL_Delay(66);


    }


    if( renderObj->hasChildren && renderObj->doesRenderChildObjects ){
        //for( int x=0, l=(int)renderObj->childUiObjects.size(); x<l; x++ ){

        //resolvedRenderObjMat = glm::scale(resolvedRenderObjMat, glm::vec3(0.0f, 0.0f, 0.01));
        for( int x=0,l=renderObj->childListIndex; x<l; x++ ){
            renderObjects(uniformLocations, renderObj->childList[x], resolvedRenderObjMat);
        }


        // this is the CRUMMY_ANDROID test, but we stop rendering basically on the zoomSlider, which is good testing if we can only render one draw call...
//        for( int x=0,l=SDL_min(2,renderObj->childListIndex); x<l; x++ ){
//            renderObjects(uniformLocations, renderObj->childList[x], resolvedRenderObjMat);
//        }

        // renderChildrenInReverse

//        for( int x=renderObj->childListIndex-1; x>-1; x-- ){
//            renderObjects(uniformLocations, renderObj->childList[x]);
//        }
    }


    return 0;
}
