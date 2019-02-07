//
//  uiObject.h /// really this should be uiObject.h.inc  inc.h ?  its a partial??
// struct MyStruct ; /* Forward declaration */ - maybe we can use this?
//  ColorPick SDL
//
//  Created by Sam Larison on 8/27/17.
//
//

// TODO rename uxObject
#ifndef ColorPick_iOS_SDL_uiObject_h
#define ColorPick_iOS_SDL_uiObject_h

#define SCROLLY_WIDTH 0.1
#define SCROLLY_MISSING_WIDTH 0.01

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

// ux member...
static Float_Rect unitRect;

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

//typedef struct ColorListState
//{
//    ColorListState(){
//        is_delete_state = false;
//    }
//    bool is_delete_state = false;
//} ColorListState;


typedef struct ColorList // cannot really use this since we do not persist ths to disk
{
    ColorList(){
        is_delete_state = false;
    }
    ColorList(SDL_Color icolor){
        color = icolor;
        is_delete_state = false;
    }
    SDL_Color color;
    bool is_delete_state;
} ColorList;

static bool setColorNotifyOfChange(SDL_Color * color, SDL_Color * bcolor){
    return setColorNotifyOfChange(color, bcolor->r, bcolor->g, bcolor->b, bcolor->a);
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


// thjis is probably a bad idea
//static void containRectWithinObjects(Float_Rect * rect,  uiObject* startObj, uiObject* endParentObj){
//
//        // asserrt startObj != endParentObj
//
//    uiObject* curObj = startObj;
//    uiObject* parentObj = curObj->parentObject;
//
//    float scale_applicator_y = 1.0;
//    float scale_applicator_x = 1.0;
//
//    while( parentObj != endParentObj ){
//
//        // when parentObj has an OOB render rect (negative position and or exceeding 1.0 when account for position)
//        // we will in that condition adjust rect accordingly (equal oposite) and continue
//
//        scale_applicator_y /= curObj->renderRect.h / parentObj->renderRect.h;
//
//        if( curObj->renderRect.y < 0.0 ){
//            //rect->y += -parentObj->renderRect.y;
//            // parent leaves its parent obj
//            // what size of its parent object is parentObj (maths)
//
//
//           // rect->y += -parentObj->renderRect.y
////
////            parentObj->renderRect.h
////
////            curObj->renderRect.h; // this is our height percentage
//
//
//
//        }
//
//
//            // this method containRectWithin is really teh wrong treatment for this... we really only care to exclude negative values or values that exceeede the container rect height
////        containRectWithin(rect, &curObj->renderRect);
//
//        curObj = parentObj;
//        uiObject* parentObj = curObj->parentObject;
//    }
//
//    // so we should once more contain in the current object now...
//    //containRectWithin(rect, &curObj->renderRect);
//
//}


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
        interactionCallbackTypeClick=false;
        //canMove=false;
        hasParentObject=false;
        parentObject=nullptr;
        hasChildren=false;
        isRoot=false;
        cropParentObject=nullptr;
        modalParent=nullptr;
        modalDismissal=nullptr;
        hasCropParent=false;
        useCropParentOrig=false;
        calculateCropParentOrig=false;
        is_being_viewed_state=true;
        default_viewed_state = true;
        hasMovementBoundary=false;
        childListIndex = 0;
        Ux::setColor(&foregroundColor, 255, 255, 255, 255);

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
        interactionBeginCallback=nullptr;
        interactionWheelCallback=nullptr;
        myScrollController=nullptr; // not genric enough... remove it
        myUiController = nullptr;
        myIntegerIndex = -1;
        forceDelta=nullptr;

        matrix = glm::mat4(1.0f);
        matrixInheritedByDescendants = false;
        isCentered=false;

        is_circular = false;
        noStacking();

        squarify_enabled=false;
        squarify_keep_hz=false;
        squarify_keep_vt=false;
        squarify_keep_contained=false;

        isTextParentOfTextLength=0;
        containText=false;
        containTextPadding=0.0;
        textPadding = glm::vec2(0.0, 0.0);

        //textSpacing=0.0;
        textDirection=TEXT_DIR_ENUM::NO_TEXT; // this is really child node direction... we can print the child nodes in some pre-defined ways
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
    int isTextParentOfTextLength;
    bool containText; // this is merely a font rendering setting.... not containsText, which is textDirection==TEXT_DIR_ENUM::NO_TEXT
    float containTextPadding;

    glm::vec2 textPadding;

    //float textSpacing; // don't use this, just matrix scale the parent object of the text... or fix this...
    uint8_t textDirection;
    bool testChildCollisionIgnoreBounds = false;

    bool is_being_viewed_state = true; // some object interactions need to track the state of the toggle into/outof view
    bool default_viewed_state = true; // by default this object is either vis or hid, origBoundaryRect is which?

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

//    Float_Rect croppedBoundryRect; /* private */
//    Float_Rect croppedOrigBoundryRect; /* private */
    Float_Rect renderRect; /* private */
    Float_Rect collisionRect; /* private */
    Float_Rect origRenderRect; /* private */ // really only u used for non standard crop parent object
//    Float_Rect computedCropRenderRect; /* private */

    bool is_circular;

    //Float_Rect scrollyRect; //what tpye or sort of corrindants es this?

    glm::mat4 matrix;
    bool matrixInheritedByDescendants;

    uiObject *interactionProxy; // if our interaction is suppose to effect a different object
    uiAminChain* myCurrentAnimation;

    // when collision occurs we must define what to do
    bool hasInteraction;
    anInteractionFn interactionFn;
    //void (*interactionFn)(uiObject *interactionObj, uiInteraction *delta);
    bool hasInteractionCb;
    bool interactionCallbackTypeClick;
    anInteractionFn interactionCallback;

    anInteractionFn interactionBeginCallback; // effectively mousedown
    anInteractionFn interactionWheelCallback;

    bool hasAnimCb;
    anAnimationPercentCallback animationPercCallback;

    bool hasBoundaryCb; // not stricly needed if we use the nullptr test
    aStateChangeFn boundaryEntreredCallback;
    anInteractionAllowedFn shouldCeaseInteractionChecker;
    //void (*interactionCallback)(uiObject *interactionObj); // once an interaction is complete, a reference to the object may be used to determine all sorts of thigns

    void setDefaultHidden(){
        default_viewed_state = false;
        is_being_viewed_state = false;
    }

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

    void setBoundaryRectForAnimState(float x, float y, float w, float h, float xH, float yH, float wH, float hH){
        if( is_being_viewed_state ){
            Ux::setRect(&boundryRect, x, y, w, h);
        }else{
            Ux::setRect(&boundryRect, xH, yH, wH, hH);
        }
        if( default_viewed_state ){
            Ux::setRect(&origBoundryRect, x, y, w, h);
        }else{
            Ux::setRect(&origBoundryRect, xH, yH, wH, hH);
        }
    }

    void setBoundaryRectForAnimState(Float_Rect *r, Float_Rect *rH){
        if( is_being_viewed_state ){
            Ux::setRect(&boundryRect, r);
        }else{
            Ux::setRect(&boundryRect, rH);
        }
        if( default_viewed_state ){
            Ux::setRect(&origBoundryRect, r);
        }else{
            Ux::setRect(&origBoundryRect, rH);
        }
    }

    void resetPosition(){
        Ux::setRect(&boundryRect, &origBoundryRect); // consider alternative interactionObj->setAnimation( myUxRef->uxAnimations->resetPosition(interactionObj) );
    }

    void setChildNodeDirection(uint8_t textDir){ // see TEXT_DIR_ENUM
        setChildNodeDirection(textDir, false);
    }

    void setChildNodeDirection(uint8_t textDir, bool containChildNodes){
        containText = containChildNodes;
        textDirection = textDir;
        // it is arguable we shoudl auto organize the child nodes right now (rather than during update)
        organizeChildNodesBasedOnTextDir();
    }

    void allowInteraction(){
        doesNotCollide = false;
    }

    void hideAndNoInteraction(){
        hide();
        doesNotCollide = true;
    }

    void showAndAllowInteraction(){
        show();
        doesNotCollide = false;
    }

    void hide(){
        doesInFactRender = false;
        doesRenderChildObjects = false;
    }
    void show(){
        doesInFactRender = true;
        doesRenderChildObjects = true;
    }

    void squarifyChildren(){
        if( hasChildren  ){
            for( int x=0,l=childListIndex; x<l; x++ ){
                childList[x]->squarify();
            }
        }
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

    void setShouldCeaseInteractionChek( anInteractionAllowedFn p_timeToCancelAnimCheckFn ){ // bubble fn
        shouldCeaseInteractionChecker = p_timeToCancelAnimCheckFn; // when panning, this is the test(when set) to determine if the interaction should be gifted to the parent object instead.  Allows a scrollable object to be panned out of view when the scroll distance is maximized.
        hasBoundaryCb = true;
    }

    // todo what if we do not want to bubble to parent object or better yet, on different condiiton, do not bubble at all but simply change state (toggle)
    // or really just handle this in interactionCallback? which we will do for delete x

    void setInteractionBegin( anInteractionFn p_interactionBegin ){
        this->canCollide = true; // so for click functions we are just setting this automatically nnow!
        interactionBeginCallback = p_interactionBegin;
    }

    void setInteractionWheel( anInteractionFn p_interactionWheel ){
        this->canCollide = true; // so for click functions we are just setting this automatically nnow!
        interactionWheelCallback = p_interactionWheel;
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

    // "click" is a little better in that it has to be a mouse up on teh same element that was moused down
    void setClickInteractionCallback( anInteractionFn p_interactionCallback ){
        this->canCollide = true; // so for click functions we are just setting this automatically nnow!
        setInteractionCallback(p_interactionCallback);
        interactionCallbackTypeClick = true;
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

//    bool isNotAnimating(){ // check obj itself and parent objects to see if we aren't animating...
//        bool hasAnim = false;
////        if( myCurrentAnimation!= nullptr ){
////            hasAnim = !myCurrentAnimation->chainCompleted;
////        }
//        if( !hasAnim ){
//            if( hasParentObject ){
//                return parentObject->isNotAnimating();
//            }else{
//                return hasAnim; // false
//            }
//        }else{
//            return hasAnim; // true
//        }
//    }
//
//    bool isInAnimation(){
//        return !isNotAnimating();
//    }

    bool isAnimating(){ // maybe this can take a hit from isInHiddenState and also check the parent objects... if any parent is animating we are animating... see isNotAnimating which does this...
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


    void addChildStackedRight(uiObject *c){
        c->stackRight();
        addChild(c);
    }

    bool hasStacking(){
        if(stack_right||stack_bottom||stack_left) return true;
        return false;
    }

    void noStacking(){
        stack_right=false;
        stack_bottom=false;
        stack_left=false;
    }

    void stackRight(){
        noStacking();
        stack_right=true;
    }

    void stackBottom(){
        noStacking();
        stack_bottom=true;
    }

    void empty(){
        // lazy empty...
        for( int x=0,l=childListIndex; x<l; x++ ){
            childList[x]->hasParentObject=false;
            childList[x]->hasCropParent = false;
            childList[x]->parentObject=nullptr;
        }
        hasChildren=false;
        childListIndex = 0;
    }

    void addBottomChild(uiObject *c){
        if( this->childListIndex < this->childListMax - 1 ){
            //SDL_Log("Before %i", this->childListIndex);
            // shift all child obj by one
            if( this->childListIndex > 0 ){
                SDL_memmove(&childList[1], childList, sizeof(uiObject*) * this->childListIndex);
            }
            this->childListIndex++;
            _addChildAtIndex(0, c);
            //SDL_Log("After %i", this->childListIndex);
        }else{
            SDL_Log("ERROR::: Max Child UI Objects %d Exceeded !!!!!!!! addBottomChild", childListMax);
        }
    }

    void addChild(uiObject *c){
        // so the child boundryRect, could be global space, or could be local space already....
        // c->parentObject = this; // neat?
        //childUiObjects.push_back(c);


        if( this->childListIndex < this->childListMax - 1 ){
            _addChildAtIndex(this->childListIndex++, c);

        }else{
            SDL_Log("ERROR::: Max Child UI Objects %d Exceeded !!!!!!!!", childListMax);
        }
    }

    void _addChildAtIndex(int indexToUse, uiObject *c){
        // internal - unsafe to call function
        //if( this->childListIndex < this->childListMax - 1 ){
        this->childList[indexToUse] = c;
        this->hasChildren=true;
        c->parentObject = this; // neat?
        c->hasParentObject =true;
        c->matrixInheritedByDescendants = this->matrixInheritedByDescendants;
        if( this->myScrollController != nullptr ){
            c->myScrollController = this->myScrollController;
        }
        if( this->myUiController != nullptr ){
            c->myUiController = this->myUiController;
        }

        if( this->hasCropParent && !c->hasCropParent ){
            c->setCropParent(this->cropParentObject);
        }
//        }else{
//            SDL_Log("ERROR::: Max Child UI Objects %d Exceeded !!!!!!!!", childListMax);
//        }
    }

    // the crop is the origional position of teh crop parent, not the current position...
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

    uiObject* scale(float scaler){
        this->matrix = glm::scale(this->matrix,  glm::vec3(scaler,scaler,1.0));
        return this;
    }

    uiObject* identity(){ // reset matrix
        this->matrix = glm::mat4(1.0f);
        return this;
    }


    bool stack_right;
    bool stack_bottom;
    bool stack_left;

    bool hasParentObject;
    uiObject *parentObject;

    uiObject* findWheelOrPinchInteractionObject(){
        if( interactionWheelCallback != nullptr ) return this;
        if( hasParentObject ){
            return parentObject->findWheelOrPinchInteractionObject();
        }
        return nullptr;
    }

    bool isInHiddenState(){ // check obj itself and parent objects to see if we are hidden...
        if( !is_being_viewed_state ) return true;
        if( hasParentObject ){
            return parentObject->isInHiddenState();
        }
        return false;
    }

    bool hasCropParent;
    bool useCropParentOrig;
    bool calculateCropParentOrig;
    uiObject *cropParentObject;

    int childListIndex = 0;
    const static int childListMax = 128;
    uiObject* childList[childListMax]; // ui object may have a max of 128 child objects each

    uiObject* modalParent; // not all objecrts use this but all objects that become modals should specify their modal parent, even if its only rootUiObject
    anInteractionFn modalDismissal;

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
    void updateRenderPosition(){ // this is an entry point only DO NOT recurs into this variant...
        // TODO: we may pass a way to avoid updating  child nodes? (for testing if optimztion possible?)

        Float_Rect parentRenderRect;
        Float_Rect parentCollisionRect;

        if( hasParentObject ){
            parentRenderRect = parentObject->renderRect;
            parentCollisionRect = parentObject->collisionRect;
            if( hasStacking() ){
                // since we are stacking we must update the parent object instead
                parentObject->updateRenderPosition();
                return;
            }
        }else{
            // note we s hould get here rarely if its workign right
            if( isRoot ){
                SDL_Log("Update Render Position - Root element!");
            }else{
                // updating an object that has no parent (not in hirearchy, its a root object of its own sort)
            }
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


        //
//        setRect(&croppedBoundryRect, &boundryRect);
//        containRectWithin(&croppedBoundryRect, new Float_Rect()); // leaking memory
//
//        setRect(&croppedOrigBoundryRect, &origBoundryRect);
//        containRectWithin(&croppedOrigBoundryRect, new Float_Rect());


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

//            origRenderRect.w = parentRenderRect.w * croppedOrigBoundryRect.w;
//            origRenderRect.h = parentRenderRect.h * croppedOrigBoundryRect.h;
//            origRenderRect.x = (parentRenderRect.x + scaleRectForRenderX(&croppedOrigBoundryRect, &parentRenderRect)) ;
//            origRenderRect.y = (parentRenderRect.y + scaleRectForRenderY(&croppedOrigBoundryRect, &parentRenderRect)) ;

//
//            if( hasCropParent ){
//
//                if( cropParentObject->hasCropParent ){
//
//
//                    //containRenderRectWithinRender(&origRenderRect, &cropParentObject->renderRect);
//                }
//
//
//                //                Float_Rect tempRect;//leak?
//                //                setRect(&tempRect, &origBoundryRect);
//                //                containRectWithin(&tempRect, &cropParentObject->renderRect);
//               //containRenderRectWithinRender(&origRenderRect, &cropParentObject->renderRect);
//            }
        }

//        if( useCropParentOrig && hasCropParent ){
//            // this means we are child of a tile, probably and we should really crop 2x
//
//            /// maybe we can just use depth buffer for this ****
//            if( cropParentObject->hasCropParent ){
//                // its exclusively cropParentObject->useCropParentOrig
//
//                uiObject* scrollerItself = cropParentObject->cropParentObject; // aka parent crop parent
//                uiObject* tileItself = cropParentObject; // aka nearest crop parent
//                Float_Rect rect; //
//                Float_Rect prect; //
//
//
//                setRect(&rect, &tileItself->origBoundryRect);
//
//
//                setRect(&prect, &tileItself->parentObject->collisionRect); // we need to compute origional collision rect??
//                containRectWithin(&prect, &cropParentObject->cropParentObject->collisionRect);
//
//                if( prect.y != tileItself->parentObject->collisionRect.y ){
//                    SDL_Log("Some strange string here so we can debug it");
//                }
//
//                //containRectWithin(&rect, &tileItself->boundryRect); // maybe we could use something like this... but it has to take 2 uiObjects instead, and walk whatever tree exists between them... containing rects within along hte way... doable probably
//
//                // containRectWithinObjects(&rect, &startObject, &parentObject)
////                containRectWithinObjects(&rect, tileItself, scrollerItself);
//
//                // contain containRectWithinObjects,a nd really within scrollerItself->boundryRect ultimately while examining each object...
//
//                // so use parent of scroller?? or.... this is not so trival is it??
////
////                computedCropRenderRect.w = parentRenderRect.w * origBoundryRect.w;
////                computedCropRenderRect.h = parentRenderRect.h * origBoundryRect.h;
////                computedCropRenderRect.x = (parentRenderRect.x + scaleRectForRenderX(&origBoundryRect, &parentRenderRect)) ;
////                computedCropRenderRect.y = (parentRenderRect.y + scaleRectForRenderY(&origBoundryRect, &parentRenderRect)) ;
//
//
//
//
//                computedCropRenderRect.w = tileItself->parentObject->renderRect.w * rect.w;
//                computedCropRenderRect.h = tileItself->parentObject->renderRect.h * rect.h;
//                computedCropRenderRect.x = (tileItself->parentObject->renderRect.x + scaleRectForRenderX(&rect, &tileItself->parentObject->renderRect)) ;
//                computedCropRenderRect.y = (tileItself->parentObject->renderRect.y + scaleRectForRenderY(&rect, &tileItself->parentObject->renderRect)) ;
//                // aka  cropParentObject->origRenderRect
//
//                 // vs cropParentObject->cropParentObject->renderRect
//
//// better off looking at the crop parent's render rect that we are.... but thats tough.... collision rect is easy-ish
//
//
//
//                if( prect.y != tileItself->parentObject->collisionRect.y ){
//                    SDL_Log("Some strange string here so we can debug it");
//                }
//
//
//
//
//                // we can presumably use tileItself->origBoundryRect and tileItself->parentObject->renderRect
//                // (the FIRST of which will be rect, and has been properly sized to be the tileItself->origBoundryRect resized so that it does not extend beyond scrollerItself->boundryRect
//
//            }
//
//
//        }

        //    collisionRect.x = renderRect.x;
        //    collisionRect.y = renderRect.y;
        //    collisionRect.w = renderRect.w;
        //    collisionRect.h = renderRect.h;
        collisionRect.x = (parentCollisionRect.x + (boundryRect.x * parentRenderRect.w));
        collisionRect.y = (parentCollisionRect.y + (boundryRect.y * parentRenderRect.h));

        //todo optimize use aboev caluclation ??
        //    collisionRect.w = parentCollisionRect.w * boundryRect.w;
        //    collisionRect.h = parentCollisionRect.h * boundryRect.h;
        collisionRect.w = renderRect.w;
        collisionRect.h = renderRect.h; // ok optimized?

//        if( isTextParentOfTextLength > 0 ){
//            if( containText != true ){
//                if( textDirection == TEXT_DIR_ENUM::LTR ){
//                    collisionRect.w *= isTextParentOfTextLength; // * isParentOfTextOfScale ?? ??  since we will space this 
//                }
//            }
//        }

        if( testChildCollisionIgnoreBounds ){
            isInBounds = true; // we will let the parent or child go out of bounds on their own accord, lets assume this contains children that need to render....
        }else{

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

    // note: this only works because we've already called updateRenderPosition on ALL child nodes before we are trying to stack anything
    // but the limitation is stacking the children of a stacked element might not work right...
    void applyStackingDisplacement(float xDisp, float yDisp){
        renderRect.x += xDisp;
        collisionRect.x  += (xDisp * 0.5);
        renderRect.y += yDisp;
        collisionRect.y  += (yDisp * 0.5);
        applyStackingDisplacementToChildNodes(xDisp,yDisp);
    }

    void applyStackingDisplacementToChildNodes(float xDisp, float yDisp){
        for( int x=0,l=childListIndex; x<l; x++ ){
            childList[x]->applyStackingDisplacement(xDisp,yDisp);
        }
    }

    float applyStacking(Float_Rect* parentRenderRect, float stackingOffset){

        if( stack_right ){
            float newPosition = parentRenderRect->x + parentRenderRect->w - renderRect.w - stackingOffset;

            float displacement = newPosition - renderRect.x;
            applyStackingDisplacement(displacement, 0.0);

            return stackingOffset + (renderRect.w * 2.0);
        }

        if(stack_bottom){
            float newPosition = parentRenderRect->y + parentRenderRect->h - renderRect.h - stackingOffset;

            float displacement = newPosition - renderRect.y;
            applyStackingDisplacement(0.0, displacement);

            return stackingOffset + (renderRect.h * 2.0);

        }

        return 0.0;
    }

    void propagateTextSettings(){
        for( int x=0,l=childListIndex; x<l; x++ ){
            Ux::setColor(&childList[x]->backgroundColor, &backgroundColor);
            Ux::setColor(&childList[x]->foregroundColor, &foregroundColor);
        }
    }

    void organizeChildNodesBasedOnTextDir(){

        int ctr=0;
        int i=0;
        //int charOffset = 0;
        //char * i;
        uiObject* letter;


        int len=getChildCount();


//        printObj->doesNotCollide = true;
//        printObj->doesInFactRender = printObj->containText; // the container is never the size to render.. unless contains text?!


        if( containText == true ){
            // HMM this is really something totally different than containText which is used for responsive.....
            float letterWidth = 1.0 / len; // for containText mode

            if( textDirection == TEXT_DIR_ENUM::LTR ){
                for( ctr=0,i=0; i<len; i++/*,ctr++*/ ){
                    letter = childList[i];
                    letter->setBoundaryRect( (i*letterWidth), 0.0, letterWidth, 1.0 );
                }
            }else if( textDirection == TEXT_DIR_ENUM::RTL ){
                for( ctr=0,i=len-1; i>-1; i--,ctr++ ){
                    letter = childList[ctr];
                    letter->setBoundaryRect( (i*letterWidth), 0.0, letterWidth, 1.0 );
                }
            }else if( textDirection == TEXT_DIR_ENUM::TTB ){
                for( ctr=0,i=0; i<len; i++/*,ctr++*/ ){
                    letter = childList[i];
                    letter->setBoundaryRect( 0.0, (i*letterWidth), 1.0, letterWidth );
                }
            }else if( textDirection == TEXT_DIR_ENUM::BTT ){
                for( ctr=0,i=len-1; i>-1; i--,ctr++ ){
                    letter = childList[ctr];
                    letter->setBoundaryRect( 0.0, (i*letterWidth), 1.0, letterWidth );
                }
            }
        }else{
            if( textDirection == TEXT_DIR_ENUM::LTR ){
                for( ctr=0,i=0; i<len; i++/*,ctr++*/ ){
                    letter = childList[i];
                    letter->setBoundaryRect( (i*1.0), 0.0, 1.0, 1.0);
                }
            }else if( textDirection == TEXT_DIR_ENUM::RTL ){
                for( ctr=0,i=len-1; i>-1; i--,ctr++ ){
                    letter = childList[ctr];
                    letter->setBoundaryRect( (i*1.0), 0.0, 1.0, 1.0);
                }
            }else if( textDirection == TEXT_DIR_ENUM::TTB ){
                for( ctr=0,i=0; i<len; i++/*,ctr++*/ ){
                    letter = childList[i];
                    letter->setBoundaryRect( 0.0, (i*1.0), 1.0, 1.0);
                }
            }else if( textDirection == TEXT_DIR_ENUM::BTT ){
                for( ctr=0,i=len-1; i>-1; i--,ctr++ ){
                    letter = childList[ctr];
                    letter->setBoundaryRect( 0.0, (i*1.0), 1.0, 1.0); // arguable this should render bottom up... it is still top down from first char position with reversed text
                }
            }
        }
    }

};



#endif
