
#ifndef ColorPick_iOS_SDL_uiScore_h
#define ColorPick_iOS_SDL_uiScore_h



struct uiScore{


    uiScore(uiObject* parentObj){

        int_score = 0;
        int_max_score = 0;
        maxLen = 14;
        score_disp_char = (char*)SDL_malloc( sizeof(char) * maxLen );

        //int_score = SDL_MAX_SINT32 - 3; // TEST ONLY!  - simulate overflow condition.....

        Ux* uxInstance = Ux::Singleton(); // some useful helper?

        uiObjectItself = new uiObject();

        score = new uiObject();
        score_position = new uiObject();

        uiObjectItself->myUiController = this; // this propagates to the other child objects


        //uiObjectItself->setInteractionCallback(tileClickedFn);


        //deleteColorPreview = new uiViewColor(uiObjectItself, Float_Rect(0.0, 0.0, 1.0, 0.27777777777778), false);



//        score->hasBackground=true;
//        Ux::setColor(&score->backgroundColor, 32, 0, 0, 128);
        score->hasForeground=true;
        Ux::setColor(&score->foregroundColor, 255, 255, 255, 255); // control texture color/opacity, multiplied (Default 255, 255, 255, 255)
        //uxInstance->printCharToUiObject(score, CHAR_CHECKMARK_ICON, DO_NOT_RESIZE_NOW);
        //uxInstance->printStringToUiObject(score, "xAll", DO_NOT_RESIZE_NOW);

        //score->squarify();
        score->squarifyKeepHz();



        //maths
        float w = 0.125;
        float hw = w * 0.5;

        //no->setBoundaryRect( 0.0+pad, 0.5-hh, w, h);

        score_position->setBoundaryRect( 0.5, 0.5, w, w);

        score->setBoundaryRect( -0.5, -0.5, 1.0, 1.0); // on right



        score_position->addChild(score);
        uiObjectItself->addChild(score_position);

        parentObj->addChild(uiObjectItself);

        resize();

        hide();

        bool widescreen = uxInstance->widescreen;
        float bounceIntensity = -0.001;

        // just so we can call end on these??? this could crash the app at boot time right??
//        chain1 = uxInstance->uxAnimations->emptyChain(); // uxInstance->uxAnimations->reset_matrix(score_position)->preserveReference();
//        chain2 = uxInstance->uxAnimations->emptyChain(); //uxInstance->uxAnimations->softBounce(score_position, widescreen?0:bounceIntensity, widescreen?bounceIntensity:0)->preserveReference();
//        chain3 = uxInstance->uxAnimations->emptyChain(); //uxInstance->uxAnimations->scale_bounce(score_position, 0.007, uxInstance->uxAnimations->mat_zeroscale)->preserveReference();

        chain1 = uxInstance->uxAnimations->reset_matrix(score_position)->preserveReference();
        chain2 = uxInstance->uxAnimations->softBounce(score_position, widescreen?0:bounceIntensity, widescreen?bounceIntensity:0)->preserveReference();
        chain3 = uxInstance->uxAnimations->scale_bounce(score_position, 0.007, uxInstance->uxAnimations->mat_zeroscale)->preserveReference();


    }

    //anInteractionFn tileClicked=nullptr;

    uiObject* uiObjectItself; // no real inheritance here, this its the uiSqware, I would use self->
    uiObject *score_position;
    uiObject *score;

    char* score_disp_char;
    Sint32 int_score;
    Sint32 int_max_score;
    int maxLen;

    uiAminChain* chain1;
    uiAminChain* chain2;
    uiAminChain* chain3;

    void loose(uiObject *p_dispalyNearUiObject){
        loose(p_dispalyNearUiObject, SCORE_EFFECTS::DEFAULT);
    }

    void loose(uiObject *p_dispalyNearUiObject, int effectNum){
        display(p_dispalyNearUiObject, -int_score, effectNum);
        int_score = 0; // this is sort of redundant probably....
    }

    void display(uiObject *p_dispalyNearUiObject, int numberToDisplay){
        display(p_dispalyNearUiObject, numberToDisplay, SCORE_EFFECTS::DEFAULT);
    }

    void display(uiObject *p_dispalyNearUiObject, int numberToDisplay, int effectNum){

        //SDL_Log("this is the biggest int %i " , SDL_MAX_SINT32); // 2147483647  ( 10 char, sign, extra)
        Ux* uxInstance = Ux::Singleton();
        bool widescreen = uxInstance->widescreen;
        float bounceIntensity = -0.001;


        uiObjectItself->show();
        //score_position->matrix = glm::mat4(1.0);

        // we had better create a new animation for EACH one we free here...
        chain1->endAnimation();
        chain1 = uxInstance->uxAnimations->reset_matrix(score_position)->preserveReference();

        if( int_score + numberToDisplay < SDL_MAX_SINT32 ){

            if( int_score + numberToDisplay < 0 ){
                // this is pretty special... means we overflowed Sint32...
                // we could modulate color/ animation/ etc...
            }

            Float_Rect* dispRect = &p_dispalyNearUiObject->collisionRect; // this rect has good w/h that we can use (its scaled to boundary space)
            score_position->setBoundaryRect(dispRect->x + (dispRect->w * 0.5),
                                            dispRect->y + (dispRect->h * 0.5),
                                            score_position->origBoundryRect.w,
                                            score_position->origBoundryRect.h );
            //score_position->updateRenderPosition();

            if( numberToDisplay > 0 ){
                SDL_snprintf(score_disp_char, maxLen, "+%i", numberToDisplay);
                chain2->endAnimation();
                chain3->endAnimation();
                switch( effectNum ){
                    case SCORE_EFFECTS::NOMOVE:
                        chain2 = uxInstance->uxAnimations->spin(score_position)->preserveReference();
                        chain3 = uxInstance->uxAnimations->scale_bounce(score_position, 0.007, uxInstance->uxAnimations->mat_zeroscale)->preserveReference(); // TODO: inherit anim
                        break;
                    case SCORE_EFFECTS::MOVE_UP:
                        chain2 = uxInstance->uxAnimations->slideUpFullHeight(score_position)->preserveReference();
                        chain3 = uxInstance->uxAnimations->scale_bounce(score_position, 0.007, uxInstance->uxAnimations->mat_zeroscale)->preserveReference(); // TODO: inherit anim
                        break;
                    case SCORE_EFFECTS::DEFAULT:
                    default:
                        chain2 = uxInstance->uxAnimations->softBounce(score_position, widescreen?0:bounceIntensity, widescreen?bounceIntensity:0)->preserveReference();
                        chain3 = uxInstance->uxAnimations->scale_bounce(score_position, 0.007, uxInstance->uxAnimations->mat_zeroscale)->preserveReference(); // TODO: inherit anim
                        break;
                }

            }else{

                if( numberToDisplay > -1 ){
                    SDL_snprintf(score_disp_char, maxLen, "-%i", numberToDisplay); // -0
                }else{
                    // "loose" condition
                    SDL_snprintf(score_disp_char, maxLen, "%i", numberToDisplay);
                }
                chain2->endAnimation();
                chain3->endAnimation();
                switch( effectNum ){
                    case SCORE_EFFECTS::NOMOVE:
                        chain2 = uxInstance->uxAnimations->spin(score_position, -1)->preserveReference();
                        chain3 = uxInstance->uxAnimations->scale_bounce(score_position, 0.005, uxInstance->uxAnimations->mat_zeroscale)->preserveReference();
                        break;
                    case SCORE_EFFECTS::MOVE_UP:
                        chain2 = uxInstance->uxAnimations->slideUpFullHeight(score_position)->preserveReference();
                        chain3 = uxInstance->uxAnimations->scale_bounce(score_position, 0.005, uxInstance->uxAnimations->mat_zeroscale)->preserveReference();
                        break;
                    case SCORE_EFFECTS::DEFAULT:
                    default:
                        chain2 = uxInstance->uxAnimations->emphasizedBounce(score_position, widescreen?bounceIntensity:0, widescreen?0:bounceIntensity)->preserveReference();
                        chain3 = uxInstance->uxAnimations->scale_bounce(score_position, 0.005, uxInstance->uxAnimations->mat_zeroscale)->preserveReference();
                        break;
                }
            }

            float text_length = (float)SDL_strlen(score_disp_char);
            //score->boundryRect.x =  text_length * -0.5; // center
            score->boundryRect.x =  -text_length + 0.5; // right aligned (center the rightmost char)
            //score->boundryRect.x = -0.5;



            //if( score->boundryRect.x + -0.5 + score_position->boundryRect.x < 0 ) score_position->boundryRect.x += score->boundryRect.x + -0.5 + score_position->boundryRect.x; // keep "on screen"

            float hidLeftAmt = score_position->boundryRect.x + (score_position->origBoundryRect.w * score->boundryRect.x);
            if( hidLeftAmt < 0 ){
                score_position->boundryRect.x -= hidLeftAmt; // minus a negative
            }


            uxInstance->printStringToUiObject(score, score_disp_char, DO_NOT_RESIZE_NOW);





        }

        int_score += numberToDisplay;

        if( int_score > int_max_score ){
            int_max_score = int_score;
        }
    }


    void hide(){
        uiObjectItself->hide();
    }

    void resize(){
        update();
    }

    void update(){
        uiObjectItself->updateRenderPosition();
    }

};


#endif
