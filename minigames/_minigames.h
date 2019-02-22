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
    &minigameN->getStartTime, \
    &minigameN->getElapsedTime, \
    &minigameN->getTimeLimit

struct Minigames{

    const static int maxGames = 10;

    typedef void (*aGenericGameCb)(void* gameItself);
    typedef int (*anIntGenericGameCb)(void* gameItself);

    typedef struct GameListObj
    {
        GameListObj(Uint8 gameId, void* p_gameItself,
                    aGenericGameCb pShowFn,
                    aGenericGameCb pBeginFn,
                    aGenericGameCb pResizeFn,
                    aGenericGameCb pUpdateFn,
                    aGenericGameCb pRenderFn,
                    aGenericGameCb pEndFn,
                    anIntGenericGameCb pGetStartTimeFn,
                    anIntGenericGameCb pGetElapsedTimeFn,
                    anIntGenericGameCb pGetTimeLimitFn
        ){
            gameEnumIndex = gameId;
            gameItself = p_gameItself;
            showFn=pShowFn;
            beginFn=pBeginFn;
            resizeFn = pResizeFn;
            updateFn=pUpdateFn;
            renderFn=pRenderFn;
            endFn=pEndFn;
            startTimeFn=pGetStartTimeFn;
            timeLimitFn=pGetTimeLimitFn;
            elapsedTimeFn=pGetElapsedTimeFn;
        }

        void show(){showFn(gameItself);}
        void begin(){beginFn(gameItself);}
        void resize(){resizeFn(gameItself);}
        void update(){updateFn(gameItself);}
        void render(){renderFn(gameItself);}
        void end(){endFn(gameItself);}
        int getStartTime(){return startTimeFn(gameItself);} // not used anymore...
        int getElapsedTime(){return elapsedTimeFn(gameItself);}
        int getTimeLimit(){return timeLimitFn(gameItself);}

        Uint8 gameEnumIndex;
        void* gameItself;
        aGenericGameCb showFn;
        aGenericGameCb beginFn;
        aGenericGameCb resizeFn;
        aGenericGameCb updateFn;
        aGenericGameCb renderFn;
        aGenericGameCb endFn;
        anIntGenericGameCb startTimeFn;
        anIntGenericGameCb elapsedTimeFn;
        anIntGenericGameCb timeLimitFn;

    } GameListObj;

    static int indexForGame(GameListObj* gameListObj){
        //SDL_Log("index lookup... %i", gameListObj->gameEnumIndex);
        return gameListObj->gameEnumIndex;
    }
    int randomInt(int min, int max){
        return min + rand() % (max - min + 1);
    }
    int randomGameId(){
        return randomInt(1,TOTAL_GAMES-1);
    }
    GameListObj* randomGame(){
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
    Ux::uiObject* minigamesCloseX;
    Ux::uiText* gameTimer;
    int my_timer_id;

    typedef enum  {
        GAME0_RESERVED=0,
        GAME1,
        GAME2,
        TOTAL_GAMES
    } MINIGAMES_ENUM;

#include "game1/minigame1.h"

    Minigames(){
        Ux* myUxRef = Ux::Singleton();

        // todo: if we get ogg and then set the ref now ogg->minigames = this; .... then when we construct our minigamess they'll be able to get the reference below...
        OpenGLContext* ogg=OpenGLContext::Singleton();
        ogg->minigames = this;



        // constructor....
        gameList = new Ux::uiList<GameListObj, Uint8>(maxGames);
        gameList->index(MINIGAMES_ENUM::TOTAL_GAMES, indexForGame);


        Minigame1* minigame1 = new Minigame1(MINIGAMES_ENUM::GAME1);
        Minigame1* minigame2 = new Minigame1(MINIGAMES_ENUM::GAME2);

        GameListObj aGame1 = GameListObj(makeGameArgs(minigame1));
        GameListObj aGame2 = GameListObj(makeGameArgs(minigame2));

        gameList->add(aGame1);
        gameList->add(aGame2);

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

        Ux::uiObject* controls = new Ux::uiObject();
        Ux::uiObject* controlBarTop = new Ux::uiObject();
        controlBarTop->setBoundaryRect(0.0, 0.0, 1.0, 0.1);

        controls->addChild(controlBarTop);
        myUxRef->minigamesUiContainer->addChild(controls);

        minigamesCloseX = new Ux::uiObject();
        myUxRef->printCharToUiObject(minigamesCloseX, CHAR_CLOSE_ICON, DO_NOT_RESIZE_NOW);
        minigamesCloseX->hasForeground = true;
        minigamesCloseX->squarify();
        Ux::setColor(&minigamesCloseX->foregroundColor, 255, 255, 255, 96); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        minigamesCloseX->setClickInteractionCallback(&interactionCloseXClicked);
        minigamesCloseX->setBoundaryRect(0.05, 0.0, 0.1, 1.0);
        controlBarTop->addChild(minigamesCloseX);


        gameTimer = (new Ux::uiText(controlBarTop, 1.0/5.0))->pad(0.0,0.0)->margins(0.0,0.25,0.0,0.25)->print("00:00");
    }


    static void miniGameTextAnimComplete(Ux::uiAnimation* uiAnim){
        OpenGLContext* ogg=OpenGLContext::Singleton();
        // TELL THE CURRENT GAME THE TEXT ANIMATION IS DONE :)

        ogg->minigames->currentGame->begin();
//
    }

    static Uint32 my_timer_callback(Uint32 interval, void* parm){
        Ux* myUxRef = Ux::Singleton();
        OpenGLContext* ogg=OpenGLContext::Singleton();
        Minigames* self = ogg->minigames;

        if( myUxRef->isMinigameMode && self->printRemainingTime() ){
            ogg->renderShouldUpdate = true;
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

        int minutes = remainMs / 60000;
        int seconds = (remainMs - (minutes * 60000)) / 1000;

        SDL_Log("Game Timer State is %i %i %i %i",elapsedMs,  remainMs,  minutes, seconds);

        SDL_snprintf(myUxRef->print_here, 6,  "%02i:%02i", minutes, seconds);
        gameTimer->print(myUxRef->print_here);

        return remainMs > 0;
    }

    // maybe this should be more generic to state -user won game... and be a "more mandatory" callback....
    // game ends itself but really displays results screen first....
    // organize the functions useful for the game to call in the same place...
    void ceaseUpdatingTime(){
        // call this if your game has ended to reduce rendering...
        if( my_timer_id > -1 ){
            SDL_RemoveTimer(my_timer_id);
            my_timer_id=-1;
        }
        my_timer_callback(1, nullptr); // one final update...
    }

    void beginNextGame(){
        Ux* myUxRef = Ux::Singleton();
        currentGame = randomGame(); // roll again

        Ux::uiAminChain* myAnimChain = myUxRef->defaultScoreDisplay->displayExplanation(" Mini Game ");
        myAnimChain->addAnim((new Ux::uiAnimation(myUxRef->defaultScoreDisplay->explanation_position))->setAnimationReachedCallback(miniGameTextAnimComplete) );

        myUxRef->updateModal(myUxRef->minigamesUiContainer /*minigamesCloseX*/, &interactionCloseXClicked);

        myUxRef->isMinigameMode = true;

        currentGame->show(); // we try to call show last, in case show determines it should endGame that it will work right.

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

    void endGame(){
        Ux* myUxRef = Ux::Singleton();
        OpenGLContext* ogg=OpenGLContext::Singleton();

        myUxRef->isMinigameMode = false;

        myUxRef->endModal(myUxRef->minigamesUiContainer);

        myUxRef->minigameColorList->clear();

        ogg->begin3dDropperAnimation(OpenGLContext::ANIMATION_ZOOM_INTO_DROPPER, nullptr);

        currentGame->end();

        myUxRef->resizeUiElements();

    }

    static void interactionCloseXClicked(Ux::uiObject *interactionObj, uiInteraction *delta){
        OpenGLContext* ogg=OpenGLContext::Singleton();
        Minigames* self = ogg->minigames;
        self->endGame();
    }

    void resize(){
        // todo: resize close x ?
        currentGame->resize();
    }

    void update(){
        currentGame->update();
    }

    void render(){
        currentGame->render();
    }

};


#endif
