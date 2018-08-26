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

static float distance(float x1, float y1, float x2, float y2){
    return SDL_sqrtf(SDL_powf(y1-y2, 2.0) + SDL_powf(x1-x2, 2.0));
}

static bool screenCollisionRadial(float x1, float y1, Float_Rect * collisionRect){
    float xPct = (x1 - collisionRect->x) / collisionRect->w;
    float yPct = (y1 - collisionRect->y) / collisionRect->h;
    float ydiff = 0.5-xPct;
    float xdiff = 0.5-yPct;
    float distance = SDL_sqrtf(SDL_powf(ydiff, 2.0) + SDL_powf(xdiff, 2.0));
    if( distance < 0.5){
       return true;
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

static void containRectWithin(Float_Rect * rect, Float_Rect * co /*container*/){
    // modifies rect
    if( co->x > rect->x ){
        rect->w -= co->x - rect->x;
        rect->x = co->x;
    }
    if( co->y > rect->y ){
        rect->h -= co->y - rect->y;
        rect->y = co->y;
    }
    if( rect->w > co->w ){
        rect->w = co->w;
    }
    if( rect->h > co->h  ){
        rect->h = co->h;
    }
}

static void containRenderRectWithinRender(Float_Rect * rect, Float_Rect * co /*container*/){
    // modifies rect
//    float hh = rect->h * 0.5;
//    float chh = co->h * 0.5;
//
//    float yloss = (co->y - chh) - ( rect->y - hh );
//
//    if( yloss > 0 ){
//        rect->h -= yloss;// * 0.5;
//        rect->y += yloss;// * 0.5;
//    }
}

static float scaleRectForRenderX(Float_Rect * rect, Float_Rect * prect){
    return ((( ((rect->x)+  (rect->w * 0.5) ) * 2.0 ) - 1.0) ) * prect->w;
}
static float scaleRectForRenderY(Float_Rect * rect, Float_Rect * prect){
    return ((( (rect->y+  (rect->h * 0.5) ) * 2.0) - 1.0) ) * prect->h;
}


static bool setColorNotifyOfChange(SDL_Color * color, Uint8 r, Uint8 g, Uint8 b, Uint8 a){
    bool changed = color->r != r || color->g != g || color->b != b;
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = a;
    return changed;
}

static void setColor(SDL_Color * color, Uint8 r, Uint8 g, Uint8 b, Uint8 a){
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = a;
}


static void setColor(SDL_Color * color, SDL_Color * bcolor){
    SDL_memcpy(color, bcolor, sizeof(SDL_Color));
//    color->r = bcolor->r;
//    color->g = bcolor->g;
//    color->b = bcolor->b;
//    color->a = bcolor->a;
}


static bool colorEquals(SDL_Color * color, SDL_Color * bcolor){
    //return SDL_memcmp(color, bcolor, sizeof(SDL_Color)) == 0; // there is no short circut with memcmp....
    return color->r == bcolor->r && color->g == bcolor->g && color->b == bcolor->b;
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
        //canMove=false;
        hasParentObject=false;
        parentObject=nullptr;
        hasChildren=false;
        isRoot=false;
        cropParentObject=nullptr;
        hasCropParent=false;
        useCropParentOrig=false;
        calculateCropParentOrig=false;
        is_being_viewed_state=false;
        hasMovementBoundary=false;
        childListIndex = 0;
        Ux::setColor(&foregroundColor, 255, 255, 255, 255);

        containText=false;
        doesInFactRender=true;
        doesRenderChildObjects=true;
        isInBounds=true;
        testChildCollisionIgnoreBounds = false;
        textureCropRect = Float_Rect();
        roundedCornersRect = Float_Rect(0.0,0.0,0.0,0.0);
        interactionProxy=nullptr;
        myCurrentAnimation=nullptr;

        boundaryEntreredCallback=nullptr;
        shouldCeaseInteractionChecker=nullptr;
        myScrollController=nullptr; // not genric enough... remove it
        myUiController = nullptr;
        myIntegerIndex = -1;
        forceDelta=nullptr;

        matrix = glm::mat4(1.0f);
        isCentered=false;

        is_circular = false;
        stack_right = false;
        stack_left = false;

        squarify_enabled=false;
        squarify_keep_hz=false;
        squarify_keep_vt=false;
        squarify_keep_contained=false;

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

    uiScrollController *myScrollController; // child objects will inherit this I think! - we should just use myUiController though.... TODO remove this
    // GENERICS -- all UI object may have and or use these, but the purpose depends on exact context in which this is used....
    void * myUiController; // ditto!
    int myIntegerIndex;

    uiInteraction *forceDelta;

    Float_Rect boundryRect; // please call setBoundaryRect if you are going to animate the object
    Float_Rect origBoundryRect;
    Float_Rect roundedCornersRect;
    SDL_Color backgroundColor;
    SDL_Color lastBackgroundColor;// onetime use state reset
    SDL_Color foregroundColor;
    /*
     background texture ??
     background shader
     blend mode ??
     */

    Float_Rect renderRect; /* private */
    Float_Rect origRenderRect; /* private */
    Float_Rect collisionRect; /* private */

    bool is_circular;

    //Float_Rect scrollyRect; //what tpye or sort of corrindants es this?

    glm::mat4 matrix;

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

    void resetPosition(){
        Ux::setRect(&boundryRect, &origBoundryRect); // consider alternative interactionObj->setAnimation( myUxRef->uxAnimations->resetPosition(interactionObj) );
    }

    void hide(){
        doesInFactRender = false;
        doesRenderChildObjects = false;
    }
    void show(){
        doesInFactRender = true;
        doesRenderChildObjects = true;
    }

    void squarify(){
        squarify(true, false, false);
    }
    void squarifyKeepHz(){
        squarify(false, true, false);
    }
    void squarifyKeepVt(){
        squarify(false, false, true);
    }
    void squarify(bool contained, bool keep_hz, bool keep_vt){
        // pick one bool only... first bool is enough
        squarify_keep_contained=contained;
        squarify_keep_hz=keep_hz;
        squarify_keep_vt=keep_vt;
        squarify_enabled=contained||keep_hz||keep_vt;
    }

    void setMovementBoundaryRect(Float_Rect *r){
        Ux::setRect(&movementBoundaryRect, r);
        hasMovementBoundary=true;
    }

    void setMovementBoundaryRect(float x, float y, float w, float h){
        Ux::setRect(&movementBoundaryRect, x, y, w, h);
        hasMovementBoundary=true;
    }

    void setRoundedCorners(float radius){
        setRoundedCorners(radius,radius,radius,radius);
        if( radius >= 0.4999 ){
            is_circular = true;
        }
    }
    void setRoundedCorners(float tl, float tr, float br, float bl){
        Ux::setRect(&roundedCornersRect, tl, tr, br, bl);
    }

    bool collidesWithPoint(float x, float y){
        if( pointInRect(&collisionRect, x, y) ){
            if( is_circular ){
                //float dist = screenCollisionRadial(x, y, &collisionRect );
                //SDL_Log("%f radius distance %f",dist , collisionRadius);
                if(  screenCollisionRadial(x, y, &collisionRect ) ){
                    return true;
                }
                return false;
            }
            return true;
        }
        return false;
    }

    void updateAnimationPercent(float hzPerc, float vtPerc){
        if( hasMovementBoundary ){
            // we will constrain these to 100%
            if( hzPerc < 0 ) hzPerc = 0;
            else if( vtPerc < 0 ) vtPerc = 0;
            if( hzPerc > 1.0 ) hzPerc = 1.0;
            else if( vtPerc > 1.0 ) vtPerc = 1.0;

            boundryRect.x = computedMovementRect.x + (computedMovementRect.w * hzPerc);
            boundryRect.y = computedMovementRect.y + (computedMovementRect.h * vtPerc);


            updateRenderPosition();

        }
    }

    void setBoundsEnterFunction( aStateChangeFn p_boundaryEntreredCallback ){
        boundaryEntreredCallback = p_boundaryEntreredCallback;
        hasBoundaryCb = true;
    }

    void setShouldCeaseInteractionChek( anInteractionAllowedFn p_timeToCancelAnimCheckFn ){
        shouldCeaseInteractionChecker = p_timeToCancelAnimCheckFn; // when panning, this is the test(when set) to determine if the interaction should be gifted to the parent object instead.  Allows a scrollable object to be panned out of view when the scroll distance is maximized.
        hasBoundaryCb = true;
    }

    // todo what if we do not want to bubble to parent object or better yet, on different condiiton, do not bubble at all but simply change state (toggle)
    // or really just handle this in interactionCallback? which we will do for delete x


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

    bool isAnimating(){
        if( myCurrentAnimation!= nullptr ){
            return !myCurrentAnimation->chainCompleted;
        }
        return false;
    }

    bool hasForeground;
    /*
     texture
     shader
     */

    //bool canMove;





    // VV V V V VV    VV V V VVVV V
    bool hasMovementBoundary;
    Float_Rect movementBoundaryRect; // relative to parent rect, our rect should fall within parent boundary rect, call setter fn
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
    SDL_Point offset = {0,0}; // int only?
    // position is top left corner of boundary, or offset from that some amount
    //^
    bool squarify_enabled; // private
    bool squarify_keep_hz; // private
    bool squarify_keep_vt; // private
    bool squarify_keep_contained; // private


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
            if( this->myUiController != nullptr ){
                c->myUiController = this->myUiController;
            }

            if( this->hasCropParent && !c->hasCropParent ){
                c->setCropParent(this->cropParentObject);
            }

        }else{
            SDL_Log("ERROR::: Max Child UI Objects %d Exceeded !!!!!!!!", childListMax);
        }
    }

    void setCropModeOrigPosition(){
        this->useCropParentOrig=true;
        if( this->hasCropParent ){
            this->cropParentObject->calculateCropParentOrig=true;
        }
    }

    void setCropParent(uiObject *c){
        setCropParent(c, false);
    }

    void setCropParent(uiObject *c, bool forceSet){
        if( forceSet || this->hasCropParent==false ){
            this->cropParentObject = c; // ORDER MATTERS HERE!!
            this->hasCropParent = true;
        }else{
            //SDL_Log("WARN object already has crop parent");
        }
    }

    void setCropParentRecursive(uiObject *c){
        setCropParentRecursive(c, false);
    }

    void setCropParentRecursive(uiObject *c, bool forceSetAll){
        setCropParent(c, forceSetAll);
        if( hasChildren  ){
            for( int x=0,l=childListIndex; x<l; x++ ){
                childList[x]->setCropParentRecursive(c, forceSetAll);
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

    // chainables: unlike bove
    uiObject* rotate(float deg){
        this->matrix = glm::rotate(this->matrix,  deg, glm::vec3(0.0f, 0.0f, 1.0f));
        return this;
    }

    bool stack_right;
    bool stack_left;

    bool hasParentObject;
    uiObject *parentObject;

    bool hasCropParent;
    bool useCropParentOrig;
    bool calculateCropParentOrig;
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









    












    // updateRenderPosition
    void updateRenderPosition(){
        // TODO: we may pass a way to avoid updating  child nodes? (for testing if optimztion possible?)

        Float_Rect parentRenderRect;
        Float_Rect parentCollisionRect;

        if( hasParentObject ){
            parentRenderRect = parentObject->renderRect;
            parentCollisionRect = parentObject->collisionRect;
        }else{
            // note we s hould get here rarely if its workign right
            SDL_Log("EXPECTED ONCE AT BOOT ONLY - updateRenderPosition root element (reshape/resize window ok)");
            parentRenderRect = Float_Rect();
            parentCollisionRect = Float_Rect();
        }

        updateRenderPosition(parentRenderRect, parentCollisionRect);
    }

    void updateRenderPosition(Float_Rect parentRenderRect, Float_Rect parentCollisionRect){


        //    renderRect.x = parentRenderRect.x + boundryRect.x;
        //    renderRect.y = parentRenderRect.y + boundryRect.y;
        renderRect.w = parentRenderRect.w * boundryRect.w; //uniformLocations->ui_scale
        renderRect.h = parentRenderRect.h * boundryRect.h;



        //glm::mat4 uiMatrix = glm::mat4(1.0f);

        //uiMatrix = glm::scale(uiMatrix, glm::vec3(0.9,0.9,1.0));
        //uiMatrix = glm::rotate(uiMatrix,  2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
        //

        //matrix = glm::scale(uiMatrix, glm::vec3(0.998,0.998,1.0));

        //matrix = glm::scale(uiMatrix, glm::vec3(0.998,0.998,1.0));

        //    colorPickState->viewport_ratio = win_w/win_h;

        // to width
        //float ysc=(renderRect.w / renderRect.h);
        //matrix = glm::scale(uiMatrix, glm::vec3(1.0,colorPickState->viewport_ratio,1.0));


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


        // these are needed on some objects only in certain edge cases..............
        // assuming we don't animate size, this is a bit of hack to reduce maths
        if( calculateCropParentOrig ){
            origRenderRect.w = parentRenderRect.w * origBoundryRect.w;
            origRenderRect.h = parentRenderRect.h * origBoundryRect.h;
            origRenderRect.x = (parentRenderRect.x + scaleRectForRenderX(&origBoundryRect, &parentRenderRect)) ;
            origRenderRect.y = (parentRenderRect.y + scaleRectForRenderY(&origBoundryRect, &parentRenderRect)) ;


            if( hasCropParent ){
                //                Float_Rect tempRect;//leak?
                //                setRect(&tempRect, &origBoundryRect);
                //                containRectWithin(&tempRect, &cropParentObject->renderRect);
                containRenderRectWithinRender(&origRenderRect, &cropParentObject->renderRect);
            }
        }

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


        if( squarify_enabled ){
            if( squarify_keep_contained ){

                float parent_ratio  = (parentRenderRect.w  * colorPickState->viewport_ratio) / parentRenderRect.h;
                float item_ratio = renderRect.w / renderRect.h;

                if( parent_ratio < 1.0 ){
                    // taller than wide, squarify_keep_hz

                    renderRect.h *= item_ratio; //squarify itself (this should now match the viewport ratio, so still non square)
                    renderRect.h *= colorPickState->viewport_ratio;

                    float diff = collisionRect.h - renderRect.h;
                    collisionRect.h = renderRect.h;
                    collisionRect.y += (diff * 0.5);

                }else{// parent wider than tall
                      // we should match parent height
                      // essentially squarify_keep_vt

                    renderRect.w /= item_ratio; //squarify itself (this should now match the viewport ratio, so still non square)
                    renderRect.w /= colorPickState->viewport_ratio;

                    float diff = collisionRect.w - renderRect.w;
                    collisionRect.w = renderRect.w;
                    collisionRect.x += (diff * 0.5);

                }

            //if( isDebugObject )
//                SDL_Log("keeping it real "\
//                        "\n\t parent_ratio %f, parentRenderRect.w %f, parentRenderRect.h %f"\
//                        "\n\t item_ratio %f ,renderRect.w %f, renderRect.h %f"\
//                        "\n\t colorPickState->viewport_ratio %f",
//                        parent_ratio, parentRenderRect.w, parentRenderRect.h,
//                        item_ratio,renderRect.w, renderRect.h,
//                        colorPickState->viewport_ratio);

            }else if( squarify_keep_hz ){

                float item_ratio = renderRect.w / renderRect.h;

                renderRect.h *= item_ratio; //squarify itself (this should now match the viewport ratio, so still non square)

                renderRect.h *= colorPickState->viewport_ratio;

                float diff = collisionRect.h - renderRect.h;

                collisionRect.h = renderRect.h;
                collisionRect.y += (diff * 0.5);


            }else if ( squarify_keep_vt ){

                float item_ratio = renderRect.w / renderRect.h;

                renderRect.w /= item_ratio; //squarify itself (this should now match the viewport ratio, so still non square)

                renderRect.w /= colorPickState->viewport_ratio;

                float diff = collisionRect.w - renderRect.w;

                collisionRect.w = renderRect.w;
                collisionRect.x += (diff * 0.5);

            }
        }


        if( hasMovementBoundary ){ //seems here to set if not intialized

            // relative to parent rect, our rect should fall within parent boundary rect
            Ux::setRect(&computedMovementRect,
                        movementBoundaryRect.x,
                        movementBoundaryRect.y,
                        (movementBoundaryRect.w /*- movementBoundaryRect.x*/ /** parentRenderRect.w*/) - boundryRect.w,
                        (movementBoundaryRect.h /*- movementBoundaryRect.y*/ /** parentRenderRect.h*/) - boundryRect.h
                        );

        }

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
            float stackingOffset=0;
            // we could thread this???
            for( int x=0,l=childListIndex; x<l; x++ ){
                childList[x]->updateRenderPosition(renderRect, collisionRect);
                // for this child list... we might slowly stack these elements?
                stackingOffset = childList[x]->applyStacking(&renderRect, stackingOffset);
            }
        }
        
        
    }

    float applyStacking(Float_Rect* parentRenderRect, float stackingOffset){

        if( stack_right ){


            float newPosition = parentRenderRect->x + parentRenderRect->w - renderRect.w - stackingOffset;

            float displacement = newPosition - renderRect.x;
            renderRect.x += displacement;

            collisionRect.x  += (displacement * 0.5);

        }else if(stack_left){

        }

        return stackingOffset + renderRect.w;
    }
    

    
};



#endif
