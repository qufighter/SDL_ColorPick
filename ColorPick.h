//
//  ColorPick.h
//  ___PROJECTNAME___
//
//  Created by Sam Larison on 8/24/15.
//
//

#ifndef _____PROJECTNAME_____ColorPick__
#define _____PROJECTNAME_____ColorPick__

#include "main.h"

#include "shader.h"

#include "textures.h"

#include "ux.h"
//
//#include "ux-anim.h"

#include "FileChooser.h" // platform specific include!  this is for ios


#define MIN_FISHEYE_ZOOM 0.1f
#define MAX_FISHEYE_ZOOM 64.0f

#define SHD_TEXTURE_SIZE 2048

#define VIEW_MAX_CLIP_DISTANCE 100


typedef struct {
    float Position[3];
    float Color[4];
} Vertex;

static const GLfloat squareVertices[] = {
    -1.0f, -1.0f,
    1.0f, -1.0f,
    -1.0f,  1.0f,
    1.0f,  1.0f,
};

static const GLubyte squareColors[] = {
    255, 255,   0, 255,
    0,   255, 255, 255,
    0,     0,   0,   0,
    255,   0, 255, 255,
};

static const GLubyte squareTexCoords[] = {
    0, 255,
    255, 255,
    0, 0,
    255, 0,
};


class OpenGLContext {

public:
    static OpenGLContext* Singleton();

    SDL_GLContext gl;

    SDL_Window *sdlWindow;
    bool createContext(SDL_Window *sdlWindow); // Creation of our OpenGL 3.x context
    void destroyContext();
    void createUI(void);
    void setupScene(void); // All scene information can be setup here
    void reshapeWindow(int w, int h); // Method to get our window width and height on resize
    //void updateFrame(Uint32 elapsedMs);
    void renderScene(void); // Render scene (display method from previous OpenGL tutorials)
    void chooseFile(void);

    SDL_Surface *fullPickImgSurface;

//    void keyDown(int key);
//    void keyUp(int key);
//    int totalKeys = 1073742052;
//    bool downkeys[1073742052]; // SDL tends to fire the keydown event more than once...
//    bool kkeys[1073742052];	// use this to keep track of keys...hopefully keyboard doesn't have more than this! (it does!)

    Shader *shader_lit_detail; // Our GLSL shader
    Shader *shader_ui_shader_default;

    uniformLocationStruct* uniformLocations;

    GLuint	textureId_default,
            textureId_pickImage,
            textureId_fonts,
            textureId_pick_button,
            tex2_id;

    Textures *textures;

    Ux *generalUx;

    // probably should not be public but easier this way!
    int mmovex, mmovey;
    float fishEyeScale = 7.0f;
    float fishEyeScalePct = fishEyeScale / MAX_FISHEYE_ZOOM;
    // do not set directly....
    
    void setFishScale(float modifierAmt);
    static void setFishScalePerentage(float percent);

    void triggerVelocity(float x, float y);
    void clearVelocity();

    //typedef void (*imageSelectedCallback)(const char *); // < its this type
    void imageWasSelectedCb(SDL_Surface *myCoolSurface);
    void updateColorPreview(void);

    //imageSelectedCallback imageWasSelectedCb;

    void reloadShaders(void);
    bool renderShouldUpdate = true;


private:

    int textureSize = SHD_TEXTURE_SIZE;
    int halfTextureSize = textureSize * 0.5;
    int position_x = 0;
    int position_y = 0;

    Ux::uiObject* rootUiObject; // there is a root ui object

    bool has_velocity = false;
    float velocity_x =0;
    float velocity_y =0;
    float pan_friction = 0.8;



    int windowWidth; // Store the width of our window
    int windowHeight; // Store the height of our window
    float widthHeightRatio;
    unsigned int cur_shader_id;

    void createSquare(void); // Method for creating our squares Vertex Array Object
    GLuint rect_triangleStripIndexBuffer;
    unsigned int rect_vaoID[1];
    unsigned int rect_vboID[4];

    //GLint globalLightLocation, ambientLightLoc, projectionMatrixLocation, viewMatrixLocation, modelMatrixLocation, textureSampler, textureSampler2, textureSampler3, outalpha, textOffset;

//    glm::mat4 modelMatrix; // Store the model matrix
//    glm::mat4 projectionMatrix; // Store the projection matrix



protected:
    static bool ms_bInstanceCreated;
    static OpenGLContext* pInstance;

    void loadShaders(void);
    OpenGLContext(void); // Default constructor
    ~OpenGLContext(void); // Destructor for cleaning up our application

    
};

//ends up mulitply defined (?? in future?)
static OpenGLContext* openglContext = OpenGLContext::Singleton();

const static float zoomRange = MAX_FISHEYE_ZOOM - MIN_FISHEYE_ZOOM;


#endif /* defined(_____PROJECTNAME_____ColorPick__) */
