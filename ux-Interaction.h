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
        lastUpdate=SDL_GetTicks();
        //        mvx=0;
        //        mvy=0;
    }
    void update(float x, float y, float pdx, float pdy){ // todo pass delta and relative
        // maybe we should scale the coordinates to screen here instead....
        // TODO pass time in?

        int thisUpdate = SDL_GetTicks();
        int elapsed = thisUpdate - lastUpdate;
        if( elapsed < 1 ) elapsed = 1;

        rx = x - px;
        ry = y - py;

        px=x;
        py=y;

        // decay per millisecond... 1.0/1000 = .001 decelleration rate per second
        // except now 0.5/1000 = 0.0005
        float decay = (1.3 / elapsed); // and then apply the last velocity diff, which may be zero...
        vx = (vx * decay) + (decay * rx);
        vy = (vy * decay) + (decay * ry);
        //        vx = (vx + rx) * decay;// + (decay * rx);
        //        vy = (vy + rx) * decay;// + (decay * ry);

        //SDL_Log("Velocity is: %f %f", vx, vy);

        dx = px - ix;
        dy = py - iy; // when this is greater than 0 we have moved down

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

    //screenpixels: Float_Point


    int lastUpdate;
    //    int mvx;// unused ?
    //    int mvy;
};

#endif
