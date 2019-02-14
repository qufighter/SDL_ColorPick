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
    &minigameN->end

struct Minigames{

    const static int maxGames = 10;

    typedef void (*aGenericGameCb)(void* gameItself);

    typedef struct GameListObj
    {
        GameListObj(Uint8 gameId, void* p_gameItself,
                    aGenericGameCb pShowFn,
                    aGenericGameCb pBeginFn,
                    aGenericGameCb pResizeFn,
                    aGenericGameCb pUpdateFn,
                    aGenericGameCb pRenderFn,
                    aGenericGameCb pEndFn
        ){
            gameEnumIndex = gameId;
            gameItself = p_gameItself;
            showFn=pShowFn;
            beginFn=pBeginFn;
            resizeFn = pResizeFn;
            updateFn=pUpdateFn;
            renderFn=pRenderFn;
            endFn=pEndFn;
        }

        void show(){showFn(gameItself);}
        void begin(){beginFn(gameItself);}
        void resize(){resizeFn(gameItself);}
        void update(){updateFn(gameItself);}
        void render(){renderFn(gameItself);}
        void end(){endFn(gameItself);}

        Uint8 gameEnumIndex;
        void* gameItself;
        aGenericGameCb showFn;
        aGenericGameCb beginFn;
        aGenericGameCb resizeFn;
        aGenericGameCb updateFn;
        aGenericGameCb renderFn;
        aGenericGameCb endFn;

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

    typedef enum  {
        GAME0_RESERVED=0,
        GAME1,
        GAME2,
        TOTAL_GAMES
    } MINIGAMES_ENUM;

#include "game1/minigame1.h"

    Minigames(){
        Ux* myUxRef = Ux::Singleton();




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

        minigamesCloseX = new Ux::uiObject();
        myUxRef->printCharToUiObject(minigamesCloseX, CHAR_CLOSE_ICON, DO_NOT_RESIZE_NOW);
        minigamesCloseX->hasForeground = true;
        minigamesCloseX->squarify();
        Ux::setColor(&minigamesCloseX->foregroundColor, 255, 255, 255, 96); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        minigamesCloseX->setClickInteractionCallback(&interactionCloseXClicked);
        minigamesCloseX->setBoundaryRect(0.0, 0.0, 0.1, 0.1);
        myUxRef->minigamesUiContainer->addChild(minigamesCloseX);


    }


    static void miniGameTextAnimComplete(Ux::uiAnimation* uiAnim){
        OpenGLContext* ogg=OpenGLContext::Singleton();
        // TELL THE CURRENT GAME THE TEXT ANIMATION IS DONE :)

        ogg->minigames->currentGame->begin();
//
    }


    void beginNextGame(){
        Ux* myUxRef = Ux::Singleton();
        currentGame = randomGame(); // roll again

        currentGame->show();

        Ux::uiAminChain* myAnimChain = myUxRef->defaultScoreDisplay->displayExplanation(" Mini Game ");
        myAnimChain->addAnim((new Ux::uiAnimation(myUxRef->defaultScoreDisplay->explanation_position))->setAnimationReachedCallback(miniGameTextAnimComplete) );

        myUxRef->isMinigameMode = true;
        myUxRef->resizeUiElements();
    }

    void endGame(){
        Ux* myUxRef = Ux::Singleton();
        OpenGLContext* ogg=OpenGLContext::Singleton();

        myUxRef->isMinigameMode = false;
        myUxRef->resizeUiElements();

        currentGame->end();


        myUxRef->minigameColorList->clear();

        //    ogg->minigames->endGame();
        //
        ogg->begin3dDropperAnimation(OpenGLContext::ANIMATION_ZOOM_INTO_DROPPER, nullptr);
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
