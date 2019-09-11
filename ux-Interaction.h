//
//  ux-anim.h
//  ColorPick SDL
//
//  Created by Sam Larison on 8/21/16.
//
//


#ifndef __ColorPick_iOS_SDL__uiInteraction__
#define __ColorPick_iOS_SDL__uiInteraction__




struct uiInteraction
{
    uiInteraction(){
        dx=0;
        dy=0;
        //        mvx=0;
        //        mvy=0;
        friction = 1.3;  // really inverse friction. higher values for less friction.
//        friction = 0.2;
//        friction = 4.2;
        friction = 2.2;

        isSecondInteraction=false;
        fingerStateDown = false; // its set to true externally..... if requierd...
        didCollideWithObject=false;
        isInteracting=false;

        interactionObject = nullptr;
        lastInteractionObject = nullptr;

        useInstantaneousVelocity=false;

        fingerId=0;
    }
    void begin(int ticks, float x, float y){

        px=x; // previous
        py=y;
        ix=x; // initial
        iy=y;
        rx=0; // relative since last update
        ry=0;
        dx=0; // difference
        dy=0;
        tx=0;
        ty=0;
        vx=0; // velocity
        vy=0;
        wheel = 0;
        isSecondInteraction=false;

        fingerStateDown = false; // its set to true externally..... if requierd...
        didCollideWithObject=false;
        isInteracting=false;

//        interactionObject = nullptr;
//        lastInteractionObject = nullptr;

        // was unsure why aboev commented otu but probably conflicts with Ux::triggerInteraction handling of these..
        // in any case this always fires first....
        lastInteractionObject = interactionObject;
        interactionObject = nullptr;
        // HOWEVER note, objectCollides will still assign an object, so it will appear we last interacted with
        // the object even though we only mouse UP on the object
        // this is probably best handled in Ux::interactionComplete where
        // we have logic that detects if the interaction object changed

//        lastUpdate=SDL_GetTicks();
//        if( ticks != lastUpdate ){
//            SDL_Log("Begin Disparity; evticks: %i ticks: %i diff: %i", ticks, lastUpdate, lastUpdate - ticks);
//        }
        lastUpdate=ticks;

        //        mvx=0;
        //        mvy=0;
    }
    bool isZeroed(){
        return px == ix && py == iy;
    }
    void done(int ticks, float x, float y){ // optional bool performFinalUpdate ?
        // last update...
        //this->update(x, y, 0, 0); // tell me why update on mouse up....  they dont wanna move anymore
        rx =0;
        ry =0; // reset these so we can keep calling update?  not always needed... plus sometimes rx and ry are needed!
        this->update(ticks);
        //isInteracting=false; // whomever set this true should handle setting it false....
    }
    void update(int ticks, float x, float y){ // todo pass delta and relative?
        rx = x - px;
        ry = y - py;

        tx += SDL_fabsf(rx);
        ty += SDL_fabsf(ry);

        px=x;
        py=y;

        dx = px - ix;
        dy = py - iy; // when this is greater than 0 we have moved down

        this->update(ticks);
    }
    void update(int ticks){ // update without movement.... or after movement applied
//        int thisUpdate = SDL_GetTicks();
//        if( ticks != thisUpdate ){
//            SDL_Log("Update Disparity; evticks: %i ticks: %i diff: %i", ticks, thisUpdate, thisUpdate - ticks);
//        }
        int thisUpdate = ticks;

        int elapsed = thisUpdate - lastUpdate;
        if( elapsed < 1 ) elapsed = 1;

        // decay per millisecond... 1.0/1000 = .001 decelleration rate per second
        // except now 0.5/1000 = 0.0005
        // 1.3 / 1000 =  0.0013
        float decay = (friction / elapsed); // and then apply the last velocity diff, which may be zero...
        if( decay > 1.0 ){
            decay = 0.99999999;
        }
        // todo the following is probably wrong.... vx responds to decay, but rx should nearly respondn to inverse of decay (movement over more time is slower movement though)
        // it seems odd to decay the instantaneous velocity immediately...

        if( useInstantaneousVelocity ){
            vx = (vx * decay) + (rx);
            vy = (vy * decay) + (ry);
        }else{
            vx = (vx * decay) + (decay * rx);
            vy = (vy * decay) + (decay * ry);
        }
        //        vx = (vx + rx) * decay;// + (decay * rx);
        //        vy = (vy + rx) * decay;// + (decay * ry);

        //SDL_Log("Velocity is: %f %f", vx, vy);

        // velocity should be measured per unit time?

        lastUpdate=thisUpdate;
    }

    bool wasStationary(){
        // we didn't move at all (let alone move and move back)
        // really not sure if this works good... float == 0 not exactly great
        return tx == 0 && ty == 0;
    }

    void fixX(Float_Rect r, Float_Rect p){
        float min = r.x;
        if( px < min ){
            px = min;
            return; // "short circut" the rest
        }
        float max = r.x + r.w; //(r.w * p.w);
        if( px > max ){
            px = max;
        }


        //px = r.x + (r.w * 0.5 * p.w);// last known drag position was mid button
    }

    // whatever our drag py was, was not represented by object movement constraints
    void fixY(Float_Rect r, Float_Rect p){
        //interactionObj->collisionRect,  interactionObj->parentObject->collisionRect
        float min = r.y;
        if( py < min ){
            py = min;
            return; // "short circut" the rest
        }
        float max = r.y + r.h;//(r.h * p.h);
        if( py > max ){
            py = max;
        }
    }
    //    int distanceMoved(){// SIMPLIFIED to x + y
    //        rx = (px - ix);
    //        ry = (py - iy);
    //        return  rx + ry;
    //    }


    bool useInstantaneousVelocity;

    float px; // previous
    float py;
    float ix; // initial
    float iy;
    float rx; // relative (since last update)
    float ry;
    float dx; // delta not distance - difference
    float dy;
    float tx; // total (direction independent)
    float ty;
    float vx; // velocity
    float vy;

    float wheel;

    float friction;
    //screenpixels: Float_Point

    Sint64 fingerId;
    bool fingerStateDown; // todo: for mousStateDown - seems pertty obvious.... but this should go back to false when the interaction is expired....
    bool didCollideWithObject;// did collide??
    bool isInteracting;
    void* interactionObject;
    void* lastInteractionObject; // if the finger/mouse changed objects....

    int lastUpdate;
    bool isSecondInteraction;
    //    int mvx;// unused ?
    //    int mvy;
};

#endif
