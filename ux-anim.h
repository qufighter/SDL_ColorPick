//
//  ux-anim.h
//  ColorPick iOS SDL
//
//  Created by Sam Larison on 8/21/16.
//
//


#ifndef __ColorPick_iOS_SDL__uxanim__
#define __ColorPick_iOS_SDL__uxanim__




struct uiAnimation
{
    Ux::uiObject *myUiObject;

    int ani_done_count;
    int required_done_count;

    //OH JUST MAKE A CONSTNT ALREADY
    float friction; //const_friction
    float threshold;
    float negThreshold;
    float friction_factor;
    // float fvx = 0.95; // friction x/y ?
    // float fvy = 0.95;
    // float fsvx = 0.95; // friction scale velocity x
    // float fsvy = 0.95;
    // float frv = 0.95; // friction rotational velocity

    // float tvx = 0.0001;
    // float tvy = 0.0001;
    // float tsvx = 0.0001;
    // float tsvy = 0.0001;
    // float trv = 0.0001;

    float vx; // velocity
    float vy;
    float svx; // scale velocity
    float svy; 
    float rv; // rotational velocity

    bool is_move;
    bool scale_velocity_set;
    bool rotate_velocity_set;
    bool move_velocity_set;
    int moveSteps; // could be dynamic per unit time
    float moveProgress;
    int moveDurationMs;
    float totalMoves;
    float moveScaleFactor;
    float px;
    float py;

    uiAnimation(Ux::uiObject *uiObject){
        this->initialMoveVelocity(0,0);
        this->initialScaleVelocity(0,0);
        this->initialRotationVelocity(0);
        this->setFriction(const_friction);
        this->setThreshold(const_threshold);
        myUiObject = uiObject;
        myAnimationCallbackFn = defaultUiObjectAnimationupdater;
        myGetBoundsFn = defaultUiObjectGetBoundsFn;
        animationReachedCallbackFn = nullptr;
        is_move=false;
        scale_velocity_set=false;
        rotate_velocity_set=false;
        move_velocity_set=false;
    }

    uiAnimation* setFriction(float ifriction){
        friction=ifriction;
        friction_factor = 1.0 - friction;
        return this;
    }

    uiAnimation* setThreshold(float ithreshold){
        threshold = ithreshold;
        negThreshold = -threshold;
        return this;
    }

    uiAnimation* initialMoveVelocity(float ivx, float ivy){
        vx=ivx;
        vy=ivy;
        move_velocity_set=true;
        return this;
    }

    uiAnimation* initialScaleVelocity(float isvx, float isvy){
        svx=isvx;
        svy=isvy;
        scale_velocity_set=true;
        return this;
    }

    uiAnimation* initialRotationVelocity(float irv){
        rv=irv;
        rotate_velocity_set=true;
        return this;
    }

    // you CAN moveTo and combine with animations like scale/rotate BUT those should be handled be a seperate parallel animation chain.... at least for now
    uiAnimation* moveTo(float ix, float iy){
        px = ix;
        py = iy;
        is_move = true;
        moveSteps = 1;
        moveProgress = 0;
        moveDurationMs = 1000;
//        totalMoves = moveSteps;
//        moveScaleFactor = -30.0; // 0 is linear scale, but also "disabled" ....  but this says "how many moves"

        /// we can define ways to scale move steps - linear does not scale move steps
        // otherwise we can scale based on progress over the total moves
        // we need to apply the factors equally so they are balanced over the duration -
        // every addition someplace requires a coresponding subtraction at another move step
        // this will result in an ease out and ease in capabilities
        return this;
    }

    uiAnimation* moveRelative(float ix, float iy){
        return moveTo(myUiObject->boundryRect.x + ix, myUiObject->boundryRect.y + iy);
    }


    uiAnimation* resetPosition(){
        return moveTo(myUiObject->origBoundryRect.x, myUiObject->origBoundryRect.y);
    }

    uiAnimation* immediately(){
        moveDurationMs = 0;
        return this;
    }

    uiAnimation* setAnimationCallback(animationUpdateCallbackFn pAnimUpdateFn){
        if( pAnimUpdateFn != nullptr)
            myAnimationCallbackFn = pAnimUpdateFn;
        return this;
    }

    uiAnimation* setGetBoundsFn(animationGetRectFn pGetBoundsFn){
        if( pGetBoundsFn!= nullptr )
            myGetBoundsFn = pGetBoundsFn;
        return this;
    }

    uiAnimation* setAnimationReachedCallback(animationCallbackFn pFn){
        if( pFn!= nullptr )
            animationReachedCallbackFn = pFn;
        return this;
    }

    // NOTE: always reutrn this; above
    // define other functions to chain off the constructor....
    // that return this
    // to define various operations to perform... .moveVelocity().scaleVelocity().rotationalVeloclity()
    // other htan the function names being too long to effectively chain, this is great!
    //

    animationGetRectFn myGetBoundsFn;
    animationUpdateCallbackFn myAnimationCallbackFn; // detemrine what we will update, default to defaultUiObjectAnimationupdater
    animationCallbackFn animationReachedCallbackFn;

    // this is animationGetRectFn
    static Float_Rect* defaultUiObjectGetBoundsFn(uiAnimation* self){
        return new Float_Rect(self->myUiObject->boundryRect);
    }

    // this is animationUpdateCallbackFn
    static void defaultUiObjectAnimationupdater(uiAnimation* self, Float_Rect *newBoundaryRect){
        Ux::setRect(&self->myUiObject->boundryRect, newBoundaryRect->x, newBoundaryRect->y, newBoundaryRect->w, newBoundaryRect->h);
        self->myUiObject->updateRenderPosition(); // this is occuring in the animation timeout thread.... if threaded
    }

    bool update(Uint32 elapsedMs){ // todo pass delta and relative

        if( animationReachedCallbackFn != nullptr ){
            animationReachedCallbackFn(this);
            return true; // this animation is done (that was it)
        }

        Float_Rect* newBounds = myGetBoundsFn(this);

        if( is_move ) {
            // we are moving to a point, so we compute a new vx/vy dynamically

            vx = (px - newBounds->x);
            vy = (py - newBounds->y);
// this is easing out "unfortunately" since the total range is only used the first time....
// so ease out being deafult we can still probably define ways to scale for ease in or linear
// and manipulate progressBar accordinly?

            moveProgress += elapsedMs;
            float progressBar = moveProgress / moveDurationMs;
            if( progressBar > 1.0 ) progressBar = 1.0;
//            float remainignPct = 1.0 - progressBar;
//            float progressScaling = ((progressBar * 2.0 ) - 1.0);

            vx = vx * progressBar;
            vy = vy * progressBar;


            //int move = moveSteps - totalMoves;
            // TODO we don't need a progress factor unless non zero moveScaleFactor
//            float progressFactor = (((moveSteps / totalMoves) * 2.0) * -1) + 1.0;
//
//            float scaledMoveSteps = moveSteps + (progressFactor  * moveScaleFactor );

//            vx = vx / moveSteps;
//            vy = vy / moveSteps;

//            float tvx = vx * elapsedMs;
//            float tvy = vy * elapsedMs;
//
//            if( vx < threshold && vx > negThreshold ){
//                vx=threshold*2;
//            }
//            if( vy < threshold && vy > negThreshold ){
//                vy=threshold*2;
//            }

//            vx = vx / elapsedMs;
//            vy = vy / elapsedMs;

            ani_done_count=0;
            if( vx < threshold && vx > negThreshold ){ vx  = 0; ani_done_count++; }
            if( vy < threshold && vy > negThreshold ){ vy  = 0; ani_done_count++; }
            if( ani_done_count == 2 ){
                moveProgress = moveDurationMs + 1;
                newBounds->y = py;
                newBounds->x = px;
            }else{
                newBounds->y += vy;
                newBounds->x += vx;
            }

            myAnimationCallbackFn(this, newBounds);

            if( moveProgress > moveDurationMs ){
                // we are totally done animating...., tell the chain to drop us
                return true; // done bool
            }
            return false;
        }

        updateUxObjectFromAnimation(elapsedMs, newBounds);

        // apply elapsedMs ?


        // tempf =  friction;
        // vx *= tempf;
        // vy *= tempf;
        //        svx *= tempf;
        //        svy *= tempf;
        //        rv *= tempf;


//        if( svx < threshold ){ svx = 0; ani_done_count++; }
//        if( svy < threshold ){ svy = 0; ani_done_count++; }
//        if( rv < threshold  ){ rv  = 0; ani_done_count++; }


        if( ani_done_count >= required_done_count){
            // we are totally done animating...., tell the chain to drop us
            return true; // done bool
        }

        return false;
    }

    void updateUxObjectFromAnimation(Uint32 elapsedMs, Float_Rect* newBounds){ // not used when is_move
        required_done_count = 0;
        ani_done_count=0;

        if( move_velocity_set ){
            newBounds->y += (vy * elapsedMs);
            newBounds->x += (vx * elapsedMs);

            vx -= (friction_factor * vx) * elapsedMs;
            vy -= (friction_factor * vy) * elapsedMs;

            if( vx < threshold && vx > negThreshold ){ vx  = 0; ani_done_count++; }
            if( vy < threshold && vy > negThreshold ){ vy  = 0; ani_done_count++; }

            required_done_count+=2;
        }

        if( scale_velocity_set ){

            // doing scale or rotate with the bounds rect is just painful... (and not compatible with moveTo, or move really) lets pipe this into the shader
        }

        if( rotate_velocity_set ){


        }
        myAnimationCallbackFn(this, newBounds);
    }

};


// chains of uiAnimation will be processed
struct uiAminChain
{

    uiAminChain(){//constructor
        animListIndex = 0;
        animListCurrent=0;
        invalidateChain=false;
        chainCompleted=false;
    }

    bool autoFree=true;
    bool invalidateChain=false;
    bool chainCompleted;
    bool result_done;
    int animListCurrent;
    int animListIndex;
    int animListMaxLen = 5; //derp
    uiAnimation* animList[5]; //  each chain can hold 5 animiations in queue

    bool update(Uint32 elapsedMs){

        if( this->invalidateChain ){
            this->end();
            this->autoFree = true; // we can enable auto free though so the garbage gets collected!
            return true; // chain completed
        }

        if( animListCurrent < animListIndex ){

            result_done = this->animList[animListCurrent]->update(elapsedMs);
            if( result_done ){

                free(this->animList[animListCurrent]); // GARBAGE COLLECTION ?! (we may mark an animation to not auto delete in the future, for reusing it)

                animListCurrent++;
                if( animListCurrent >= animListMaxLen ){
                    return true; // chain completed
                }
            }

            return false;
        }
        return true; // chain completed
    }

    void end(){ // think about thread safe-ness if update is stll called in different thread
        while( animListCurrent < animListIndex ){
            free(this->animList[animListCurrent]); // GARBAGE COLLECTION ?! (we may mark an animation to not auto delete in the future, for reusing it)
            animListCurrent++;
            if( animListCurrent >= animListMaxLen ){
                break; // chain completed
            }
        }
    }

    void endAnimation(){ // thread safe, but you must call preserveReference if you want to call anything later
        if( this->chainCompleted ){
            free(this);
        }else{
            this->invalidateChain = true;
        }
    }

    uiAminChain* preserveReference(){ // this ref will be preserved until endAmination is called or the next update runs.
        autoFree = false;
        return this;
    }

    uiAminChain* addAnim(uiAnimation* myAnim){

        // so if animListCurrent is already progressed above zero, we could drop all the animations that are "done" in the list, up to the current one...
        // this would allow us to chain more animiations dynamically as earlier animaitions complete....
        // this would be good for a game sort of game, so we implement that here (not tested)
        // this can probably be done with a single std call instead
        // also it should be noted that we are much more likely to wish to replace whatever the "next" animation is than to push it onto a list....
        if( animListCurrent > 0 ){
            for( int x=animListCurrent,nextIndex=0,l=animListIndex; x<l; x++ ){
                animList[nextIndex++] = animList[x];
            }
            animListIndex-=animListCurrent;
            animListCurrent = 0;
        }
        // btw today we never append to a runnign chain after init time :! (today is 2018)

        if( this->animListIndex < this->animListMaxLen ){
            this->animList[this->animListIndex++] = myAnim;
        }else{
            SDL_Log("ERROR::: Max Animation Objects In Animation Chain %d Exceeded !!!!!!!!", animListMaxLen);
        }

        return this;
    }

};


struct UxAnim
{


    UxAnim(){//constructor

        animChainIndex = 0;
        newChainIndex = 0;
        lastTimerTime = SDL_GetTicks();
        shouldUpdate = false;
        aniIsUpdating = false; // prevent multiple simultaneous updates from different timer threads? !
        //    currentTime = SDL_GetTicks();
        //    openglContext->updateFrame(currentTime - lastTimerTime);

    }


    int animChainsDeletedCount;
    bool result_done;
    bool result2_done;
    bool shouldUpdate;

    bool aniIsUpdating = false;

    int animChainIndex;
    static const int animChainsMax = 128;
    uiAminChain* animChains[animChainsMax]; //  128 parallel anim chains
    int newChainIndex;
    uiAminChain* newChains[animChainsMax];

    Uint32 lastTimerTime;
    Uint32 currentTime;
    SDL_TimerID my_timer_id;

    bool updateAnimations(float elapsedMs){
        /// hmmm begs question if timer thread will multi fire....

        if( aniIsUpdating ) return false; // If we return false it won't have an effect on the runnign animation
        aniIsUpdating = true; // one at a time please
        animChainsDeletedCount = 0;


        for( int x=0,l=animChainIndex; x<l; x++ ){

            animChains[x-animChainsDeletedCount] = animChains[x]; // we hope this will copy teh reference....

            result_done = animChains[x]->update(elapsedMs);

            if( result_done ){
                if( animChains[x]->autoFree ){
                    // WE GET AN EXCEPTION HERE SOMETIMES>>>>>
                    // malloc: *** error for object 0x7fbb4330: pointer being freed was not allocated
                    // sometimes this is caused by adding an animation chain while updating another animation perhaps?
                    free(animChains[x]); // GARBAGE COLLECTION ?! (we may mark a chain to not auto delete in the future, for reusing it?)
                }else{
                    animChains[x]->chainCompleted = true; // the only way the garbage can be otherwise collected
                }
                // we wish to drop this chain from the array of chains, so all subsequent chains move down by N index based on how many we have subtracted thus far
                animChainsDeletedCount++;
            }

        }

        animChainIndex -= animChainsDeletedCount; // we shifted everythign after deleted items into position
        // but while we do this we could be loosing references to items that are not yet free'd!
        // I gues we cannot delete anything so hastily in teh multi threaded universe, we MUST leave it to the thread, removing threading is a TODO though

        shouldUpdate = true; // main loop render will check this - not ideal... but works - can likely be modified later!!!!!! TODO
        aniIsUpdating = false;

        
        for( int x=0,l=newChainIndex; x<l; x++ ){
            pushAnimChain(newChains[x]);
        }
        newChainIndex=0;

        if( animChainIndex > 0 ) return false;
        else{
            SDL_RemoveTimer(my_timer_id);
            SDL_Log("removing timer due to comlpeted");
        }

        return true;
    }


    static Uint32 my_callbackfunc(Uint32 interval, void* parm){

        UxAnim* self = (UxAnim*)parm;

        SDL_Log("Our animition timere is firign loke mad"); // useful to see that animations DO stop...
        self->currentTime = SDL_GetTicks();

        self->result2_done = self->updateAnimations(self->currentTime - self->lastTimerTime);
        // TODO check result !!!


        self->lastTimerTime = self->currentTime;

// TODO : I think using one thread may help provide better timing of the animations...... and may be generally safer!

//        SDL_Event event;
//        SDL_UserEvent userevent;
        /* In this example, our callback pushes an SDL_USEREVENT event
         into the queue, and causes our callback to be called again at the
         same interval: */  // the idea of this is that we could process in the main thread by sending the event...

//        userevent.type = SDL_USEREVENT;
//        userevent.code = 0;
//        userevent.data1 = NULL;
//        userevent.data2 = NULL;
//
//        event.type = SDL_USEREVENT;
//        event.user = userevent;
//        SDL_PushEvent(&event);

        if( self->result2_done ) return 0; // If the returned value from the callback is 0, the timer is canceled.  We already called SDL_RemoveTimer fyi

        return interval;
    }

    void pushAnimChain(uiAminChain* myAnimChain){
        if( aniIsUpdating ){
            // TODO: make this call thread safe... we can only push at the end of the update
            if( newChainIndex < animChainsMax - 1 ){
                newChains[newChainIndex++] = myAnimChain;
            }else{
                SDL_Log("ERROR::: Max Animation Chains -newChains- %d Exceeded !!!!!!!!", animChainsMax);
            }
            SDL_Log("WARNING: adding animation is currently being updated from the update thread... safely...");
            return;
            // I think there may be ohter negative implications of allowing this though, specifically uiElements that hold references to the aniChain
            // which may have already "forgot" their currently running animation as soon as this function is called, may leak memory
            // I suggest using the arbatraryCompletionCallback instead animationReachedCallbackFn ?
        }

        if( animChainIndex < animChainsMax - 1 ){
            animChains[animChainIndex++] = myAnimChain;
        }else{
            SDL_Log("ERROR::: Max Animation Chains %d Exceeded !!!!!!!!", animChainsMax);
        }

        lastTimerTime = SDL_GetTicks();
        SDL_bool wasRunning = SDL_RemoveTimer(my_timer_id);
        my_timer_id = SDL_AddTimer(30, my_callbackfunc, this);
        // todo return uiAminChain* myAnimChain... so our create functions can be one line! (hurray?)
    }

    /*
     A we can chain addAnim now too!
     */

    uiAminChain* moveTo(Ux::uiObject *uiObject, float x, float y, animationUpdateCallbackFn pAnimUpdateFn, animationGetRectFn pGetBoundsFn){
        uiAminChain* myAnimChain = new uiAminChain();
        myAnimChain->addAnim((new uiAnimation(uiObject))->moveTo(x, y)->setAnimationCallback(pAnimUpdateFn)->setGetBoundsFn(pGetBoundsFn) );
        pushAnimChain(myAnimChain);
        return myAnimChain;
        
    }

    uiAminChain* bounce(Ux::uiObject *uiObject){

        uiAminChain* myAnimChain = new uiAminChain();
        myAnimChain->addAnim((new uiAnimation(uiObject))->initialMoveVelocity(0, -0.01) );
        myAnimChain->addAnim((new uiAnimation(uiObject))->initialMoveVelocity(0,  0.01) );
        myAnimChain->addAnim((new uiAnimation(uiObject))->resetPosition()->immediately()  );

        pushAnimChain(myAnimChain);
        return myAnimChain;

    }

    uiAminChain* resetPosition(Ux::uiObject *uiObject){ /* slideUpIn */ /* reset position */
        uiAminChain* myAnimChain = new uiAminChain();
        //myAnimChain->addAnim( (new uiAnimation(uiObject))->moveRelative(0, -0.5) );
        myAnimChain->addAnim( (new uiAnimation(uiObject))->resetPosition() );
        pushAnimChain(myAnimChain);
        return myAnimChain;
    }

    uiAminChain* slideDown(Ux::uiObject *uiObject){ /* slideDownOut */
        uiAminChain* myAnimChain = new uiAminChain();
        //myAnimChain->addAnim( (new uiAnimation(uiObject))->moveRelative(0, 0.5) );
        myAnimChain->addAnim( (new uiAnimation(uiObject))->moveTo(0, 1.0) );
        pushAnimChain(myAnimChain);
        return myAnimChain;
    }

    uiAminChain* slideDownFullHeight(Ux::uiObject *uiObject){ /* slideDownOut */
        uiAminChain* myAnimChain = new uiAminChain();
        //myAnimChain->addAnim( (new uiAnimation(uiObject))->moveRelative(0, 0.5) );
        myAnimChain->addAnim( (new uiAnimation(uiObject))->moveTo(0, uiObject->origBoundryRect.y + uiObject->origBoundryRect.h) ); // WE SHOULD CONSIDER A WAY TO JUST USE THE OBJECTS MOVEMENT BOUNDARY RECT?
        pushAnimChain(myAnimChain);
        return myAnimChain;
    }

//    uiAminChain* impulseUp(Ux::uiObject *uiObject){ /* slideDownOut */
//        uiAminChain* myAnimChain = new uiAminChain();
//        //myAnimChain->addAnim( (new uiAnimation(uiObject))->moveRelative(0, 0.5) );
//        myAnimChain->addAnim( (new uiAnimation(uiObject))->initialMoveVelocity(0, -1.0) );
//        pushAnimChain(myAnimChain);
//        return myAnimChain;
//    }
//
//    uiAminChain* impulseDown(Ux::uiObject *uiObject){ /* slideDownOut */
//        uiAminChain* myAnimChain = new uiAminChain();
//        //myAnimChain->addAnim( (new uiAnimation(uiObject))->moveRelative(0, 0.5) );
//        myAnimChain->addAnim( (new uiAnimation(uiObject))->initialMoveVelocity(0, 1.0) );
//        pushAnimChain(myAnimChain);
//        return myAnimChain;
//    }

    uiAminChain* rvbounce(Ux::uiObject *uiObject){

        uiAminChain* myAnimChain = new uiAminChain();
        //->addAnim( (new uiAnimation(uiObject))->moveRelative(-0.1667, 0) );

        myAnimChain->addAnim( (new uiAnimation(uiObject))->initialMoveVelocity(-0.001, 0) );
        //myAnimChain->addAnim( (new uiAnimation(uiObject))->initialMoveVelocity(0.001, 0) );

        myAnimChain->addAnim( (new uiAnimation(uiObject))->resetPosition() );
//        myAnimChain->addAnim( (new uiAnimation(uiObject->childList[0]))->initialMoveVelocity(0, -0.01) );
//        //myAnimChain->addAnim( (new uiAnimation(uiObject->childList[0]))->initialMoveVelocity(0, 0.01) );
//
//        myAnimChain->addAnim( (new uiAnimation(uiObject->childList[0]))->resetPosition() );

        pushAnimChain(myAnimChain);
        return myAnimChain;
    }


};

#endif
