//
//  main.h
//  ___PROJECTNAME___
//
//  Created by Sam Larison on 8/24/15.
//
//

#ifndef ___PROJECTNAME____main_h
#define ___PROJECTNAME____main_h

//#define DEVELOPER_TEST_MODE 1

#define GL_GLEXT_PROTOTYPES 1

// # needed for cmake build

//#ifdef __EMSCRIPTEN__
//#include "SDL2/SDL.h"
//#else
#include "SDL.h"
//#endif


//#elif defined(__MACOSX__) && defined(__LP64__)  /* this is deprecated in 10.12 sdk; favor gcc atomics. */

#if defined(__MACOSX__) || defined(__WIN32__) || defined(__LINUX__) || defined(__EMSCRIPTEN__)
#define COLORPICK_PLATFORM_DESKTOP 1
#endif


#if defined(__MACOSX__) || defined(__LINUX__) || defined(__WIN32__) || defined(__EMSCRIPTEN__)
#define PICK_FROM_SCREEN_ENABLED = true
#endif

#define FREE_FOR_NEW SDL_free

//#define NO_MESH_LOADING 1


// ColorPick BASIC.... UNCOMMENT THE FOLLOWING:
//#define COLORPICK_BASIC_MODE 1
// we ALSO neeed to swap out the following:
// 1) icons < for android this means running the correct buildIcons*.sh script before you bundle your release
// 2) com.vidsbee.colorpicksdl com.vidsbee.colorpickbasicsdl < for android this means check build.gradle (only there, not AndroidManifest), to result in
//     com.vidsbee.colorpickbasicsdl/com.vidsbee.colorpicksdl.SDLColorPickActivity )

// SIMPLER BASIC MODE:
// see android-project/buildAllRelease.sh OR/Also:
// git cherry-pick -n color-pick-basic-mode-enable # (-n is --no-commit).
// git revert -n color-pick-basic-mode-enable
// git revert --abort # cleans up the state somehow, even though empty diff

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#define COLORPICK_MISSING_MAIN_LOOP 1

#define SDL_ceilf SDL_ceil
#define SDL_fabsf SDL_fabs
#define SDL_floorf SDL_floor
#define SDL_powf SDL_pow


#endif

#ifdef __IPHONEOS__
#include "SDL_opengl.h"
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#define COLORPICK_MISSING_MAIN_LOOP 1
#elif __ANDROID__ || __EMSCRIPTEN__

#define GL_GLEXT_PROTOTYPES 1
#define NO_SDL_GLEXT 1
#include "SDL_opengl.h"  // oks

#include "SDL_opengles2.h"

#define COLORPICK_OPENGL_ES2

////#include <GLES/gl.h>
////#include <GLES/glext.h>
////#include "EGL/egl.h"
////#include "EGL/eglext.h"
//#include <GLES2/gl2.h>  // THE ABOVE SDL_opengles2.h does this for free :)
//#include <GLES2/gl2ext.h>
////#include <GLES3/gl3.h>
////#include <GLES3/gl3ext.h>
////#include "SDL_opengl.h"
////#include "SDL_opengles.h"
//
////#include "SDL_opengles2.h"
////#include "SDL_opengles2_gl2.h"
////#include "SDL_opengles2_gl2ext.h"

// this is so dumb... need to wrap with ifndefs though... (WARNING - these require extensions to OpenGL ES 2.0 (most are 3.0 features))
#define glGenVertexArrays glGenVertexArraysOES // danger - use will break rendering on fire TV
#define glBindVertexArray glBindVertexArrayOES // danger
#define glDeleteVertexArrays glDeleteVertexArraysOES // danger
//#define glIsVertexArray glIsVertexArrayOES
//#define glUnmapBuffer glUnmapBufferOES
#elif __WINDOWS__

#include <external/glew/include/GL/glew.h>
//#include <external/glew/include/GL/wglew.h>

#undef NO_SDL_GLEXT
// #define SDL_VIDEO_OPENGL_WGL 1
// #define SDL_VIDEO_DRIVER_WINDOWS 1
#undef GL_GLEXT_PROTOTYPES // prototypes handled by glew...
#include "SDL_opengl.h"

//#include "SDL_windowsopengl.h"

 // #include <GL/glu.h>
 // #include <GL/gl.h>

//
//
//#ifdef __WINDOWS__
//#define MAIN_THREAD_ANIMATIONS 1 # depe
//#endif

#define FREE_FOR_NEW free // on windows, using the new operator to consturct means we really CANNOT use SDL_Free, and its tough to detect... sprinked around the project this way!

#else

#include "SDL_opengl.h"
//#include "SDL_opengl_glext.h" // should already be included.
//#include <OpenGL/gl.h>
//#include <OpenGL/gl3.h> // here is a nifty one, this isn't actually needed on mac osx seemingly :)
#endif

#include "SDL_image.h"
// font rendering constants, ascii.png (continued)
// THESE ARE CHAR DEFS.... NOT INT OFFSET DEFS
//#define CHAR_LINEAR_SLOPE 133
//#define CHAR_ASYMP_SLOPE 134
//#define CHAR_ARR_UP 135
//#define CHAR_ARR_DN 136
//#define CHAR_BARS 137
//
//#define CHAR_VERTICAL_BAR 138
//#define CHAR_CIRCLE_PLUSS 139
//#define CHAR_CIRCLE 140
//
//#define CHAR_ROUNDED_LEFT 141
//#define CHAR_ROUNDED_RIGHT 142

//okay here are the int defs... should be more reliable right?
#define CHAR_APP_ICON 127
#define CHAR_ZOOM_PLUSS 128

#define CHAR_ZOOM_SPEEDY_GUY 129
#define CHAR_ASYMP_SLOPE 130
#define CHAR_ARR_UP 131
#define CHAR_ARR_DN 132

//#define CHAR_BARS 133
#define CHAR_MOUSE_CURSOR 133

#define CHAR_CIRCLE_PLAIN 134
#define CHAR_CIRCLE_PLUSS 135
#define CHAR_CIRCLE 136

#define CHAR_ROUNDED_LEFT 137
#define CHAR_ROUNDED_RIGHT 138

#define CHAR_SORT_ORDERING 139

// row 2
#define CHAR_CLOSE_ICON 140
#define CHAR_CHECKMARK_ICON 141
#define CHAR_CANCEL_ICON 142

#define CHAR_ARR_LEFT 143
#define CHAR_ARR_RIGHT 144

#define CHAR_ADD_ALL_DUDE 145

#define CHAR_PAPERCLIP 146
#define CHAR_SAVE_ICON 147
#define CHAR_GEAR_ICON 148

#define CHAR_BACK_ICON 149
#define CHAR_LIGHTENING_BOLT 150
#define CHAR_OPEN_FILES 151



#if __cplusplus > 199711L
#define register      // Deprecated in C++11.
#endif  // #if __cplusplus > 199711L

// GLM include files
#include "external_static/glm/glm.hpp"
#include "external_static/glm/gtx/projection.hpp"
#include "external_static/glm/gtc/matrix_transform.hpp"
#include "external_static/glm/gtx/fast_square_root.hpp"
#include "external_static/glm/gtx/perpendicular.hpp"
#include "external_static/glm/gtx/normal.hpp"



#ifndef COLORPICK_PLATFORM_DESKTOP
#define OMIT_SCROLLBAR_ARROWS
#endif

//#define OMIT_SCROLLY_ARROWS



#define DebugMessage(str)	DebugStr(reinterpret_cast<const char *>(str) )
void DebugStr(const char *c_str ); // since we have SDL_Log not sure if this is useful anymore ... maybe
void debugGLerror(const char *c_str);
void debugGLerror();

void ReshapeWindow();
void ReshapeWindow(bool fromMain);
void ShowFrame(void*);

typedef enum { SHADER_POSITION, SHADER_COLOR, SHADER_TEXTURE, SHADER_NORMAL } SHADER_BIND_LOCATIONS;

typedef enum  {
    IMAGE_SELECTOR_READY,
    NEW_HUE_CHOSEN,
    GENERIC_ARBITRARY_CALL,
    MESH_FILE_READ,
    VIEW_RECENTLY_ROTATED,
    RENDER_VIEW_AGAIN,
    ANIMATE_MAIN_THREAD,
    PICK_AGAIN_NOW, // for after "refresh" and wish to pick again as though we never stopped (desktop only) PICK_FROM_SCREEN_ENABLED
    PICK_AT_POSITION,
    IDBFS_INITIAL_SYNC_COMPLETED,
} USER_EVENT_ENUM; // these may not be supported as of now on IOS ... ??


struct uniformLocationStruct
{
    GLint   globalLightLocation,
    globalAmbientLight,
    projectionMatrixLocation,
    viewMatrixLocation,
    modelMatrixLocation,
    textureSampler,
    textureSampler2,
    textureSampler3,
    ui_modelMatrix,
    ui_scale,
    ui_position,
    ui_color,
    ui_foreground_color,
    ui_crop,
    ui_crop2,
    ui_corner_radius,
    widthHeightRatio,
    color_additive,
    fishScale,
    fishScalePct,
    textureWidth,
    positionOffset,
    zoomedPositionOffset,
    textureCoord,
    texture_crop,
    normalLightingMat,
    outalpha,
    textOffset;
    uniformLocationStruct(){
        //constructor (set all to -1, however this occurs naturally)
    }
};


/// these are hi-dpi sizes
#ifdef COLORPICK_PLATFORM_DESKTOP
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480

//#define SCREEN_WIDTH 640
//#define SCREEN_HEIGHT 960

//ws boot test (passed)
//#define SCREEN_WIDTH 480
//#define SCREEN_HEIGHT 320

// ipohone xs max test (/3)
//#define SCREEN_WIDTH 414
//#define SCREEN_HEIGHT 896

//#define SCREEN_WIDTH 1920
//#define SCREEN_HEIGHT 1080

#else
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 960

//#define SCREEN_WIDTH 1920
//#define SCREEN_HEIGHT 1080

#endif


//#define MAIN_THREAD_ANIMATIONS


#if defined(__ANDROID__) //|| defined(__WIN32__)

#else
//#ifndef __ANDROID__
// android requires some events and handled in the main thread, even some click events
// this is all click events related to creating a new image:  (picker for hsv, return to last image)
#define USE_EVENT_WATCH_FOR_EVENTS
#endif


// static seems a bit redundant, no?
static SDL_Window *sdl_Window;
static int win_w = SCREEN_WIDTH;
static int win_h = SCREEN_HEIGHT;


//static Uint32 lastTimerTime = 0;
//static Uint32 currentTime;



// TODO: sdl to provide the device scale?
static float ui_mmv_scale = 2.0; // Device Scale!?



//static bool didInteract; instead each interaction will have one of these!


#include "ColorPickState.h"




#include <iostream> // cpp functioanlity, not .c
#include <stdio.h>


#if defined(_DEBUG) || defined(DEBUG) || (defined(__GNUC__) && !defined(__OPTIMIZE__))
#define COLORPICK_DEBUG_MODE 1
#endif


#ifdef __IPHONEOS__
// not borderless ?
#define COLORPICK_CLOCK_BAR_PRESENT 1
#endif


#endif
