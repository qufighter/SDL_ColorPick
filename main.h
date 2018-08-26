//
//  main.h
//  ___PROJECTNAME___
//
//  Created by Sam Larison on 8/24/15.
//
//

#ifndef ___PROJECTNAME____main_h
#define ___PROJECTNAME____main_h

#define GL_GLEXT_PROTOTYPES 1
#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_image.h"

#if __IPHONEOS__
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/gl3.h>
#endif


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
#define CHAR_LINEAR_SLOPE 129
#define CHAR_ASYMP_SLOPE 130
#define CHAR_ARR_UP 131
#define CHAR_ARR_DN 132
#define CHAR_BARS 133

#define CHAR_VERTICAL_BAR 134
#define CHAR_CIRCLE_PLUSS 135
#define CHAR_CIRCLE 136

#define CHAR_ROUNDED_LEFT 137
#define CHAR_ROUNDED_RIGHT 138

#define CHAR_CLOSE_ICON 140
#define CHAR_CHECKMARK_ICON 141
#define CHAR_CANCEL_ICON 142

#define CHAR_ARR_LEFT 143
#define CHAR_ARR_RIGHT 144

#define CHAR_SHADOWED_BARS 145
#define CHAR_VERTICAL_BAR_POINTED 146

#define CHAR_SAVE_ICON 147
#define CHAR_GEAR_ICON 148

#define CHAR_PLUSS 149
#define CHAR_LIGHTENING_BOLT 150

// GLM include files
#include "glm/glm.hpp"
#include "glm/gtx/projection.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/fast_square_root.hpp"
#include "glm/gtx/perpendicular.hpp"


// ^ glm possibly not needed for such simple project





#define DebugMessage(str)	DebugStr(reinterpret_cast<const char *>(str) )
void DebugStr(const char *c_str ); // since we have SDL_Log not sure if this is useful anymore ... maybe
void debugGLerror(const char *c_str);
void debugGLerror();

void ReshapeWindow();
void ShowFrame(void*);

typedef enum { SHADER_POSITION, SHADER_COLOR, SHADER_TEXTURE, SHADER_NORMAL } SHADER_BIND_LOCATIONS;

/// these are hi-dpi sizes
#ifdef COLORPICK_PLATFORM_DESKTOP
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480
#else
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 960
#endif


// static seems a bit redundant, no?
static SDL_Window *window;
static int win_w = SCREEN_WIDTH;
static int win_h = SCREEN_HEIGHT;


//static Uint32 lastTimerTime = 0;
//static Uint32 currentTime;



// TODO: sdl to provide the device scale?
static float ui_mmv_scale = 2.0; // Device Scale!?



static bool didInteract;




static int collected_x =0;
static int collected_y =0;

static float input_velocity_x =0;
static float input_velocity_y =0;

#include "ColorPickState.h"



#include <iostream> // cpp functioanlity, not .c
#include <stdio.h>


#endif
