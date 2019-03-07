//
//
#ifndef ColorPick_iOS_SDL_MixMaster_h
#define ColorPick_iOS_SDL_MixMaster_h

struct MixMaster{

    const char* gameName = "Mix Master";
    const int maxSwatches = 6;
    const int timeLimit = 60000; // one minute....
    const int scoreBreakdownLn = 10;

    Uint8 gameIndex; // we try to keep this matching the childList index of minigamesUiContainer ....
    int startTime;
    int lastTicks;
    int lastMoveMade;
    int lastMoveFinished;
    float tileHeight;
    float halfTileHeight;
    int activeSwatches;

    int totalMixes;
    int totalDestinations; //(totalMixes * 2)

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

    Ux::uiList<Ux::uiSwatch*, Uint8>* mixedList; // list of mixed colors (destinations for 2 colors)
    Ux::uiList<Ux::uiSwatchesGradient*, Uint8>* mixedShow; // when they pick 2 colors, show how they mix now


    Minigames* minigames;

    MixMaster(Uint8 pGameIndex){
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
        mixedList = new Ux::uiList<Ux::uiSwatch*, Uint8>(maxSwatches); //over allocated... meh..
        mixedShow = new Ux::uiList<Ux::uiSwatchesGradient*, Uint8>(maxSwatches); //over allocated... meh..


        gameSwatchesHolder->setBoundaryRect(0.1, 0.1, 1.0-0.2, 1.0-0.2); // margins all round


        gameSwatchesHolder->setModeWhereChildCanCollideAndOwnBoundsIgnored();

        int x;

        for( x=0; x<maxSwatches; x++ ){
            Ux::uiSwatch* tmp2 = new Ux::uiSwatch(gameSwatchesHolder, Float_Rect(0.25,0.25,0.5,0.5)); // ignore these rect....
            //tmp2->displayHex();
            //            tmp2->hideBg();

            tmp2->uiObjectItself->setInteractionCallback(&genericExplainColor);

            mixedList->add(tmp2);
        }

        for( x=0; x<maxSwatches; x++ ){
            Ux::uiSwatch* tmp2 = new Ux::uiSwatch(gameSwatchesHolder, Float_Rect(0.25,0.25,0.5,0.5)); // ignore these rect....
            //tmp2->displayHex(); // this is MEH methinks ??
            tmp2->hideBg();
            matchList->add(tmp2);
        }

        for( x=0; x<maxSwatches; x++ ){
            Ux::uiSwatch* tmp1 = new Ux::uiSwatch(gameSwatchesHolder, Float_Rect(0.25,0.25,0.5,0.5)); // ignore these rect....
            tmp1->uiObjectItself->setInteraction(&interactionSwatchDragMove);
            tmp1->uiObjectItself->setInteractionCallback(&interactionSwatchDragMoveConstrain);

            //tmp1.displayHex(); // testing only...
            pickList->add(tmp1);
        }

        for( x=0; x<maxSwatches; x++ ){

            Ux::uiSwatchesGradient* mixedGrad = (new Ux::uiSwatchesGradient(gameSwatchesHolder, Float_Rect(0.25,0.25,0.5,0.5)));// ignore these rect....
            mixedShow->add(mixedGrad);
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
        MixMaster* self = (MixMaster*)ogg->minigames->currentGame->gameItself; // helper?
        if( self->isComplete ){
            // lock it up..
            return;
        }

        //Ux::uiSwatch* swatch = (Ux::uiSwatch*)interactionObj->myUiController;
        myUxRef->interactionDragMove(interactionObj, delta);
    }

    static void genericExplainColor(Ux::uiObject *interactionObj, uiInteraction *delta){
        Ux* myUxRef = Ux::Singleton();
        Ux::uiSwatch* swatch = (Ux::uiSwatch*)interactionObj->myUiController;
        SDL_snprintf(myUxRef->print_here, 7,  "%02x%02x%02x", swatch->last_color.r, swatch->last_color.g, swatch->last_color.b);
        myUxRef->defaultScoreDisplay->displayExplanation(myUxRef->print_here);

    }

    static void interactionSwatchDragMoveConstrain(Ux::uiObject *interactionObj, uiInteraction *delta){
        Ux* myUxRef = Ux::Singleton();
        Ux::uiSwatch* swatch = (Ux::uiSwatch*)interactionObj->myUiController;

        OpenGLContext* ogg=OpenGLContext::Singleton();
        MixMaster* self = (MixMaster*)ogg->minigames->currentGame->gameItself; // helper?
        if( self->isComplete ){
            // lock it up..
            SDL_snprintf(myUxRef->print_here, 7,  "%02x%02x%02x", swatch->last_color.r, swatch->last_color.g, swatch->last_color.b);
            myUxRef->defaultScoreDisplay->displayExplanation(myUxRef->print_here);
            return;
        }
        self->lastMoveMade = SDL_GetTicks();

        myUxRef->interactionDragMoveConstrain(interactionObj, delta, &interactionSwatchDragMoveConstrainToParentObject);
    }

    static void interactionSwatchDragMoveConstrainToParentObject(Ux::uiAnimation* uiAnim){
        Ux* uxInstance = Ux::Singleton();
        Ux::uiObject* interactionObj = uiAnim->myUiObject;
        interactionObj->cancelCurrentAnimation();
        uxInstance->interactionConstrainToParentObject(uiAnim, &interactionSwatchDragMoveConstrainToParentObject); // this might apply a constrain to viewport animation, in which case we won't try to override it...

        if( !interactionObj->isAnimating() ){ // its in the viewport still... lets snap it to the nearest dest if close enough...

            OpenGLContext* ogg=OpenGLContext::Singleton();
            MixMaster* self = (MixMaster*)ogg->minigames->currentGame->gameItself; // helper?

            //self->checkIfGameIsCompleted(uiAnim); // mostly to reset
            self->isGameComplete();

            for( int x=0; x<self->activeSwatches; x++ ){
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

    static void checkIfGameIsCompleted(Ux::uiAnimation* uiAnim){
        OpenGLContext* ogg=OpenGLContext::Singleton();
        MixMaster* self = (MixMaster*)ogg->minigames->currentGame->gameItself; // helper?

        if( self->isGameComplete() ){
            //SDL_Log("Looks like you won!");
            //self->showMatches();
            self->showColors();

            if( self->isComplete ) return; // once only.... ???
            self->countSolveAttempts();
            self->isComplete = true; // complete, and won, lock it up!
            self->lastTicks = SDL_GetTicks();

            ogg->minigames->gameIsCompleted(); // save CPUs.

            // at this piont, we can allow the game to end and lock it up somehow.... ?
            // and we show the score....
            self->computeGameScore();


        }else{
            if( self->isReadyToScore ){
                //SDL_Log("Looks like LOSS");
                self->countSolveAttempts();
                self->showMatches();
            }else{
                //SDL_Log("Looks like INCOMPLETE");
            }
        }
    }

    void countSolveAttempts(){
        if( lastMoveFinished != lastMoveMade ){ // a given move is not more than one solve event
            lastMoveFinished=lastMoveMade;
            solveAttempts += 1;
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

            if(finalScore > 0){
                uxInstance->defaultScoreDisplay->display(gameSwatchesHolder->childList[0], finalScore, SCORE_EFFECTS::MOVE_UP);
            }
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

            //SDL_Log("Minigame: just applied final score.... %i", finalScore);

            return finalScore;
        }else{
            return 0; // impossible to reach, right?
        }
    }

    void showColors(){ // when we win!  shows all move colors
        for( int y=0; y<activeSwatches; y++ ){
            Ux::uiSwatch* move = *pickList->get(y);

            // only "used" moves
            if( move->uiObjectItself->boundryRect.x > 0.5 ){
                move->displayHex();
                move->refresh();
            }
        }

        for( int x=0; x<totalMixes; x++ ){
            Ux::uiSwatchesGradient* mixGrad = *mixedShow->get(x);
            mixGrad->show();
        }
    }

    void showMatches(){
        Ux* uxInstance = Ux::Singleton();
        OpenGLContext* ogg=OpenGLContext::Singleton();
        MixMaster* self = (MixMaster*)ogg->minigames->currentGame->gameItself; // helper?

        // we verify before this is called, every dest has 1 and only 1 match....

        int destCtr = 0;
        for( int x=0; x<self->totalMixes; x++ ){

            Ux::uiSwatch* mixSwatch = *self->mixedList->get(x);
            Ux::uiSwatchesGradient* mixGrad = *self->mixedShow->get(x);
            Ux::uiSwatch* dest1 = *self->matchList->get(destCtr);
            Ux::uiSwatch* dest2 = *self->matchList->get(destCtr+1);
            destCtr+=2;

            Ux::uiSwatch* dest1move = nullptr;
            Ux::uiSwatch* dest2move = nullptr;

            // this is a duplicate search (see also "this is reallly a dupe search... oh well??")
            // instead, each swatch ( in this case mixSwatch ) can house several transient references we'll have populated..... ref to other swatches... we'll know which ones to look at...
            for( int y=0; y<self->activeSwatches; y++ ){
                // see if any moves are placed here...
                Ux::uiSwatch* move = *self->pickList->get(y);
                if( dest1->uiObjectItself->boundryRect.x == move->uiObjectItself->boundryRect.x && dest1->uiObjectItself->boundryRect.y == move->uiObjectItself->boundryRect.y ){
                    dest1move = move;
                }

                if( dest2->uiObjectItself->boundryRect.x == move->uiObjectItself->boundryRect.x && dest2->uiObjectItself->boundryRect.y == move->uiObjectItself->boundryRect.y ){
                    dest2move = move;
                }
            }

            // so we know for sure we have both right?  just chekcing...
            if( dest1move != nullptr  && dest2move != nullptr ){

                SDL_Color mixed = Ux::mixColors( &dest1move->last_color, &dest2move->last_color );

                //showMix(mixGrad, &dest1move->last_color, &dest2move->last_color, &mixed);
                mixGrad->show();

                if( !Ux::colorEquals(&mixed, &mixSwatch->last_color ) ){
                    SDL_snprintf(uxInstance->print_here, 7,  "   "); // once at boot....
//                    dest1move->hexDisplay->color(mixed.r, mixed.g, mixed.b, mixed.a); // lets show what color it mixes to somewhares...
//                    dest2move->hexDisplay->color(mixed.r, mixed.g, mixed.b, mixed.a); // lets show what color it mixes to somewhares...
                    // mayhaps this is swatch border color ??? could help.. .but we'll need to make sure this gets reset tho.... ( add to swatch ->update() to auto fix it ?? )
                    dest1move->print(uxInstance->print_here);
                    dest2move->print(uxInstance->print_here);
                    uxInstance->printCharToUiObject(dest1move->hexDisplay->getTextChar(1), CHAR_CANCEL_ICON, DO_NOT_RESIZE_NOW);
                    uxInstance->printCharToUiObject(dest2move->hexDisplay->getTextChar(1), CHAR_CANCEL_ICON, DO_NOT_RESIZE_NOW);
                }else{
                    SDL_snprintf(uxInstance->print_here, 7,  "   OK"); // once at boot....
                    dest1move->print(uxInstance->print_here);
                    dest2move->print(uxInstance->print_here);
                    uxInstance->printCharToUiObject(dest1move->hexDisplay->getTextChar(1), CHAR_CHECKMARK_ICON, DO_NOT_RESIZE_NOW);
                    uxInstance->printCharToUiObject(dest2move->hexDisplay->getTextChar(1), CHAR_CHECKMARK_ICON, DO_NOT_RESIZE_NOW);
                }

            }


        }

    }

    void updateMix(Ux::uiSwatchesGradient* mixGrad, SDL_Color* destClr1, SDL_Color* destClr2, SDL_Color* mixed){
        // TODO: perhaps the swatch can have a uiSwatchesGradient for better alignment??
        mixGrad->update(destClr1,destClr2,mixed)->hide();
//        mixGrad->clearStops()->hide()
////            ->addStop(0.0, destClr1->r, destClr1->g, destClr1->b, 0)
////            ->addStop(0.1, destClr1)
//            ->addStop(0.2, destClr1)
//            ->addStop(0.4, mixed)
//            ->addStop(0.6, mixed)
//            ->addStop(0.8, destClr2)
////            ->addStop(0.9, destClr2)
////            ->addStop(1.0, destClr2->r, destClr2->g, destClr2->b, 0)
//            ->update();
//            //->show();
    }

    bool isGameComplete(){
        Ux* uxInstance = Ux::Singleton();
        OpenGLContext* ogg=OpenGLContext::Singleton();
        MixMaster* self = (MixMaster*)ogg->minigames->currentGame->gameItself; // helper?

        bool isWin = true; // lets see if any of them are non
        bool isReadyToScore = true;


        int destCtr = 0;
        for( int x=0; x<self->totalMixes; x++ ){

            Ux::uiSwatch* mixSwatch = *self->mixedList->get(x);
            Ux::uiSwatchesGradient* mixGrad = *self->mixedShow->get(x);
            Ux::uiSwatch* dest1 = *self->matchList->get(destCtr);
            Ux::uiSwatch* dest2 = *self->matchList->get(destCtr+1);
            destCtr+=2;
//
//            SDL_Color mixed = Ux::mixColors( dest1->, &myDestList->get(mixIndex+1)->color );
//            mixSwatch->update( &mixed );


            Ux::uiSwatch* dest1move = nullptr;
            Ux::uiSwatch* dest2move = nullptr;

            for( int y=0; y<self->activeSwatches; y++ ){
                // see if any moves are placed here...
                Ux::uiSwatch* move = *self->pickList->get(y);

                move->print(""); // we clear too many times, maybe this should be the outer loop?

                if( dest1->uiObjectItself->boundryRect.x == move->uiObjectItself->boundryRect.x && dest1->uiObjectItself->boundryRect.y == move->uiObjectItself->boundryRect.y ){
                    if( dest1move != nullptr ){
                        // more than one color is here... lets slide it out... win is impossible now...
                        isWin = false;
                        move->uiObjectItself->setAnimation( uxInstance->uxAnimations->moveTo(move->uiObjectItself,move->uiObjectItself->origBoundryRect.x,move->uiObjectItself->origBoundryRect.y, nullptr, nullptr) );
                    }
                    dest1move = move;
                }

                if( dest2->uiObjectItself->boundryRect.x == move->uiObjectItself->boundryRect.x && dest2->uiObjectItself->boundryRect.y == move->uiObjectItself->boundryRect.y ){
                    if( dest2move != nullptr ){
                        // more than one color is here... lets slide it out... win is impossible now...
                        isWin = false;
                        move->uiObjectItself->setAnimation( uxInstance->uxAnimations->moveTo(move->uiObjectItself,move->uiObjectItself->origBoundryRect.x,move->uiObjectItself->origBoundryRect.y, nullptr, nullptr) );
                    }
                    dest2move = move;
                }
            }

            if( dest1move != nullptr  && dest2move != nullptr ){

                //if(!isWin) continue; // arguably if we are going to save another loop later, we'd instead keep going here.....

                SDL_Color mixed = Ux::mixColors( &dest1move->last_color, &dest2move->last_color );

                updateMix(mixGrad, &dest1move->last_color, &dest2move->last_color, &mixed); // we want to keep these hidden... until we know is ready to score.. then show them all...

                if( !Ux::colorEquals(&mixed, &mixSwatch->last_color ) ){
                    isWin = false;
                }

            }else{
                mixGrad->hide(); // it hides in both if and else, but it updates above too
                isReadyToScore = false;
                isWin = false;
            }

//            if( !hasMatch ){
//                isWin = false;
//                isReadyToScore = false;
//            }

        }

        self->isReadyToScore = isReadyToScore;
        return isWin;
    }

    // "reset state"
    static void show(void* gameItself){
        MixMaster* self = (MixMaster*)gameItself;
        SDL_Log("%s show", self->gameName);
        Ux* myUxRef = Ux::Singleton();

        self->isComplete= false;
        self->isReadyToScore = false;
        self->solveAttempts = 0;
        self->lastMoveMade = 0;
        self->lastMoveFinished = 0;

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
        myColorList->free();

        myColorList = myDestList->clone();
        myColorList->sort(&Ux::randomSort);
        myColorList->sort(&Ux::randomSort);

        float height = 1.0 / (totalTiles + 0.0f);
        self->tileHeight = height;
        self->halfTileHeight = height * 0.5;

        float vertPad = 0.01;
        float vertPadDist = vertPad / (totalTiles - 1.0f);
        height -= vertPad;

        //colorPickState->viewport_ratio

        self->activeSwatches = myColorList->total();

        self->totalMixes = self->activeSwatches * 0.5;
        self->totalDestinations = self->totalMixes * 2;

        int mixIndex=0;

        for( int x=0; x<self->maxSwatches; x++ ){

            Ux::uiSwatch* move = *self->pickList->get(x);
            Ux::uiSwatch* dest = *self->matchList->get(x);
            Ux::uiSwatch* mixSwatch = *self->mixedList->get(x);
            Ux::uiSwatchesGradient* mixGrad = *self->mixedShow->get(x);

            mixGrad->hide();

            if( x < self->activeSwatches ){

                float y = (self->tileHeight * x) + (vertPadDist * x);

                move->show();
                dest->show();

                move->uiObjectItself->setBoundaryRect(0.0, y, 0.4, height);
                dest->uiObjectItself->setBoundaryRect(0.6, y, 0.4, height);

                move->hideHex()->update(&myColorList->get(x)->color);
                dest->update(&myDestList->get(x)->color);

                if( x >= self->totalDestinations  ){
                    dest->hide();
                }

                if( x < self->totalMixes ){

                    mixSwatch->show();

                    y = (self->tileHeight * x * 2) + (vertPadDist * x * 2) + (vertPadDist * 2);

                    mixSwatch->uiObjectItself->setBoundaryRect(0.5/*0.6-0.1*/, y, 0.4, height * 2);

                    //mixGrad->uiObjectItself->setBoundaryRect(0.9/*0.6-0.1*/, y + self->halfTileHeight, 0.1, height);
                    mixGrad->uiObjectItself->setBoundaryRect(0.5/*0.6-0.1*/, y + self->halfTileHeight, 0.1, height);

                    SDL_Color mixed = Ux::mixColors( &myDestList->get(mixIndex)->color, &myDestList->get(mixIndex+1)->color );
                    mixSwatch->update( &mixed );

                    mixIndex+=2;


                }else{
                    mixSwatch->hide();
                }

            }else{
                move->hide();
                dest->hide();
                mixSwatch->hide();
            }
        }

        myColorList->free();
        myDestList->free();

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
        MixMaster* self = (MixMaster*)gameItself;
        SDL_Log("%s begin", self->gameName);
        self->startTime = SDL_GetTicks();
        Ux* myUxRef = Ux::Singleton();

    }

    static void resize(void* gameItself){
        MixMaster* self = (MixMaster*)gameItself;
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
        MixMaster* self = (MixMaster*)gameItself;
        //SDL_Log("%s update", self->gameName);
    }

    static void render(void* gameItself){
        MixMaster* self = (MixMaster*)gameItself;
        //SDL_Log("%s render", self->gameName);
    }

    static void end(void* gameItself){
        MixMaster* self = (MixMaster*)gameItself;
        SDL_Log("%s end", self->gameName);
        self->gameRootUi->hideAndNoInteraction();
    }

    static const char* getGameName(void* gameItself){
        MixMaster* self = (MixMaster*)gameItself;
        return self->gameName;
    }

    static int getTimeLimit(void* gameItself){
        MixMaster* self = (MixMaster*)gameItself;
        return self->timeLimit;
    }

    //    static int getStartTime(void* gameItself){ // is this used?
    //        MixMaster* self = (MixMaster*)gameItself;
    //        return self->startTime;
    //    }

    //    static int getRemainingTime(void* gameItself){ // NOT IMPLEMENTED
    //        MixMaster* self = (MixMaster*)gameItself;
    //        return self->getTimeLimit(self) - self->getElapsedTime(self);
    //    }

    static int getElapsedTime(void* gameItself){
        // maybe game can pause itself...
        MixMaster* self = (MixMaster*)gameItself;

        if( self->isComplete ){
            return self->lastTicks - self->startTime;
        }
        return SDL_GetTicks() - self->startTime; // generally we return teh current ticks, or when the game ended
    }

};


#endif
