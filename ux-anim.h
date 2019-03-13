//
//  ux-anim.h
//  ColorPick SDL
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
    bool is_reset;
    bool scale_velocity_set;
    bool rotate_velocity_set;
    bool move_velocity_set;
    bool dest_matrix_set;
    glm::mat4 initial_matrix;
    glm::mat4 dest_matrix;
    int moveSteps; // could be dynamic per unit time
    float durationProgress;
    int durationMs;  // not used when impulse (initial velocity) is used... then duration is left to friction...
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
        is_reset=false;
        dest_matrix_set=false;
        scale_velocity_set=false;
        rotate_velocity_set=false;
        move_velocity_set=false;
        durationProgress = 0;
        durationMs=1000;
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
//        durationProgress = 0;
//        durationMs = 1000;
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
        // todo - what if our orig position moves while we are going there?  we really need to update this as teh animation progresses......
        is_reset = true;
        return moveTo(myUiObject->origBoundryRect.x, myUiObject->origBoundryRect.y);
    }

    uiAnimation* resetMatrix(glm::mat4 destMatrix){
        is_reset = true;
        dest_matrix_set = true;
        dest_matrix = destMatrix;
        durationMs = 500;
//        return moveTo(myUiObject->origBoundryRect.x, myUiObject->origBoundryRect.y);
        return this;
    }

    uiAnimation* resetMatrix(){
        return resetMatrix(glm::mat4(1.0f));
    }

    uiAnimation* immediately(){
        durationMs = 0;
        return this;
    }

    uiAnimation* setDuration(int ms){
        durationMs = ms;
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
        return new Float_Rect(self->myUiObject->boundryRect); // why are we copying the rect? its leaking hte memory?  (well the rect is in fact modified so we had better copy it?)  try freeing it right after its returned... you will find SDL_free(newBoundaryRect);, so the scrollAnimationUpdaterCb always handles freeing this, so its not !
    }

    // this is animationUpdateCallbackFn - if you implement your own you have to free the rect when you are done with it
    static void defaultUiObjectAnimationupdater(uiAnimation* self, Float_Rect *newBoundaryRect){
        Ux::setRect(&self->myUiObject->boundryRect, newBoundaryRect->x, newBoundaryRect->y, newBoundaryRect->w, newBoundaryRect->h);
        SDL_free(newBoundaryRect);
        self->myUiObject->updateRenderPosition(); // this is occuring in the animation timeout thread.... if threaded
    }

    void start(){
        //this is called at the beginnign of this animation a single time, by the chain or above
        // its used to capture starting points for this animation, if they are arbitrary (dependent on previous animations)
        initial_matrix = glm::mat4(myUiObject->matrix); // do we need to copy this?/ probably....
    }

    bool update(Uint32 elapsedMs){ // todo pass delta and relative

        if( animationReachedCallbackFn != nullptr ){
            animationReachedCallbackFn(this);
            return true; // this animation is done (that was it)
        }

        Float_Rect* newBounds = myGetBoundsFn(this);

        if( is_move ) {
            // we are moving to a point, so we compute a new vx/vy dynamically

            if( is_reset ){
                px = myUiObject->origBoundryRect.x;
                py = myUiObject->origBoundryRect.y;
            }

            vx = (px - newBounds->x);
            vy = (py - newBounds->y);
// this is easing out "unfortunately" since the total range is only used the first time....
// so ease out being deafult we can still probably define ways to scale for ease in or linear
// and manipulate progressBar accordinly?

            durationProgress += elapsedMs;
            float progressBar = durationProgress / durationMs;
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
                durationProgress = durationMs + 1;
                newBounds->y = py;
                newBounds->x = px;
            }else{
                newBounds->y += vy;
                newBounds->x += vx;
            }

            myAnimationCallbackFn(this, newBounds);

            if( durationProgress > durationMs ){
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

        if( dest_matrix_set ){

            durationProgress += elapsedMs;
            float progressBar = durationProgress / durationMs;

            if( progressBar >= 1.0 ){
                progressBar = 1.0;
                ani_done_count++;
                //SDL_Log("Mat reset done");
            }

            // we perform a "linear" reset here for starters...
            glm::mat4 diffMatrix = dest_matrix - initial_matrix;

            myUiObject->matrix = initial_matrix + (diffMatrix * progressBar);

//            initial_matrix = glm::mat4(myUiObject->matrix);
//            dest_matrix

            required_done_count+=1;
        }

        if( scale_velocity_set ){

            myUiObject->matrix = glm::scale(myUiObject->matrix, glm::vec3(1.0-(svx * elapsedMs),1.0-(svy * elapsedMs),1.0));

            svx -= (friction_factor * svx) * elapsedMs;
            svy -= (friction_factor * svy) * elapsedMs;

            //SDL_Log("%f %f ljkljkljklj", svx, svy);

            if( svx < threshold && svx > negThreshold ){ svx  = 0; ani_done_count++; }
            if( svy < threshold && svy > negThreshold ){ svy  = 0; ani_done_count++; }

            required_done_count+=2;

        }

        if( rotate_velocity_set ){

            myUiObject->matrix = glm::rotate(myUiObject->matrix, rv, glm::vec3(0.0f, 0.0f, 1.0f));

            rv -= (friction_factor * rv) * elapsedMs;

            if( rv < threshold && rv > negThreshold ){ rv  = 0; ani_done_count++; }

            required_done_count+=1;

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
    static const int animListMaxLen = 5; //  each chain can hold 5 animiations in queue
    uiAnimation* animList[animListMaxLen];

    void startEventForCurrentAnim(){
        this->animList[animListCurrent]->start();
    }

    bool update(Uint32 elapsedMs){

        if( this->invalidateChain ){
            this->end();
            this->autoFree = true; // we can enable auto free though so the garbage gets collected!
            return true; // chain completed
        }

        if( 0 < animListIndex ){ // means we have animations in the chain

            result_done = this->animList[animListCurrent]->update(elapsedMs);
            if( result_done ){

                SDL_free(this->animList[animListCurrent]); // GARBAGE COLLECTION ?! (we may mark an animation to not auto delete in the future, for reusing it)

                animListCurrent++;
                if( animListCurrent >= animListIndex ){
                //if( animListCurrent >= animListMaxLen ){ // hmmm previously itw ould always go to 5..
                    return true; // chain completed
                }else{
                    startEventForCurrentAnim();
                }
            }

            return false;
        }
        return true; // chain completed
    }

    void end(){ // think about thread safe-ness if update is stll called in different thread
        while( animListCurrent < animListIndex ){
            SDL_free(this->animList[animListCurrent]); // GARBAGE COLLECTION ?! (we may mark an animation to not auto delete in the future, for reusing it)
            animListCurrent++;
            if( animListCurrent >= animListMaxLen ){
                break; // chain completed
            }
        }
    }

    void endAnimation(){ // thread safe, but you must call preserveReference if you want to call anything later
        if( this->chainCompleted ){
            SDL_free(this);
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

        mat_identity = glm::mat4(1.0f);
        mat_zeroscale = glm::scale(mat_identity, glm::vec3(0.0,0.0,1.0));
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


    glm::mat4 mat_identity;
    glm::mat4 mat_zeroscale;

    Uint32 lastTimerTime;
    Uint32 currentTime;
    SDL_TimerID my_timer_id;

    // maybe this can just be a helper on the chain itself?  .free() ??
    void freeAnimationChain(uiAminChain* myAnimChain){
        if( myAnimChain->autoFree ){
            // WE GET AN EXCEPTION HERE SOMETIMES>>>>>
            // malloc: *** error for object 0x7fbb4330: pointer being freed was not allocated
            // sometimes this is caused by adding an animation chain while updating another animation perhaps?  (I think this is mostly caused by the debugger... a breakpoint at "ERROR::: Max Animation" will ultimiately cause exceptins here)
            SDL_free(myAnimChain); // GARBAGE COLLECTION ?! (we may mark a chain to not auto delete in the future, for reusing it?)
        }else{
            myAnimChain->chainCompleted = true; // the only way the garbage can be otherwise collected
        }
    }

    bool updateAnimations(float elapsedMs){
        /// hmmm begs question if timer thread will multi fire....

        if( aniIsUpdating ) return false; // If we return false it won't have an effect on the runnign animation
        aniIsUpdating = true; // one at a time please
        animChainsDeletedCount = 0;


        for( int x=0,l=animChainIndex; x<l; x++ ){

            animChains[x-animChainsDeletedCount] = animChains[x]; // we hope this will copy teh reference....

            result_done = animChains[x]->update(elapsedMs);

            if( result_done ){
                freeAnimationChain(animChains[x]);
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
            SDL_Log("Animition timer completed");
        }

        return true;
    }

    bool updateAnimationsMain(bool autoContinue){
        currentTime = SDL_GetTicks();

        result2_done = updateAnimations(currentTime - lastTimerTime);
        // TODO check result !!!

        lastTimerTime = currentTime;

        if( autoContinue && !result2_done ){
            my_timer_id = SDL_AddTimer(30, my_callbackfunc, this);
        }

        return result2_done;
    }

    static Uint32 my_callbackfunc(Uint32 interval, void* parm){

        UxAnim* self = (UxAnim*)parm;

        //SDL_Log("Animition timer is firing..."); // useful to see that animations DO stop...

#ifndef MAIN_THREAD_ANIMATIONS
        bool isDone = self->updateAnimationsMain(false);
        if( isDone ) return 0; // If the returned value from the callback is 0, the timer is canceled.  We already called SDL_RemoveTimer fyi
        return interval;
#else
        SDL_Event event;
        SDL_UserEvent userevent;

        userevent.type = SDL_USEREVENT;
        userevent.code = USER_EVENT_ENUM::ANIMATE_MAIN_THREAD;
        userevent.data1 = NULL;
        userevent.data2 = NULL;

        event.type = SDL_USEREVENT;
        event.user = userevent;

        SDL_PushEvent(&event);

        return 0; // callback won't fire at same interval exactly... we cancel it here
#endif
    }

    void pushAnimChain(uiAminChain* myAnimChain){
        if( aniIsUpdating ){
            // TODO: make this call thread safe... we can only push at the end of the update
            if( newChainIndex < animChainsMax - 1 ){
                newChains[newChainIndex++] = myAnimChain;
            }else{
                SDL_Log("ERROR::: Max Animation Chains -newChains- %d Exceeded !!!!!!!!", animChainsMax);
                //freeAnimationChain(myAnimChain);
                // todo: in cases like these, we should always do something to advance the animations to completion
            }
            SDL_Log("WARNING: adding animation is currently being updated from the update thread... safely...");
            return;
            // I think there may be ohter negative implications of allowing this though, specifically uiElements that hold references to the aniChain
            // which may have already "forgot" their currently running animation as soon as this function is called, may leak memory
            // I suggest using the arbatraryCompletionCallback instead animationReachedCallbackFn ?
        }

        if( animChainIndex < animChainsMax - 1 ){
            animChains[animChainIndex++] = myAnimChain;
            if( myAnimChain->animListIndex > 0 && myAnimChain->animListCurrent < myAnimChain->animListIndex ){
               myAnimChain->startEventForCurrentAnim();
            }
        }else{
            SDL_Log("ERROR::: Max Animation Chains %d Exceeded !!!!!!!!", animChainsMax);
            //freeAnimationChain(myAnimChain); // this one is troubling.... we won't know its not been pushed.... we don't know if the reference is preserved yet...
            // todo: in cases like these, we should always do something to advance the animations to completion
            // JUST COMPLETE THESE ANIMATIONS IMMEDIATELY???
        }

        lastTimerTime = SDL_GetTicks();
        /*SDL_bool wasRunning = */SDL_RemoveTimer(my_timer_id);
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


    uiAminChain* resetPosition(Ux::uiObject *uiObject){ /* slideUpIn */ /* reset position */
        uiAminChain* myAnimChain = new uiAminChain();
        //myAnimChain->addAnim( (new uiAnimation(uiObject))->moveRelative(0, -0.5) );
        myAnimChain->addAnim( (new uiAnimation(uiObject))->resetPosition() );
        pushAnimChain(myAnimChain);
        return myAnimChain;
    }

    uiAminChain* slideLeft(Ux::uiObject *uiObject){ /* slideDownOut */
        uiAminChain* myAnimChain = new uiAminChain();
        //myAnimChain->addAnim( (new uiAnimation(uiObject))->moveRelative(0, 0.5) );
        myAnimChain->addAnim( (new uiAnimation(uiObject))->moveTo(-1.0, uiObject->origBoundryRect.y) );
        pushAnimChain(myAnimChain);
        return myAnimChain;
    }

    uiAminChain* slideRight(Ux::uiObject *uiObject){ /* slideDownOut */
        uiAminChain* myAnimChain = new uiAminChain();
        //myAnimChain->addAnim( (new uiAnimation(uiObject))->moveRelative(0, 0.5) );
        myAnimChain->addAnim( (new uiAnimation(uiObject))->moveTo(1.0, uiObject->origBoundryRect.y) );
        pushAnimChain(myAnimChain);
        return myAnimChain;
    }

    uiAminChain* slideDown(Ux::uiObject *uiObject){ /* slideDownOut */
        uiAminChain* myAnimChain = new uiAminChain();
        //myAnimChain->addAnim( (new uiAnimation(uiObject))->moveRelative(0, 0.5) );
        myAnimChain->addAnim( (new uiAnimation(uiObject))->moveTo(uiObject->origBoundryRect.x, 1.0) );
        pushAnimChain(myAnimChain);
        return myAnimChain;
    }

    uiAminChain* slideUp(Ux::uiObject *uiObject){ /* slideDownOut */
        uiAminChain* myAnimChain = new uiAminChain();
        //myAnimChain->addAnim( (new uiAnimation(uiObject))->moveRelative(0, 0.5) );
        myAnimChain->addAnim( (new uiAnimation(uiObject))->moveTo(uiObject->origBoundryRect.x, -1.0) );
        pushAnimChain(myAnimChain);
        return myAnimChain;
    }

    uiAminChain* slideLeftFullWidth(Ux::uiObject *uiObject){ /* slideDownOut */
        uiAminChain* myAnimChain = new uiAminChain();
        //myAnimChain->addAnim( (new uiAnimation(uiObject))->moveRelative(0, 0.5) );
        myAnimChain->addAnim( (new uiAnimation(uiObject))->moveTo(uiObject->origBoundryRect.x - uiObject->origBoundryRect.w, uiObject->origBoundryRect.y) ); // WE SHOULD CONSIDER A WAY TO JUST USE THE OBJECTS MOVEMENT BOUNDARY RECT?
        pushAnimChain(myAnimChain);
        return myAnimChain;
    }

    uiAminChain* slideRightFullWidth(Ux::uiObject *uiObject){ /* slideDownOut */
        uiAminChain* myAnimChain = new uiAminChain();
        //myAnimChain->addAnim( (new uiAnimation(uiObject))->moveRelative(0, 0.5) );
        myAnimChain->addAnim( (new uiAnimation(uiObject))->moveTo(uiObject->origBoundryRect.x + uiObject->origBoundryRect.w, uiObject->origBoundryRect.y) ); // WE SHOULD CONSIDER A WAY TO JUST USE THE OBJECTS MOVEMENT BOUNDARY RECT?
        pushAnimChain(myAnimChain);
        return myAnimChain;
    }

    uiAminChain* slideDownFullHeight(Ux::uiObject *uiObject){ /* slideDownOut */
        uiAminChain* myAnimChain = new uiAminChain();
        //myAnimChain->addAnim( (new uiAnimation(uiObject))->moveRelative(0, 0.5) );
        myAnimChain->addAnim( (new uiAnimation(uiObject))->moveTo(uiObject->origBoundryRect.x, uiObject->origBoundryRect.y + uiObject->origBoundryRect.h) ); // WE SHOULD CONSIDER A WAY TO JUST USE THE OBJECTS MOVEMENT BOUNDARY RECT?
        pushAnimChain(myAnimChain);
        return myAnimChain;
    }

    uiAminChain* slideUpFullHeight(Ux::uiObject *uiObject){ /* slideDownOut */
        uiAminChain* myAnimChain = new uiAminChain();
        //myAnimChain->addAnim( (new uiAnimation(uiObject))->moveRelative(0, 0.5) );
        myAnimChain->addAnim( (new uiAnimation(uiObject))->moveTo(uiObject->origBoundryRect.x, uiObject->origBoundryRect.y - uiObject->origBoundryRect.h) ); // WE SHOULD CONSIDER A WAY TO JUST USE THE OBJECTS MOVEMENT BOUNDARY RECT?
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

    uiAminChain* emphasizedBounce(Ux::uiObject *uiObject, float ivx, float ivy){
        uiAminChain* myAnimChain = new uiAminChain();
        myAnimChain->addAnim((new uiAnimation(uiObject))->initialMoveVelocity(ivx, ivy) );
        myAnimChain->addAnim((new uiAnimation(uiObject))->initialMoveVelocity(-ivx, -ivy) );
        myAnimChain->addAnim( (new uiAnimation(uiObject))->resetPosition() );
        pushAnimChain(myAnimChain);
        return myAnimChain;
    }

    uiAminChain* softBounce(Ux::uiObject *uiObject, float ivx, float ivy){
        uiAminChain* myAnimChain = new uiAminChain();
        myAnimChain->addAnim( (new uiAnimation(uiObject))->initialMoveVelocity(ivx, ivy) );
        myAnimChain->addAnim( (new uiAnimation(uiObject))->resetPosition() );
        pushAnimChain(myAnimChain);
        return myAnimChain;
    }


    uiAminChain* bounce(Ux::uiObject *uiObject){ // orig emphasizedBounce

        uiAminChain* myAnimChain = new uiAminChain();
        myAnimChain->addAnim((new uiAnimation(uiObject))->initialMoveVelocity(0, -0.01) );
        myAnimChain->addAnim((new uiAnimation(uiObject))->initialMoveVelocity(0,  0.01) );
        myAnimChain->addAnim((new uiAnimation(uiObject))->resetPosition()->immediately()  );

        pushAnimChain(myAnimChain);
        return myAnimChain;

    }

    uiAminChain* rvbounce(Ux::uiObject *uiObject){ // orig soft bounce "reverse" bounce...
        return rvbounce(uiObject, 0.001);
    }
    uiAminChain* rvbounce(Ux::uiObject *uiObject, float intensity){ // orig soft bounce

        uiAminChain* myAnimChain = new uiAminChain();
        //->addAnim( (new uiAnimation(uiObject))->moveRelative(-0.1667, 0) );

        myAnimChain->addAnim( (new uiAnimation(uiObject))->initialMoveVelocity(-intensity, 0) );
        //myAnimChain->addAnim( (new uiAnimation(uiObject))->initialMoveVelocity(0.001, 0) );

        //myAnimChain->addAnim( (new uiAnimation(uiObject))->initialRotationVelocity(5) );
        //myAnimChain->addAnim( (new uiAnimation(uiObject))->initialScaleVelocity(-0.01, 0.0) );

        //uiMatrix = glm::scale(uiMatrix, glm::vec3(0.9,0.9,1.0));
        //uiMatrix = glm::rotate(uiMatrix,  2.0f, glm::vec3(0.0f, 0.0f, 1.0f));


        myAnimChain->addAnim( (new uiAnimation(uiObject))->resetPosition() );
//        myAnimChain->addAnim( (new uiAnimation(uiObject->childList[0]))->initialMoveVelocity(0, -0.01) );
//        //myAnimChain->addAnim( (new uiAnimation(uiObject->childList[0]))->initialMoveVelocity(0, 0.01) );
//
//        myAnimChain->addAnim( (new uiAnimation(uiObject->childList[0]))->resetPosition() );

        pushAnimChain(myAnimChain);
        return myAnimChain;
    }

    uiAminChain* emptyChain(){
        uiAminChain* myAnimChain = new uiAminChain();
        //pushAnimChain(myAnimChain);
        return myAnimChain;
    }

    uiAminChain* spin(Ux::uiObject *uiObject, float impulse){
        uiAminChain* myAnimChain = new uiAminChain();
        myAnimChain->addAnim( (new uiAnimation(uiObject))->initialRotationVelocity(impulse) );
        //myAnimChain->addAnim( (new uiAnimation(uiObject))->resetPosition() );
        pushAnimChain(myAnimChain);
        return myAnimChain;
    }

    uiAminChain* spin(Ux::uiObject *uiObject){
        return spin(uiObject, 5);
    }

    uiAminChain* spin_reset(Ux::uiObject *uiObject, float impulse){
        return spin_reset(uiObject, impulse, glm::mat4(1.0f));
    }

    uiAminChain* spin_reset(Ux::uiObject *uiObject, float impulse, glm::mat4 resetToMat){ // orig soft bounce resetToMat){ // orig soft bounce
        uiAminChain* myAnimChain = new uiAminChain();
        myAnimChain->addAnim( (new uiAnimation(uiObject))->initialRotationVelocity(impulse) );
        //myAnimChain->addAnim( (new uiAnimation(uiObject))->resetPosition() );
        myAnimChain->addAnim( (new uiAnimation(uiObject))->resetMatrix(resetToMat) );
        pushAnimChain(myAnimChain);
        return myAnimChain;
    }

    uiAminChain* spin_reset(Ux::uiObject *uiObject){
        return spin_reset(uiObject, 5);
    }

    uiAminChain* spin_negative(Ux::uiObject *uiObject, float impulse){
        return spin_negative(uiObject, impulse, glm::mat4(1.0f));
    }

    uiAminChain* spin_negative(Ux::uiObject *uiObject, float impulse, glm::mat4 resetToMat){
        uiAminChain* myAnimChain = new uiAminChain();
        myAnimChain->addAnim( (new uiAnimation(uiObject))->initialRotationVelocity(impulse) );
        myAnimChain->addAnim( (new uiAnimation(uiObject))->initialRotationVelocity(-impulse * 2) );
        myAnimChain->addAnim( (new uiAnimation(uiObject))->resetMatrix(resetToMat) );
        pushAnimChain(myAnimChain);
        return myAnimChain;
    }

    uiAminChain* reset_matrix(Ux::uiObject *uiObject){
        uiAminChain* myAnimChain = new uiAminChain();
        myAnimChain->addAnim( (new uiAnimation(uiObject))->resetMatrix(glm::mat4(1.0)) );
        myAnimChain->animList[0]->durationMs = 100; // argue: instead resetMatrix needs to accept more args... and so does this fn...
        pushAnimChain(myAnimChain);
        return myAnimChain;
    }

    uiAminChain* scale_bounce(Ux::uiObject *uiObject){
        return scale_bounce(uiObject, 0.005);
    }

    uiAminChain* scale_bounce(Ux::uiObject *uiObject, float intensity){
        return scale_bounce(uiObject, intensity, glm::mat4(1.0f));
    }

    uiAminChain* scale_bounce(Ux::uiObject *uiObject, float intensity, glm::mat4 resetToMat){
        return scale_bounce(uiObject, intensity, resetToMat, 1000);
    }

    uiAminChain* scale_bounce(Ux::uiObject *uiObject, float intensity, glm::mat4 resetToMat, int durationMs){
        uiAminChain* myAnimChain = new uiAminChain();
        myAnimChain->addAnim( (new uiAnimation(uiObject))->initialScaleVelocity(-intensity, -intensity)->setDuration(durationMs) );  // duration does not strictly work, its not time bounded.... friction bounded
        //myAnimChain->addAnim( (new uiAnimation(uiObject))->resetPosition() );
        myAnimChain->addAnim( (new uiAnimation(uiObject))->resetMatrix(resetToMat)->setDuration(durationMs) );
        pushAnimChain(myAnimChain);
        return myAnimChain;
    }

    uiAminChain* flip_hz(Ux::uiObject *uiObject, int durationMs, animationCallbackFn halfFlippedCb, animationCallbackFn fullyFlippedCb){
        uiAminChain* myAnimChain = new uiAminChain();

        glm::mat4 scaleMat = glm::scale(glm::mat4(1.0), glm::vec3(0.0, 1.0, 1.0) );
        //glm::mat4 rotateMat = glm::rotate(glm::mat4(1.0), 90.0f, glm::vec3(0.0f, 1.0f, 0.0f) );
        // thereis no persp matrix, so these are pretty much equiv for 2d.. ^ probably to exchange if we ever use this 3d?
        myAnimChain->addAnim( (new uiAnimation(uiObject))->resetMatrix(scaleMat)->setDuration(durationMs * 0.5) );
        //myAnimChain->addAnim( (new uiAnimation(uiObject))->resetMatrix(rotateMat)->setDuration(durationMs * 0.5) );

        if( halfFlippedCb != nullptr ){
            myAnimChain->addAnim((new uiAnimation(uiObject))->setAnimationReachedCallback(halfFlippedCb) );
        }

        myAnimChain->addAnim( (new uiAnimation(uiObject))->resetMatrix(glm::mat4(1.0))->setDuration(durationMs * 0.5) );

        if( fullyFlippedCb != nullptr ){
            myAnimChain->addAnim((new uiAnimation(uiObject))->setAnimationReachedCallback(fullyFlippedCb) );
        }

        pushAnimChain(myAnimChain);
        return myAnimChain;
    }



};

#endif
