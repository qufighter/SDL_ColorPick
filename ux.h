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

typedef enum { BUTTON_CLEAR_ALL, PICK_HISRTORY_EXTRA_BUTTONS_TOTAL } PICK_HISRTORY_EXTRA_BUTTONS;


//const static float historyPreviewHeight = 0.9; // todo this should be a return value


//static bool is_monitoring=false;  // THIS IS A DEBUGING VARIABLE!


typedef struct Float_Point
{
    Float_Point(){
        x=0.0;y=0.0;
    }
    Float_Point(float ix,float iy){
        x=ix;y=iy;
    }
    Float_Point(Float_Point * toClone){
        x=toClone->x;y=toClone->y;
    }
    float x, y;
} Float_Point;


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



#include "ux-Interaction.h"

class Ux {
public:

    
static Ux* Singleton();

#include "uiObject.h" // referrs to Ux:: which referrs to uiObject...
#include "uiScrollController.h"
#include "uiViewColor.h"
//#include "uiRoundedRectangle.h"
//#include "uiSqware.h"  // its because we don't really have square, and if we do we do not really need this, because squares would already be squares
#include "uiNavArrows.h"
#include "uiYesNoChoice.h"

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
    static void clickDeletePalleteColor(uiObject *interactionObj, uiInteraction *delta);
    static void clickDeleteHistoryColor(uiObject *interactionObj, uiInteraction *delta);
    static void removePalleteColor(uiObject *interactionObj, uiInteraction *delta);
    static void removeHistoryColor(uiObject *interactionObj, uiInteraction *delta);
    static void interactionHZ(uiObject *interactionObj, uiInteraction *delta);
    static void interactionSliderVT(uiObject *interactionObj, uiInteraction *delta);
    static void interactionHorizontal(uiObject *interactionObj, uiInteraction *delta);
    static void interactionVert(uiObject *interactionObj, uiInteraction *delta);
    static void interactionDirectionalArrowClicked(uiObject *interactionObj, uiInteraction *delta);
    static bool bubbleInteractionIfNonClick(uiObject *interactionObj, uiInteraction *delta);
    static bool bubbleInteractionIfNonHorozontalMovement(uiObject *interactionObj, uiInteraction *delta); // return true always, unless the interaction should be dropped and not bubble for some reason....


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
    // pallete max CANNOT exceede the size of Uint8 now, which is about 256
    Uint8 palleteColorsIndex[COLOR_INDEX_MAX]; // we do not search the array
    //Uint8* palleteColorsIndex = (Uint8*)SDL_malloc( COLOR_INDEX_MAX ); // totally equivilent to above

    void colorTileAddChildObjects(uiObject *historyTile, anInteractionFn removeClickedFn);
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

        uiYesNoChoice* defaultYesNoChoiceDialogue;

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
