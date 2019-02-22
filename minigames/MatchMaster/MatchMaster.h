//
//
#ifndef ColorPick_iOS_SDL_MatchMaster_h
#define ColorPick_iOS_SDL_MatchMaster_h

struct MatchMaster{

    const char* gameName = "Match Master";
    const int maxSwatches = 6;
    const int timeLimit = 60000; // one minute....
    const int scoreBreakdownLn = 10;

    Uint8 gameIndex; // we try to keep this matching the childList index of minigamesUiContainer ....
    int startTime;
    int lastTicks;
    float tileHeight;
    float halfTileHeight;
    int activeSwatches;

    bool isReadyToScore;
    bool isComplete;
    int solveAttempts = 0;

    Ux::uiObject* gameRootUi;
    Ux::uiObject* gameSwatchesHolder;
    Ux::uiObject* scoreBreakdownHolder;

    Ux::uiText* scoreBreakdown0;
    Ux::uiText* scoreBreakdown1;
    Ux::uiText* scoreBreakdown2;
    Ux::uiText* scoreBreakdown3;
    Ux::uiText* scoreBreakdown4;

    Ux::uiList<Ux::uiSwatch*, Uint8>* pickList; // this is list of movable colors
    Ux::uiList<Ux::uiSwatch*, Uint8>* matchList; // list of match destinations

    Minigames* minigames; // handy ref?  cannot populate in constructor (yet??)

    MatchMaster(Uint8 pGameIndex){
        gameIndex = pGameIndex;
        Ux* myUxRef = Ux::Singleton();
        OpenGLContext* ogg=OpenGLContext::Singleton();
        minigames = ogg->minigames;

//        if( myUxRef->minigamesUiContainer->childListIndex != gameIndex ){
//            SDL_Log("WARNING: creating minigames in wrong ordering, or unexpected games found"); // does it matter?  we can just track myUxRef->minigamesUiContainer->childListIndex iuf we need it, but since we have the ref to gameRootUi... it should not really be needed
//        }

        gameRootUi = new Ux::uiObject();
        gameRootUi->hideAndNoInteraction();
        myUxRef->minigamesUiContainer->addChild(gameRootUi);

        gameSwatchesHolder = new Ux::uiObject();
        gameRootUi->addChild(gameSwatchesHolder);

        pickList = new Ux::uiList<Ux::uiSwatch*, Uint8>(maxSwatches);
        matchList = new Ux::uiList<Ux::uiSwatch*, Uint8>(maxSwatches);

        gameSwatchesHolder->setBoundaryRect(0.1, 0.1, 1.0-0.2, 1.0-0.2);

        int x;

        for( x=0; x<maxSwatches; x++ ){
            Ux::uiSwatch* tmp2 = new Ux::uiSwatch(gameSwatchesHolder, Float_Rect(0.25,0.25,0.5,0.5)); // ignore these rect....
            tmp2->displayHex();
            tmp2->hideBg();
            matchList->add(tmp2);
        }

        for( x=0; x<maxSwatches; x++ ){
            Ux::uiSwatch* tmp1 = new Ux::uiSwatch(gameSwatchesHolder, Float_Rect(0.25,0.25,0.5,0.5)); // ignore these rect....
            tmp1->uiObjectItself->setInteraction(&interactionSwatchDragMove);
            tmp1->uiObjectItself->setInteractionCallback(interactionSwatchDragMoveConstrain);

            //tmp1.displayHex(); // testing only...
            pickList->add(tmp1);
        }

        scoreBreakdownHolder =new Ux::uiObject();
        //scoreBreakdownHolder->setBoundaryRect(0.05, 0.1, 0.5, 1.0-0.2); // do this in resize instead....
        gameRootUi->addChild(scoreBreakdownHolder);
        float charSize = 1.0/scoreBreakdownLn;
        scoreBreakdown0 = (new Ux::uiText(scoreBreakdownHolder, charSize))->margins(-0.5,0.0,0.0,0.0)->print("Score0");
        scoreBreakdown1 = (new Ux::uiText(scoreBreakdownHolder, charSize))->margins(-0.25,0.0,0.0,0.0)->print("Score1");
        scoreBreakdown2 = (new Ux::uiText(scoreBreakdownHolder, charSize))->margins(0.0,0.0,0.0,0.0)->print("Score2");
        scoreBreakdown3 = (new Ux::uiText(scoreBreakdownHolder, charSize))->margins(0.25,0.0,0.0,0.0)->print("Score3");
        scoreBreakdown4 = (new Ux::uiText(scoreBreakdownHolder, charSize))->margins(0.5,0.0,0.0,0.0)->print("Score4");


        tileHeight = 0.2;
        halfTileHeight = 0.1;
        activeSwatches=0;
        //just a reminder.... don't init here, init in show();

    }

    static void interactionSwatchDragMove(Ux::uiObject *interactionObj, uiInteraction *delta){
        Ux* myUxRef = Ux::Singleton();

        OpenGLContext* ogg=OpenGLContext::Singleton();
        MatchMaster* self = (MatchMaster*)ogg->minigames->currentGame->gameItself; // helper?
        if( self->isComplete ){
            // lock it up..
            return;
        }

        //Ux::uiSwatch* swatch = (Ux::uiSwatch*)interactionObj->myUiController;
        myUxRef->interactionDragMove(interactionObj, delta);
    }

    static void interactionSwatchDragMoveConstrain(Ux::uiObject *interactionObj, uiInteraction *delta){
        Ux* myUxRef = Ux::Singleton();
        Ux::uiSwatch* swatch = (Ux::uiSwatch*)interactionObj->myUiController;

        OpenGLContext* ogg=OpenGLContext::Singleton();
        MatchMaster* self = (MatchMaster*)ogg->minigames->currentGame->gameItself; // helper?
        if( self->isComplete ){
            // lock it up..

            SDL_snprintf(myUxRef->print_here, 7,  "%02x%02x%02x", swatch->swatchItself->backgroundColor.r, swatch->swatchItself->backgroundColor.g, swatch->swatchItself->backgroundColor.b);

            //SDL_snprintf(myUxRef->print_here, 7,  "%02x%02x%02x", interactionObj->childList[0]->backgroundColor.r, interactionObj->childList[0]->backgroundColor.g, interactionObj->childList[0]->backgroundColor.b);
            myUxRef->defaultScoreDisplay->displayExplanation(myUxRef->print_here);
            return;
        }

        myUxRef->interactionDragMoveConstrain(interactionObj, delta, &interactionSwatchDragMoveConstrainToParentObject);
    }

    static void interactionSwatchDragMoveConstrainToParentObject(Ux::uiAnimation* uiAnim){
        Ux* uxInstance = Ux::Singleton();
        Ux::uiObject* interactionObj = uiAnim->myUiObject;
        interactionObj->cancelCurrentAnimation();
        uxInstance->interactionConstrainToParentObject(uiAnim, &interactionSwatchDragMoveConstrainToParentObject); // this might apply a constrain to viewport animation, in which case we won't try to override it...

        if( !interactionObj->isAnimating() ){ // its in the viewport still... lets snap it to the nearest dest if close enough...

            OpenGLContext* ogg=OpenGLContext::Singleton();
            MatchMaster* self = (MatchMaster*)ogg->minigames->currentGame->gameItself; // helper?

            self->checkIfGameIsCompleted(uiAnim); // mostly to reset

            for( int x=0; x<self->maxSwatches; x++ ){
                if( x < uxInstance->minigameColorList->total() ){
                    Ux::uiSwatch* dest = *self->matchList->get(x);
                    float dist = glm::distance(
                       glm::vec2(interactionObj->boundryRect.x, interactionObj->boundryRect.y),
                       glm::vec2(dest->uiObjectItself->boundryRect.x, dest->uiObjectItself->boundryRect.y)
                    );
                    if( dist < self->halfTileHeight ){
                        //SDL_Log("Distance from this one is %f", dist);
                        //interactionObj->setAnimation( uxInstance->uxAnimations->moveTo(interactionObj,dest->uiObjectItself->boundryRect.x,dest->uiObjectItself->boundryRect.y, nullptr, nullptr) );

                        Ux::uiAminChain* myAnimChain = new Ux::uiAminChain();
                        myAnimChain->addAnim((new Ux::uiAnimation(interactionObj))->moveTo(dest->uiObjectItself->boundryRect.x,dest->uiObjectItself->boundryRect.y) );
                        myAnimChain->addAnim((new Ux::uiAnimation(interactionObj))->setAnimationReachedCallback(&checkIfGameIsCompleted) );
                        interactionObj->setAnimation(myAnimChain); // imporrtant to do this before we push it..
                        uxInstance->uxAnimations->pushAnimChain(myAnimChain);

                        // NOTE: when the above animation completes, this swatch is "locked" until we move it again...
                        // once all swatches are locked.... then the game is ready to complete...
                        break;
                    }
                }
            }
        }
    }

    static void checkIfGameIsCompleted(Ux::uiAnimation* uiAnim){
        OpenGLContext* ogg=OpenGLContext::Singleton();
        MatchMaster* self = (MatchMaster*)ogg->minigames->currentGame->gameItself; // helper?

        if( self->isGameComplete() ){
            //SDL_Log("Looks like you won!");
            //self->showMatches();
            self->showColors();

            if( self->isComplete ) return; // once only.... ???
            self->solveAttempts += 1;
            self->isComplete = true; // complete, and won, lock it up!
            self->lastTicks = SDL_GetTicks();

            ogg->minigames->gameIsCompleted(); // save CPUs.

            // at this piont, we can allow the game to end and lock it up somehow.... ?
            // and we show the score....
            self->computeGameScore();


        }else{
            if( self->isReadyToScore ){
                //SDL_Log("Looks like LOSS");
                self->solveAttempts += 1;
                self->showMatches();
            }else{
                //SDL_Log("Looks like INCOMPLETE");
            }
        }
    }

    int computeGameScore(){
        Ux* uxInstance = Ux::Singleton();

        if( solveAttempts > 0 ){

            int elapsedMs = lastTicks - startTime;
            //activeSwatches; // more for more points.... its sort of mulitplier?? we can subtract one less than minimum though...

            //solveAttempts; // divide by this....

            int timeBonus = (timeLimit - elapsedMs) / 1000;

            if( timeBonus < 1 ) timeBonus = 1;

            int finalScore = (timeBonus * activeSwatches) / solveAttempts;


            uxInstance->defaultScoreDisplay->display(gameSwatchesHolder->childList[0], finalScore, SCORE_EFFECTS::MOVE_UP);

            scoreBreakdownHolder->show();

            SDL_snprintf(uxInstance->print_here, scoreBreakdownLn,  "%i swatch", activeSwatches);
            scoreBreakdown0->print(uxInstance->print_here);

            SDL_snprintf(uxInstance->print_here, scoreBreakdownLn,  ":%02i bonus", timeBonus);
            scoreBreakdown1->print(uxInstance->print_here);


            if( solveAttempts != 1 ){
                SDL_snprintf(uxInstance->print_here, scoreBreakdownLn,  "/%i tries", solveAttempts);
            }else{
                SDL_snprintf(uxInstance->print_here, scoreBreakdownLn,  "/%i try", solveAttempts);
            }
            scoreBreakdown2->print(uxInstance->print_here);


            SDL_snprintf(uxInstance->print_here, scoreBreakdownLn,  "+%i", finalScore);
            scoreBreakdown3->print(uxInstance->print_here);

            SDL_snprintf(uxInstance->print_here, scoreBreakdownLn,  "x%i combo", uxInstance->defaultScoreDisplay->combo_chain);
            scoreBreakdown4->print(uxInstance->print_here);

            SDL_Log("Minigame: just applied final score.... %i", finalScore);

            return finalScore;
        }else{
            return 0; // impossible to reach, right?
        }
    }

    void showMatches(){
        Ux* uxInstance = Ux::Singleton();
        OpenGLContext* ogg=OpenGLContext::Singleton();
        MatchMaster* self = (MatchMaster*)ogg->minigames->currentGame->gameItself; // helper?

        // we verify before this is called, every dest has 1 and only 1 match....

        for( int x=0; x<self->activeSwatches; x++ ){
            Ux::uiSwatch* dest = *self->matchList->get(x);
            for( int y=0; y<self->activeSwatches; y++ ){
                Ux::uiSwatch* move = *self->pickList->get(y);

                // this is reallly a dupe search... oh well??
                if( dest->uiObjectItself->boundryRect.x == move->uiObjectItself->boundryRect.x && dest->uiObjectItself->boundryRect.y == move->uiObjectItself->boundryRect.y ){
                    if( !Ux::colorEquals(&dest->last_color, &move->last_color) ){
                        SDL_snprintf(uxInstance->print_here, 7,  "  ");
                        move->print(uxInstance->print_here);
                        uxInstance->printCharToUiObject(move->hexDisplay->getTextChar(1), CHAR_CANCEL_ICON, DO_NOT_RESIZE_NOW);

                    }else{
                        SDL_snprintf(uxInstance->print_here, 7,  "   OK"); // just saying, I like uxInstance->print_here and all, but we could have snprintf'd this once at boot time....
                        move->print(uxInstance->print_here);
                        uxInstance->printCharToUiObject(move->hexDisplay->getTextChar(1), CHAR_CHECKMARK_ICON, DO_NOT_RESIZE_NOW);
                    }


                }
            }
        }

    }

    void showColors(){ // when we win!
        for( int y=0; y<activeSwatches; y++ ){
            Ux::uiSwatch* move = *pickList->get(y);
            move->displayHex();
            move->refresh();
        }
    }

    bool isGameComplete(){
        Ux* uxInstance = Ux::Singleton();
        OpenGLContext* ogg=OpenGLContext::Singleton();
        MatchMaster* self = (MatchMaster*)ogg->minigames->currentGame->gameItself; // helper?

        bool isWin = true; // lets see if any of them are non
        bool isReadyToScore = true;

        for( int x=0; x<self->activeSwatches; x++ ){
            Ux::uiSwatch* dest = *self->matchList->get(x);
            bool hasMatch = false;
            for( int y=0; y<self->activeSwatches; y++ ){
                // see if any moves are placed here...
                Ux::uiSwatch* move = *self->pickList->get(y);

                move->print(""); // we clear too many times, maybe this should be the outer loop?

                if( dest->uiObjectItself->boundryRect.x == move->uiObjectItself->boundryRect.x && dest->uiObjectItself->boundryRect.y == move->uiObjectItself->boundryRect.y ){

                    if( hasMatch ){
                        // more than one color is here... lets slide it out... win is impossible now...
                        isWin = false;
                        move->uiObjectItself->setAnimation( uxInstance->uxAnimations->moveTo(move->uiObjectItself,move->uiObjectItself->origBoundryRect.x,move->uiObjectItself->origBoundryRect.y, nullptr, nullptr) );
                    }else{
                        // first match, lets evaluate it...
                        if( !Ux::colorEquals(&dest->last_color, &move->last_color) ){
                            isWin = false;
                        }
                    }

                    hasMatch = true;
                }
            }

            if( !hasMatch ){
                isWin = false;
                isReadyToScore = false;
            }

        }

        self->isReadyToScore = isReadyToScore;
        return isWin;
    }

    // "reset state"
    static void show(void* gameItself){
        MatchMaster* self = (MatchMaster*)gameItself;
        SDL_Log("%s show", self->gameName);
        Ux* myUxRef = Ux::Singleton();

        self->isComplete= false;
        self->isReadyToScore = false;
        self->solveAttempts = 0;

        self->scoreBreakdownHolder->hide();

        //    minigameColorList = new uiList<ColorList, Uint8>(minigameColorListMax)

        Ux::uiList<Ux::ColorList, Uint8>* myColorList;
        Ux::uiList<Ux::ColorList, Uint8>* myDestList;

        myColorList = myUxRef->minigameColorList->clone();
        myColorList->sort(&Ux::randomSort);

        int totalTiles = SDL_min(myColorList->total(), self->maxSwatches);

        myDestList = new Ux::uiList<Ux::ColorList, Uint8>(totalTiles);
        for( int x=0; x<totalTiles; x++ ){ // clipped
            myDestList->add(*myColorList->get(x));
        }
        SDL_free(myColorList);

        myColorList = myDestList->clone();
        myColorList->sort(&Ux::randomSort);

        float height = 1.0 / (totalTiles + 0.0f);
        self->tileHeight = height;
        self->halfTileHeight = height * 0.5;

        float vertPad = 0.01;
        float vertPadDist = vertPad / (totalTiles + 0.0f);
        height -= vertPad;

        //colorPickState->viewport_ratio

        self->activeSwatches = myColorList->total();

        for( int x=0; x<self->maxSwatches; x++ ){

            Ux::uiSwatch* move = *self->pickList->get(x);
            Ux::uiSwatch* dest = *self->matchList->get(x);

            if( x < self->activeSwatches ){

                float y = (self->tileHeight * x) + (vertPadDist * x);

                move->show();
                dest->show();

                move->uiObjectItself->setBoundaryRect(0.0, y, 0.4, height);
                dest->uiObjectItself->setBoundaryRect(0.6, y, 0.4, height);

                move->hideHex()->update(&myColorList->get(x)->color);
                dest->update(&myDestList->get(x)->color);

            }else{
                move->hide();
                dest->hide();
            }
        }


        SDL_free(myColorList);
        SDL_free(myDestList);

        self->gameRootUi->showAndAllowInteraction();
        self->startTime = SDL_GetTicks();


        // test code, it works, so delete it!
//                        if( myUxRef->minigameColorList->total() < 99 ){
//                            SDL_Log("Error: somehow we begin the game without enough swatches!");
//                            self->minigames->endGame();
//                            return; // this breaks the rest... need to finish this function I think...
//                        }
    }

    static void begin(void* gameItself){
        MatchMaster* self = (MatchMaster*)gameItself;
        SDL_Log("%s begin", self->gameName);
        self->startTime = SDL_GetTicks();
        Ux* myUxRef = Ux::Singleton();

    }

    static void resize(void* gameItself){
        MatchMaster* self = (MatchMaster*)gameItself;
        SDL_Log("%s resize", self->gameName);
        Ux* uxInstance = Ux::Singleton();
        if( uxInstance->widescreen ){
            self->scoreBreakdownHolder->setBoundaryRect(0.25, 0.1, 0.25, 1.0-0.2);
        }else{
            self->scoreBreakdownHolder->setBoundaryRect(0.05, 0.1, 0.5, 1.0-0.2);
        }

        self->gameRootUi->updateRenderPosition();
    }

    static void update(void* gameItself){
        MatchMaster* self = (MatchMaster*)gameItself;
        //SDL_Log("%s update", self->gameName);
    }

    static void render(void* gameItself){
        MatchMaster* self = (MatchMaster*)gameItself;
        //SDL_Log("%s render", self->gameName);
    }

    static void end(void* gameItself){
        MatchMaster* self = (MatchMaster*)gameItself;
        SDL_Log("%s end", self->gameName);
        self->gameRootUi->hideAndNoInteraction();
    }

    static const char* getGameName(void* gameItself){
        MatchMaster* self = (MatchMaster*)gameItself;
        return self->gameName;
    }

    static int getTimeLimit(void* gameItself){
        MatchMaster* self = (MatchMaster*)gameItself;
        return self->timeLimit;
    }

//    static int getStartTime(void* gameItself){ // is this used?
//        MatchMaster* self = (MatchMaster*)gameItself;
//        return self->startTime;
//    }

//    static int getRemainingTime(void* gameItself){ // NOT IMPLEMENTED
//        MatchMaster* self = (MatchMaster*)gameItself;
//        return self->getTimeLimit(self) - self->getElapsedTime(self);
//    }

    static int getElapsedTime(void* gameItself){
        // maybe game can pause itself...
        MatchMaster* self = (MatchMaster*)gameItself;

        if( self->isComplete ){
            return self->lastTicks - self->startTime;
        }
        return SDL_GetTicks() - self->startTime; // generally we return teh current ticks, or when the game ended
    }

};


#endif
