
#ifndef ColorPick_iOS_SDL_uiToolMenu_h
#define ColorPick_iOS_SDL_uiToolMenu_h



struct uiToolMenu{

    // origionally uiScore.h
    uiToolMenu(uiObject* parentObj){

        longestMenuItemLen=0;
        lastMenuItemIndex=-1;

        Ux* uxInstance = Ux::Singleton(); // some useful helper?

        uiObjectItself = new uiObject();
        uiObjectItself->myUiController = this; // this propagates to the other child objects

        menu_items = new uiObject();
        menu_position = new uiObject();


        uiObjectItself->hasBackground=true;
        Ux::setColor(&uiObjectItself->backgroundColor, 0, 0, 0, 128); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)

        uiObjectItself->setInteractionCallback(clickShieldFunction);
        uiObjectItself->hideAndNoInteraction();

        menu_position->testChildCollisionIgnoreBounds = true;
        menu_items->testChildCollisionIgnoreBounds = true;

        menu_items->squarifyKeepHz();

        //maths
        float w = 0.125;
        //float hw = w * 0.5;

        //no->setBoundaryRect( 0.0+pad, 0.5-hh, w, h);

        menu_position->setBoundaryRect( 0.5, 0.5, w, w);

        menu_items->setBoundaryRect( -0.5, -0.5, 1.0, 1.0);

        menu_position->addChild(menu_items);
        uiObjectItself->addChild(menu_position);

        parentObj->addChild(uiObjectItself);

        resize();

        hide();

//        bool widescreen = uxInstance->widescreen;
//        float bounceIntensity = -0.001;

        // just so we can call end on these??? this could crash the app at boot time right??
        chain1 = uxInstance->uxAnimations->emptyChain(); // uxInstance->uxAnimations->reset_matrix(menu_position)->preserveReference();
        chain2 = uxInstance->uxAnimations->emptyChain(); //uxInstance->uxAnimations->softBounce(menu_position, widescreen?0:bounceIntensity, widescreen?bounceIntensity:0)->preserveReference();

//        chain1 = uxInstance->uxAnimations->reset_matrix(menu_position)->preserveReference();
//        chain2 = uxInstance->uxAnimations->softBounce(menu_position, widescreen?0:bounceIntensity, widescreen?bounceIntensity:0)->preserveReference();
    }

    //anInteractionFn tileClicked=nullptr;

    uiObject* uiObjectItself; // AKA shield // no real inheritance here, this its the uiSqware, I would use self->
    uiObject *menu_position;
    uiObject *menu_items;

    int longestMenuItemLen;
    int lastMenuItemIndex;

    uiAminChain* chain1;
    uiAminChain* chain2;

    static void clickShieldFunction(uiObject *interactionObj, uiInteraction *delta){
        uiToolMenu* self = ((uiToolMenu*)interactionObj->myUiController);
        self->hide();
    }


    static void touchMenuChoice(uiObject *interactionObj, uiInteraction *delta){
        Ux* uxInstance = Ux::Singleton();
        uxInstance->uxAnimations->scale_bounce(interactionObj, 0.006);
    }

    void clearMenuItems(){
        uiObject* tempMenuItem;
        for( int x=0,l=menu_items->childListIndex; x<l; x++ ){
            tempMenuItem = menu_items->childList[x];
            tempMenuItem->hideAndNoInteraction();
        }
        longestMenuItemLen = 0;
        lastMenuItemIndex = -1;
    }

    uiObject* getOrCreateMenuItem(int index){
        if( index < menu_items->childListIndex ){
            return menu_items->childList[index];
        }else{


            uiText* text = new uiText(menu_items, 1.0);


            text->backgroundClickCallback(&touchMenuChoice)
                ->backgroundColor(255, 255, 255, 192)
                ->color(64, 64, 64, 255)
                ->scale(1.21);

            text->uiObjectItself->setBoundaryRect(0,index * 1.25, 1,1 );

//            uiObject* new_menu_item = new uiObject();
//            uiObject* new_menu_item_bg = new uiObject();
//            uiObject* new_menu_item_txt = new uiObject();

            //new_menu_item->setBoundaryRect(0,index * 1.25, 1,1 );

//            new_menu_item_bg->hasBackground=true;
//            Ux::setColor(&new_menu_item_bg->backgroundColor, 255, 255, 255, 192);
//            new_menu_item_txt->hasForeground=true;
//            Ux::setColor(&new_menu_item_txt->foregroundColor, 64, 64, 64, 255);

            //new_menu_item->testChildCollisionIgnoreBounds = true;
            text->uiObjectItself->testChildCollisionIgnoreBounds = true;

//            new_menu_item->addChild(new_menu_item_bg);
//            new_menu_item->addChild(new_menu_item_txt);
            //menu_items->addChild(new_menu_item);

            //new_menu_item->matrixInheritedByDescendants=true;
            //new_menu_item_bg->setInteractionBegin(&touchMenuChoice);

//            return new_menu_item;
            return text->uiObjectItself;
        }
    }

    void addMenuItem(uiObject *p_cbUiObject, const char* menuText, anInteractionFn p_interactionCallback){
        Ux* uxInstance = Ux::Singleton();

        lastMenuItemIndex += 1;

        // WE ARE USING menu_item as a TEMP reference here.....
        uiObject* menu_item = getOrCreateMenuItem(lastMenuItemIndex);
//        uiObject* menu_item_bg = menu_item->childList[0];
//        uiObject* menu_item_txt = menu_item->childList[1];


        uiText* text = (uiText*)menu_item->myUiController;


        menu_item->showAndAllowInteraction();


        text->print(menuText);
        //uxInstance->printStringToUiObject(menu_item_txt, menuText, DO_NOT_RESIZE_NOW);
        int menuItemLen = text->length();// SDL_strlen(menuText);
        
        text->backgroundClickCallback(p_interactionCallback)->backgroundClickProxy(p_cbUiObject);

//        menu_item_bg->boundryRect.w = menuItemLen * 1.0;
//        menu_item_bg->setClickInteractionCallback(p_interactionCallback);
//        menu_item_bg->doesNotCollide = false;
//        menu_item_bg->interactionProxy = p_cbUiObject;

        if( menuItemLen > longestMenuItemLen ){
            longestMenuItemLen=menuItemLen;
        }

    }

    int count(){
        return lastMenuItemIndex+1;
    }

    int lastIndex(){
        return lastMenuItemIndex;
    }

    uiObject* lastUiObject(){
        return getOrCreateMenuItem(lastMenuItemIndex);
    }


    void display(uiObject *p_dispalyNearUiObject){


        //SDL_Log("this is the biggest int %i " , SDL_MAX_SINT32); // 2147483647  ( 10 char, sign, extra)
        Ux* uxInstance = Ux::Singleton();
        //bool widescreen = uxInstance->widescreen;
        float bounceIntensity = -0.0005;


        uiObjectItself->show();
        //menu_position->matrix = glm::mat4(1.0);

        // we had better create a new animation for EACH one we free here...
        chain1->endAnimation();
        chain1 = uxInstance->uxAnimations->reset_matrix(menu_position)->preserveReference();

        handlePositioning(p_dispalyNearUiObject);
        chain2->endAnimation();

      //  chain2 = uxInstance->uxAnimations->spin(menu_position)->preserveReference();
      //  chain2 = uxInstance->uxAnimations->slideUpFullHeight(menu_position)->preserveReference();
        chain2 = uxInstance->uxAnimations->softBounce(menu_position, 0, -bounceIntensity)->preserveReference();

        uiObjectItself->showAndAllowInteraction();

        menu_position->updateRenderPosition();

        //displayExplanation("-Yes it work-");
        //displayExplanation("-Yes-");
        //myUxRef->defaultScoreDisplay->displayExplanation("-Yes-");
    }

    void handlePositioning(uiObject *p_dispalyNearUiObject){
        Ux* uxInstance = Ux::Singleton();
        // we need to handle positioning of the main "menu_position" element before animation starts, but after we know the text...
        float text_length = longestMenuItemLen;

        float menu_item_size_scaling = 1.0/text_length;
        // what this does is effectively keep things from ever getting larger than the smallest size ever...
//        if( menu_item_size_scaling > menu_position->origBoundryRect.w ){ // enforce "max" (default) size:
//            menu_item_size_scaling=menu_position->origBoundryRect.w;
//        }

        menu_items->boundryRect.x =  text_length * -0.5; // center
        //        menu_items->boundryRect.x =  -text_length + 0.5; // right aligned (center the rightmost char)
        //menu_items->boundryRect.x = -0.5;

        Float_Rect* dispRect = &p_dispalyNearUiObject->collisionRect; // this rect has good w/h that we can use (its scaled to boundary space)

        float xPosition = dispRect->x + (dispRect->w * 0.5);
        float yPosition = dispRect->y;


        if( uxInstance->widescreen ){

            yPosition +=  (dispRect->h * 0.5); // + (-menu_items->childListIndex * (menu_item_size_scaling * menu_items->boundryRect.h) * 0.5);

            //xPosition += (0.5*dispRect->w) + (0.5*(1.0-dispRect->w));

            //menu_item_size_scaling *= 1.0-dispRect->w;
            menu_item_size_scaling *=0.5;

        }else{
            yPosition += dispRect->h + 0.05;
        };

        menu_position->setBoundaryRect(xPosition, yPosition, menu_item_size_scaling, menu_item_size_scaling );

        // if we go off the left edgeo f teh screen, lets try to stay on it...
        float hidLeftAmt = menu_position->boundryRect.x + (menu_item_size_scaling * menu_items->boundryRect.x);
        if( hidLeftAmt < 0 ){
            xPosition -= hidLeftAmt; // minus a negative
        }

        menu_position->setBoundaryRect(xPosition, yPosition, menu_item_size_scaling, menu_item_size_scaling );

        //if( menu_item->boundryRect.x + -0.5 + menu_position->boundryRect.x < 0 ) menu_position->boundryRect.x += menu_item->boundryRect.x + -0.5 + menu_position->boundryRect.x; // keep "on screen"

    }

    void hide(){
        uiObjectItself->hideAndNoInteraction();
    }

    void resize(){
        update();
    }

    void update(){
        uiObjectItself->updateRenderPosition();
    }

};


#endif
