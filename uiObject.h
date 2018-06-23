//
//  uiObject.h /// really this should be uiObject.h.inc  inc.h ?  its a partial??
// struct MyStruct ; /* Forward declaration */ - maybe we can use this?
//  ColorPick iOS SDL
//
//  Created by Sam Larison on 8/27/17.
//
//

#ifndef ColorPick_iOS_SDL_uiObject_h
#define ColorPick_iOS_SDL_uiObject_h

#define SCROLLY_WIDTH 0.1


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

static void setRect(Float_Rect * rect, Float_Rect * rect2){
    rect->x = rect2->x;
    rect->y = rect2->y;
    rect->w = rect2->w;
    rect->h = rect2->h;
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




/* Forward declarations */
struct uiAnimation; /* see ux-anim.h for full definition */
struct uiAminChain; /* see ux-anim.h for full definition */
struct uiScrollController;
struct uiObject;


// typedef used scross all these! possibly move to ux.h if we can move hte fwd decl too
typedef void (*animationUpdateCallbackFn)(uiAnimation* self, Float_Rect *newBoundaryRect);
typedef void (*animationCallbackFn)(uiAnimation* self);
typedef Float_Rect* (*animationGetRectFn)(uiAnimation* self);

// these types uses exclusively by uiObject
typedef bool (*anInteractionAllowedFn)(uiObject *interactionObj, uiInteraction *delta); // a function that return true if the interaction did pass to a valid interactionObject (aka the results of didBubble if releasing the interaction to another object)
typedef void (*anInteractionFn)(uiObject *interactionObj, uiInteraction *delta);
typedef void (*anAnimationPercentCallback)(uiObject *interactionObj, float animPercent);
typedef void (*aStateChangeFn)(uiObject *interactionObj);


// move to own file, but requires the above....

struct uiObject
{
    uiObject(){
        isDebugObject=false;// used to print messge for certain object
        initialized=true;
        hasBackground=false;
        hasForeground=false;
        canCollide=false; // with cursor
        doesNotCollide=false;
        hasInteraction=false;
        hasInteractionCb=false;
        canMove=false;
        isCentered=false;
        hasParentObject=false;
        parentObject=nullptr;
        hasChildren=false;
        isRoot=false;
        cropParentObject=nullptr;
        hasCropParent=false;
        is_being_viewed_state=false;
        childListIndex = 0;
        Ux::setColor(&foregroundColor, 255, 255, 255, 255);

        containText=false;
        doesInFactRender=true;
        doesRenderChildObjects=true;
        isInBounds=true;
        testChildCollisionIgnoreBounds = false;
        textureCropRect = Float_Rect();
        interactionProxy=nullptr;
        myCurrentAnimation=nullptr;

        boundaryEntreredCallback=nullptr;
        shouldCeaseInteractionChecker=nullptr;
        myScrollController=nullptr;
    }
    bool isDebugObject;
    bool isRoot;
    bool initialized;
    bool hasChildren = false;

    bool canCollide; // with cursor
    bool doesNotCollide;
    bool doesInFactRender; // child objects still will be rendered, some objects are just containers and need not render anything
    bool doesRenderChildObjects;
    bool isInBounds; // equivilent to needs render
    bool containText;
    bool testChildCollisionIgnoreBounds = false;

    bool is_being_viewed_state = false; // some object interactions need to track the state of the toggle into/outof view

    bool hasBackground;
    uiScrollController *myScrollController; // child objects will inherit this I think!
    Float_Rect boundryRect; // please call setBoundaryRect if you are going to animate the object
    Float_Rect origBoundryRect;
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

    Float_Rect scrollyRect; //what tpye or sort of corrindants es this?


    uiObject *interactionProxy; // if our interaction is suppose to effect a different object
    uiAminChain* myCurrentAnimation;

    // when collision occurs we must define what to do
    bool hasInteraction;
    anInteractionFn interactionFn;
    //void (*interactionFn)(uiObject *interactionObj, uiInteraction *delta);
    bool hasInteractionCb;
    anInteractionFn interactionCallback;

    bool hasAnimCb;
    anAnimationPercentCallback animationPercCallback;

    bool hasBoundaryCb; // not stricly needed if we use the nullptr test
    aStateChangeFn boundaryEntreredCallback;
    anInteractionAllowedFn shouldCeaseInteractionChecker;
    //void (*interactionCallback)(uiObject *interactionObj); // once an interaction is complete, a reference to the object may be used to determine all sorts of thigns

    void setModeWhereChildCanCollideAndOwnBoundsIgnored(){
        testChildCollisionIgnoreBounds = true;
        canCollide = false;
    }

    void setBoundaryRect(Float_Rect *r){
        Ux::setRect(&boundryRect, r);
        Ux::setRect(&origBoundryRect, r);
    }

    void setBoundaryRect(float x, float y, float w, float h){
        Ux::setRect(&boundryRect, x, y, w, h);
        Ux::setRect(&origBoundryRect, x, y, w, h);
    }

    void updateAnimationPercent(float hzPerc, float vtPerc){
        // we will constrain these to 100%
        if( hzPerc < 0 ) hzPerc = 0;
        else if( vtPerc < 0 ) vtPerc = 0;
        if( hzPerc > 1.0 ) hzPerc = 1.0;
        else if( vtPerc > 1.0 ) vtPerc = 1.0;
        boundryRect.x = computedMovementRect.x + (computedMovementRect.w * hzPerc);
        boundryRect.y = computedMovementRect.y + (computedMovementRect.h * vtPerc);
        updateRenderPosition();
    }

    void setBoundsEnterFunction( aStateChangeFn p_boundaryEntreredCallback ){
        boundaryEntreredCallback = p_boundaryEntreredCallback;
        hasBoundaryCb = true;
    }

    void setShouldCeaseInteractionChek( anInteractionAllowedFn p_timeToCancelAnimCheckFn ){
        shouldCeaseInteractionChecker = p_timeToCancelAnimCheckFn; // when panning, this is the test(when set) to determine if the interaction should be gifted to the parent object instead.  Allows a scrollable object to be panned out of view when the scroll distance is maximized.
        hasBoundaryCb = true;
    }

    void setInteraction( anInteractionFn p_interactionFn ){
        this->canCollide = true; // so for click functions we are just setting this automatically nnow!
        interactionFn = p_interactionFn;
        hasInteraction = true;
    }
    // interaction complete callback aka click
    void setInteractionCallback( anInteractionFn p_interactionCallback ){
        this->canCollide = true; // so for click functions we are just setting this automatically nnow!
        interactionCallback = p_interactionCallback;
        hasInteractionCb = true;
    }
    void setAnimationPercCallback( anAnimationPercentCallback p_animationPercentageCallback ){
        animationPercCallback = p_animationPercentageCallback;
        hasAnimCb = true;
    }

    void cancelCurrentAnimation(){
        if( myCurrentAnimation!= nullptr ){
            myCurrentAnimation->endAnimation(); // thread safe, and will probably garbage collect :)
            myCurrentAnimation = nullptr;
        }
    }

    void setAnimation(uiAminChain* chain){
        cancelCurrentAnimation(); // why not! ? extra null check?/ pffft
        myCurrentAnimation = chain->preserveReference(); // do not auto garbage collect!
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



        // so if we dynamically alloc some random child ui objects and they are
        // added to a X/ Y that is outside of boundary rect we wont' know now
        // but when we compute position of child objects perhasp they can exceed parent
     //  bounds and throw up the data to the parent about the sscroll rect and in this
  //  case make some nifty barz to click and drag like old school scrolling shouuld work
   //     then invert it for teh background drag for now until we find a way to compute the
    //        step size for a true pixel within teh bounds

        // so each time we recompute the boundary rect and the child object has completed recomputing positions that exceed 1.0 x/y ........

        // the computation would only apply when objects change positions on the screen



        if( this->childListIndex < this->childListMax - 1 ){
            this->childList[this->childListIndex++] = c;
            this->hasChildren=true;
            c->parentObject = this; // neat?
            c->hasParentObject =true;
            if( this->myScrollController != nullptr ){
                c->myScrollController = this->myScrollController;
            }

            if( this->hasCropParent ){
                c->setCropParent(this->cropParentObject);
            }

        }else{
            SDL_Log("ERROR::: Max Child UI Objects %d Exceeded !!!!!!!!", childListMax);
        }
    }

    void setCropParent(uiObject *c){
        this->cropParentObject = c; // ORDER MATTERS HERE!!
        this->hasCropParent = true;


    }

    void setCropParentRecursive(uiObject *c){
        setCropParent(c);
        if( hasChildren  ){
            for( int x=0,l=childListIndex; x<l; x++ ){
                childList[x]->setCropParentRecursive(c);
            }
        }
    }

    uiObject* getChildOrNullPtr(int index){
        if( index > -1 && index < childListIndex ){
            return childList[index];
        }
        return nullptr;
    }

    uiObject* bubbleInteraction(){
        // seek interactable parent objects, return nullptr if none found

        uiObject* foundObj = this;

        while(foundObj->hasParentObject ){
            foundObj = foundObj->parentObject;
            if( foundObj->hasInteraction ){
                return foundObj;
            }
        }

        return nullptr;
    }


    bool hasParentObject;
    uiObject *parentObject;

    bool hasCropParent;
    uiObject *cropParentObject;

    int childListIndex = 0;
    int childListMax = 128; //derp
    uiObject* childList[128]; // ui object may have a max of 128 child objects each

    int getChildCount(){
        return childListIndex;
    }

    //uiObject *parentScrollObjectObject; /// gthinking about this if our x/y is outside of 0-1.0 range then we trigger a scrolly behavior
    // on our barnt scroll object whatever level up from here that might be
    // yet we wwouln't want to get noisy about it and have every color between here and infinity
    // notify the scroll bar to recompute its total height, no
    /// really only if the bounds exceed
    // but for our curent scrolled view
    // what we need to render and how it impacts hte bounds - no way thats not enough to know the total scroll height
    /// so as we render we may constantly expand the
    // parentScrollObjectObject -> scrollyRect property (brand new)

    /// scrolly rect should maybe be a large -x,y position to indicate the amount of scroll possible
    // while the w/h could be teh current offset position?
    // or just reverse this
    // w/h are the amount of scroll possible (could be negative or positive really? what simpler...) thinking negative quietly.
    // x/y are the current offset so we can always get back to the origional renderRect x/y which become transformed by this amount
    // what coulbe simplerly




    // updateRenderPosition v v vv v v v vvvv vvv vvvv vvv









    









    // these "private" props only used by the update loops!
    Float_Rect parentRenderRect;
    Float_Rect parentCollisionRect;



    // updateRenderPosition
    void updateRenderPosition(){
        // TODO: we may pass a way to avoid updating  child nodes? (for testing if optimztion possible?)


        // todo this can be optmized out of the recursion?
        if( hasParentObject ){  // todo why cannot use ( hasParentObject )  !!!!!!!!!!!!!!!!!!!!!!!
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

        bool wasOob = false;
        if( hasBoundaryCb && !isInBounds ){
            wasOob = true;
        }

        isInBounds = false;
        if( collisionRect.x < 1.0f && collisionRect.x + collisionRect.w > 0.0f ){
            if( collisionRect.y < 1.0f && collisionRect.y + collisionRect.h > 0.0f ){
                isInBounds = true;
                if( wasOob && boundaryEntreredCallback != nullptr ){
                    boundaryEntreredCallback(this);
                }
            }
        }

        // relative to parent rect, our rect should fall within parent boundary rect
        Ux::setRect(&computedMovementRect,
                    movementBoundaryRect.x,
                    movementBoundaryRect.y,
                    (movementBoundaryRect.w /*- movementBoundaryRect.x*/ /** parentRenderRect.w*/) - boundryRect.w,
                    (movementBoundaryRect.h /*- movementBoundaryRect.y*/ /** parentRenderRect.h*/) - boundryRect.h
                    );


        //    if( renderRect.w < 1.0 ){
        //        renderRect.x -= (parentRenderRect.w * 0.5) + (renderRect.w * parentRenderRect.w);
        //    }

        //    if( renderRect.w < 1.0 ){
        //        renderRect.x -= (parentRenderRect.w * 0.5) + (renderRect.w * parentRenderRect.w);
        //    }




        // an optional "enforceMovementBoundary" could be useful - since animations could move things outside of this easily....
        // most animations will reset themselves though....
        // in any case the logic before delta->fixX may work... I guess if hte animation may continue it could also need to fixX ?  may animate unrealistically then....


        if( hasChildren  ){
            for( int x=0,l=childListIndex; x<l; x++ ){
                childList[x]->updateRenderPosition();
            }
        }
        
        
    }
    

    
};



#endif
