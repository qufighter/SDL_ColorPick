//
//  ux-anim.h
//  ColorPick iOS SDL
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
        friction = 4.2;
    }
    void begin(float x, float y){

        px=x; // previous
        py=y;
        ix=x; // initial
        iy=y;
        rx=0; // relative since last update
        ry=0;
        dx=0; // difference
        dy=0;
        vx=0; // velocity
        vy=0;
        isSecondInteraction=false;
        lastUpdate=SDL_GetTicks();
        //        mvx=0;
        //        mvy=0;
    }
    bool isZeroed(){
        return px == ix && py == iy;
    }
    void done(float x, float y){ // optional bool performFinalUpdate ?
        // last update...
        //this->update(x, y, 0, 0); // tell me why update on mouse up....  they dont wanna move anymore
        rx =0;
        ry =0; // reset these so we can keep calling update?  not always needed... plus sometimes rx and ry are needed!
    }
    void update(float x, float y){ // todo pass delta and relative
        // maybe we should scale the coordinates to screen here instead....
        // TODO pass time in?

        rx = x - px;
        ry = y - py;

        px=x;
        py=y;

        dx = px - ix;
        dy = py - iy; // when this is greater than 0 we have moved down

        this->update();

    }
    void update(){ // update without movement.... or after movement applied
        int thisUpdate = SDL_GetTicks();
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
//        vx = (vx * decay) + (decay * rx);
//        vy = (vy * decay) + (decay * ry);
        vx = (vx * decay) + (rx);
        vy = (vy * decay) + (ry);
        //        vx = (vx + rx) * decay;// + (decay * rx);
        //        vy = (vy + rx) * decay;// + (decay * ry);

        //SDL_Log("Velocity is: %f %f", vx, vy);

        // velocity should be measured per unit time?

        lastUpdate=thisUpdate;
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




    float px; // previous
    float py;
    float ix; // initial
    float iy;
    float rx; // relative (since last update)
    float ry;
    float dx; // delta not distance - difference
    float dy;
    float vx; // velocity
    float vy;

    float friction;
    //screenpixels: Float_Point


    int lastUpdate;
    bool isSecondInteraction;
    //    int mvx;// unused ?
    //    int mvy;
};

#endif
