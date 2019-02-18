//
//
#ifndef ColorPick_iOS_SDL_minigame1_h
#define ColorPick_iOS_SDL_minigame1_h

struct Minigame1{

    const char* gameName = "Test Game 1";
    const int maxSwatches = 9;
    long gameNameLen;
    Uint8 gameIndex; // we try to keep this matching the childList index of minigamesUiContainer ....
    int startTime;

    Ux::uiObject* gameRootUi;
    Ux::uiObject* gameSwatchesHolder;
    Ux::uiText* gameHeading;

    Ux::uiList<Ux::uiSwatch, Uint8>* pickList; // this is list of movable colors
    Ux::uiList<Ux::uiSwatch, Uint8>* matchList; // list of match destinations

    Minigames* minigames; // handy ref?  cannot populate in constructor (yet??)

    Minigame1(Uint8 pGameIndex){
        gameIndex = pGameIndex;
        gameNameLen = SDL_strlen(gameName);
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

        pickList = new Ux::uiList<Ux::uiSwatch, Uint8>(maxSwatches);
        matchList = new Ux::uiList<Ux::uiSwatch, Uint8>(maxSwatches);

        int x;

        for( x=0; x<maxSwatches; x++ ){
            Ux::uiSwatch tmp2 = Ux::uiSwatch(gameSwatchesHolder, Float_Rect(0.25,0.25,0.5,0.5));
            tmp2.displayHex();
            tmp2.hideBg();
            matchList->add(tmp2);
        }

        for( x=0; x<maxSwatches; x++ ){
            Ux::uiSwatch tmp1 = Ux::uiSwatch(gameSwatchesHolder, Float_Rect(0.25,0.25,0.5,0.5));
            tmp1.uiObjectItself->setInteraction(&interactionSwatchDragMove);
            tmp1.uiObjectItself->setInteractionCallback(interactionSwatchDragMoveConstrain);
            //tmp1.displayHex(); // testing only...
            pickList->add(tmp1);
        }

        //last for on top
        gameHeading = (new Ux::uiText(gameRootUi, 1.0/gameNameLen))->pad(0.0,0.0)->margins(0.25,0.0,0.0,0.0)->print(gameName);
    }

    static void interactionSwatchDragMove(Ux::uiObject *interactionObj, uiInteraction *delta){
        Ux::uiSwatch* swatch = (Ux::uiSwatch*)interactionObj->myUiController;
        Ux* myUxRef = Ux::Singleton();
        myUxRef->interactionDragMove(interactionObj, delta);
    }

    static void interactionSwatchDragMoveConstrain(Ux::uiObject *interactionObj, uiInteraction *delta){
        Ux::uiSwatch* swatch = (Ux::uiSwatch*)interactionObj->myUiController;
        Ux* myUxRef = Ux::Singleton();
        myUxRef->interactionDragMoveConstrain(interactionObj, delta, &interactionSwatchDragMoveConstrainToParentObject);
    }

    static void interactionSwatchDragMoveConstrainToParentObject(Ux::uiAnimation* uiAnim){
        Ux* uxInstance = Ux::Singleton();
        Ux::uiObject* interactionObj = uiAnim->myUiObject;
        interactionObj->cancelCurrentAnimation();
        uxInstance->interactionConstrainToParentObject(uiAnim, &interactionSwatchDragMoveConstrainToParentObject); // this might apply a constrain to viewport animation, in which case we won't try to override it...

        if( !interactionObj->isAnimating() ){ // its in the viewport still... lets snap it to the nearest dest if close enough...

            OpenGLContext* ogg=OpenGLContext::Singleton();
            Minigame1* self = (Minigame1*)ogg->minigames->currentGame->gameItself; // helper?

            for( int x=0; x<self->maxSwatches; x++ ){
                if( x < uxInstance->minigameColorList->total() ){
                    Ux::uiSwatch* dest = self->matchList->get(x);
                    float dist = glm::distance(
                       glm::vec2(interactionObj->boundryRect.x, interactionObj->boundryRect.y),
                       glm::vec2(dest->uiObjectItself->boundryRect.x, dest->uiObjectItself->boundryRect.y)
                    );
                    if( dist < 0.16 ){
                        //SDL_Log("Distance from this one is %f", dist);
                        interactionObj->setAnimation( uxInstance->uxAnimations->moveTo(interactionObj,dest->uiObjectItself->boundryRect.x,dest->uiObjectItself->boundryRect.y, nullptr, nullptr) );
                        // NOTE: when the above animation completes, this swatch is "locked" until we move it again...
                        // once all swatches are locked.... then the game is ready to complete...
                        break;
                    }
                }
            }
        }
    }

    // "reset state"
    static void show(void* gameItself){
        Minigame1* self = (Minigame1*)gameItself;
        SDL_Log("%s show", self->gameName);
        Ux* myUxRef = Ux::Singleton();

        float height = 1.0 / myUxRef->minigameColorList->total();

        for( int x=0; x<self->maxSwatches; x++ ){

            Ux::uiSwatch* move = self->pickList->get(x);
            Ux::uiSwatch* dest = self->matchList->get(x);

            if( x < myUxRef->minigameColorList->total() ){

                move->uiObjectItself->setBoundaryRect(0.0, height * x, height, height);
                dest->uiObjectItself->setBoundaryRect(0.5, height * x, height, height);

                move->update(&myUxRef->minigameColorList->get(x)->color); // TODO: we need to get a random color - so we need a way to have each int map to a different int without repeats...
                dest->update(&myUxRef->minigameColorList->get(x)->color);

            }else{
                move->hide();
                dest->hide();
            }
        }

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
        Minigame1* self = (Minigame1*)gameItself;
        SDL_Log("%s begin", self->gameName);
        self->startTime = SDL_GetTicks();
        Ux* myUxRef = Ux::Singleton();
        // move game title down....  we could do something else maybe...         // see interactionDragMoveConstrain for custom chain...
        self->gameHeading->uiObjectItself->setAnimation(
            myUxRef->uxAnimations->moveTo(self->gameHeading->uiObjectItself,self->gameHeading->uiObjectItself->boundryRect.x, 0.56 , nullptr, nullptr) );
    }

    static void resize(void* gameItself){
        Minigame1* self = (Minigame1*)gameItself;
        SDL_Log("%s resize", self->gameName);
        // check ux widescreen?
        self->gameRootUi->updateRenderPosition();
    }

    static void update(void* gameItself){
        Minigame1* self = (Minigame1*)gameItself;
        //SDL_Log("%s update", self->gameName);
    }

    static void render(void* gameItself){
        Minigame1* self = (Minigame1*)gameItself;
        //SDL_Log("%s render", self->gameName);
    }

    static void end(void* gameItself){
        Minigame1* self = (Minigame1*)gameItself;
        SDL_Log("%s end", self->gameName);
        self->gameRootUi->hideAndNoInteraction();
    }


};


#endif
