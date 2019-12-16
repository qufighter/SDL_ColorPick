//
//


#ifndef ColorPick_iOS_SDL_minigames_h
#define ColorPick_iOS_SDL_minigames_h

#define makeGameArgs(minigameN) minigameN->gameIndex, minigameN, \
    &minigameN->show, \
    &minigameN->begin, \
    &minigameN->resize, \
    &minigameN->update, \
    &minigameN->render, \
    &minigameN->end, \
    &minigameN->getElapsedTime, \
    &minigameN->getTimeLimit, \
    &minigameN->getGameName

struct Minigames{

    const static int maxGames = 10;
    const static int msPerMinute = 60000;

    typedef void (*aGenericGameCb)(void* gameItself);
    typedef int (*anIntGenericGameCb)(void* gameItself);
    typedef const char* (*aConstCharStarGenericGameCb)(void* gameItself);

    typedef struct GameListObj
    {
        GameListObj(Uint8 gameId, void* p_gameItself,
                    aGenericGameCb pShowFn,
                    aGenericGameCb pBeginFn,
                    aGenericGameCb pResizeFn,
                    aGenericGameCb pUpdateFn,
                    aGenericGameCb pRenderFn,
                    aGenericGameCb pEndFn,
                    anIntGenericGameCb pGetElapsedTimeFn,
                    anIntGenericGameCb pGetTimeLimitFn,
                    aConstCharStarGenericGameCb pGetNameFn
        ){
            gameEnumIndex = gameId;
            gameItself = p_gameItself;
            showFn=pShowFn;
            beginFn=pBeginFn;
            resizeFn = pResizeFn;
            updateFn=pUpdateFn;
            renderFn=pRenderFn;
            endFn=pEndFn;
            timeLimitFn=pGetTimeLimitFn;
            elapsedTimeFn=pGetElapsedTimeFn;
            gameNameFn=pGetNameFn;
        }

        void show(){showFn(gameItself);}
        void begin(){beginFn(gameItself);}
        void resize(){resizeFn(gameItself);}
        void update(){updateFn(gameItself);}
        void render(){renderFn(gameItself);}
        void end(){endFn(gameItself);}
        int getElapsedTime(){return elapsedTimeFn(gameItself);}
        int getTimeLimit(){return timeLimitFn(gameItself);}
        const char* getGameName(){return gameNameFn(gameItself);}

        Uint8 gameEnumIndex;
        void* gameItself;
        aGenericGameCb showFn;
        aGenericGameCb beginFn;
        aGenericGameCb resizeFn;
        aGenericGameCb updateFn;
        aGenericGameCb renderFn;
        aGenericGameCb endFn;
        anIntGenericGameCb elapsedTimeFn;
        anIntGenericGameCb timeLimitFn;
        aConstCharStarGenericGameCb gameNameFn;

    } GameListObj;

    static int indexForGame(GameListObj* gameListObj){
        //SDL_Log("index lookup... %i", gameListObj->gameEnumIndex);
        return gameListObj->gameEnumIndex;
    }
    int randomGameId(){
        return Ux::randomInt(1,TOTAL_GAMES-1);
    }
    GameListObj* randomGame(){
        //SDL_Log("rolling random game...");

        return gameList->get(Ux::randomInt(0,gameList->total()-1));

        int foundGame = gameList->locateIndex(randomGameId());


        if( foundGame > -1 ){
            return gameList->get(foundGame);
        }else{
            SDL_Log("\n\n\nERROR: We picked a random game that does not exist!!");
            return gameList->get(0);
            //return nullptr;
        }
    }

    Ux::uiList<GameListObj, Uint8>* gameList;
    GameListObj* currentGame;
    Ux::uiObject* controls;
    Ux::uiObject* controlBarTopHolder;
    Ux::uiObject* controlBarTop;
    Ux::uiObject* minigamesCloseX;
    Ux::uiObject* minigamesColorPickIcon;
    Ux::uiObject* gameHeadingHolder;
    Ux::uiText* gameHeading;
    Ux::uiText* gameTimer;

    int my_timer_id;
    bool gotMinigameAnnounceDone;
    bool gameCompleted;

    typedef enum  {
        GAME0_RESERVED=0,
        GAME1_MATCH_MAKER,
        GAME2_MIX_MASTER,
        GAME3_FLIP_MASTER,
        TOTAL_GAMES
    } MINIGAMES_ENUM;

#include "MatchMaster/MatchMaster.h"
#include "MixMaster/MixMaster.h"
#include "FlipMaster/FlipMaster.h"

    Minigames(){
        Ux* myUxRef = Ux::Singleton();

        // todo: if we get ogg and then set the ref now ogg->minigames = this; .... then when we construct our minigamess they'll be able to get the reference below...
        OpenGLContext* ogg=OpenGLContext::Singleton();
        ogg->minigames = this;



        // constructor....
        gameList = new Ux::uiList<GameListObj, Uint8>(maxGames);
        gameList->index(MINIGAMES_ENUM::TOTAL_GAMES, indexForGame);

        MatchMaster* matchmaker = new MatchMaster(MINIGAMES_ENUM::GAME1_MATCH_MAKER);
//        MixMaster* minigame1 = new MixMaster(MINIGAMES_ENUM::GAME1_MATCH_MAKER);
        MixMaster* mixmaster = new MixMaster(MINIGAMES_ENUM::GAME2_MIX_MASTER);
        FlipMaster* flipmatcher = new FlipMaster(MINIGAMES_ENUM::GAME3_FLIP_MASTER);

        gameList->add(GameListObj(makeGameArgs(matchmaker)));
        gameList->add(GameListObj(makeGameArgs(mixmaster)));
        gameList->add(GameListObj(makeGameArgs(flipmatcher)));

//        SDL_Log("testing this out");
//        for( int i=0;i<99;i++){
//            //SDL_Log("here we are with integer...%i", randomGameId());
//            randomGame();
//        }

        currentGame = randomGame();

        if( gameList->_out_of_space ){
            SDL_Log("\n\n\nERROR: we do not have enough maxGames for all the minigames items!!!  Increase maxGames \n\n\n");
            SDL_Quit();
            exit(1);
        }


        // all games should be added now, anything next will be attop it all....
        // shall we set up common controls on teh minigame conterner here too??



        // TODO use one or more contaienrs for these types of controls!::

        controls = new Ux::uiObject();
        controlBarTopHolder = new Ux::uiObject();
        controlBarTopHolder->setBoundaryRect(0.0, 0.0, 1.0, 0.1);
        controlBarTop = new Ux::uiObject();
        controlBarTop->setBoundaryRect(0.0, 0.0, 1.0, 1.0);

        controls->addChild(controlBarTopHolder);

        myUxRef->minigamesUiContainer->addChild(controls);

        minigamesCloseX = new Ux::uiObject();
        myUxRef->printCharToUiObject(minigamesCloseX, CHAR_CLOSE_ICON, DO_NOT_RESIZE_NOW);
        minigamesCloseX->hasForeground = true;
        minigamesCloseX->squarify();
        Ux::setColor(&minigamesCloseX->foregroundColor, 255, 255, 255, 96); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        minigamesCloseX->setClickInteractionCallback(&interactionCloseXClicked);
        minigamesCloseX->setBoundaryRect(0.05, 0.0, 0.1, 0.1); // see resize, not really set here...
        controls->addChild(minigamesCloseX);

        minigamesColorPickIcon = new Ux::uiObject();
        myUxRef->printCharToUiObject(minigamesColorPickIcon, CHAR_APP_ICON, DO_NOT_RESIZE_NOW);
        minigamesColorPickIcon->hasForeground = true;
        minigamesColorPickIcon->squarify();
        Ux::setColor(&minigamesColorPickIcon->foregroundColor, 255, 255, 255, 128); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        minigamesColorPickIcon->setClickInteractionCallback(&interactionCloseXClicked);
        minigamesColorPickIcon->setBoundaryRect(1.0-0.1-0.05, 0.0, 0.1, 0.1); // see resize, not really set here...
        minigamesColorPickIcon->rotate(-45);
        controls->addChild(minigamesColorPickIcon);


#ifdef COLORPICK_CLOCK_BAR_PRESENT
        Ux::uiGradientLinear* topGrad = (new Ux::uiGradientLinear(controlBarTopHolder, Float_Rect(0.0,0.0,1.0,1.0)))
//            ->addStop(0.25, 255,0,0,255)
//            ->addStop(0.75, 0,0,255,255)
//            ->addStop(0.5, 0,255,0,255)
            ->addStop(0.0, 128,128,128,255)
            //->addStop(0.65, 128,128,128,0)
            ->addStop(0.65, 0,0,0,255)
            ->update();
#endif

        controlBarTopHolder->addChild(controlBarTop);

        gameTimer = (new Ux::uiText(controlBarTop, 1.0/5.0))->pad(0.0,0.0)->margins(0.0,0.35,0.0,0.35)->print("00:00");
        controlBarTopHolder->setClickInteractionCallback(&interactionHeadingClick);


        gameHeadingHolder = new Ux::uiObject();
        controls->addChild(gameHeadingHolder);

        gameHeadingHolder->setBoundaryRect(0.0, 0.0, 1.0, 1.0);

        //last for on top
        gameHeading = (new Ux::uiText(gameHeadingHolder, 0.1))->pad(0.0,0.0)->margins(0.25,0.0,0.0,0.0)->print("");
//        gameHeading->text_itself->setClickInteractionCallback(&interactionHeadingClick);
//        gameHeading->text_itself->doesNotCollide = false;
        
    }

    static void interactionHeadingClick(Ux::uiObject *interactionObj, uiInteraction *delta){
        OpenGLContext* ogg=OpenGLContext::Singleton();
        Minigames* self = ogg->minigames;
        Ux* myUxRef = ogg->generalUx;
        if( self->gameCompleted ){
            interactionCloseXClicked(interactionObj, delta);
        }else{
            myUxRef->defaultYesNoChoiceDialogue->display(interactionObj, &interactionCloseXClicked, nullptr);
            myUxRef->defaultYesNoChoiceDialogue->displayAdditionalMessage("Exit Minigame?");
            myUxRef->defaultYesNoChoiceDialogue->assignScoringProcessor(nullptr);
        }
    }

    bool minigamesEnabled(){
        Ux* uxInstance = Ux::Singleton();
        return uxInstance->settingsScroller->getBooleanSetting(Ux::uiSettingsScroller::UI_SETTING_MINIGAMES_ON);
    }

    bool isGameModeHardMode(){
        Ux* uxInstance = Ux::Singleton();
        return !uxInstance->settingsScroller->getBooleanSetting(Ux::uiSettingsScroller::UI_SETTING_GAME_EASY_MODE);
    }

    int requiredSwatchesForMinigames(){
        if( isGameModeHardMode() ){
            return 6;
        }
        return 3;
    }


    static Uint32 my_timer_callback(Uint32 interval, void* parm){
        Ux* myUxRef = Ux::Singleton();
        OpenGLContext* ogg=OpenGLContext::Singleton();
        Minigames* self = ogg->minigames;

        if( myUxRef->isMinigameMode && self->printRemainingTime() ){
            ogg->renderShouldUpdate = true;

            if( !self->gotMinigameAnnounceDone ){
                self->miniGameTextAnimComplete(nullptr);
            }

            return interval;
        }
        return 0; // ends the timer...
    }

    bool printRemainingTime(){
        Ux* myUxRef = Ux::Singleton();

        int elapsedMs = currentGame->getElapsedTime();
        int remainMs = currentGame->getTimeLimit() - elapsedMs;

        if( remainMs > 0 ){

        }else{
            /// our game is actually over!!!
            remainMs = 0;
        }

        int minutes = remainMs / msPerMinute;
        int seconds = (remainMs - (minutes * msPerMinute)) / 1000;

        //SDL_Log("Game Timer State is %i %i %i %i",elapsedMs,  remainMs,  minutes, seconds);

        SDL_snprintf(myUxRef->print_here, 6,  "%02i:%02i", minutes, seconds);
        gameTimer->print(myUxRef->print_here);

        return remainMs > 0;
    }

    void ceaseUpdatingTime(){
        // call this if your game has ended to reduce rendering... ( instead of calling directly, call gameIsCompleted() )
        if( my_timer_id > -1 ){
            SDL_RemoveTimer(my_timer_id);
            my_timer_id=-1;
        }
        my_timer_callback(0, nullptr); // one final update...
    }

    static void interactionCloseXClicked(Ux::uiObject *interactionObj, uiInteraction *delta){
        OpenGLContext* ogg=OpenGLContext::Singleton();
        Minigames* self = ogg->minigames;
        self->endGame();
    }

    void resize(){
        Ux* uxInstance = Ux::Singleton();
        if( uxInstance->widescreen ){

            gameHeadingHolder->setBoundaryRect(0.25, 0.0, 0.5, 1.0);

#ifdef COLORPICK_CLOCK_BAR_PRESENT

            controlBarTop->setBoundaryRect(0.25, 0.5, 0.5, 0.5); // this is really clock holder and nothing more
            minigamesCloseX->setBoundaryRect(0.05, 0.05, 0.1, 0.1);
            minigamesColorPickIcon->setBoundaryRect(1.0-0.1-0.05, 0.05, 0.1, 0.1);
#else

            controlBarTop->setBoundaryRect(0.25, 0.25, 0.5, 0.75); // this is really clock holder and nothing more
            minigamesCloseX->setBoundaryRect(0.05, 0.0, 0.1, 0.1);
            minigamesColorPickIcon->setBoundaryRect(1.0-0.1-0.05, 0.0, 0.1, 0.1);
#endif



        }else{
            gameHeadingHolder->setBoundaryRect(0.0, 0.0, 1.0, 1.0);

            float topPad = 0.0f;
            if( uxInstance->screenRatio < 0.5 ){
                topPad = 0.025;
            }
            controlBarTop->setBoundaryRect(0.0, topPad * 8, 1.0, 1.0);
            minigamesCloseX->setBoundaryRect(0.05, topPad, 0.1, 0.1);
            minigamesColorPickIcon->setBoundaryRect(1.0-0.1-0.05, topPad, 0.1, 0.1);
        }
        currentGame->resize();
        controls->updateRenderPosition();
    }

    void resizeFromGame(Ux::uiObject* gameRootUi){
        // reducing some code shared in all games... a class of shared minigame functions can go somewhere... maybe this should be another include
        Ux* uxInstance = Ux::Singleton();
        if( uxInstance->widescreen ){
            gameRootUi->setBoundaryRect(0.0, 0.0, 1.0, 1.0);
        }else{
            if( uxInstance->screenRatio < 0.5 ){
                gameRootUi->setBoundaryRect(0.0, 0.1, 1.0, 0.8);
            }else{
                gameRootUi->setBoundaryRect(0.0, 0.0, 1.0, 1.0);
            }
        }
        gameRootUi->updateRenderPosition();
    }

    void update(){
        currentGame->update();
    }

    void render(){
        currentGame->render();
    }

    static void miniGameTextAnimComplete(Ux::uiAnimation* uiAnim /* DO NOT USE */){
        OpenGLContext* ogg=OpenGLContext::Singleton();
        Minigames* self = ogg->minigames;
        Ux* myUxRef = Ux::Singleton();

        if( self->gotMinigameAnnounceDone ) return; // once only...
        self->gotMinigameAnnounceDone = true;


        // TELL THE CURRENT GAME THE TEXT ANIMATION IS DONE :)

        ogg->minigames->currentGame->begin();

        self->gameHeading->uiObjectItself->setAnimation(
            myUxRef->uxAnimations->moveTo(self->gameHeading->uiObjectItself,self->gameHeading->uiObjectItself->boundryRect.x, 0.57 , nullptr, nullptr) );

    }

    /* external API */
    void beginNextGame(){
        Ux* myUxRef = Ux::Singleton();

        int lastGame = currentGame->gameEnumIndex;
        int attempts = 0;

        while( lastGame == currentGame->gameEnumIndex && attempts < 10){
            currentGame = randomGame(); // roll again
            attempts++;
        }

        gotMinigameAnnounceDone = false;
        Ux::uiAminChain* myAnimChain = myUxRef->defaultScoreDisplay->displayExplanation(" Mini Game ");
        myAnimChain->addAnim((new Ux::uiAnimation(myUxRef->defaultScoreDisplay->explanation_position))->setAnimationReachedCallback(miniGameTextAnimComplete) );


        myUxRef->updateModal(myUxRef->minigamesUiContainer /*minigamesCloseX*/, &interactionCloseXClicked);

        myUxRef->isMinigameMode = true;

        gameCompleted = false;

        currentGame->show(); // we try to call show last, in case show determines it should endGame that it will work right.

        //gameHeadingHolder->setBoundaryRect(0.0, 0.0, 1.0, 1.0);

        gameHeading->uiObjectItself->resetPosition();

        gameHeading->size(1.0 / SDL_strlen(currentGame->getGameName()))->print(currentGame->getGameName());

        my_timer_id = -1;

        if( currentGame->getTimeLimit() > 0 ){
            // this game has a time limit.... lets make sure to show elapsed time....
            printRemainingTime();
            my_timer_id = SDL_AddTimer(1000, my_timer_callback, this);
        }else{
            // perhaps games that don't have a time limit, can (optionally?) show a timer that counts upwards ??
        }

        myUxRef->resizeUiElements();
    }

    /* you may call this from the minigame... if you want to auto exit after some amount of time */
    void endGame(){
        Ux* myUxRef = Ux::Singleton();
        OpenGLContext* ogg=OpenGLContext::Singleton();

        if( !myUxRef->isMinigameMode ) return; // we must have already exited the minigame....

        myUxRef->isMinigameMode = false;

        myUxRef->endModal(myUxRef->minigamesUiContainer);
        //myUxRef->disableControllerCursor(); // enter selection ads last color loosing score!  or did I hold enter too long????

        myUxRef->minigameColorList->clear();

        ogg->begin3dDropperAnimation(OpenGLContext::ANIMATION_ZOOM_INTO_DROPPER, nullptr);

        currentGame->end();

        myUxRef->resizeUiElements();

    }

    // when the user won (or lost) the game, call this
    // we'll stop any timer display from updating needlessly
    void gameIsCompleted(){
        ceaseUpdatingTime();
        gameCompleted=true;
    }

};


#endif
