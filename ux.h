//
//  ux.h
//  ColorPick SDL
//
//  Created by Sam Larison on 8/21/16.
//
//

#ifndef __ColorPick_iOS_SDL__ux__
#define __ColorPick_iOS_SDL__ux__

#define SIX_ACROSS 6.0
#define SIX_ACROSS_RATIO 0.16666666666667  // 1.0 / SIX_ACROSS

//#define COLOR_INDEX_MAX 16581376 //  255^3 +1
//#define COLOR_INDEX_MAX 16777217 //  256^3 +1
//#define COLOR_INDEX_MAX 16843009 // 16777216 + 65536 + 256 + 1 ( this is over allocated)
#define COLOR_INDEX_MAX 16777216 // 16711680 + 65280 + 255 + 1



#define RESIZE_NOW true
#define DO_NOT_RESIZE_NOW false

//#include <stdio.h>
#include "main.h"
using namespace std;
//#include <vector>







#include "shader.h"

// font rendering constants, ascii.png (continued)
const static int text_firstLetterOffset=32; //space is first ascii char represented
const static int text_stride = 12;
const static float text_xStep = 0.08333333333333;
const static float text_yStep = 0.1;



const static float const_friction = 0.99;
const static float const_threshold = 0.0001; // velocity below this threshold stops

#define BTN_NEGATIVE_START -2

typedef enum { BUTTON_CLEAR_HISTORY, BUTTON_ADD_ALL_HISTORY, BUTTON_SORT_HISTORY, PICK_HISRTORY_EXTRA_BUTTONS_TOTAL } PICK_HISRTORY_EXTRA_BUTTONS;
typedef enum { BUTTON_CLEAR_PALLETE, BUTTON_SAVE_PALLETE, PALLETE_EXTRA_BUTTONS_TOTAL } PALLETE_EXTRA_BUTTONS;
typedef enum { BUTTON_PALLETE_HELP, EMPTY_PALLETE_PALLETE_EXTRA_BUTTONS_TOTAL } EMPTY_PALLETE_EXTRA_BUTTONS;

//const static float historyPreviewHeight = 0.9; // todo this should be a return value


//static bool is_monitoring=false;  // THIS IS A DEBUGING VARIABLE!

typedef enum  {
    DEFAULT,
    NOMOVE,
    MOVE_UP
} SCORE_EFFECTS;

typedef enum  {
    RESULT_NO,
    RESULT_NO_FAST,
    RESULT_YES,
    RESULT_YES_FAST
} YES_NO_RESULTS;

typedef enum  {
    NO_TEXT,
    LTR,
    RTL,
    BTT,
    TTB
} TEXT_DIR_ENUM;

typedef enum  {
    TOP,
    RIGHT,
    BOTTOM,
    LEFT
} SQUARE_EDGE_ENUM;

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

typedef struct HSV_Color
{
    Uint16 h; // max 360...
    Uint8 s;
    Uint8 v;

    HSV_Color(){
        h=0;s=0;v=0;
    }

    float diffc(float c, float v, float diff){
        return (v - c) / 6.0 / diff + 1.0 / 2.0;
    }

    void fromColor(SDL_Color* clr){
        float rr, gg, bb; // type?
        float r = clr->r / 255.0;
        float g = clr->g / 255.0;
        float b = clr->b / 255.0;
        float f_h = 0.0, f_s;
        float f_v = SDL_max(SDL_max(r, g), b);
        float diff = f_v - SDL_min(SDL_min(r, g), b);
        if (diff == 0.0) {
            f_h = f_s = 0.0;
        } else {
            f_s = diff / f_v;
            rr = diffc(r, f_v, diff);
            gg = diffc(g, f_v, diff);
            bb = diffc(b, f_v, diff);
            if (r == f_v) {
                f_h = bb - gg;
            }else if (g == f_v) {
                f_h = (1.0 / 3.0) + rr - bb;
            }else if (b == f_v) {
                f_h = (2.0 / 3.0) + gg - rr;
            }
            if (f_h < 0.0) {
                f_h += 1.0;
            }else if (f_h > 1.0) {
                f_h -= 1.0;
            }
        }

        h = f_h * 360.0;
        s = f_s * 100.0;
        v = f_v * 100.0;
    }

} HSV_Color;

#include "ux-Interaction.h"

class Ux {
public:

static Ux* Singleton();


#include "uiList.h" // referrs to Ux:: which referrs to uiObject...
#include "ux-HueGradientData.h"

#include "uiObject.h" // referrs to Ux:: which referrs to uiObject...
#include "uiText.h"
#include "uiEdgeShadow.h"
#include "uiScrollController.h"

#include "uiControlBooleanToggle.h"
#include "uiControlAchievementToggle.h"
#include "uiControlButton.h"
#include "uiViewColor.h"
    //#include "uiRoundedRectangle.h"
    //#include "uiSqware.h"  // its because we don't really have square, and if we do we do not really need this, because squares would already be squares
#include "uiNavArrows.h"
#include "uiYesNoChoice.h"
#include "uiHueGradientScroller.h"
#include "uiHueGradient.h"
#include "uiScore.h"
#include "uiSwatch.h"
#include "uiToolMenu.h"
#include "uiRunningMan.h"
#include "uiHistoryPreviewScroller.h"
#include "uiSortChooser.h"
#include "uiHistoryPalleteEditor.h"
#include "uiSettingsScroller.h"

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

    void GetPrefPath(char* preferencesPath, const char* filename, char** historyPath);

    void writeOutState(void);
    void readInState(void);
    void readInState(char* filepath, void* dest, int destMaxSize, int* readSize);

    void updateRenderPositions(void);
    void updateRenderPosition(uiObject *renderObj);

    void updateModal(uiObject *newModal, anInteractionFn modalDismissal);
    void endModal(uiObject *oldModal); // this is to be called AFTER modal is hidden
    void endCurrentModal();
    bool hasCurrentModal();

    int renderObject(uniformLocationStruct* uniformLocations);
    int renderObjects(uniformLocationStruct* uniformLocations, uiObject* renderObj, glm::mat4 inheritMat);

    void printStringToUiObject(uiObject* printToThisObj, const char* text, bool resizeText);
    void printCharToUiObject(uiObject* letter, char character, bool resizeText);
    void printCharToUiObject(uiObject* letter, int character, bool resizeText);
    void printCharOffsetUiObject(uiObject* letter, int charOffset);
    void hideHistoryPalleteIfShowing();

    bool objectCollides(float x, float y);
    bool objectCollides(uiObject* testObj, float x, float y);

    bool triggerInteraction(); // mouseup, mouse didn't move
    bool triggerInteraction(bool isStart); // mouseup, mouse didn't move

    void wheelOrPinchInteraction(float delta);

    void doOpenURL(char* url);

    bool bubbleCurrentInteraction();
    bool interactionUpdate(uiInteraction *delta);
    bool interactionComplete(uiInteraction *delta);

    static void loadTestImageByIndex(int index);

    static void interactionHistoryEnteredView(uiObject *interactionObj);
    static void interactionNoOp(uiObject *interactionObj, uiInteraction *delta);
    static void interactionFileBrowserTime(uiObject *interactionObj, uiInteraction *delta);
    static void interactionReturnToPreviousSurface(uiObject *interactionObj, uiInteraction *delta);
    static void interactionBeginReturnToPreviousSurface(uiObject *interactionObj, uiInteraction *delta);
    static void interactionVisitSettings(uiObject *interactionObj, uiInteraction *delta);
    static void interactionAddHistory(uiObject *interactionObj, uiInteraction *delta);
    static void clickZoomSliderBg(uiObject *interactionObj, uiInteraction *delta);
    static void interactionHZ(uiObject *interactionObj, uiInteraction *delta);
    static void interactionSliderVT(uiObject *interactionObj, uiInteraction *delta);
    static void interactionHorizontal(uiObject *interactionObj, uiInteraction *delta);
    static void interactionVert(uiObject *interactionObj, uiInteraction *delta);

    static void interactionDragMove(uiObject *interactionObj, uiInteraction *delta);
    static void interactionDragMoveConstrain(uiObject *interactionObj, uiInteraction *delta);
    static void interactionConstrainToParentObject(uiAnimation* uiAnim);


    static void hueClicked(SDL_Color* c);
    static void hueClickedPickerHsv(SDL_Color* c);
    static void pickerForHuePercentage(float percent);
    static void interactionDirectionalArrowClicked(uiObject *interactionObj, uiInteraction *delta);
    static bool bubbleInteractionIfNonClickOrHiddenPalletePreview(uiObject *interactionObj, uiInteraction *delta);
    static bool bubbleInteractionIfNonClick(uiObject *interactionObj, uiInteraction *delta);
    static bool bubbleInteractionIfNonHorozontalMovement(uiObject *interactionObj, uiInteraction *delta);
    static bool bubbleInteractionIfNonHorozontalMovementScroller(uiObject *interactionObj, uiInteraction *delta); // return true always, unless the interaction should be dropped and not bubble for some reason....
    static bool bubbleWhenHidden(uiObject *interactionObj, uiInteraction *delta);

    static int indexForColor(SDL_Color* c);
    static int indexForColor(ColorList* cli);


    //uiList<SDL_Color, Uint8>* pickHistoryList; // WARN - do not enable index if using Uint8 - max Uint8 is far less than pickHistoryMax
    uiList<ColorList, Uint8>* pickHistoryList; // WARN - do not enable index if using Uint8 - max Uint8 is far less than pickHistoryMax
    //uiList<ColorListState, Uint8>* pickHistoryListState;

    //int pickHistoryIndex = 0;
    //int lastPickHistoryIndex = -1;
    //int largestPickHistoryIndex=0; // how far in history we have gone, to allow loop if we have greater history available
    static const int pickHistoryMax = 8192;  // cannot be less than SIX_ACROSS
    //static const int pickHistoryMax = 7;

    //SDL_Color pickHistory[pickHistoryMax];
//    static bool updateUiObjectFromHistory(uiObject *historyTile, int offset);
//    static int getHistoryTotalCount();

    //uiList<SDL_Color, Uint8>* palleteList;
    uiList<ColorList, Uint8>* palleteList;
    //uiList<ColorListState, Uint8>* palleteListState;

    //int palleteIndex = 0;
    //int lastPalleteIndex = -1;
    //int largestPalleteIndex=-1; // how far in history we have gone, to allow loop if we have greater history available
    static const int palleteMax = 254; // 254 // WARN do not exeede max size Uint8 palleteColorsIndex 255
    //static const int palleteMax = 4;
    //SDL_Color palleteColors[palleteMax];
//    static bool updateUiObjectFromPallete(uiObject *historyTile, int offset);
//    static int getPalleteTotalCount();

    // palleteMax CANNOT exceede the size of Uint16 now, which is about 65536
    // palleteMax CANNOT exceede the size of Uint8 now, which is about 256
    //Uint8 palleteColorsIndex[COLOR_INDEX_MAX]; // we do not search the array
    //Uint8* palleteColorsIndex = (Uint8*)SDL_malloc( sizeof(Uint8) * COLOR_INDEX_MAX ); // totally equivilent to above

    static const int minigameColorListMax = 3; // 254 // WARN do not exeede max size Uint8 palleteColorsIndex 255 (each entry in the index, see COLOR_INDEX_MAX, will point to an entry in the list, so if you have 256 in the list you can point to each with Uint8 - you can index with another type but it will use a lot more memory.  the used memory will always be COLOR_INDEX_MAX * sizeof(uint8) bytes.
    uiList<ColorList, Uint8>* minigameColorList;
    uiObject *minigameCounterText;

    void updateColorValueDisplay(SDL_Color* color);
    void addCurrentToPickHistory();
    void updatePickHistoryPreview();
    void updatePalleteScroller();

    bool wouldLooseIfColorAdded();

    float screenRatio = 1.0f;
    bool widescreen = false;
    bool isMinigameMode = false;

    float clock_bar = 0.04; // clock bar height (ios etc)
    float hue_picker = 0.05; // huePicker "height" (or width when widescreen)
    float history_preview = 0.1; // historyPreview "height" (or width when widescreen)

    uxHueGradientData* hueGradientData;

    uiInteraction currentInteraction;
    uiObject *rootUiObject; // there is a root ui object

    uiObject *mainUiContainer;
    uiObject *minigamesUiContainer;

    uiObject* currentModal;

    UxAnim *uxAnimations;

    // if we have 1off create functions, we will need to store a bunch of references to the "important" objects

        uiObject *bottomBar;
        uiObject *bottomBarRightStack;
        uiObject *pickSourceBtn;
        uiObject *addHistoryBtn;
        uiObject *optionsGearBtn;

        uiObject *returnToLastImgBtn;


        uiObject *zoomSliderHolder;
        uiObject *zoomSliderBg;
        uiObject *zoomSlider;
        uiRunningMan *runner;

        uiViewColor *curerntColorPreview;


        uiYesNoChoice* defaultYesNoChoiceDialogue;
        uiObject* defaultYesNoChoiceHolder;
        uiHueGradient* huePicker;
#ifndef OMIT_SCROLLY_ARROWS
        uiNavArrows* movementArrows;
#endif
        uiScore* defaultScoreDisplay;

        uiHistoryPalleteEditor* historyPalleteEditor;
        uiSettingsScroller* settingsScroller;
        uiToolMenu* rClickMenu;

        uiObject *historyPreviewHolder;
        uiObject *historyPreview;
        //uiObject *historyFullsize;



//        uiObject *renderedletters[2048]; // we should just make ach letter once
//        int renderedLettersCtr=0;

    bool isInteracting = false;
    uiObject* interactionObject; // if we are dragging or have clicked and object the refrence will be here
    uiObject* lastInteractionObject; // to help detect double click




    SDL_Color* currentlyPickedColor;
    HSV_Color* lastHue;  // bad var name?  this is an SDL_Color in ColorPick.h...

    char* historyPath;
    char* palletePath;
    char* settingPath;
    char* scoresPath;


    static const int max_print_here = 128;
    char* print_here;

private:


protected:

    static bool ms_bInstanceCreated;
    static Ux* pInstance;
};



#endif /* defined(__ColorPick_iOS_SDL__ux__) */
