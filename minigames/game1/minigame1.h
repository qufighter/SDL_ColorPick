//
//
#ifndef ColorPick_iOS_SDL_minigame1_h
#define ColorPick_iOS_SDL_minigame1_h

struct Minigame1{

    const char* gameName = "Test Game 1";
    long gameNameLen;
    Uint8 gameIndex; // we try to keep this matching the childList index of minigamesUiContainer ....
    int startTime;

    Ux::uiObject* gameRootUi;
    Ux::uiText* gameHeading;

    Ux::uiSwatch* mySwatch; // fixme... should be arrays

    Minigame1(Uint8 pGameIndex){
        gameIndex = pGameIndex;
        gameNameLen = SDL_strlen(gameName);
        Ux* myUxRef = Ux::Singleton();

//        if( myUxRef->minigamesUiContainer->childListIndex != gameIndex ){
//            SDL_Log("WARNING: creating minigames in wrong ordering, or unexpected games found"); // does it matter?  we can just track myUxRef->minigamesUiContainer->childListIndex iuf we need it, but since we have the ref to gameRootUi... it should not really be needed
//        }

        gameRootUi = new Ux::uiObject();
        gameRootUi->hideAndNoInteraction();
        myUxRef->minigamesUiContainer->addChild(gameRootUi);

        mySwatch = (new Ux::uiSwatch(gameRootUi, Float_Rect(0.25,0.25,0.5,0.5)))->displayHex();
        mySwatch->uiObjectItself->setInteraction(&interactionSwatchDragMove);
        mySwatch->uiObjectItself->setInteractionCallback(interactionSwatchDragMoveConstrain);

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

        // we plan to clear the animation first, then see if one is assigned by the following (make the creation of animation conditional on movement)
        // if no constrain animation is set, then we can perform our own animation.

        myUxRef->interactionDragMoveConstrain(interactionObj, delta);
    }

    // "reset state"
    static void show(void* gameItself){
        Minigame1* self = (Minigame1*)gameItself;
        SDL_Log("%s show", self->gameName);
        Ux* myUxRef = Ux::Singleton();


        // todo: set all boundaries and all colors
        // hide any extras....

        if( myUxRef->minigameColorList->total() > 0 ){
            self->mySwatch->update(&myUxRef->minigameColorList->get(0)->color);
        }



        self->gameRootUi->showAndAllowInteraction();
        self->startTime = SDL_GetTicks();
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
