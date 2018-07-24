//
//  ux.h
//  ColorPick iOS SDL
//
//  Created by Sam Larison on 8/21/16.
//
//

#ifndef __ColorPick_iOS_SDL__ux__
#define __ColorPick_iOS_SDL__ux__

#define SIX_ACROSS 6.0
#define SIX_ACROSS_RATIO 0.16666666666667  // 1.0 / SIX_ACROSS

#define COLOR_INDEX_MAX 16581376

#define RESIZE_NOW true
#define DO_NOT_RESIZE_NOW false

//#include <stdio.h>
#include "main.h"
using namespace std;
#include <vector>

#include "shader.h"

// font rendering constants, ascii.png (continued)
const static int text_firstLetterOffset=32; //space is first ascii char represented
const static int text_stride = 12;
const static float text_xStep = 0.08333333333333;
const static float text_yStep = 0.1;



const static float const_friction = 0.99;
const static float const_threshold = 0.0001; // velocity below this threshold stops


//const static float historyPreviewHeight = 0.9; // todo this should be a return value


//static bool is_monitoring=false;  // THIS IS A DEBUGING VARIABLE!


typedef struct Float_Rect
{
    Float_Rect(){
        x=0.0;y=0.0;w=1.0;h=1.0;
    }

    Float_Rect(float ix,float iy,float iw,float ih){
        x=ix;y=iy;w=iw;h=ih;
    }
    Float_Rect(Float_Rect * toClone){
        x=toClone->x;y=toClone->y;w=toClone->w;h=toClone->h;
    }
    float x, y;
    float w, h;
} Float_Rect;


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
        vx=0;
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
    float dx;// delta not distance
    float dy;
    float px;
    float py;
    float ix;
    float iy;
    float vx;
    float vy;
    float rx;
    float ry;
    int lastUpdate;
//    int mvx;// unused ?
//    int mvy;
};
class Ux {
public:

    
static Ux* Singleton();

#include "uiObject.h" // referrs to Ux:: which referrs to uiObject...
#include "uiScrollController.h"
#include "uiViewColor.h"
//#include "uiRoundedRectangle.h"
//#include "uiSqware.h"  // its because we don't really have square, and if we do we do not really need this, because squares would already be squares
#include "uiNavArrows.h"

#include "ux-anim.h"
    

    int
    randomInt(int min, int max)
    {
        return min + rand() % (max - min + 1);
    }


    Ux(void); // Default constructor
    ~Ux(void); // Destructor for cleaning up our application

    void updateStageDimension(float w, float h);
    void resizeUiElements(void);

    uiObject* create();

    void updateRenderPositions(void);
    void updateRenderPosition(uiObject *renderObj);


    int renderObject(uniformLocationStruct* uniformLocations);
    int renderObjects(uniformLocationStruct* uniformLocations, uiObject* renderObj);

    void printStringToUiObject(uiObject* printToThisObj, char* text, bool resizeText);
    void printCharToUiObject(uiObject* letter, char character, bool resizeText);
    void printCharToUiObject(uiObject* letter, int character, bool resizeText);
    void printCharOffsetUiObject(uiObject* letter, int charOffset);

    bool objectCollides(float x, float y);
    bool objectCollides(uiObject* testObj, float x, float y);

    bool triggerInteraction(); // mouseup, mouse didn't move


    bool bubbleCurrentInteraction();
    bool interactionUpdate(uiInteraction *delta);
    bool interactionComplete(uiInteraction *delta);

    static void interactionHistoryEnteredView(uiObject *interactionObj);
    static void interactionNoOp(uiObject *interactionObj, uiInteraction *delta);
    static void interactionToggleHistory(uiObject *interactionObj, uiInteraction *delta);
    static void interactionTogglePalletePreview(uiObject *interactionObj, uiInteraction *delta);
    static void interactionTouchRelease(uiObject *interactionObj, uiInteraction *delta);
    static void clickPalleteColor(uiObject *interactionObj, uiInteraction *delta);
    static void clickHistoryColor(uiObject *interactionObj, uiInteraction *delta);
    static void interactionHZ(uiObject *interactionObj, uiInteraction *delta);
    static void interactionSliderVT(uiObject *interactionObj, uiInteraction *delta);
    static void interactionVert(uiObject *interactionObj, uiInteraction *delta);
    static void interactionDirectionalArrowClicked(uiObject *interactionObj, uiInteraction *delta);

    static bool bubbleInteractionIfNonClick(uiObject *interactionObj, uiInteraction *delta);

    int pickHistoryIndex = 0;
    int lastPickHistoryIndex = -1;
    int largestPickHistoryIndex=0; // how far in history we have gone, to allow loop if we have greater history available
    static const int pickHistoryMax = 2048;
    SDL_Color pickHistory[pickHistoryMax];
    static bool updateUiObjectFromHistory(uiObject *historyTile, int offset);
    static int getHistoryTotalCount();


    int palleteIndex = 0;
    int lastPalleteIndex = -1;
    int largestPalleteIndex=-1; // how far in history we have gone, to allow loop if we have greater history available
    static const int palleteMax = 254; // WARN do not exeede max size Uint8 palleteColorsIndex 255
    SDL_Color palleteColors[palleteMax];
    static bool updateUiObjectFromPallete(uiObject *historyTile, int offset);
    static int getPalleteTotalCount();

    // pallete max CANNOT exceede the size of Uint16 now, which is about 65536
    Uint8 palleteColorsIndex[COLOR_INDEX_MAX]; // we do not search the array
    //Uint8* palleteColorsIndex = (Uint8*)malloc( COLOR_INDEX_MAX ); // totally equivilent to above


    void updateColorValueDisplay(SDL_Color* color);
    void addCurrentToPickHistory();
    void updatePickHistoryPreview();

    //bool updateAnimations(float elapsedMs);





    float screenRatio = 1.0f;
    bool widescreen = false;

    uiInteraction currentInteraction;
    uiObject *rootUiObject; // there is a root ui object

    UxAnim *uxAnimations;

    // if we have 1off create functions, we will need to store a bunch of references to the "important" objects

        uiObject *bottomBar;
        uiObject *pickSourceBtn;
        uiObject *zoomSlider;

        uiViewColor *curerntColorPreview;
        uiViewColor *palleteSelectionColorPreview;

        uiNavArrows* movementArrows;

        uiObject *historyPalleteHolder;
        uiScrollController *historyScroller;
        uiScrollController *palleteScroller;
        uiObject *newHistoryFullsize;
        uiObject *newHistoryPallete;

        uiObject *historyPreview;
        uiObject *historyFullsize;


//        uiObject *renderedletters[2048]; // we should just make ach letter once
//        int renderedLettersCtr=0;

    bool isInteracting = false;
    uiObject* interactionObject; // if we are dragging or have clicked and object the refrence will be here




    SDL_Color* currentlyPickedColor;


private:


protected:

    static bool ms_bInstanceCreated;
    static Ux* pInstance;
};



#endif /* defined(__ColorPick_iOS_SDL__ux__) */
