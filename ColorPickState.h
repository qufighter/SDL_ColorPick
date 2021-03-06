//
//  ColorPickState.h
//  ___PROJECTNAME___
//
//  Created by Sam Larison on 8/24/15.
//
//

//data:text/html;plain,<b>bold</b>

#ifndef _____PROJECTNAME_____ColorPickState__
#define _____PROJECTNAME_____ColorPickState__


class ColorPickState {



public:
    static ColorPickState* Singleton();

    // probably should not be public but easier this way!
    int mmovex, mmovey; /// needs to be ux accessible singleton.......

    bool movedxory;

    int last_thread_mousex, last_thread_mousey; // for use on desktop platforms where PICK_FROM_SCREEN_ENABLED to avoid spamming the event queue where a timer is used to monitor position...

    float viewport_ratio;
    float ui_mmv_scale; // if we are low dpi 1.0, otherwise 2.0

    int drawableWidth; //size of gl context pixel size and might be hidpi - these may have little relation to screen coordinates
    int drawableHiehgt;

    int windowWidth; // this is size of window in screen coorindates (not pixels
    int windowHeight;

    float halfWindowWidth;
    float halfWindowHeight;

    bool appInForeground;

    void updateComputedSizes(void);

    float convertToScreenXCoord(float input);
    float convertToScreenYCoord(float input);


private:

protected:


    ColorPickState(void); // Default constructor
    ~ColorPickState(void); // Destructor for cleaning up our application

    
};
//


static bool cps_ms_bInstanceCreated=false;
static ColorPickState* cps_pInstance=0;

static ColorPickState* colorPickState = ColorPickState::Singleton();



#endif /* defined(_____PROJECTNAME_____ColorPickState__) */
