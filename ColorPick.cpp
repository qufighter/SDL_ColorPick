//
//  ColorPick.c
//  ___PROJECTNAME___
//
//  Created by Sam Larison on 8/24/15.
//
//

#include "ColorPick.h"

//OpenGLContext::pInstance = NULL;
bool OpenGLContext::ms_bInstanceCreated = false;
OpenGLContext* OpenGLContext::pInstance = NULL;

OpenGLContext* OpenGLContext::Singleton() {
    if(!ms_bInstanceCreated){
        pInstance = new OpenGLContext();
        ms_bInstanceCreated=true;
    }
    return pInstance;
}

OpenGLContext::OpenGLContext(void) {
    //constructor
//    colorPickState->mmovex=0;
//    colorPickState->mmovey=0;

    textureNone=0;
    lastHue=nullptr;
    position_x = 0;
    position_y = 0;
}

bool OpenGLContext::createContext(SDL_Window *PsdlWindow) {

    sdlWindow = PsdlWindow;

    gl = SDL_GL_CreateContext(sdlWindow);

    if( gl == 0 ){
        printf("SDL_GL_CreateContext failed: %s\n", SDL_GetError());
        return false;
    }

    return true; // We have successfully created a context, return true
}

void OpenGLContext::destroyContext() {
     SDL_GL_DeleteContext(gl);
}

//void OpenGLContext::keyDown(int k){
//    if( k > 0 && k < totalKeys && !downkeys[k] ){
//        kkeys[k] = 1;
//        downkeys[k] = 1;
//    }
//}
void OpenGLContext::keyUp(SDL_Keycode k){
//    if( k > 0 && k < totalKeys ){
//        kkeys[k] = 0;
//        downkeys[k] = 0;
//    }

    SDL_Log("keyup %d", k);

    switch(k){
        case SDLK_UP:
            colorPickState->mmovey=1;
            break;
        case SDLK_DOWN:
            colorPickState->mmovey=-1;
            break;
        case SDLK_RIGHT:
            colorPickState->mmovex=-1;
            break;
        case SDLK_LEFT:
            colorPickState->mmovex=1;
            break;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            generalUx->addCurrentToPickHistory();
            break;
    }

    has_velocity=false;
    renderShouldUpdate=true;

}

void OpenGLContext::chooseFile(void) {

    //    OpenGLContext* ogg=OpenGLContext::Singleton();
    //    ogg->loadNextTestImage();

    loadNextTestImage(); // TODO maths remove this please

//  this is where we should proivide a callback...... circular import references exist
    beginImageSelector();
}

void OpenGLContext::updateColorPreview(void){
    //glClearColor(0.0f, 1.0f, 0.0f, 0.0f);

//    glClearColor(textures->selectedColor.r / 255.0f,
//                 textures->selectedColor.g / 255.0f,
//                 textures->selectedColor.b / 255.0f,
//                 textures->selectedColor.a / 255.0f); // IF WE DONT DO THIS WE WONT HAVE THE REFERENCES UP NEXT!

    generalUx->updateColorValueDisplay(&textures->selectedColor);

}

void OpenGLContext:: imageWasSelectedCb(SDL_Surface *myCoolSurface){

    if( lastHue!=nullptr ) SDL_free(lastHue);
    lastHue = nullptr;
    SDL_Log("an image was selected !!!!");

    if( myCoolSurface == NULL ) return;

    // free previous surface
    position_x = 0;
    position_y = 0;
    SDL_FreeSurface(fullPickImgSurface);

    // at least for ios we shold standardize this format now
    fullPickImgSurface = textures->ConvertSurface(myCoolSurface);
    loadedImageMaxSize = SDL_max(fullPickImgSurface->clip_rect.w, fullPickImgSurface->clip_rect.h);

    textures->LoadTextureSized(fullPickImgSurface, textureId_default, textureId_pickImage, textureSize, &position_x, &position_y, lastHue);

    // tada: also shrink image to be within some other texture.....
    //  then we can maintain magic when zoomed out...

    // TODO store a duplicate of the last surface - so when we move to hue picker we can move back easily???

    updateColorPreview();

    renderShouldUpdate = true;
    SDL_Log("really done loading new surface.... right?");
}

void OpenGLContext:: loadNextTestImage(){

    if( lastHue!=nullptr ) SDL_free(lastHue);
    lastHue = nullptr;
    SDL_Log("an image was selected !!!!");

    SDL_Surface *myCoolSurface = textures->LoadSurface(*testTexturesBuiltin->next());
    if( myCoolSurface == NULL ) return;

    // free previous surface
    position_x = 0;
    position_y = 0;
    SDL_FreeSurface(fullPickImgSurface);

    // at least for ios we shold standardize this format now using textures->ConvertSurface
    fullPickImgSurface = textures->ConvertSurface(myCoolSurface);

    if( fullPickImgSurface == NULL ) return;

    loadedImageMaxSize = SDL_max(fullPickImgSurface->clip_rect.w, fullPickImgSurface->clip_rect.h);

    textures->LoadTextureSized(fullPickImgSurface, textureId_default, textureId_pickImage, textureSize, &position_x, &position_y, lastHue);

    // tada: also shrink image to be within some other texture.....
    //  then we can maintain magic when zoomed out... (done?! textureId_default)

    // TODO store a duplicate of the last surface - so when we move to hue picker we can move back easily???

    updateColorPreview();

    renderShouldUpdate = true;

    SDL_Log("really done loading new surface.... right?");
}

void OpenGLContext:: pickerForHue(SDL_Color* color){

    lastHue = new SDL_Color();
    Ux::setColor(lastHue, color);
    SDL_Log("we wish to get a picker for a color hue !!!!"); // TODO: second arg for selecting color???? position_y ??position_x

    if( colorPickerFGSurfaceGradient == NULL ) return;

//    position_x = 0;
//    position_y = 0;

    SDL_FreeSurface(fullPickImgSurface);// free previous surface


    // at least for ios we shold standardize this format now using textures->ConvertSurface
    fullPickImgSurface = textures->ConvertSurface(SDL_DuplicateSurface(colorPickerFGSurfaceGradient));
    loadedImageMaxSize = SDL_max(fullPickImgSurface->clip_rect.w, fullPickImgSurface->clip_rect.h);

    textures->LoadTextureSized(fullPickImgSurface, textureId_default, textureId_pickImage, textureSize, &position_x, &position_y, lastHue);

    updateColorPreview();

    renderShouldUpdate = true;

    SDL_Log("really done loading new surface.... right?");

}

void OpenGLContext::createUI(void) {

    generalUx = Ux::Singleton(); // new Ux();
    rootUiObject = generalUx->create(); // if all create function are 1off... no ret needed?

    // we can now set this refernce from UX -> create
    generalUx->zoomSlider->setAnimationPercCallback(&OpenGLContext::setFishScalePerentage);

}

void OpenGLContext::setupScene(void) {

    createUI();
    setFishScale(0.0, 1.0);


    textures = new Textures();
 //glEnable (GL_DEPTH_TEST);
    // we may need to dynamify our shader paths too in case its in the bundle [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"vsh"];

    loadShaders();
    debugGLerror("shaders completely loaded");



    createSquare();

    debugGLerror("createSquare completely done");


    // todo - store texture paths and possibly resolve using [NSBundle mainBundle] pathForResource:@"256" ofType:@"png"]

//    textureId_default = textures->LoadTexture("textures/4.png"); // NOT SQUARE IMAGE WILL NOT WORK
//    textureId_default = textures->LoadTexture("textures/default.png");
//    textureId_pickImage = textures->LoadTexture("textures/snow.jpg");

//    textureId_default = textures->LoadTexture("textures/snow.jpg");
 //   textureId_pickImage = textures->LoadTexture("textures/default.png");
 //   textureId_default = textures->LoadTexture("textures/blank.png");


//    ColorPick iOS SDL(4046,0x10d290380) malloc: Heap corruption detected, free list is damaged at 0x600000101b00
//    *** Incorrect guard value: 4448617117
//    ColorPick iOS SDL(4046,0x10d290380) malloc: *** set a breakpoint in malloc_error_break to debug
    // todo: wrong number of items in list -> crash simulator
    Ux::uiList<const char*, Uint8>* textureList = new Ux::uiList<const char*, Uint8>(128);
    textureList->add("textures/4.png");
   // textureList->add("textures/simimage_NOEXIST.png");
    textureList->add("textures/ascii.png");
textureList->add("textures/simimage.png");
 //   textureList->add("textures/p04_shape1.bmp");
 //   textureList->add("textures/p10_shape1.bmp");
    textureList->add("textures/p10_shape1_rotated.png");
//    textureList->add("textures/p16_shape1.bmp");
//    textureList->add("textures/p04_shape1.bmp");
//    textureList->add("textures/p10_shape1.bmp");
//    textureList->add("textures/p16_shape1.bmp");
    
    textureList->add("textures/default.png"); // ok
    //textureList->add("textures/snow.jpg");
    textureList->add("textures/cp_bg.png");
    textureList->add("textures/SDL_logo.png"); // TODO: bad colors (in preview) (picks accurate colro?!)
    //textureList->add("textures/blank.png");
    textureList->add("textures/crate.jpg");
    textureList->add("textures/bark.jpg");
    textureList->add("textures/bark2.png"); // TODO fix this! png index bad colors (in preview) (picks accurate colro?!)
    textureList->add("textures/256.png"); // ok
    textureList->add("textures/512.png");  //TODO fix this!  png index! bad colors (in preview) (picks accurate colro?!)
    textureList->add("textures/unnamed.jpg");
    textureList->add("textures/DSC04958.JPG");
    textureList->add("textures/IMG_0172.jpg");
    textureList->add("textures/anim.gif");
    testTexturesBuiltin = new Ux::uiListLoopingIterator<Ux::uiList<const char*, Uint8>, const char*>(textureList);

    //fullPickImgSurface = textures->LoadSurface("textures/4.png");
    //fullPickImgSurface = textures->LoadSurface("textures/default.png"); // ok
    //fullPickImgSurface = textures->LoadSurface("textures/snow.jpg");
    //fullPickImgSurface = textures->LoadSurface("textures/simimage.png");
    //fullPickImgSurface = textures->LoadSurface("textures/SDL_logo.png"); // TODO: bad colors (in preview) (picks accurate colro?!)
    //fullPickImgSurface = textures->LoadSurface("textures/blank.png");
    //fullPickImgSurface = textures->LoadSurface("textures/unnamed.jpg");
    //fullPickImgSurface = textures->LoadSurface("textures/crate.jpg");
    //fullPickImgSurface = textures->LoadSurface("textures/bark.jpg");
    //fullPickImgSurface = textures->LoadSurface("textures/bark2.png"); // TODO fix this! png index bad colors (in preview) (picks accurate colro?!)
    //fullPickImgSurface = textures->LoadSurface("textures/256.png"); // ok
    //fullPickImgSurface = textures->LoadSurface("textures/512.png");  //TODO fix this!  png index! bad colors (in preview) (picks accurate colro?!)
    //fullPickImgSurface = textures->LoadSurface("textures/unnamed.jpg");
    //fullPickImgSurface = textures->LoadSurface("textures/DSC04958.JPG");
    //fullPickImgSurface = textures->LoadSurface("textures/IMG_0172.jpg");
    //fullPickImgSurface = textures->LoadSurface("textures/anim.gif");


    colorPickerFGSurfaceGradient = textures->LoadSurface("textures/cp_bg.png");
    fullPickImgSurface = textures->ConvertSurface(textures->LoadSurface(*testTexturesBuiltin->next()));
    loadedImageMaxSize = SDL_max(fullPickImgSurface->clip_rect.w, fullPickImgSurface->clip_rect.h);




// move this?!
// // we will try to automagically generate cp_bg.png - this failed since we are avoiding going pixel by piel and SDL_ComposeCustomBlendMode doesn't work with surfaces
//
//    SDL_Surface* temp1 = SDL_DuplicateSurface(colorPickerFGSurfaceGradient);
//    SDL_Surface* temp2 = SDL_DuplicateSurface(colorPickerFGSurfaceGradient);
//    SDL_Rect rect;
//
//    for( int row =0; row<colorPickerFGSurfaceGradient->h; row++ ){
//        rect.x = 0.0f;
//        rect.y = row + 0.0f;
//        rect.w = colorPickerFGSurfaceGradient->w;//*2;
//        rect.h = 1;
//        SDL_FillRect(temp1, &rect, SDL_MapRGBA(colorPickerFGSurfaceGradient->format, 255-row, 255-row, 255-row, row) );
//    }
//
//    for( int col =0; col<colorPickerFGSurfaceGradient->w; col++ ){
//        rect.x = col + 0.0f;
//        rect.y = 0.0f;
//        rect.w = 1;//*2;
//        rect.h = colorPickerFGSurfaceGradient->h;
//        SDL_FillRect(temp2, &rect, SDL_MapRGBA(colorPickerFGSurfaceGradient->format, 255-col, 255-col, 255-col, 255-col) );
//    }
//
//    SDL_SetSurfaceBlendMode(temp1, SDL_BLENDMODE_NONE);
//
//    rect.x = 0.0f;
//    rect.y = 0.0f;
//    rect.w = colorPickerFGSurfaceGradient->w;
//    rect.h = colorPickerFGSurfaceGradient->h;
//    int didBlit = SDL_BlitSurface(temp1,
//                                  &rect, // src rect entire surface
//                                  colorPickerFGSurfaceGradient,
//                                  &rect);
//
//
//    //SDL_SetSurfaceBlendMode(temp2, SDL_BLENDMODE_BLEND);
//    //SDL_SetSurfaceBlendMode(temp2, SDL_BLENDMODE_MOD);
//    SDL_SetSurfaceBlendMode(temp2, SDL_BLENDMODE_ADD);
//
//    // todo patch sdl so custom blend mode works on surface???
////    int success = SDL_SetSurfaceBlendMode(temp1,
////                            SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE,
////                                                     SDL_BLENDOPERATION_ADD,
////                                                     SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE,
////                                                     SDL_BLENDOPERATION_REV_SUBTRACT));
//
//
//    rect.x = 0.0f;
//    rect.y = 0.0f;
//    rect.w = colorPickerFGSurfaceGradient->w;
//    rect.h = colorPickerFGSurfaceGradient->h;
//    didBlit = SDL_BlitSurface(temp2,
//                                  &rect, // src rect entire surface
//                                  colorPickerFGSurfaceGradient,
//                                  &rect) && didBlit;
//
//
//    SDL_FreeSurface(temp2);
//    SDL_FreeSurface(temp1);





    textureId_default = textures->GenerateTexture();
    textureId_pickImage = textures->GenerateTexture();
    textures->LoadTextureSized(fullPickImgSurface, textureId_default, textureId_pickImage, textureSize, &position_x, &position_y, lastHue);



    updateColorPreview();

//    SDL_Color c;
//    c.r=255;
//    c.g=255;
//    c.b=0;
//    pickerForHue(&c);

    textureId_fonts = textures->LoadTexture("textures/ascii.png");

    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    //projectionMatrix = glm::perspective(60.0f, (float)windowWidth / (float)windowHeight, 0.1f, (float)VIEW_MAX_CLIP_DISTANCE);  // Create our perspective projection matrix


//    modelMatrix = glm::mat4(1.0f);
//
//    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, -2.0f));
    //modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 1.0f, 1.0f));

//    modelMatrix = glm::translate(modelMatrix, treepos + glm::vec3(0.0f, 0.0f, (float)i*bspace-bpos));
//    modelMatrix = glm::scale(modelMatrix, treeScale * pineTreeBranchScales[i]);

    debugGLerror("setupScene nearly done");


    glClearColor(0.0f, 1.0f, 0.0f, 0.0f);

    debugGLerror("setupScene completely done");


}

void OpenGLContext::loadShaders(void){
    GLuint _glVaoID;// shaders wont validate on osx unless we bind a VAO: Validation Failed: No vertex array object bound.
    glGenVertexArrays(1, &_glVaoID );	glBindVertexArray( _glVaoID );

//#ifdef COLORPICK_PLATFORM_DESKTOP
//    shader_lit_detail = new Shader("shaders/Shader.vsh", "shaders/Shader.fsh");
//#else
    shader_lit_detail = new Shader("shaders/Shader.vsh", "shaders/ShaderGLES.fsh");
//#endif

    shader_ui_shader_default = new Shader("shaders/uiShader.vsh", "shaders/uiShader.fsh");

    glDeleteVertexArrays(1, &_glVaoID );	_glVaoID = 0;
}

void OpenGLContext::reloadShaders(void){
    // reload just doesn't work - because the build files are not in teh same location
    shader_lit_detail->reload();
    shader_ui_shader_default->reload();
}

/**
 reshapeWindow is called every time our window is resized, and it sets our windowWidth and windowHeight
 so that we can set our viewport size.
 */
void OpenGLContext::reshapeWindow(int w, int h) {
    windowWidth = w; // Set the window width
    windowHeight = h; // Set the window height

    //projectionMatrix = glm::perspective(54.0f, (float)windowWidth / (float)windowWidth, 0.1f, (float)VIEW_MAX_CLIP_DISTANCE);  // Create our perspective projection matrix

    //widthHeightRatio = (windowWidth *1.0f) / windowHeight;


    glViewport(0, 0, windowWidth, windowHeight); // Set the viewport size to fill the window


    generalUx->updateStageDimension(windowWidth, windowHeight);

    renderShouldUpdate = true;

    //SDL_GL_MakeCurrent(sdlWindow, gl);
}


void  OpenGLContext::setFishScale(float modifierAmt, float scaler){
    // SDL_Log("%f", modifierAmt);
    modifierAmt *= scaler;

    if( modifierAmt < -0.5 ){
        modifierAmt=-0.5;
    }
    //if( fabs(modifierAmt) < 0.0001) return;
    fishEyeScale *= 1.0f + modifierAmt;
    if( fishEyeScale > MAX_FISHEYE_ZOOM ){
        fishEyeScale = MAX_FISHEYE_ZOOM;
    }else if( fishEyeScale < MIN_FISHEYE_ZOOM ){
        fishEyeScale = MIN_FISHEYE_ZOOM;
    }

    fishEyeScalePct = fishEyeScale / MAX_FISHEYE_ZOOM;
    //SDL_Log("FISH EYE SCALE %f", fishEyeScale);

        // todo should be a helper on the UI object zoomSlider instead

    generalUx->zoomSlider->updateAnimationPercent(fishEyeScalePct, 0.0);
}

//static for use as UI:: callback
void OpenGLContext::setFishScalePerentage(Ux::uiObject *interactionObj, float percent){
    SDL_Log("MMV PERC SLIDER %f %%", (percent));
    // from static function we must get instance

    OpenGLContext* myOglContext = OpenGLContext::Singleton();

        // todo need to use logscale or crop
    myOglContext->fishEyeScalePct = percent;


    myOglContext->fishEyeScale = MIN_FISHEYE_ZOOM + (zoomRange * myOglContext->fishEyeScalePct);

}

//static for use as UI:: callback
//void OpenGLContext::setFishScalePerentage(Ux::uiObject *interactionObj, float percent){
//    SDL_Log("MMV PERC SLIDER %f %%", (percent));
//    // from static function we must get instance
//
//    OpenGLContext* myOglContext = OpenGLContext::Singleton();
//
//    // todo need to use logscale or crop
//    myOglContext->fishEyeScalePct = percent;
//
//
//    myOglContext->fishEyeScale = MIN_FISHEYE_ZOOM + (zoomRange * myOglContext->fishEyeScalePct);
//    
//}

void OpenGLContext::triggerVelocity(float x, float y){
    velocity_x = x;
    velocity_y = y;
    has_velocity = true;
}
void OpenGLContext::clearVelocity(){
    has_velocity = false;
}

//void OpenGLContext::updateFrame(Uint32 elapsedMs) {
//    generalUx->updateAnimations(elapsedMs);
//}

/**
 renderScene will contain all our rendering code.
 Next up we are going to clear our COLOR, DEPTH and STENCIL buffers to avoid overlapping of renders.
 Any of your other rendering code will go here. Finally we are going to swap buffers.
 */
float velocityMin = 0.0000001f;

void OpenGLContext::renderScene(void) {

    debugGLerror("renderScene begin");


    if( has_velocity ){
        colorPickState->mmovex = velocity_x;
        colorPickState->mmovey = velocity_y;

        velocity_x *= pan_friction;
        velocity_y *= pan_friction;
        // set movex and movey accordingly
        if( fabs(velocity_y) < 0.01 && fabs(velocity_x) < 0.01 ){
            has_velocity = false;
            renderShouldUpdate=false;
        }
    }

    // update
    if(colorPickState->mmovex != 0 || colorPickState->mmovey != 0){
        position_x += colorPickState->mmovex;
        colorPickState->mmovex=0;
        position_y += colorPickState->mmovey;
        colorPickState->mmovey=0;

        //SDL_Log("position before %d %d" , position_x, position_y);

        // pass a reference to the posiiton - then use that, rather than above
        // since textureId_default is not changing..... we should omit it here... (we keep moving it, but we can move it in shader...) // maths
        // can also just omit when zoomed. - excep when u can see it... so near edges it does not work // fishEyeScale < 3.0f ? textureId_default : textureNone
        //textures->LoadTextureSized(fullPickImgSurface, textureId_default, textureId_pickImage, textureSize, &position_x, &position_y, lastHue);

        textures->LoadTextureSized(fullPickImgSurface, textureNone, textureId_pickImage, textureSize, &position_x, &position_y, lastHue);

// lets figure out how to maintain this position into the shader for the full image texture...

        updateColorPreview();

    }else{
        renderShouldUpdate=false;
    }
    // then re-crop our source texture properly




    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
    debugGLerror("renderScene glClear");

    uniformLocations = shader_lit_detail->bind(); // Bind our shader
    cur_shader_id = shader_lit_detail->id();

//
//    uniformLocations->modelMatrixLocation = glGetUniformLocation(cur_shader_id, "modelMatrix");
//    uniformLocations->viewMatrixLocation = glGetUniformLocation(cur_shader_id, "viewMatrix");
//    uniformLocations->projectionMatrixLocation = glGetUniformLocation(cur_shader_id, "projectionMatrix");
//    uniformLocations->fishScale = glGetUniformLocation(cur_shader_id, "fishEyeScale");
//    uniformLocations->textureSampler = glGetUniformLocation(cur_shader_id, "texture1");
//    uniformLocations->textureSampler2 = glGetUniformLocation(cur_shader_id, "texture2");
//    uniformLocations->textureSampler3 = glGetUniformLocation(cur_shader_id, "texture3");
//    uniformLocations->textureCoord = glGetAttribLocation(cur_shader_id, "TexCoordIn");
//    uniformLocations->normalLightingMat = glGetAttribLocation(cur_shader_id, "lightingMat");


 //   modelMatrix = glm::translate(modelMatrix, glm::vec3(0.2f, 0.0f, 0.0f));


//    glEnable(GL_BLEND);  //this enables alpha blending, important for faux shader
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



    glUniform1f(uniformLocations->fishScale, fishEyeScale);
    glUniform1f(uniformLocations->fishScalePct, fishEyeScalePct);
    glUniform1f(uniformLocations->textureWidth, (float)textureSize);

    glUniform2f(uniformLocations->positionOffset, (float)position_x / -loadedImageMaxSize, (float)position_y/ -loadedImageMaxSize);
    //glUniform2f(uniformLocations->positionOffset, 0.1f, 0.1f);

//    glUniform4f(uniformLocations->ui_foreground_color,
//                renderObj->foregroundColor.r/255.0,
//                renderObj->foregroundColor.g/255.0,
//                renderObj->foregroundColor.b/255.0,
//                renderObj->foregroundColor.a/255.0
//                );

    glUniform1f(uniformLocations->widthHeightRatio, colorPickState->viewport_ratio);

//    glUniformMatrix4fv(uniformLocations->modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
//    glUniformMatrix4fv(uniformLocations->projectionMatrixLocation,	1, GL_FALSE, &projectionMatrix[0][0]); // Send our projection matrix to the shader

    glUniform1i(uniformLocations->textureSampler, 0);
    glUniform1i(uniformLocations->textureSampler2, 1);
    //glUniform1i(uniformLocations->textureSampler3, 3);

    debugGLerror("renderScene glUniform done");


    glActiveTexture( GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, textureId_pickImage);

    glActiveTexture( GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D,  textureId_default);

    glActiveTexture( GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D,  textureId_fonts); // TODO -  well its bound now, wh y not leave this in the main loop for no reason?


    debugGLerror("renderScene textuers bound");


    glDisable(GL_BLEND);

    debugGLerror("renderScene glDisable(GL_BLEND");



    glBindVertexArray(rect_vaoID[0]); // Bind our Vertex Array Object GL_INVALID_OPERATION (except android?)
    debugGLerror("renderScene glBindVertexArray(rect_vaoID");
//
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rect_triangleStripIndexBuffer); // its already bound ?
//        debugGLerror("renderScene glBindBuffer(GL_ELEMENT_ARRAY_BUFFER");
//


    glDrawArrays(GL_TRIANGLES, 0, 6);

    debugGLerror("renderScene our renderinng done");


    /* NEXT UP: RENDER UI */
    uniformLocations = shader_ui_shader_default->bind(); // Bind our shader


    debugGLerror("renderScene ui shader bound");


    glUniform1i(uniformLocations->textureSampler, 0);
    //glUniform1i(uniformLocations->textureSampler2, 1);
   // glUniform1i(uniformLocations->textureSampler3, 1);

    debugGLerror("renderScene ui texture uniform set");


    glActiveTexture( GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D,  textureId_fonts); // well its bound now, wh y not leave this in the main loop for no reason?

    debugGLerror("renderScene ui textureId_fonts bound");


    // // Send our global light to the shader

    glEnable(GL_BLEND);  //this enables alpha blending, important for faux shader
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    debugGLerror("renderScene glEnable GL_BLEND");


    //glDrawArrays(GL_TRIANGLES, 0, 6);

    debugGLerror("generalUx->renderObject begin");
    generalUx->renderObject(uniformLocations); // renders all obj
    debugGLerror("generalUx->renderObject ending");

}


void OpenGLContext::createSquare(void) {
    float *vertices = new float[18];	// Vertices for our square
    float *colors = new float[18]; // Colors for our vertices
    float* texCoord = new float[12]; // Colors for our vertices
    float* normals = new float[18]; // Colors for our vertices

    srand ( (unsigned int)(time(NULL)) );
    float c1=0;//(float)(rand() % 10);
    float c2=0;//(float)(rand() % 10);
    float c3=0;//(float)(rand() % 10);
    float c4=0;//(float)(rand() % 10);

    GLuint squareTriangleIndicies[4];
    squareTriangleIndicies[0]=0;
    squareTriangleIndicies[1]=1;
    squareTriangleIndicies[2]=3;
    squareTriangleIndicies[3]=2;

    glGenBuffers(1, &rect_triangleStripIndexBuffer); // when using glDrawElements on a "core" context you can't store the indicies in ram
    debugGLerror("new square - glGenBuffers");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rect_triangleStripIndexBuffer);
    debugGLerror("new square - glBindBuffer");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareTriangleIndicies), squareTriangleIndicies, GL_STATIC_DRAW);
    debugGLerror("new square - glBufferData");

    int n=-1;
    float sq_size = 0.725;
    sq_size = 0.985;
    //sq_size = 0.998;
    sq_size = 1.0;

    vertices[0] = -sq_size; vertices[1] = -sq_size; vertices[2] = c1; // Bottom left corner
    colors[0] = 1.0; colors[1] = 1.0; colors[2] = 1.0; // Bottom left corner
    texCoord[0] = 0.0; texCoord[1] = 1.0;
    normals[++n] = 0.0;normals[++n] = 0.0;normals[++n] = -1.0;

    vertices[3] = -sq_size; vertices[4] = sq_size; vertices[5] = c3; // Top left corner
    colors[3] = 1.0; colors[4] = 0.0; colors[5] = 0.0; // Top left corner
    texCoord[2] = 0.0; texCoord[3] = 0.0;
    normals[++n] = 0.0;normals[++n] = 0.0;normals[++n] = -1.0;

    vertices[6] = sq_size; vertices[7] = sq_size; vertices[8] = c2; // Top Right corner
    colors[6] = 0.0; colors[7] = 1.0; colors[8] = 0.0; // Top Right corner
    texCoord[4] = 1.0; texCoord[5] = 0.0;
    normals[++n] = 0.0;normals[++n] = 0.0;normals[++n] = -1.0;

    vertices[9] = sq_size; vertices[10] = -sq_size; vertices[11] = c4; // Bottom right corner
    colors[9] = 0.0; colors[10] = 0.0; colors[11] = 1.0; // Bottom right corner
    texCoord[6] = 1.0; texCoord[7] = 1.0;
    normals[++n] = 0.0;normals[++n] = 0.0;normals[++n] = -1.0;

    vertices[12] = -sq_size; vertices[13] = -sq_size; vertices[14] = c1; // Bottom left corner
    colors[12] = 1.0; colors[13] = 1.0; colors[14] = 1.0; // Bottom left corner
    texCoord[8] = 0.0; texCoord[9] = 1.0;
    normals[++n] = 0.0;normals[++n] = 0.0;normals[++n] = -1.0;

    vertices[15] = sq_size; vertices[16] = sq_size; vertices[17] = c2; // Top Right corner
    colors[15] = 0.0; colors[16] = 1.0; colors[17] = 0.0; // Top Right corner
    texCoord[10] = 1.0; texCoord[11] = 0.0;
    normals[++n] = 0.0;normals[++n] = 0.0;normals[++n] = -1.0;


    glGenVertexArrays(1, &rect_vaoID[0]); // Create our Vertex Array Object

    glBindVertexArray(rect_vaoID[0]); // Bind our Vertex Array Object so we can use it

    glGenBuffers(4, &rect_vboID[0]); // Generate our two Vertex Buffer Object


    glBindBuffer(GL_ARRAY_BUFFER, rect_vboID[0]); // Bind our Vertex Buffer Object
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), vertices, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
    glVertexAttribPointer((GLuint)SHADER_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
    glEnableVertexAttribArray(SHADER_POSITION); // Enable the first our Vertex Array Object

    //you can comment out this block, we dont use colors for rects
//    glBindBuffer(GL_ARRAY_BUFFER, rect_vboID[1]); // Bind our second Vertex Buffer Object
//    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), colors, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
//    glVertexAttribPointer((GLuint)SHADER_COLOR, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
//    glEnableVertexAttribArray(SHADER_COLOR); // Enable the second vertex attribute array

    glBindBuffer(GL_ARRAY_BUFFER, rect_vboID[2]); // Bind our second Vertex Buffer Object
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), texCoord, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
    glVertexAttribPointer((GLuint)SHADER_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
    glEnableVertexAttribArray(SHADER_TEXTURE); // Enable the second vertex attribute array

    //you can comment out this block we dn't use normals for rendering rects
//    glBindBuffer(GL_ARRAY_BUFFER, rect_vboID[3]); // Bind our second Vertex Buffer Object
//    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), normals, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
//    glVertexAttribPointer((GLuint)SHADER_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
//    glEnableVertexAttribArray(SHADER_NORMAL); // Enable the second vertex attribute array


    
    glBindVertexArray(0); // Disable our Vertex Buffer Object
    delete [] vertices; // Delete our vertices from memory
    delete [] colors; // Delete our vertices from memory 
    delete [] texCoord;
    delete [] normals;
}
