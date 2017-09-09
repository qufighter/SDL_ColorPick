//
//  uiObject.h
//  ColorPick iOS SDL
//
//  Created by Sam Larison on 8/27/17.
//
//

#ifndef ColorPick_iOS_SDL_uiObject_h
#define ColorPick_iOS_SDL_uiObject_h


// FUNCTIONS DEFINED HERE ARE SATAIC UX:: member functions


static bool pointInRect(Float_Rect* rect, float x, float y){
    if( x > rect->x && y > rect->y ){
        if( x < rect->x + rect->w && y < rect->y + rect->h ){
            return true;
        }
    }
    return false;
}

static void setRect(Float_Rect * rect, float x, float y, float w, float h){
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;
}


static void setColor(SDL_Color * color, Uint8 r, Uint8 g, Uint8 b, Uint8 a){
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = a;
}


static void setColor(SDL_Color * color, SDL_Color * bcolor){
    color->r = bcolor->r;
    color->g = bcolor->g;
    color->b = bcolor->b;
    color->a = bcolor->a;
}



static float scaleRectForRenderX(Float_Rect * rect, Float_Rect * prect){
    return ((( ((rect->x)+  (rect->w * 0.5) ) * 2.0 ) - 1.0) ) * prect->w;
}
static float scaleRectForRenderY(Float_Rect * rect, Float_Rect * prect){
    return ((( (rect->y+  (rect->h * 0.5) ) * 2.0) - 1.0) ) * prect->h;
}




struct uiObject
{
    uiObject(){
        initialized=true;
        hasBackground=false;
        hasForeground=false;
        canCollide=false;
        doesNotCollide=false;
        hasInteraction=false;
        hasInteractionCb=false;
        canMove=false;
        isCentered=false;
        hasParentObject=false;
        parentObject=nullptr;
        hasChildren=false;
        isRoot=false;
        childListIndex = 0;
        Ux::setColor(&foregroundColor, 255, 255, 255, 255);

        containText=false;
        doesInFactRender=true;
        textureCropRect = Float_Rect();
    }
    bool isRoot;
    bool initialized;
    bool hasChildren = false;

    bool canCollide;
    bool doesNotCollide;
    bool doesInFactRender; // child objects still will be rendered
    bool containText;

    bool hasBackground;
    Float_Rect boundryRect;
    SDL_Color backgroundColor;
    SDL_Color lastBackgroundColor;// onetime use state reset
    SDL_Color foregroundColor;
    /*
     background texture ??
     background shader
     blend mode ??
     */

    Float_Rect renderRect; /* private */
    Float_Rect collisionRect; /* private */

    typedef void (*anInteractionFn)(uiObject *interactionObj, uiInteraction *delta);
    typedef void (*anAnimationPercentCallback)(float animPercent);

    // when collision occurs we must define what to do
    bool hasInteraction;
    anInteractionFn interactionFn;
    //void (*interactionFn)(uiObject *interactionObj, uiInteraction *delta);
    bool hasInteractionCb;
    anInteractionFn interactionCallback;

    bool hasAnimCb;
    anAnimationPercentCallback animationPercCallback;

    //void (*interactionCallback)(uiObject *interactionObj); // once an interaction is complete, a reference to the object may be used to determine all sorts of thigns

    void updateAnimationPercent(float perc){
        // if animation is HZ
        boundryRect.x = computedMovementRect.w * perc;
        updateRenderPosition();
    }

    void setInteraction( anInteractionFn p_interactionFn ){
        interactionFn = p_interactionFn;
        hasInteraction = true;
    }
    // interaction complete callback
    void setInteractionCallback( anInteractionFn p_interactionCallback ){
        interactionCallback = p_interactionCallback;
        hasInteractionCb = true;
    }
    void setAnimationPercCallback( anAnimationPercentCallback p_animationPercentageCallback ){
        animationPercCallback = p_animationPercentageCallback;
        hasAnimCb = true;
    }
    bool hasForeground;
    /*
     texture
     shader
     */

    bool canMove;





    // VV V V V VV    VV V V VVVV V
    Float_Rect movementBoundaryRect; // relative to parent rect, our rect should fall within parent boundary rect
    Float_Rect computedMovementRect;
    Float_Rect textureCropRect; //used for font rendering

    // /static helpers make static somewhere
    // unused helper functions!
//    float xMovePercent(){
//        return boundryRect.x / movementBoundaryRect.x;  // see interactionObj->boundryRect.x / interactionObj->computedMovementRect.w
//    }
//    float yMovePercent(){
//        return boundryRect.y / movementBoundaryRect.y;
//    }
    // also not quite valid helper functions &^ ^ ^ ^ ^

    bool isCentered; // argubaly offsets our (render+collision) rect... somehow
    // but may leave "boundary" rect intact... maybe boundary rect shold just be position point
    // maybe render+collision rect is always offset by a position
    // allowing boundary{-15,-15,30,30} to center on a given position

    // alternative is all position are offesets from boundary.x,y - position then defaults to 0 but when centered
    // position is applied to/from x,y coordinate... this simplifies collision+rendering, and extra computation only needed
    // when interacting/repositioning the object
    SDL_Point offset = {0,0};
    // position is top left corner of boundary, or offset from that some amount
    //^

    void addChild(uiObject *c){
        // so the child boundryRect, could be global space, or could be local space already....
        // c->parentObject = this; // neat?
        //childUiObjects.push_back(c);

        if( this->childListIndex < this->childListMax - 1 ){
            this->hasChildren=true;
            this->childList[this->childListIndex++] = c;
            c->parentObject = this; // neat?
            this->hasParentObject =true;
        }else{
            SDL_Log("ERROR::: Max Child UI Objects %d Exceeded !!!!!!!!", childListMax);

        }
    }

    bool hasParentObject;
    uiObject *parentObject;

    int childListIndex = 0;
    int childListMax = 128; //derp
    uiObject* childList[128]; // ui object may have a max of 8 child objects each

    // updateRenderPosition v v vv v v v vvvv vvv vvvv vvv

























    // these "private" props only used by the update loops!
    Float_Rect parentRenderRect;
    Float_Rect parentCollisionRect;


    // updateRenderPosition
    void updateRenderPosition(){

        // todo this can be optmized out of the recursion?
        if( !isRoot ){  // todo why cannot use hasParentObject  !!!!!!!!!!!!!!!!!!!!!!!
            parentRenderRect = parentObject->renderRect;
            parentCollisionRect = parentObject->collisionRect;
        }else{
            // note we s hould get here rarely if its workign right
            SDL_Log("EXPECTED ONCE AT BOOT ONLY - updateRenderPosition root element");
            parentRenderRect = Float_Rect();
            parentCollisionRect = Float_Rect();
        }


        //    renderRect.x = parentRenderRect.x + boundryRect.x;
        //    renderRect.y = parentRenderRect.y + boundryRect.y;
        renderRect.w = parentRenderRect.w * boundryRect.w;
        renderRect.h = parentRenderRect.h * boundryRect.h;

        /*
         // scaleRectForRenderX(&boundryRect) < = is this not the collision rect for x
         */

        //if( boundryRect.w < 1.0 ){
        renderRect.x = (parentRenderRect.x + scaleRectForRenderX(&boundryRect, &parentRenderRect)) ;
        // ((( (boundryRect.x+  (boundryRect.w * 0.5) ) * 2.0) - 1.0) );
        //}

        //if( boundryRect.h < 1.0 ){
        renderRect.y = parentRenderRect.y + scaleRectForRenderY(&boundryRect, &parentRenderRect);
        //}

        //    collisionRect.x = renderRect.x;
        //    collisionRect.y = renderRect.y;
        //    collisionRect.w = renderRect.w;
        //    collisionRect.h = renderRect.h;
        collisionRect.x = (parentCollisionRect.x + (boundryRect.x * parentRenderRect.w));
        collisionRect.y = (parentCollisionRect.y + (boundryRect.y * parentRenderRect.h));

        //todo optimize use aboev caluclation ??  // maths
        //    collisionRect.w = parentCollisionRect.w * boundryRect.w;
        //    collisionRect.h = parentCollisionRect.h * boundryRect.h;
        collisionRect.w = renderRect.w;
        collisionRect.h = renderRect.h; // ok optimized?



        // relative to parent rect, our rect should fall within parent boundary rect
        Ux::setRect(&computedMovementRect,
                    movementBoundaryRect.x,
                    movementBoundaryRect.y,
                    (movementBoundaryRect.w * parentRenderRect.w) - renderRect.w,
                    (movementBoundaryRect.h * parentRenderRect.h) - renderRect.h
                    );


        //    if( renderRect.w < 1.0 ){
        //        renderRect.x -= (parentRenderRect.w * 0.5) + (renderRect.w * parentRenderRect.w);
        //    }

        //    if( renderRect.w < 1.0 ){
        //        renderRect.x -= (parentRenderRect.w * 0.5) + (renderRect.w * parentRenderRect.w);
        //    }

        if( hasChildren  ){
            for( int x=0,l=childListIndex; x<l; x++ ){
                childList[x]->updateRenderPosition();
            }
        }
        
        
    }
    

    
};



#endif
