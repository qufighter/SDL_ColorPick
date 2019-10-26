//
//
#ifndef ColorPick_iOS_SDL_FlipMaster_h
#define ColorPick_iOS_SDL_FlipMaster_h

struct FlipMaster{

    const char* gameName = "Flip Matcher";
    const int maxSwatches = 12;  // max 6 colors and one match for each
    const int timeLimit = 60000; // one minute....
    const int scoreBreakdownLn = 10;

    Uint8 gameIndex; // we try to keep this matching the childList index of minigamesUiContainer ....
    int startTime;
    int lastTicks;

    //int unflipCounter = 0;

    int activeSwatches;
    int matchedSwatches;

    bool isComplete;
    int solveAttempts = 0;

    Ux::uiObject* gameRootUi;
    Ux::uiObject* gameSwatchesHolder;
    Ux::uiObject* scoreBreakdownHolder;
    Ux::uiObject* scoreBreakdownSpacer;

    Ux::uiText* scoreBreakdown0;
    Ux::uiText* scoreBreakdown1;
    Ux::uiText* scoreBreakdown2;
    Ux::uiText* scoreBreakdown3;
    Ux::uiText* scoreBreakdown4;

    Ux::uiList<Ux::uiSwatch*, Uint8>* pickList; // this is list of flip-able colors

    Ux::uiSwatch* flipA;
    Ux::uiSwatch* flipB;

    Ux::uiSwatch* flippedA;
    Ux::uiSwatch* flippedB;

    Minigames* minigames;

    bool currentlyEvaluatingWin;

    FlipMaster(Uint8 pGameIndex){
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
        //matchList = new Ux::uiList<Ux::uiSwatch*, Uint8>(maxSwatches);

        gameSwatchesHolder->setBoundaryRect(0.1, 0.1, 1.0-0.2, 1.0-0.2); // some margins all around...
        //gameSwatchesHolder->squarify();

        for( int x=0; x<maxSwatches; x++ ){
            Ux::uiSwatch* tmp1 = new Ux::uiSwatch(gameSwatchesHolder, Float_Rect(0.25,0.25,0.5,0.5)); // ignore these rect....
            tmp1->uiObjectItself->setClickInteractionCallback(interactionSwatchClick);
            //tmp1.displayHex(); // testing only...
            pickList->add(tmp1);
        }

        // TODO: remove ? score as we go ?? ....
        scoreBreakdownHolder =new Ux::uiObject();
        scoreBreakdownSpacer =new Ux::uiObject();
        scoreBreakdownHolder->setBoundaryRect(0.0, 0.0, 0.5, 1.0); // do this in resize instead....
        scoreBreakdownHolder->addChild(scoreBreakdownSpacer);
        gameRootUi->addChild(scoreBreakdownHolder);

        scoreBreakdownHolder->setBackgroundColor(0,0,0,192);

        float charSize = 1.0/scoreBreakdownLn;
        scoreBreakdown0 = (new Ux::uiText(scoreBreakdownSpacer, charSize))->margins(-0.5,0.0,0.0,0.0)->print("Score0");
        scoreBreakdown1 = (new Ux::uiText(scoreBreakdownSpacer, charSize))->margins(-0.25,0.0,0.0,0.0)->print("Score1");
        scoreBreakdown2 = (new Ux::uiText(scoreBreakdownSpacer, charSize))->margins(0.0,0.0,0.0,0.0)->print("Score2");
        scoreBreakdown3 = (new Ux::uiText(scoreBreakdownSpacer, charSize))->margins(0.25,0.0,0.0,0.0)->print("Score3");
        scoreBreakdown4 = (new Ux::uiText(scoreBreakdownSpacer, charSize))->margins(0.5,0.0,0.0,0.0)->print("Score4");


        activeSwatches=0;
        currentlyEvaluatingWin=false;
        //just a reminder.... don't init here, init in show();

    }

    static void interactionSwatchClick(Ux::uiObject *interactionObj, uiInteraction *delta){
        OpenGLContext* ogg=OpenGLContext::Singleton();
        FlipMaster* self = (FlipMaster*)ogg->minigames->currentGame->gameItself; // helper?
        Ux* myUxRef = ogg->generalUx;
        Ux::uiSwatch* swatch = (Ux::uiSwatch*)interactionObj->myUiController;


        if( self->isComplete ){
            // lock it up..
            SDL_snprintf(myUxRef->print_here, 7,  "%02x%02x%02x", swatch->last_color.r, swatch->last_color.g, swatch->last_color.b);
            //SDL_snprintf(myUxRef->print_here, 7,  "%02x%02x%02x", swatch->swatchItself->backgroundColor.r, swatch->swatchItself->backgroundColor.g, swatch->swatchItself->backgroundColor.b);
            //SDL_snprintf(myUxRef->print_here, 7,  "%02x%02x%02x", interactionObj->childList[0]->backgroundColor.r, interactionObj->childList[0]->backgroundColor.g, interactionObj->childList[0]->backgroundColor.b);
            myUxRef->defaultScoreDisplay->displayExplanation(myUxRef->print_here);
            return;
        }

        if( swatch->isFlipped() ){
            return; // cannot flip a flipped swatch....
        }

        if( self->flipA != nullptr && self->flipB != nullptr && !self->currentlyEvaluatingWin && self->flipA!=swatch && self->flipB !=swatch  ){
            self->currentlyEvaluatingWin = true;
            // if the above are a match... we can probably reset and let them keep going!...
            // during this we should arguably LOCK or delay the animation thread that will call checkIfGameIsCompleted.... (animations may not be threaded though... so we won't use semaphore)
            // the following is probably nearly atomic op though
            if( Ux::colorEquals(&self->flipA->last_color, &self->flipB->last_color) ){
                self->flippedA = self->flipA;
                self->flippedB = self->flipB;
                self->isGameComplete(); // this should reset it all....
            }
            self->currentlyEvaluatingWin = false;
        }

        if( self->flipA == nullptr ){
            // first flip...
            self->flipA = swatch;
        }else if ( self->flipB == nullptr && swatch != self->flipA ){
            // second flip...
            self->flipB = swatch;
        }else{
            // already have two flips, (and they didn't match, or we would have reset it above) block it!
            return;
        }

        interactionObj->setAnimation(myUxRef->uxAnimations->flip_hz(interactionObj, 1000, &halfFlipped, &fullyFlipped));
    }

    static void halfFlipped(Ux::uiAnimation* uiAnim){
        OpenGLContext* ogg=OpenGLContext::Singleton();
        FlipMaster* self = (FlipMaster*)ogg->minigames->currentGame->gameItself; // helper?
        //SDL_Log("Half flipped");
        //Ux* uxInstance = Ux::Singleton();
        Ux::uiObject* interactionObj = uiAnim->myUiObject;
        Ux::uiSwatch* swatch = (Ux::uiSwatch*)interactionObj->myUiController;
        swatch->displayBg()->displayHex()->refresh();
    }

    static void fullyFlipped(Ux::uiAnimation* uiAnim){
        OpenGLContext* ogg=OpenGLContext::Singleton();
        FlipMaster* self = (FlipMaster*)ogg->minigames->currentGame->gameItself; // helper?
        //SDL_Log("Fully flipped");
        Ux* uxInstance = Ux::Singleton();
        Ux::uiObject* interactionObj = uiAnim->myUiObject;
        Ux::uiSwatch* swatch = (Ux::uiSwatch*)interactionObj->myUiController;

        if( swatch == self->flipA ){
            self->flippedA = swatch;
        }else if( swatch == self->flipB ){
            self->flippedB = swatch;
        }else{
            SDL_Log("Hmm - seems like a flippin' error - a flipped swatch isn't one we track... (maybe we already scored it)"); // could be we just already scored that flip since it was a match!
            return;
        }

        if( self->flippedA != nullptr && self->flippedB != nullptr ){
            // both are flipped :)
            //SDL_Log("we have 2 flips now... lets compare them and evaluate in anothe fn");
            checkIfGameIsCompleted(uiAnim);
        }

    }

    static void halfUnflipped(Ux::uiAnimation* uiAnim){
        OpenGLContext* ogg=OpenGLContext::Singleton();
        FlipMaster* self = (FlipMaster*)ogg->minigames->currentGame->gameItself; // helper?
        //SDL_Log("Half unflipped");
        //Ux* uxInstance = Ux::Singleton();
        Ux::uiObject* interactionObj = uiAnim->myUiObject;
        Ux::uiSwatch* swatch = (Ux::uiSwatch*)interactionObj->myUiController;
        swatch->hideHex()->hideBg()->refresh();
    }

    static void fullyUnflipped(Ux::uiAnimation* uiAnim){
//        OpenGLContext* ogg=OpenGLContext::Singleton();
//        FlipMaster* self = (FlipMaster*)ogg->minigames->currentGame->gameItself; // helper?
//        self->unflipCounter++;
//        if( self->unflipCounter >= 2 ){
//            // only unlock when both unflips done...
////            self->flippedA = nullptr;
////            self->flippedB = nullptr;
////            self->flipA = nullptr;
////            self->flipB = nullptr;
//        }
    }

    static void checkIfGameIsCompleted(Ux::uiAnimation* uiAnim){
        OpenGLContext* ogg=OpenGLContext::Singleton();
        FlipMaster* self = (FlipMaster*)ogg->minigames->currentGame->gameItself; // helper?
        int ctr = 0;
        while( self->currentlyEvaluatingWin && ctr < 50 ){
            SDL_Delay(10);
            ctr++;
        }
        if( ctr > 49 ){
            SDL_Log("Error: somehow the win evaluation is not unlocked after 50 attempts.... this should never occur...");
        }
        self->currentlyEvaluatingWin=true;
        if( self->isGameComplete() ){
            //SDL_Log("Looks like you won!");
            if( self->isComplete ){
                self->currentlyEvaluatingWin = false;
                return; // once only.... ???
            }
            self->isComplete = true; // complete, and won, lock it up!
            self->lastTicks = SDL_GetTicks();
            ogg->minigames->gameIsCompleted(); // save CPUs.
            self->computeGameScore();// and we show the score....
        }
        self->currentlyEvaluatingWin=false;
    }

    bool isGameComplete(){
        Ux* uxInstance = Ux::Singleton();
        OpenGLContext* ogg=OpenGLContext::Singleton();
        FlipMaster* self = (FlipMaster*)ogg->minigames->currentGame->gameItself; // helper?
        bool isWin = false; // lets see if any of them are non

        if( self->flippedA != nullptr && self->flippedB != nullptr ){
            self->solveAttempts++;
            // if they match, leave em, otherwise, flip them back....
            if( Ux::colorEquals(&self->flippedA->last_color, &self->flippedB->last_color) ){
                //SDL_Log("matches"); // we leave them flipped and unlock everything....
                self->flippedA = nullptr;
                self->flippedB = nullptr;
                self->flipA = nullptr;
                self->flipB = nullptr;
                self->matchedSwatches += 2; // progress towards win...
            }else{
                //SDL_Log("no matches");
                //self->unflipCounter=0;

                self->flippedA->uiObjectItself->setAnimation(uxInstance->uxAnimations->flip_hz(self->flippedA->uiObjectItself, 700, &halfUnflipped, &fullyUnflipped));
                self->flippedB->uiObjectItself->setAnimation(uxInstance->uxAnimations->flip_hz(self->flippedB->uiObjectItself, 750, &halfUnflipped, &fullyUnflipped));

                self->flippedA = nullptr;
                self->flippedB = nullptr;
                self->flipA = nullptr;
                self->flipB = nullptr;
            }
        }
        //SDL_Log("lets see if the game is done! %i %i", self->matchedSwatches, self->activeSwatches );
        if( self->matchedSwatches >= self->activeSwatches ){
            isWin = true;
        }
        return isWin;
    }


    int computeGameScore(){
        Ux* uxInstance = Ux::Singleton();
        OpenGLContext* ogg=OpenGLContext::Singleton();

        if( solveAttempts > 0 ){

            int elapsedMs = lastTicks - startTime;
            //activeSwatches; // more for more points.... its sort of mulitplier?? we can subtract one less than minimum though...

            int inputSwatches = activeSwatches * 0.5;

            //solveAttempts; // divide by this....

            int timeBonus = (timeLimit - elapsedMs) / 1000;

            if( timeBonus < 1 ) timeBonus = 1;

            int finalScore = (timeBonus * activeSwatches) / solveAttempts;

            if(finalScore > 0){
                uxInstance->defaultScoreDisplay->display(gameSwatchesHolder->childList[1], finalScore, SCORE_EFFECTS::MOVE_UP);

                if( timeBonus > 50 && solveAttempts < 4 ){
                    ogg->doRequestReview();
                }
            }

            scoreBreakdownHolder->show();

            SDL_snprintf(uxInstance->print_here, scoreBreakdownLn,  "%i swatch", activeSwatches);
            scoreBreakdown0->print(uxInstance->print_here);

            SDL_snprintf(uxInstance->print_here, scoreBreakdownLn,  ":%02i bonus", timeBonus);
            scoreBreakdown1->print(uxInstance->print_here);


            if( solveAttempts != 1 ){
                SDL_snprintf(uxInstance->print_here, scoreBreakdownLn,  "/%i moves", solveAttempts);
            }else{
                SDL_snprintf(uxInstance->print_here, scoreBreakdownLn,  "/%i try", solveAttempts);
            }
            scoreBreakdown2->print(uxInstance->print_here);


            SDL_snprintf(uxInstance->print_here, scoreBreakdownLn,  "+%i", finalScore);
            scoreBreakdown3->print(uxInstance->print_here);

            SDL_snprintf(uxInstance->print_here, scoreBreakdownLn,  "x%i combo", uxInstance->defaultScoreDisplay->combo_chain);
            scoreBreakdown4->print(uxInstance->print_here);

            //SDL_Log("Minigame: just applied final score.... %i", finalScore);

            return finalScore;
        }else{
            return 0; // impossible to reach, right?
        }
    }

    // "reset state"
    static void show(void* gameItself){
        FlipMaster* self = (FlipMaster*)gameItself;
        SDL_Log("%s show", self->gameName);
        Ux* myUxRef = Ux::Singleton();

        self->isComplete= false;
        self->solveAttempts = 0;
        self->matchedSwatches = 0;
        self->currentlyEvaluatingWin = false;

        self->flipA = nullptr;
        self->flipB = nullptr;

        self->flippedA = nullptr;
        self->flippedB = nullptr;

        self->scoreBreakdownHolder->hide();

        //    minigameColorList = new uiList<ColorList, Uint8>(minigameColorListMax)

        Ux::uiList<Ux::ColorList, Uint8>* myColorList;
        //Ux::uiList<Ux::ColorList, Uint8>* myDestList;

//        myColorList = myUxRef->minigameColorList->clone();
//        myColorList->sort(&Ux::randomSort);

        int totalTiles = SDL_min(myUxRef->minigameColorList->total(), self->maxSwatches * 0.5);

        myColorList = new Ux::uiList<Ux::ColorList, Uint8>(totalTiles * 2);
        for( int x=0; x<totalTiles; x++ ){ // clipped
            myColorList->add(*myUxRef->minigameColorList->get(x));
            myColorList->add(*myUxRef->minigameColorList->get(x)); // each color added twice, has a match

        }

        myColorList->sort(&Ux::randomSort);
        myColorList->sort(&Ux::randomSort);


        //colorPickState->viewport_ratio

        self->activeSwatches = myColorList->total();


        float columnsf = SDL_sqrtf((float)self->activeSwatches);
        int columns = columnsf;

        float width = 1.0 / (columns + 0.0f);
        float tileWidth = width;
        int row = -1;

        float rows = SDL_ceilf(self->activeSwatches / (columns + 0.0f));

        float height = 1.0 / rows;
        float tileHeight = height;
        //float halfTileHeight = height * 0.5;

        float vertPad = 0.05;
        float vertPadDist = vertPad / (rows - 1.0f);
        height -= vertPad;

        float hzPad = 0.2;
        float hzPadDist = hzPad / (columns - 1.0f);
        width -= hzPad;

        //(*self->pickList->get(0))->uiObjectItself->isDebugObject=true;

        for( int x=0; x<self->maxSwatches; x++ ){

            Ux::uiSwatch* move = *self->pickList->get(x);

            if( x < self->activeSwatches ){

                float col = (x % columns);

                if( col == 0 ){
                    row++;
                }

                float xPosition = (tileWidth * col) + (hzPadDist * (col));

                float yPosition = (tileHeight * row) + (vertPadDist * row);

                move->show();

                move->uiObjectItself->setBoundaryRect(xPosition, yPosition, width, height);

//                move->uiObjectItself->setBoundaryRect(0.0, y, 0.4, height);
//                dest->uiObjectItself->setBoundaryRect(0.6, y, 0.4, height);

                move->hideHex()->hideBg()->update(&myColorList->get(x)->color);

                //move->displayHex()->displayBg()->update(&myColorList->get(x)->color);


            }else{
                move->hide();
            }
        }


        myColorList->free();

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
        FlipMaster* self = (FlipMaster*)gameItself;
        SDL_Log("%s begin", self->gameName);
        self->startTime = SDL_GetTicks();
        Ux* myUxRef = Ux::Singleton();

    }

    static void resize(void* gameItself){
        FlipMaster* self = (FlipMaster*)gameItself;
        SDL_Log("%s resize", self->gameName);
        Ux* uxInstance = Ux::Singleton();
        if( uxInstance->widescreen ){
            self->scoreBreakdownSpacer->setBoundaryRect(0.25, 0.1, 0.5, 1.0-0.2);
        }else{
            self->scoreBreakdownSpacer->setBoundaryRect(0.05, 0.1, 1.0, 1.0-0.2);
        }
        self->minigames->resizeFromGame(self->gameRootUi);
    }

    static void update(void* gameItself){
        FlipMaster* self = (FlipMaster*)gameItself;
        //SDL_Log("%s update", self->gameName);
    }

    static void render(void* gameItself){
        FlipMaster* self = (FlipMaster*)gameItself;
        //SDL_Log("%s render", self->gameName);
    }

    static void end(void* gameItself){
        FlipMaster* self = (FlipMaster*)gameItself;
        SDL_Log("%s end", self->gameName);
        self->gameRootUi->hideAndNoInteraction();
    }

    static const char* getGameName(void* gameItself){
        FlipMaster* self = (FlipMaster*)gameItself;
        return self->gameName;
    }

    static int getTimeLimit(void* gameItself){
        FlipMaster* self = (FlipMaster*)gameItself;
        return self->timeLimit;
    }

//    static int getStartTime(void* gameItself){ // is this used?
//        FlipMaster* self = (FlipMaster*)gameItself;
//        return self->startTime;
//    }

//    static int getRemainingTime(void* gameItself){ // NOT IMPLEMENTED
//        FlipMaster* self = (FlipMaster*)gameItself;
//        return self->getTimeLimit(self) - self->getElapsedTime(self);
//    }

    static int getElapsedTime(void* gameItself){
        // maybe game can pause itself...
        FlipMaster* self = (FlipMaster*)gameItself;

        if( self->isComplete ){
            return self->lastTicks - self->startTime;
        }
        return SDL_GetTicks() - self->startTime; // generally we return teh current ticks, or when the game ended
    }

};


#endif
