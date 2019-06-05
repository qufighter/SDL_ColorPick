//
//  ColorPick.h
//  ___PROJECTNAME___
//
//  Created by Sam Larison on 8/24/15.
//
//

//data:text/html;plain,<b>bold</b>

#ifndef _____PROJECTNAME_____ColorPick__
#define _____PROJECTNAME_____ColorPick__

//class OpenGLContext; /* forward decl ???? */


#include "main.h"



#include "shader.h"

#include "textures.h"

#include "meshes.h"

#include "ux.h"


//
//#include "ux-anim.h"

#include "FileChooserInclude.h"



#define MIN_FISHEYE_ZOOM 0.1f
#define FISHEYE_SLOW_ZOOM_THRESHOLD 0.1f
// you'd have to move 16.0 px to move 1px at FISHEYE_SLOW_ZOOM_MAX
#define FISHEYE_SLOW_ZOOM_MAX 0.9f // hopefully not use this <--
#define MAX_FISHEYE_ZOOM 64.0f


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

#include "minigames/_minigames.h"


public:

    typedef enum  {
        NO_ANIMATION,
        ANIMATION_ZOOM_INTO_DROPPER,
        ANIMATION_ZOOM_INTO_BULB,
        ANIMATION_ADD_COLOR,
        ANIMATION_FAILED_ADD_COLOR,
        ANIMATION_DEFAULT,
    } DROPPER_ANIMATION_ENUM;

    static OpenGLContext* Singleton();

    SDL_GLContext gl;

    SDL_Window *sdlWindow;
    bool createContext(SDL_Window *sdlWindow); // Creation of our OpenGL 3.x context
    void destroyContext();
    void prepareForHuePickerMode(bool fromHueGradient);
    void createUI(void);
    void setupScene(void); // All scene information can be setup here
    glm::vec3 worldToScreenSpace(glm::vec3 obj);
    glm::vec3 screenToWorldSpace(GLfloat screenx, GLfloat screeny, GLfloat distance);
    void reshapeWindow(int w, int h); // Method to get our window width and height on resize
    //void updateFrame(Uint32 elapsedMs);
    void renderScene(void); // Render scene (display method from previous OpenGL tutorials)

    void renderZoomedPickerBg(void);
    void renderUi(void);

    void setLight(void);

    void render3dDropper(float colorFillPercent);

    void eyedropperTestMatrix(float progress);
    void eyedropperAddColorMatrix(float progress);
    void eyedropperZoomDropperMatrix(float progress);
    void eyedropperZoomDropperBulbMatrix(float progress);


    void render3dDropperAnimation(void);
    void begin3dDropperAnimation(void);
    void begin3dDropperAnimation(int aDropperAnimation); // one of DROPPER_ANIMATION_ENUM
    void begin3dDropperAnimation(int aDropperAnimation, SDL_Color* aColor);

    void chooseFile(void);

    SDL_Window* getSdlWindow(void);

    SDL_Surface *fullPickImgSurface;
    SDL_Surface *colorPickerFGSurfaceGradient;
    SDL_Surface *lastTrueFullPickImgSurface;
    SDL_Color* lastHue;

    bool last_mode_hue_picker;
    int last_mode_position_x = 0; // this is non hue picker mode...
    int last_mode_position_y = 0;

    const float fill_requirement = 0.00001;


//    void keyDown(int key);
    void keyUp(SDL_Keycode key);
//    int totalKeys = 1073742052;
//    bool downkeys[1073742052]; // SDL tends to fire the keydown event more than once...
//    bool kkeys[1073742052];	// use this to keep track of keys...hopefully keyboard doesn't have more than this! (it does!)

    Shader *shader_3d; // Our GLSL shader
    Shader *shader_3d_Glass;
    Shader *shader_3d_unlit;
    Shader *shader_3d_ui;

    Shader *shader_lit_detail; // Our GLSL shader, rename
    Shader *shader_ui_shader_default;

    uniformLocationStruct* uniformLocations;

    GLuint textureNone = 0;

    GLuint	textureId_default,
            textureId_pickImage,
            textureId_fonts,
            textureId_pick_button,
            tex2_id;

    Textures* textures;

    Meshes* meshes;
    Mesh* eyedropper_bulb;
    Mesh* eyedropper_stem;
    Mesh* eyedropper_fill;

    Ux* generalUx;
    Minigames* minigames;

    uiInteraction pixelInteraction;

    // probably should not be public but easier this way!
    int mmovex, mmovey; /// needs to be ux accessible singleton.......


    bool no_more_shader_message_boxes;

    float fishEyeScale = 7.0f;
    float fishEyeScalePct = fishEyeScale / MAX_FISHEYE_ZOOM;
    // do not set directly....

    void updateFishScaleSlider();
    void updateFishScaleSliderRunner();
    void setFishScale(float modifierAmt, float scaler);
    static void setFishScalePerentage(Ux::uiObject *interactionObj, float percent); // anAnimationPercentCallback

    float getPixelMovementFactor();
    void triggerMovement();
    void triggerVelocity(float x, float y);
    void clearVelocity();

    //typedef void (*imageSelectedCallback)(const char *); // < its this type

    GLuint oggLoadTextureSized(SDL_Surface *surface, GLuint& contained_in_texture_id, GLuint& textureid, int size, int *x, int *y);
    void imageWasSelectedCb(SDL_Surface *myCoolSurface);
    bool restoreLastSurface();
    void loadNextTestImage();
    void loadNextTestImageByIndex(int index);
    void loadSpecificTestImage(const char* surfaceFilePath, int x, int y);
    void loadSpecificTestImage(const char* surfaceFilePath);


    int huePositionX(HSV_Color* color);
    int huePositionY(HSV_Color* color);

    void pickerForHue(HSV_Color* color, SDL_Color* desired_color);
    void pickerForHue(SDL_Color* color);
    void updateColorPreview(void);


    bool isMinigameMode(void);

    void reloadShaders(void);
    bool renderShouldUpdate = true;

    void doOpenURL(char* url);
    void doRequestReview();

private:

    unsigned int fbo;
    unsigned int rbo_color;
    unsigned int texColorBuffer;
    unsigned int rbo_depth;
    unsigned int texDepthBuffer;


    int textureSize = SHD_TEXTURE_SIZE;
    int halfTextureSize = textureSize * 0.5;
    int position_x = 0;
    int position_y = 0;
    int position_cropped_x = 0;
    int position_cropped_y = 0;
    float position_offset_x=0;
    float position_offset_y=0;
    int position_x_was = 0;
    int position_y_was = 0;

    bool setup_complete = false;

    int loadedImageMaxSize = 0;

    Ux::uiObject* rootUiObject; // there is a root ui object (we don't need a ref here really)

    Ux::uiList<const char*, Uint8>* textureList;
    Ux::uiListLoopingIterator<Ux::uiList<const char*, Uint8>, const char*>* testTexturesBuiltin;

    bool has_velocity = false;

    //float pan_friction = 0.9;

    float accumulated_velocity_x =0;
    float accumulated_velocity_y =0;
    // CONSIDER using glm::vec2
    float accumulated_movement_x =0;
    float accumulated_movement_y =0;

    int windowWidth; // Store the width of our window  - this is a dupe of var in color pick state!?
    int windowHeight; // Store the height of our window
    float widthHeightRatio;  // unused - see color pick state
    unsigned int cur_shader_id;

    void createSquare(void); // Method for creating our squares Vertex Array Object
    GLuint rect_triangleStripIndexBuffer;
    unsigned int rect_vaoID[1];
    unsigned int rect_vboID[4];

    //GLint globalLightLocation, ambientLightLoc, projectionMatrixLocation, viewMatrixLocation, modelMatrixLocation, textureSampler, textureSampler2, textureSampler3, outalpha, textOffset;

//    glm::mat4 modelMatrix; // Store the model matrix
//    glm::mat4 projectionMatrix; // Store the projection matrix

    glm::mat4 matrixModel;
    glm::mat4 matrixViews;
    glm::mat4 matrixPersp;

    glm::mat4 minigameCounterMatrix;

    Uint32 animationDropper3dStartTime;
    int animationDropper3dId;
    SDL_Color animationDropper3dColor;



protected:
    static bool ms_bInstanceCreated;
    static OpenGLContext* pInstance;

    void loadShaders(void);
    OpenGLContext(void); // Default constructor
    ~OpenGLContext(void); // Destructor for cleaning up our application


#ifdef DEVELOPER_TEST_MODE
    // the test functions can be removed from runtime codes...
    static void TEST_BEGIN(void);
    static Uint32 TEST_CALLBACK(Uint32 interval, void* parm);
    static void TEST_CAN_VIEW_ANY_COLOR(void);
#endif
    
};

//ends up mulitply defined (?? in future?)
static OpenGLContext* openglContext = OpenGLContext::Singleton();

const static float zoomRange = MAX_FISHEYE_ZOOM - MIN_FISHEYE_ZOOM;


#endif /* defined(_____PROJECTNAME_____ColorPick__) */
