//
//  uiSettingsScroller.h
//  ColorPick SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiSettingsScroller_h
#define ColorPick_iOS_SDL_uiSettingsScroller_h




/*
this is really different from uiHistoryPalleteEditor in that it takes up the full rectange of the viewport, holderBg is the real visible object
 this allows us to intercept any clicks on the close X area since its contained in our object rather than outside of it....
 */

struct uiSettingsScroller{  // we will become uxInstance->settingsScroller - and is pretty much singleton....


    const static int maxSettings = 25;

    typedef enum  {
        HEADING,
        SPACE,
        BOOLEAN_TOGGLE,
        BOOLEAN_ACHIEVEMENT,
        ACTION_BUTTON
    } SETTING_TYPES_ENUM; // if this is unused.... well maybe we know its a boolean so it has persistence....

    typedef struct SettingsListObj // cannot really use this since we do not persist ths to disk
    {
        SettingsListObj(uiObject* pTile, int pType, Uint8 pkey){
            settingType= pType;
            ourTileObject = pTile;
            settingKey = pkey;
        }
        int settingType; // one of SETTING_TYPES_ENUM
        // we should consider just holding the reference to the child object we want to render in this tile.... for max flexibility
        uiObject* ourTileObject;
        // settings need to have metadata about persistence too....
        // so that we may read/write these values.. setting_type is a good part of this but we need a key and value type possibly
        // we may also need to know which property of which object we should modify the value of if this settings is changed
        // which is a lot tricker unless the source of truth is also right here somewehre!
        Uint8 settingKey; // if applicable, this will be set....
    } SettingsListObj;

    /* see SDL_EventType enum.... for inspiration?!  btw,  0xFF  is the max uInt8 */
    typedef enum  {
        // important: do not insert new items anywhere except at the end of an area, and then do not exceede the next area...
        /* You may only add new values to the END of the enum block - even though this is int we use uint8 so do not exceede 255*/
        UI_SETTING_NONE = 0,
        UI_SETTING_GAME_ON,
        UI_SETTING_GAME_ON2,
        // OK to add some here

        UI_ACHEIVEMENT_START = 128, // 0x80
        UI_ACHEIVEMENT_INEXACT, // "*in-exact!!"
        UI_ACHEIVEMENT_RIGHT_HUE, // "Right Hue're"
        UI_ACHEIVEMENT_NOSPACE, // "out of space!"
        UI_ACHEIVEMENT_REWROTE_HISTORY, // "Wrote History"
        UI_ACHEIVEMENT_NO_FAST, // " No Thanks "
        UI_ACHEIVEMENT_MUCH_RISK, // " Much Risk "
        UI_ACHEIVEMENT_INT_OVERFLOW, // "int overflow!"

        // OK to add some here

        UI_SETTING_LAST = 255  // 0xFF
    } UI_SETTINGS_ENUM;

    typedef struct SettingsRwObject
    {
        Uint8 key;
        Uint8 value;
    } SettingsRwObject;

    static int indexForSetting(SettingsListObj* setting){
        return setting->settingKey;
    }

    uiSettingsScroller(uiObject* parentObj){
        Ux* uxInstance = Ux::Singleton();

        settingsList = new uiList<SettingsListObj, Uint8>(maxSettings); // WARN - do not enable index if using Uint8 - max Uint8 is far less than pickHistoryMax
        settingsList->index(UI_SETTINGS_ENUM::UI_SETTING_LAST, indexForSetting);

//        SDL_Log("NOise output about enums: %d %d %d %d %d %d %d",
//                UI_SETTING_NONE,
//                UI_SETTING_GAME_ON,
//                UI_SETTING_GAME_ON2,
//                UI_ACHEIVEMENT_START,
//                UI_ACHEIVEMENT_ONE,
//                UI_ACHEIVEMENT_TWO,
//                UI_SETTING_LAST
//            );

        uiObjectItself = new uiObject();
        Uint8 opacity = 220;

//        uiObjectItself->hasBackground = true;
//        Ux::setColor(&uiObjectItself->backgroundColor, 0, 255, 0, 128);
        uiObjectItself->setInteractionCallback(&interactionToggleSettings); // if we dragged and released... it will animate the rest of the way because of this
        uiObjectItself->setInteraction(&Ux::interactionVert);
        uiObjectItself->setBoundsEnterFunction(&Ux::interactionHistoryEnteredView);
        uiObjectItself->is_being_viewed_state = true;

        holderBg = new uiObject();

        holderBg->hasBackground = true;
        Ux::setColor(&holderBg->backgroundColor, 0, 0, 0, opacity);


        uiObjectItself->addChild(holderBg);

        settingsScroller = new uiScrollController();
        settingsScroller->initTilingEngine(1, 10, &updateUiObjectFromSettings, &getSettingsTotalCount, nullptr,  nullptr);
        settingsScrollerItself = settingsScroller->uiObjectItself;


        holderBgTlEdgeShadow = new uiEdgeShadow(holderBg, SQUARE_EDGE_ENUM::TOP, 0.03);
        settingsHolderBrEdgeShadow = new uiEdgeShadow(holderBg, SQUARE_EDGE_ENUM::BOTTOM, 0.03);

        holderBgTlEdgeShadow->setOpacity(opacity);
        settingsHolderBrEdgeShadow->setOpacity(opacity);

        holderBg->addChild(settingsScrollerItself);

        historyPalleteCloseX = new uiObject();
        uxInstance->printCharToUiObject(historyPalleteCloseX, CHAR_CLOSE_ICON, DO_NOT_RESIZE_NOW);
        historyPalleteCloseX->hasForeground = true;
        historyPalleteCloseX->squarify();
        Ux::setColor(&historyPalleteCloseX->foregroundColor, 255, 255, 255, 96); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        holderBg->addChild(historyPalleteCloseX);

        parentObj->addChild(uiObjectItself);// aka uiObjectItself);

        //settingsScroller->updateTiles();

        uiObject* dummyContainer=new uiObject();
        uiObject* d = dummyContainer;

        settingsList->add(SettingsListObj((new uiText(dummyContainer, 0.1425))->print("History")->uiObjectItself, SETTING_TYPES_ENUM::HEADING, UI_SETTINGS_ENUM::UI_SETTING_NONE));

        uiObject* temp =  new uiObject();
        uxInstance->printCharToUiObject(temp, CHAR_CLOSE_ICON, DO_NOT_RESIZE_NOW);
        temp->hasForeground = true;
        temp->squarify();
        Ux::setColor(&temp->foregroundColor, 255, 255, 255, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)


        settingsList->add(SettingsListObj(temp, SETTING_TYPES_ENUM::HEADING, UI_SETTINGS_ENUM::UI_SETTING_NONE));


        settingsList->add(SettingsListObj((new uiControlButton(d, "Click Here", &interactionClickGoToHistoryBtn))->uiObjectItself, SETTING_TYPES_ENUM::HEADING, UI_SETTINGS_ENUM::UI_SETTING_NONE));




        settingsList->add(SettingsListObj((new uiText(dummyContainer, 0.1425))->print("Images")->uiObjectItself, SETTING_TYPES_ENUM::HEADING, UI_SETTINGS_ENUM::UI_SETTING_NONE));



        settingsList->add(SettingsListObj((new uiControlBooleanToggle(dummyContainer, "yes", false))->uiObjectItself, SETTING_TYPES_ENUM::BOOLEAN_TOGGLE, UI_SETTINGS_ENUM::UI_SETTING_NONE));


        settingsList->add(SettingsListObj((new uiText(dummyContainer, 0.1425))->print("Settings")->uiObjectItself, SETTING_TYPES_ENUM::HEADING, UI_SETTINGS_ENUM::UI_SETTING_NONE));

        settingsList->add(SettingsListObj((new uiControlBooleanToggle(dummyContainer, "+Points", true))->uiObjectItself, SETTING_TYPES_ENUM::BOOLEAN_TOGGLE, UI_SETTINGS_ENUM::UI_SETTING_GAME_ON));

        settingsList->add(SettingsListObj((new uiText(dummyContainer, 0.1425))->print("Achievements")->uiObjectItself, SETTING_TYPES_ENUM::HEADING, UI_SETTINGS_ENUM::UI_SETTING_NONE));

        //settingsList->add(SettingsListObj((new uiControlAchievementToggle(dummyContainer, "Got +1"))->uiObjectItself, SETTING_TYPES_ENUM::BOOLEAN_ACHIEVEMENT, UI_SETTINGS_ENUM::UI_ACHEIVEMENT_ONE));


        // generalUx->defaultScoreDisplay->displayAchievement(Ux::uiSettingsScroller::UI_ACHEIVEMENT_INEXACT);
        settingsList->add(SettingsListObj((new uiControlAchievementToggle(d, "In-exact HSV", "*in-exact!!", false))->uiObjectItself, SETTING_TYPES_ENUM::BOOLEAN_ACHIEVEMENT, UI_SETTINGS_ENUM::UI_ACHEIVEMENT_INEXACT));
        settingsList->add(SettingsListObj((new uiControlAchievementToggle(d, "Right Hue're", "Right Hue're", false))->uiObjectItself, SETTING_TYPES_ENUM::BOOLEAN_ACHIEVEMENT, UI_SETTINGS_ENUM::UI_ACHEIVEMENT_RIGHT_HUE));
        settingsList->add(SettingsListObj((new uiControlAchievementToggle(d, "No Space", "out of space!", false))->uiObjectItself, SETTING_TYPES_ENUM::BOOLEAN_ACHIEVEMENT, UI_SETTINGS_ENUM::UI_ACHEIVEMENT_NOSPACE));
        settingsList->add(SettingsListObj((new uiControlAchievementToggle(d, "Wrote History", "Wrote History", false))->uiObjectItself, SETTING_TYPES_ENUM::BOOLEAN_ACHIEVEMENT, UI_SETTINGS_ENUM::UI_ACHEIVEMENT_REWROTE_HISTORY));
        settingsList->add(SettingsListObj((new uiControlAchievementToggle(d, "No Thanks", " No Thanks ", false))->uiObjectItself, SETTING_TYPES_ENUM::BOOLEAN_ACHIEVEMENT, UI_SETTINGS_ENUM::UI_ACHEIVEMENT_NO_FAST));
        settingsList->add(SettingsListObj((new uiControlAchievementToggle(d, "Much Risk", " Much Risk ", false))->uiObjectItself, SETTING_TYPES_ENUM::BOOLEAN_ACHIEVEMENT, UI_SETTINGS_ENUM::UI_ACHEIVEMENT_MUCH_RISK));
        settingsList->add(SettingsListObj((new uiControlAchievementToggle(d, "Int Overflow", "int overflow!", true))->uiObjectItself, SETTING_TYPES_ENUM::BOOLEAN_ACHIEVEMENT, UI_SETTINGS_ENUM::UI_ACHEIVEMENT_INT_OVERFLOW));

        settingsList->add(SettingsListObj((new uiText(dummyContainer, 0.1425))->print("Reset")->uiObjectItself, SETTING_TYPES_ENUM::HEADING, UI_SETTINGS_ENUM::UI_SETTING_NONE));


        if( settingsList->_out_of_space ){
            SDL_Log("\n\n\nERROR: we do not have enough maxSettings for all the settings scroller items!!!\n\n\n");
            SDL_Quit();
            exit(1);
        }

//        int position = settingsList->locateIndex(UI_SETTINGS_ENUM::UI_SETTING_GAME_ON);
//        SettingsListObj* s = settingsList->get(position);
//        SDL_Log("we found it %d", position);

    }

    uiList<SettingsListObj, Uint8>* settingsList;

    uiObject* uiObjectItself; // no real inheritance here, this its the uiSettingsScroller, I would use self->

    uiObject* holderBg;

    uiEdgeShadow* holderBgTlEdgeShadow;
    uiEdgeShadow* settingsHolderBrEdgeShadow;

    uiObject *historyPalleteCloseX;
    uiScrollController *settingsScroller;

    uiObject *settingsScrollerItself; // just a ref to historyScroller's uiObjectItself

    const char* achieveAchievement(Uint8 achievementKey){
        int position = settingsList->locateIndex(achievementKey);
        if( achievementKey > UI_SETTINGS_ENUM::UI_ACHEIVEMENT_START && achievementKey < UI_SETTINGS_ENUM::UI_SETTING_LAST && position > -1 ){
            SettingsListObj* setting = settingsList->get(position);
            switch(setting->settingType){
                case SETTING_TYPES_ENUM::BOOLEAN_ACHIEVEMENT:
                {
                    return ((uiControlAchievementToggle*)(setting->ourTileObject->myUiController))->achieveAchievement();
                    break;
                }
                default:
                    return "";
                    break;
            }

        }
        return "";
    }

    bool getBooleanSetting(Uint8 settingKey){
        int position = settingsList->locateIndex(settingKey);
        if( position > -1 ){
            SettingsListObj* setting = settingsList->get(position);
            switch(setting->settingType){
                case SETTING_TYPES_ENUM::BOOLEAN_TOGGLE:
                {
                    return ((uiControlBooleanToggle*)(setting->ourTileObject->myUiController))->value();
                    break;
                }
                default:
                    break;
            }
        }else{
            SDL_Log("We are looking up a setting that does not seem to exist! %d", settingKey);
        }
        return false;
    }

    // low level read/write

    void applyReadSettingsState(SettingsRwObject readSetting){
        SDL_Log("WE had read a setting from disk, here is the enum: %d %d", readSetting.key, readSetting.value);
        int position = settingsList->locateIndex(readSetting.key);
        if( position > -1 ){
            SettingsListObj* setting = settingsList->get(position);
            switch(setting->settingType){
                case SETTING_TYPES_ENUM::BOOLEAN_TOGGLE:
                {
                    ((uiControlBooleanToggle*)(setting->ourTileObject->myUiController))->setValue(readSetting.value);
                    break;
                }
                case SETTING_TYPES_ENUM::BOOLEAN_ACHIEVEMENT:
                {
                    ((uiControlAchievementToggle*)(setting->ourTileObject->myUiController))->setValue(readSetting.value);
                    break;
                }
                default:
                    break;
            }

        }else{
            SDL_Log("We encountered a setting that does not seem to exist! %d %d", readSetting.key, readSetting.value);
        }

    }

    void writeSettingsToFile(SDL_RWops* fileref){
        uiListIterator<uiList<SettingsListObj, Uint8>, SettingsListObj>* myIterator = settingsList->iterate();
        SettingsListObj* setting = myIterator->next();
        while(setting != nullptr){
            if( setting->settingKey != UI_SETTINGS_ENUM::UI_SETTING_NONE && setting->settingKey != UI_SETTINGS_ENUM::UI_ACHEIVEMENT_START && setting->settingKey < UI_SETTINGS_ENUM::UI_SETTING_LAST ){
                bool doWrite = false;
                Uint8 valueToWrite = 0;
                switch(setting->settingType){
                    case SETTING_TYPES_ENUM::BOOLEAN_TOGGLE:
                    {
                        valueToWrite = ((uiControlBooleanToggle*)(setting->ourTileObject->myUiController))->value();
                        doWrite = true;
                        break;
                    }
                    case SETTING_TYPES_ENUM::BOOLEAN_ACHIEVEMENT:
                    {
                        valueToWrite = ((uiControlAchievementToggle*)(setting->ourTileObject->myUiController))->value();
                        doWrite = true;
                        break;
                    }
                    default:
                        break;
                }
                if( doWrite ){
                    //SDL_Log("Going to write a setting, here is our key: %d value: %d", setting->settingKey, valueToWrite);
                    SDL_WriteU8(fileref, setting->settingKey);
                    SDL_WriteU8(fileref, valueToWrite);
                }
            }
            setting = myIterator->next();
        }
        SDL_free(myIterator);
    }

    /*SETTINGS */


    // **** SETTINGS ****


    static bool updateUiObjectFromSettings(uiObject *scrollerTile, int offset){
        Ux* myUxRef = Ux::Singleton();
        uiSettingsScroller* self = myUxRef->settingsScroller;

        if( offset > getSettingsTotalCount() - 1 || offset < 0 ){
            scrollerTile->empty();
            scrollerTile->hide();
            scrollerTile->myIntegerIndex = -1;
            return false;
        }

        //offset = myUxRef->pickHistoryList->total() - offset - 1; // we show them in reverse here

        SettingsListObj* listItem = self->settingsList->get(offset);

        if( listItem == nullptr ){
            SDL_Log("We got a nullptr");
        }


        if( listItem->ourTileObject->hasParentObject ){
            listItem->ourTileObject->parentObject->empty(); // we avoid being mulitply added.... addChild could arguably help with this sort of thing....
        }

        scrollerTile->empty();
        scrollerTile->addChild(listItem->ourTileObject);
        scrollerTile->show();

        listItem->ourTileObject->setCropParentRecursive(self->settingsScrollerItself);


//        SDL_Log("SOME BOUNDARIES %d %d %f %f %f %f -  %f %f %f %f",
//                offset, scrollerTile->childListIndex,
//                listItem->ourTileObject->boundryRect.x,
//                listItem->ourTileObject->boundryRect.y,
//                listItem->ourTileObject->boundryRect.w,
//                listItem->ourTileObject->boundryRect.h,
//                scrollerTile->boundryRect.x,
//                scrollerTile->boundryRect.y,
//                scrollerTile->boundryRect.w,
//                scrollerTile->boundryRect.h
//            );
//
//        myUxRef->printCharToUiObject(scrollerTile, 'Z'-offset, true);
//
//
//        myUxRef->printCharToUiObject(listItem->ourTileObject, 'Z'-offset, true);


        //listItem->ourTileObject->setCropParentRecursive(scrollerTile);
        //listItem->ourTileObject->setCropParent(scrollerTile);

        // this seems like the best one - however since our text is matrix scaled up.... it will always be "cropped" when its at the edge and it is messed up....
        //scrollerTile->setCropModeOrigPosition(); // influences the rendering of the X while we bounce (duplicate detected)
        //scrollerTile->setCropModeOrigPosition(); // influences the rendering of the X while we bounce (duplicate detected)

       // this should be auto set....




//        listItem->ourTileObject->setCropModeOrigPosition();
//        listItem->ourTileObject->setCropParentRecursive(scrollerTile);

//        removeButtonHolder->setCropParentRecursive(historyTile); // must set before adding child...
//        scrollerTile->setCropModeOrigPosition(); // influences the rendering of the background color while we bounce (duplicate detected)

        //scrollerTile->updateRenderPosition();

        return true;
    }

    static int getSettingsTotalCount(){
        Ux* myUxRef = Ux::Singleton();
        uiSettingsScroller* self = myUxRef->settingsScroller;
        int total = self->settingsList->total();
        return total;
    }


    static void interactionToggleSettings(uiObject *interactionObj, uiInteraction *delta){
        Ux* myUxRef = Ux::Singleton();
        uiSettingsScroller* self = myUxRef->settingsScroller;

        // PLEASE NOTE: the args may be nullptr, nullptr - we don't use them here....

        //self->newHistoryFullsize->cancelCurrentAnimation();

        if( self->uiObjectItself->is_being_viewed_state ) {
            if( myUxRef->widescreen ){
                self->uiObjectItself->setAnimation( myUxRef->uxAnimations->slideRight(self->uiObjectItself) ); // returns uiAminChain*
                //self->uiObjectItself->setAnimation( self->uxAnimations->slideRightFullWidth(self->uiObjectItself) ); // returns uiAminChain*
            }else{
                self->uiObjectItself->setAnimation( myUxRef->uxAnimations->slideDown(self->uiObjectItself) ); // returns uiAminChain*
            }
            self->uiObjectItself->is_being_viewed_state = false;
            myUxRef->endModal(self->uiObjectItself);

            /*
             any view that is influenced by settings - we should update those now.... arguably this should be unified someplace....
             */

            myUxRef->defaultScoreDisplay->updateScoreDisplay();

        }else{
            self->uiObjectItself->isInBounds = true; // nice hack
            //myUxRef->updatePickHistoryPreview();

            self->settingsScroller->updateTiles();

            self->uiObjectItself->setAnimation( myUxRef->uxAnimations->resetPosition(self->uiObjectItself) ); // returns uiAminChain*
            self->uiObjectItself->is_being_viewed_state = true;
            //self->historyScroller->allowUp = true;
            myUxRef->updateModal(self->uiObjectItself, &interactionToggleSettings);

        }
    }


    static void interactionClickGoToHistoryBtn(uiObject *interactionObj, uiInteraction *delta){

        Ux* myUxRef = Ux::Singleton();

        interactionToggleSettings(nullptr, nullptr);

        myUxRef->historyPalleteEditor->interactionToggleHistory(nullptr, nullptr);

    }

































    void resize(Float_Rect visible, Float_Rect hidden){
        Ux* uxInstance = Ux::Singleton();

        //holderBg->setBoundaryRectForAnimState(&visible,&hidden);
        holderBg->setBoundaryRect(&visible);

        if( uxInstance->widescreen ){
            uiObjectItself->setBoundaryRectForAnimState(
                                                        0.0,0.0,1.0,1.0, // vis
                                                        1.0,0.0,1.0,1.0  // hid
                                                        );
            holderBgTlEdgeShadow->resize(SQUARE_EDGE_ENUM::LEFT);
            settingsHolderBrEdgeShadow->resize(SQUARE_EDGE_ENUM::RIGHT);

            uiObjectItself->setInteraction(&Ux::interactionHorizontal);

            historyPalleteCloseX->setBoundaryRect( 1.01, 0.5-0.025, 0.05, 0.05);

            settingsScrollerItself->setBoundaryRect( 0.0, 0.0, 1.0, 1.0);
            settingsScroller->resizeTililngEngine(1, 5);

        }else{
            uiObjectItself->setBoundaryRectForAnimState(
                                                        0.0,0.0,1.0,1.0, // vis
                                                        0.0,1.0,1.0,1.0  // hid
                                                        );
            holderBgTlEdgeShadow->resize(SQUARE_EDGE_ENUM::TOP);
            settingsHolderBrEdgeShadow->resize(SQUARE_EDGE_ENUM::BOTTOM);

            uiObjectItself->setInteraction(&Ux::interactionVert);

            historyPalleteCloseX->setBoundaryRect( 0.5-0.025, 1.01, 0.05, 0.08);

            settingsScrollerItself->setBoundaryRect( 0.0, 0.0, 1.0, 1.0);
            settingsScroller->resizeTililngEngine(1, 10);

        }

        settingsScroller->updateTiles();
    }


    bool update(){
        return true;
    }











};


#endif
