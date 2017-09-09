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

//#include <stdio.h>
#include "main.h"
using namespace std;
#include <vector>

#include "shader.h"

// font rendering constants, ascii.png
const static int text_firstLetterOffset=32; //space is first ascii char represented
const static int text_stride = 12;
const static float text_xStep = 0.08333333333333;
const static float text_yStep = 0.1;


typedef struct Float_Rect
{
    Float_Rect(){
        x=0.0;y=0.0;w=1.0;h=1.0;
    }

    Float_Rect(float ix,float iy,float iw,float ih){
        x=ix;y=iy;w=iw;h=ih;
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
        px=x;
        py=y;
        ix=x;
        iy=y;
        rx=0;
        ry=0;
        dx=0;
        dy=0;
//        mvx=0;
//        mvy=0;
    }
    void update(float x, float y, float pdx, float pdy){ // todo pass delta and relative
        rx = x - px;
        ry = y - py;

        px=x;
        py=y;

        dx = px - ix;
        dy = py - iy;
    }
    void fixX(Float_Rect r, Float_Rect p){
        px = r.x + (r.w * 0.5 * p.w);
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
    float rx;
    float ry;
//    int mvx;// unused ?
//    int mvy;
};


class Ux {
public:

static Ux* Singleton();

#include "uiObject.h" // referrs to Ux:: which referrs to uiObject...


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
    void printCharOffsetUiObject(uiObject* letter, int charOffset);

    bool objectCollides(float x, float y);
    bool objectCollides(uiObject* testObj, float x, float y);

    bool triggerInteraction(); // mouseup, mouse didn't move


    bool interactionUpdate(uiInteraction *delta);
    bool interactionComplete(uiInteraction *delta);

    static void interactionNoOp(uiObject *interactionObj, uiInteraction *delta);
    static void interactionTouchRelease(uiObject *interactionObj, uiInteraction *delta);
    static void interactionHZ(uiObject *interactionObj, uiInteraction *delta);

    void updateColorValueDisplay(SDL_Color* color);
    void addCurrentToPickHistory();
    void updatePickHistoryPreview();

    float screenRatio = 1.0f;
    bool widescreen = false;

    uiInteraction currentInteraction;
    uiObject *rootUiObject; // there is a root ui object
    // if we have 1off create functions, we will need to store a bunch of references to the "important" objects

        uiObject *bottomBar;
        uiObject *pickSourceBtn;
        uiObject *zoomSlider;

        uiObject *hexValueText;
        uiObject *rgbRedText;
        uiObject *rgbGreenText;
        uiObject *rgbBlueText;


        uiObject *historyPreview;

//        uiObject *renderedletters[2048]; // we should just make ach letter once
//        int renderedLettersCtr=0;

    bool isInteracting = false;
    uiObject* interactionObject; // if we are dragging or have clicked and object the refrence will be here



    int pickHistoryIndex = 0;
    int lastPickHistoryIndex = 0;
    int largestPickHistoryIndex=0; // how far in history we have gone, to allow loop if we have greater history available
    int pickHistoryMax = 255; //5; //derp
    SDL_Color pickHistory[255]; // ui object may have a max of 8 child objects each
    SDL_Color* currentlyPickedColor;


private:

protected:

    static bool ms_bInstanceCreated;
    static Ux* pInstance;
};

#endif /* defined(__ColorPick_iOS_SDL__ux__) */
