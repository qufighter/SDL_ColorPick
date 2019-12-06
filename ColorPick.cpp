//
//  ColorPick.c
//  ___PROJECTNAME___
//
//  Created by Sam Larison on 8/24/15.
//
//

#include "ColorPick.h"

// so far no luck on android with this... some missing defines!
// note this is broken on IOS currently... even though "its supposidly comlpete..." - any time we bind buffer 0 it breaks rendering though...
#define USE_FBO_FOR_RENDERING 0


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

    setup_complete=false;
    textureNone=0;
    lastHue=nullptr;
    position_x = 0;
    position_y = 0;
    last_mode_hue_picker = false;
    lastTrueFullPickImgSurface=nullptr;
    no_more_shader_message_boxes=false;

    pixelInteraction.friction=6.9;
    pixelInteraction.useInstantaneousVelocity=true;

    // lets get our singleton references loaded now... (ux not yet ??)
    meshes = Meshes::Singleton();
    textures = Textures::Singleton();
}

SDL_Window* OpenGLContext::getSdlWindow(){
    return sdlWindow;
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

void OpenGLContext::keyDown(Uint32 timestamp, SDL_Keycode k){

    keyInteractions.keyDown(timestamp, k);

    if (k == SDLK_AC_BACK || k == SDLK_BACKSPACE || k == SDLK_ESCAPE){
        //SDL_Log("back/esc pressed");
        BackButtonEvent();  // CONSIDER MOVING TO KEYUP??? TEST IT (android) (applies to minigame and non minigame modes!)
    }else if(k == SDLK_AC_HOME){
        // nope only works on windows..... maybe android, but how to screenshot android?
        //SDL_Log("home pressed - maybe ios screenshot? time for gimicky marketing ploy");
    }

    has_velocity=false;
    renderShouldUpdate=true;
}
void OpenGLContext::keyUp(Uint32 timestamp, SDL_Keycode k){

    keyInteractions.keyUp(timestamp, k);

    if( isMinigameMode() ){

        //minigames->keyDown(k);
        
        //renderShouldUpdate=true;
    }else{

        switch(k){
            case SDLK_RETURN:
            case SDLK_KP_ENTER:
                if(keyInteractions.enter->wasNotCanceledByLaterKeypress()){
                    EnterKeyEvent();
                }
                break;
#ifdef COLORPICK_DEBUG_MODE
            case SDLK_p:
                textures->screenshot("test-snap.png", windowWidth, windowHeight);
                break;
#endif
        }

        has_velocity=false;
        renderShouldUpdate=true;
    }

}

void OpenGLContext::EnterKeyEvent(){
    if( isMinigameMode() ){

    }else{
        if( setup_complete ){ // this check is specifically to guard the enter key durign shader compilation error messaage box...
            generalUx->addCurrentToPickHistory();
        }
    }
}

void OpenGLContext::BackButtonEvent(){
    if( generalUx->hasCurrentModal() ){
        generalUx->endCurrentModal();
    }else{
#ifdef __ANDROID__
        //SDL_AndroidBackButton();
        SDL_MinimizeWindow(sdlWindow);
#endif
    }
}

void OpenGLContext::chooseFile(void) {
#ifdef DEVELOPER_TEST_MODE
    loadNextTestImage();
#else
    beginImageSelector();
#endif
}

void OpenGLContext::updateColorPreview(void){
    //glClearColor(0.0f, 1.0f, 0.0f, 0.0f);

    glClearColor(textures->selectedColor.r / 255.0f,
                 textures->selectedColor.g / 255.0f,
                 textures->selectedColor.b / 255.0f,
                 1.0f);

    generalUx->updateColorValueDisplay(&textures->selectedColor);

}

bool OpenGLContext:: restoreLastSurface(){
    if( lastTrueFullPickImgSurface!= nullptr ){
        imageWasSelectedCb(lastTrueFullPickImgSurface);
        return true;
    }else{
        SDL_Log("Error - there is no image for this!");
    }
    return false;
}

// can we move this to main.h? it is used on ios
static Uint32 render_one_more_frame(Uint32 interval, void* parm){
    SDL_Event event;
    SDL_UserEvent userevent;
    userevent.type = SDL_USEREVENT;
    userevent.code = USER_EVENT_ENUM::RENDER_VIEW_AGAIN;
    event.type = SDL_USEREVENT;
    event.user = userevent;
    SDL_PushEvent(&event);
    return 0; // end timer
}

GLuint OpenGLContext::oggLoadTextureSized(SDL_Surface *surface, GLuint& contained_in_texture_id, GLuint& textureid, int size, int *x, int *y) {
    GLuint result = textures->LoadTextureSized(surface, contained_in_texture_id, textureid, size, x, y);
    position_cropped_x = position_x;
    position_cropped_y = position_y;
    position_offset_x=0;
    position_offset_y=0;
    return result;
}

void OpenGLContext:: imageWasSelectedCb(SDL_Surface *myCoolSurface){

    if( lastHue!=nullptr ) SDL_free(lastHue);
    lastHue = nullptr;
    //SDL_Log("an image was selected !!!!");

    if( myCoolSurface == NULL ){
        generalUx->defaultScoreDisplay->displayExplanation("  Read Error");
        generalUx->printCharToUiObject(generalUx->defaultScoreDisplay->explanation->childList[0], CHAR_CANCEL_ICON, DO_NOT_RESIZE_NOW);
        return;
    }

    // free previous surface
    position_x = 0;
    position_y = 0;
    SDL_FreeSurface(fullPickImgSurface);

    if( lastTrueFullPickImgSurface != nullptr ){
        // this is a tricky call - if the surface is different we should free it now.... otherwise the arg to this is free'd later
        if( lastTrueFullPickImgSurface != myCoolSurface ){
            SDL_FreeSurface(lastTrueFullPickImgSurface);
        }else{
            // we are restoring, so lets also restore position
            position_x = last_mode_position_x;
            position_y = last_mode_position_y;
        }
        lastTrueFullPickImgSurface = nullptr;
    }
    last_mode_hue_picker = false;
    generalUx->returnToLastImgBtn->hideAndNoInteraction();
    generalUx->huePicker->hideHueSlider();

    // at least for ios we shold standardize this format now
    fullPickImgSurface = textures->ConvertSurface(myCoolSurface);
    loadedImageMaxSize = SDL_max(fullPickImgSurface->clip_rect.w, fullPickImgSurface->clip_rect.h);

    oggLoadTextureSized(fullPickImgSurface, textureId_default, textureId_pickImage, textureSize, &position_x, &position_y);

    // tada: also shrink image to be within some other texture.....
    //  then we can maintain magic when zoomed out...

    // TODO store a duplicate of the last surface - so when we move to hue picker we can move back easily???

    updateColorPreview();

    renderShouldUpdate = true;
    //SDL_Log("really done loading new surface.... right?");
#ifdef __IPHONEOS__
    SDL_AddTimer(250, render_one_more_frame, nullptr);
#endif
}

void OpenGLContext:: loadNextTestImage(){
    loadSpecificTestImage(*testTexturesBuiltin->next());
}

void OpenGLContext:: loadNextTestImageByIndex(int index){
    const char*  result = *textureList->get(index);
    if( result != nullptr ){
        loadSpecificTestImage(result);
    }
}

void OpenGLContext:: loadSpecificTestImage(const char* surfaceFilePath){

    if( lastHue!=nullptr ) SDL_free(lastHue);
    lastHue = nullptr;
    //SDL_Log("an image was selected !!!!");

    SDL_Surface *myCoolSurface = textures->LoadSurface(surfaceFilePath);
    if( myCoolSurface == NULL ) return;

    // free previous surface

    SDL_FreeSurface(fullPickImgSurface);
    if( lastTrueFullPickImgSurface != nullptr ){
        SDL_FreeSurface(lastTrueFullPickImgSurface);
        lastTrueFullPickImgSurface = nullptr;
    }
    last_mode_hue_picker = false;
    generalUx->returnToLastImgBtn->hideAndNoInteraction();
    generalUx->huePicker->hideHueSlider();

    // at least for ios we shold standardize this format now using textures->ConvertSurface
    fullPickImgSurface = textures->ConvertSurface(myCoolSurface);

    position_x = 0;
    position_y= (fullPickImgSurface->h * 0.5) - 513;

    // play animation for htis img...
    begin3dDropperAnimation();

    if( fullPickImgSurface == NULL ) return;

    loadedImageMaxSize = SDL_max(fullPickImgSurface->clip_rect.w, fullPickImgSurface->clip_rect.h);

    oggLoadTextureSized(fullPickImgSurface, textureId_default, textureId_pickImage, textureSize, &position_x, &position_y);

    // tada: also shrink image to be within some other texture.....
    //  then we can maintain magic when zoomed out... (done?! textureId_default)

    // TODO store a duplicate of the last surface - so when we move to hue picker we can move back easily???

    updateColorPreview();

    renderShouldUpdate = true;

    //SDL_Log("really done loading new surface.... right?");
}

int OpenGLContext::huePositionX(HSV_Color* color){
    int cp_x=(((color->s)/100.0)*255.0);
    //SDL_Log("current position x %i <--- computed", cp_x );
    return (-cp_x) + 127;
}

int OpenGLContext::huePositionY(HSV_Color* color){
    int cp_y=(((100.0-color->v)/100.0)*255.0);
    //SDL_Log("current position y %i <--- computed", cp_y );
    return (-cp_y) + 127;
}

void OpenGLContext::pickerForHue(HSV_Color* color, SDL_Color* desired_color){

    prepareForHuePickerMode(false);

    int ix = position_x; // used for logging debug only....
    int iy = position_y;

    position_x = huePositionX(color);
    position_y = huePositionY(color);

    SDL_Log("moved by x/y %i/%i <--- computed", ix-position_x, iy-position_y );


    pickerForHue(generalUx->huePicker->colorForPercent(1.0-(color->h/360.0)));
    //    position_x = 0;
    //    position_y = 0;

    if( textures->searchSurfaceForColor(fullPickImgSurface, desired_color, position_x, position_y, &position_x, &position_y) ){

            // this is a little overkill we just need to move the position....
            //pickerForHue(generalUx->huePicker->colorForPercent(1.0-(color->h/360.0)));
            oggLoadTextureSized(fullPickImgSurface, textureNone, textureId_pickImage, textureSize, &position_x, &position_y);
            updateColorPreview();
            renderShouldUpdate = true;

//        SDL_Log(" - - -  zeroing in attempt 2 now ------ - - - - ");
//        if( textures->searchSurfaceForColor(fullPickImgSurface, desired_color, position_x, position_y, &position_x, &position_y) ){
//
//            // this is a little overkill we just need to move the position....
//            //pickerForHue(generalUx->huePicker->colorForPercent(1.0-(color->h/360.0)));
//            oggLoadTextureSized(fullPickImgSurface, textureNone, textureId_pickImage, textureSize, &position_x, &position_y);
//            updateColorPreview();
//            renderShouldUpdate = true;
//
//        }
        float matchDistance = textures->directionalScan->lastBestDistance;
        SDL_Point lastOffset = textures->directionalScan->lastCoordinateReturned;
        float movementDist = glm::distance(glm::vec2(0,0) , glm::vec2(lastOffset.x,lastOffset.y));

        if( matchDistance == 0 ){
            // exact match, after moving
            generalUx->defaultScoreDisplay->display(generalUx->returnToLastImgBtn, movementDist, SCORE_EFFECTS::NOMOVE);
            generalUx->defaultScoreDisplay->displayExplanation(" [ ] ");
        }else{
            generalUx->defaultScoreDisplay->display(generalUx->returnToLastImgBtn, matchDistance * 14, SCORE_EFFECTS::NOMOVE);
            //generalUx->defaultScoreDisplay->displayExplanation("*in-exact!!");
            generalUx->defaultScoreDisplay->displayAchievement(Ux::uiSettingsScroller::UI_ACHEIVEMENT_INEXACT);
        }
        // next up - modulate scoring based on exactness... further off more points....

    }else{
        //  no need to move at all!!!!

        float matchDistance = textures->directionalScan->lastBestDistance;
        // rare spot on guess....
        if( matchDistance == 0 ){
            generalUx->defaultScoreDisplay->display(generalUx->returnToLastImgBtn, 1, SCORE_EFFECTS::NOMOVE);
            generalUx->defaultScoreDisplay->displayExplanation(" > < ");
        }else{
            generalUx->defaultScoreDisplay->display(generalUx->returnToLastImgBtn, matchDistance * 14, SCORE_EFFECTS::NOMOVE);
            //generalUx->defaultScoreDisplay->displayExplanation("*in-exact!!");
            generalUx->defaultScoreDisplay->displayAchievement(Ux::uiSettingsScroller::UI_ACHEIVEMENT_INEXACT);
        }
    }
}

void OpenGLContext:: pickerForHue(SDL_Color* color){

    if( lastHue!=nullptr ) SDL_free(lastHue);
    //lastHue = nullptr;
    lastHue = new SDL_Color();
    Ux::setColor(lastHue, color);
    //SDL_Log("we wish to get a picker for a color hue !!!!"); // TODO: second arg for selecting color???? position_y ??position_x

    if( colorPickerFGSurfaceGradient == NULL ) return;

//    position_x = 0;
//    position_y = 0;


    prepareForHuePickerMode(true); // this is no-op if we are already in mode

    // show the button to return now....
    generalUx->returnToLastImgBtn->showAndAllowInteraction();
    generalUx->updateModal(generalUx->returnToLastImgBtn, &Ux::interactionReturnToPreviousSurface);

    generalUx->huePicker->showHueSlider();


    //SDL_Log("current position x/y %i/%i", position_x, position_y );

    SDL_FreeSurface(fullPickImgSurface);// free previous surface




    // at least for ios we shold standardize this format now using textures->ConvertSurface
    fullPickImgSurface = textures->ConvertSurface(SDL_DuplicateSurface(colorPickerFGSurfaceGradient), lastHue);
    loadedImageMaxSize = SDL_max(fullPickImgSurface->clip_rect.w, fullPickImgSurface->clip_rect.h);


    oggLoadTextureSized(fullPickImgSurface, textureId_default, textureId_pickImage, textureSize, &position_x, &position_y); // todo we could remove this backgroundColor arg now maybe?

    updateColorPreview();

    renderShouldUpdate = true;

    //SDL_Log("really done loading new surface.... right?");

}

void OpenGLContext::prepareForHuePickerMode(bool fromHueGradient) {
    if( !last_mode_hue_picker ){

        last_mode_position_x = position_x;
        last_mode_position_y = position_y;

        //SDL_Log("current position was x/y %i/%i fisheye: %f ", position_x, position_y, fishEyeScalePct );

        // in general if we were NOT picking hue before, certain x/y should be avoided... since it is confusing to be greeted with a solid black screen
        // also possibly certain zoom should be avoided.....
        // if last_mode_hue_picker though - the rules enforced here should be skipped (except maybe zoom?)

        // we can try to approximate a good S/V value based on previous color... we still apply constraints
        // score opportunity for picking right hue?

        if( fromHueGradient ){
            HSV_Color lastSelection;
            lastSelection.fromColor(&textures->selectedColor);
            position_x = huePositionX(&lastSelection);
            position_y = huePositionY(&lastSelection);
            if( lastHue != nullptr ){
                Uint16 pickedHue = lastSelection.h;
                lastSelection.fromColor(lastHue);
                int hueDis = SDL_abs(pickedHue - lastSelection.h);
                if( hueDis < 5 ){
                    generalUx->defaultScoreDisplay->display(generalUx->returnToLastImgBtn, 10 * (5-hueDis), SCORE_EFFECTS::NOMOVE);
                    //generalUx->defaultScoreDisplay->displayExplanation("Right Hue're");
                    generalUx->defaultScoreDisplay->displayAchievement(Ux::uiSettingsScroller::UI_ACHEIVEMENT_RIGHT_HUE);
                }
            }
        }

        // 50 cutofff (when entering hsv) not dark/light enough
//        if( position_x > 0 ){
//            position_x = 0;
//        }
//        if( position_y < 0 ){
//            position_y = 0;
//        }

        if( fishEyeScalePct > 0.1 ){
            setFishScalePerentage(nullptr, 0.1);
        }

        lastTrueFullPickImgSurface = SDL_DuplicateSurface(fullPickImgSurface);
        last_mode_hue_picker = true;

        generalUx->uxAnimations->scale_bounce(generalUx->returnToLastImgBtn, 0.006);
    }
}

void OpenGLContext::createUI(void) {
    generalUx = Ux::Singleton(); // new Ux();
    rootUiObject = generalUx->create(); // if all create function are 1off... no ret needed?
    /*minigames = */ new Minigames(); // this sets the reference on ogg automatically... commented out pointlessly but also because its already assigned.. neat right?
}

void OpenGLContext::setupScene(void) {
//
//    glEnable(GL_DEPTH_TEST);
//    glDepthMask(GL_TRUE);

    createUI();

    setFishScale(0.0, 1.0);

    matrixModel = glm::mat4(1.0f);
    matrixViews = glm::mat4(1.0f);
    matrixPersp = glm::mat4(1.0f);

    matrixViews = glm::lookAt(glm::vec3(0.0,0.0,1.0), glm::vec3(0.0,0.0,0.0), glm::vec3(0.0,1.0,0.0)); // eye, center, up

    minigameCounterMatrix = glm::mat4(1.0f);
    minigameCounterMatrix = glm::rotate(minigameCounterMatrix, -45.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    minigameCounterMatrix = glm::translate(minigameCounterMatrix, glm::vec3(0.0f, 2.3f, 9.0f));


 //glEnable (GL_DEPTH_TEST);
    // we may need to dynamify our shader paths too in case its in the bundle [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"vsh"];



    loadShaders();
    debugGLerror("shaders completely loaded");

    square_mesh = createSquare();
    debugGLerror("createSquare completely done");

//    eyedropper_bulb=meshes->LoadObjectSTL("textures/models/eyedropper_bulb.stl");
//    eyedropper_stem=meshes->LoadObjectSTL("textures/models/eyedropper_stem.stl");

//    eyedropper_bulb = meshes->LoadObjectPLY("textures/models/eyedroppe_bulb.ply");
//    eyedropper_stem = meshes->LoadObjectPLY("textures/models/eyedroppe_stem.ply");

    eyedropper_bulb = meshes->LoadObjectPLY("textures/models/eyedropper2.2_bulb.ply");
    eyedropper_stem = meshes->LoadObjectPLY("textures/models/eyedropper2.2_stem.ply");
    eyedropper_fill = meshes->LoadObjectPLY("textures/models/eyedropper2.2_fill.ply");

//    eyedropper_bulb->color_additive = glm::vec4(0.0,0.0,0.0,1.0);
//    //eyedropper_stem->color_additive = glm::vec4(0.0,0.0,1.0,0.25);
//    eyedropper_stem->color_additive = glm::vec4(0.0,0.0,0.0,0.25);

    eyedropper_bulb->color_additive = glm::vec4(0.1,0.1,0.1,1.0);
    eyedropper_stem->color_additive = glm::vec4(1.0,1.0,1.0,0.5);


    debugGLerror("meshes load completely done");

    // todo - store texture paths and possibly resolve using [NSBundle mainBundle] pathForResource:@"256" ofType:@"png"]

//    textureId_default = textures->LoadTexture("textures/4.png"); // NOT SQUARE IMAGE WILL NOT WORK
//    textureId_default = textures->LoadTexture("textures/default.png");
//    textureId_pickImage = textures->LoadTexture("textures/snow.jpg");

//    textureId_default = textures->LoadTexture("textures/snow.jpg");
 //   textureId_pickImage = textures->LoadTexture("textures/default.png");
 //   textureId_default = textures->LoadTexture("textures/blank.png");


//    ColorPick SDL(4046,0x10d290380) malloc: Heap corruption detected, free list is damaged at 0x600000101b00
//    *** Incorrect guard value: 4448617117
//    ColorPick SDL(4046,0x10d290380) malloc: *** set a breakpoint in malloc_error_break to debug
    // todo: wrong number of items in list -> crash simulator
    textureList = new Ux::uiList<const char*, Uint8>(25);
    textureList->add("textures/intro-image.png");
    //textureList->add("textures/4.png");

    // to use these, after build, copy the additional textures folder test_textures into texutres........
#ifdef DEVELOPER_TEST_MODE
   // textureList->add("textures/simimage_NOEXIST.png");
    textureList->add("textures/ascii.png");
    textureList->add("textures/test_textures/anim.gif");  // TODO: broken on android...
    textureList->add("textures/test_textures/simimage.png");
 //   textureList->add("textures/p04_shape1.bmp");
 //   textureList->add("textures/p10_shape1.bmp");
    textureList->add("textures/test_textures/p10_shape1_rotated.png");
//    textureList->add("textures/p16_shape1.bmp");
//    textureList->add("textures/p04_shape1.bmp");
//    textureList->add("textures/p10_shape1.bmp");
//    textureList->add("textures/p16_shape1.bmp");
    textureList->add("textures/test_textures/default.png"); // ok
    //textureList->add("textures/snow.jpg");
    textureList->add("textures/cp_bg.png");
    textureList->add("textures/test_textures/SDL_logo.png"); // TODO: bad colors (in preview) (picks accurate colro?!)
    //textureList->add("textures/blank.png");
    textureList->add("textures/test_textures/crate.jpg");
    textureList->add("textures/test_textures/bark.jpg");
    textureList->add("textures/test_textures/bark2.png"); // TODO fix this! png index bad colors (in preview) (picks accurate colro?!)
    textureList->add("textures/test_textures/256.png"); // ok
    textureList->add("textures/test_textures/512.png");  //TODO fix this!  png index! bad colors (in preview) (picks accurate colro?!)
    textureList->add("textures/test_textures/unnamed.jpg");
    textureList->add("textures/test_textures/DSC04958.JPG");
    textureList->add("textures/test_textures/IMG_0172.jpg");
#endif

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

    //SDL_Log("position before %i %i" , position_x, position_y); < see
    position_x=0;
    position_y= (fullPickImgSurface->h * 0.5) - 513;

    //fullPickImgSurface->h
    //2047

    // slower attempt....
//
//
//    SDL_Rect rect;
//    rect.w = 1;
//    rect.h = 1;
//    SDL_Color rowc;
//    SDL_Color colc;
//    SDL_Color c;
//
//    for( int row =0; row<colorPickerFGSurfaceGradient->h; row++ ){
//        rect.y = row + 0.0f;
//        for( int col =0; col<colorPickerFGSurfaceGradient->w; col++ ){
//            rect.x = col + 0.0f;
//
//            int whiteLevel = 255-row;
//            int blackLevel = 255-col;
//
//            //int result =
//
//            rowc.r=whiteLevel;
//            rowc.g=whiteLevel;
//            rowc.b=whiteLevel;
//            rowc.a=row;
//
//
//            colc.r=blackLevel;
//            colc.g=blackLevel;
//            colc.b=blackLevel;
//            colc.a=blackLevel;
//
////            c.r= SDL_min(rowc.r, colc.r);
////            c.g= SDL_min(rowc.g, colc.g);
////            c.b= SDL_min(rowc.b, colc.b);
//            c.a= SDL_max(rowc.a, colc.a);
//
//            c.r= SDL_min(rowc.r, colc.r);
//            c.g= SDL_min(rowc.g, colc.g);
//            c.b= SDL_min(rowc.b, colc.b);
////            c.a= SDL_max(rowc.a, colc.a);
//
//            // c.a= SDL_max((rowc.a + colc.a) * 0.5, SDL_max(rowc.a, colc.a));
//
//            SDL_FillRect(colorPickerFGSurfaceGradient, &rect, SDL_MapRGBA(colorPickerFGSurfaceGradient->format, c.r, c.g, c.b, c.a) );
//
////            SDL_FillRect(colorPickerFGSurfaceGradient, &rect, SDL_MapRGBA(colorPickerFGSurfaceGradient->format, rowc.r, rowc.g, rowc.b, rowc.a) );
////            SDL_FillRect(colorPickerFGSurfaceGradient, &rect, SDL_MapRGBA(colorPickerFGSurfaceGradient->format, colc.r, colc.g, colc.b, colc.a) );
//        }
//    }
//


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






    textureId_pickImage = textures->GenerateTexture();
    textureId_default = textures->GenerateTexture();
    oggLoadTextureSized(fullPickImgSurface, textureId_default, textureId_pickImage, textureSize, &position_x, &position_y);

    //glClearColor(0.0f, 1.0f, 0.0f, 0.0f); // will be fixed next...
    updateColorPreview();

//    SDL_Color c;
//    c.r=255;
//    c.g=255;
//    c.b=0;
//    pickerForHue(&c);

    textureId_fonts = textures->LoadTexture("textures/ascii.png");




    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    //glDisable(GL_DEPTH_TEST);


    glEnable(GL_BLEND);  //this enables alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
    glDisable(GL_BLEND);

    //projectionMatrix = glm::perspective(60.0f, (float)windowWidth / (float)windowHeight, 0.1f, (float)VIEW_MAX_CLIP_DISTANCE);  // Create our perspective projection matrix


    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE); // GL_TRUE enables writes to depth buffer....
//    glClear(GL_DEPTH_BUFFER_BIT);

    glStencilMask(0x00); // disables writes to stencil buffer
    glDisable(GL_STENCIL_TEST);
//    glClear(GL_STENCIL_BUFFER_BIT);

    // clearing those buffers now somehow messes up rendering (either one) the rendering bug is in nested UI objects... (settings text) not sure how exactly....

    debugGLerror("setupScene nearly done");

//#ifndef GL_SHADER_STORAGE_BUFFER
//#define GL_SHADER_STORAGE_BUFFER          0x90D2
//#endif
//    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
//    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
//
//    debugGLerror("unmap ops done");

//    glBindFramebuffer(GL_FRAMEBUFFER, 0);  // note binding buffer 0 (when its already bound) will break IOS....

#ifdef USE_FBO_FOR_RENDERING
    glGenFramebuffers(1, &fbo);
    //    glGenRenderbuffers(1, &rbo_color);
    glGenTextures(1, &texColorBuffer);
    glGenTextures(1, &texDepthBuffer);
    glGenRenderbuffers(1, &rbo_depth);
    // the rest is in reshapeWindow
#endif

//    modelMatrix = glm::mat4(1.0f);
//
//    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, -2.0f));
    //modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 1.0f, 1.0f));

//    modelMatrix = glm::translate(modelMatrix, treepos + glm::vec3(0.0f, 0.0f, (float)i*bspace-bpos));
//    modelMatrix = glm::scale(modelMatrix, treeScale * pineTreeBranchScales[i]);



#ifdef DEVELOPER_TEST_MODE
//    TEST_BEGIN();
#endif

    debugGLerror("setupScene completely done");

    begin3dDropperAnimation(); // gets the ticks....

    setup_complete = true;
}

void OpenGLContext::loadShaders(void){
    GLuint _glVaoID;// shaders wont validate on osx unless we bind a VAO: Validation Failed: No vertex array object bound.
#ifndef COLORPICK_OPENGL_ES2
    glGenVertexArrays(1, &_glVaoID );	glBindVertexArray( _glVaoID );
#endif

    // shader names need work...
    shader_lit_detail = new Shader("shaders/Shader.vsh", "shaders/ShaderGLES.fsh");

    shader_ui_shader_default = new Shader("shaders/uiShader.vsh", "shaders/uiShader.fsh");

    if( meshes->mesh3d_enabled ){
        shader_3d_ui = new Shader("shaders/uiShader.vsh.3d.vsh", "shaders/uiShader.fsh");

        shader_3d = new Shader("shaders/3dShader.vsh", "shaders/3dShader.fsh");

        shader_3d_unlit = new Shader("shaders/3dShader.vsh", "shaders/3dShader.fsh.Unlit.fsh");

        shader_3d_Glass = new Shader("shaders/3dShaderGlass.vsh", "shaders/3dShaderGlass.fsh");
    }
#ifndef COLORPICK_OPENGL_ES2
    glBindVertexArray( 0 );
    glDeleteVertexArrays(1, &_glVaoID );    _glVaoID = 0;
#endif
}

void OpenGLContext::reloadShaders(void){
    // reload just doesn't work - because the build files are not in teh same location
    shader_lit_detail->reload();
    shader_ui_shader_default->reload();
    shader_3d_ui->reload();
    shader_3d->reload();
    shader_3d_Glass->reload();
    shader_3d_unlit->reload();
}

bool OpenGLContext::isMinigameMode(){
    return generalUx->isMinigameMode;
}

glm::vec3 OpenGLContext::worldToScreenSpace(glm::vec3 obj){
    return glm::project(obj,matrixViews,matrixPersp,glm::vec4(0.0,0.0,(float)colorPickState->windowWidth,(float)colorPickState->windowHeight));
}
glm::vec3 OpenGLContext::screenToWorldSpace(GLfloat screenx, GLfloat screeny, GLfloat distance){
    return glm::unProject(glm::vec3(screenx, screeny, distance),matrixViews,matrixPersp,glm::vec4(0.0,0.0,(float)colorPickState->windowWidth,(float)colorPickState->windowHeight));
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

    /// hmm... drawable width is passed in.... same ratio though right?
    matrixPersp = glm::perspective(60.0f, (float)colorPickState->windowWidth / (float)colorPickState->windowHeight, 0.5f, 100.0f);  // Create our perspective projection matrix

    generalUx->updateStageDimension(windowWidth, windowHeight);

    renderShouldUpdate = true;


    // TODO: are we leaking any GL memory on reshape?  i think not... but its possible we should delete and re-allocate teh texture... etc ?
    // all or some of this needs to move into reshapeWindow
#if USE_FBO_FOR_RENDERING



    //    When we're done with all framebuffer operations, do not forget to delete the framebuffer object:
    //    glDeleteFramebuffers(1, &fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    debugGLerror("glBindFramebuffer just called....");


    //    glBindRenderbuffer(GL_RENDERBUFFER, rbo_color);
    //    debugGLerror("glBindRenderbuffer rbo_color done....");
    //    glRenderbufferStorage(GL_RENDERBUFFER, GL_UNSIGNED_INT_24_8, 800, 600);
    //    debugGLerror("glRenderbufferStorage rbo_color done....");
    //    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo_color);
    //    debugGLerror("rbo_color done....");


    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, colorPickState->drawableWidth, colorPickState->drawableHiehgt, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    // attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

/*
    error: use of undeclared identifier 'GL_DEPTH24_STENCIL8'
    error: use of undeclared identifier 'GL_DEPTH_STENCIL_ATTACHMENT'
*/

    // these defines for android, special right?
#ifndef GL_DEPTH24_STENCIL8
#define GL_DEPTH24_STENCIL8               0x88F0
#endif
#ifndef GL_DEPTH_STENCIL_ATTACHMENT
#define GL_DEPTH_STENCIL_ATTACHMENT       0x821A
#endif
#ifndef GL_DEPTH_STENCIL
#define GL_DEPTH_STENCIL                  0x84F9
#endif


//    // NOTE: the following is simply unsupported on some hardware - we either need to use textues for everything, or find a way to detect and avoid the crash...
//    glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, colorPickState->drawableWidth, colorPickState->drawableHiehgt);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);


    glBindTexture(GL_TEXTURE_2D, texDepthBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, colorPickState->drawableWidth, colorPickState->drawableHiehgt, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    // attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH_STENCIL, texDepthBuffer, 0);

    debugGLerror("rbo_depth done....");


    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){

        SDL_Log("its not complete, so here it is %02x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }else{

        SDL_Log("its supposidly comlpete...");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    debugGLerror("glGenRenderbuffers just calle....");

    SDL_Log("here is my fbo %i", fbo);
    SDL_Log("here is my rbo_depthbuffer %i", rbo_depth);
    SDL_Log("here is my rbo_colorbuffer %i", rbo_color);
    SDL_Log("here is my texColorBuffer %i", texColorBuffer);

#endif



//    glMatrixMode(GL_PROJECTION)    ;        // set matrix to projection mode
//    glLoadIdentity()   ;                         // reset the matrix to its default state
    //glFrustum(-1.0, 1.0, -1.0f, 1.0f, 3.0f, 7.0f);  // apply the projection matrix

    SDL_GL_MakeCurrent(sdlWindow, gl); // < this does nothing AFACT - safe but pointless....  but may actually be expensive!!
}

void  OpenGLContext::updateFishScaleSliderRunner(){
    if( openglContext->fishEyeScale < FISHEYE_SLOW_ZOOM_MAX ){
        float intensity = 1.0 - ((openglContext->fishEyeScale-FISHEYE_SLOW_ZOOM_THRESHOLD) / (FISHEYE_SLOW_ZOOM_MAX-FISHEYE_SLOW_ZOOM_THRESHOLD));
        //SDL_Log("run intensity %f", intensity);
        generalUx->runner->show(intensity);
    }else{
        generalUx->runner->hide();
    }
}

void  OpenGLContext::updateFishScaleSlider(){
    generalUx->zoomSlider->updateAnimationPercent(fishEyeScalePct, 0.0);
    updateFishScaleSliderRunner();
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

    updateFishScaleSlider();
}

//static for use as UI:: callback
void OpenGLContext::setFishScalePerentage(Ux::uiObject *interactionObj, float percent){
    // interactionObj will be nullptr sometimes FYI
    SDL_Log("MMV PERC SLIDER %f %%", (percent));
    // from static function we must get instance

    OpenGLContext* myOglContext = OpenGLContext::Singleton();

        // todo need to use logscale or crop
    myOglContext->fishEyeScalePct = percent;


    myOglContext->fishEyeScale = MIN_FISHEYE_ZOOM + (zoomRange * myOglContext->fishEyeScalePct);

    myOglContext->updateFishScaleSliderRunner();
}

float OpenGLContext::getPixelMovementFactor(){
    // 64 - 0.25 * screen larger dimension - sqrt 64 = 8.0
    // 32 - 0.178362573099415
    // 16 - 0.126705653021442
    // 5.2 - 0.8333
    // 1.5 - 0.038011695906433
    float screen_pct = (SDL_sqrtf(fishEyeScale) / 8.0) * 0.25;
    int bigPixelSize = 1;
    int longerScreenSize = colorPickState->windowHeight;
    if( colorPickState->viewport_ratio > 1.0 ){ // wide
        bigPixelSize = colorPickState->windowWidth * screen_pct;
        longerScreenSize = colorPickState->windowWidth;
    }else{
        bigPixelSize = colorPickState->windowHeight * screen_pct;
    }
    float halfLongerScreenSize = longerScreenSize * 0.5f;

    //generalUx->currentInteraction;
//    pixelInteraction;
//    colorPickState;


    //float distFromCtr = glm::distance(glm::vec2(generalUx->currentInteraction.px, generalUx->currentInteraction.py), glm::vec2(0.5,0.5));

    float distFromCtr = glm::distance(glm::vec2(pixelInteraction.px, pixelInteraction.py),
                                      glm::vec2(colorPickState->halfWindowWidth,colorPickState->halfWindowHeight));

    //float closenessToCenter = 1.0 - (distFromCtr / (longerScreenSize * 0.5));

    float closenessToCenter = 1.0 - ((distFromCtr) / (halfLongerScreenSize));

    //SDL_Log("closeness to center: %f", closenessToCenter);

    bigPixelSize *= closenessToCenter;

    //float factor = ((openglContext->fishEyeScale - FISHEYE_SLOW_ZOOM_THRESHOLD) / (MAX_FISHEYE_ZOOM - FISHEYE_SLOW_ZOOM_THRESHOLD)) * FISHEYE_SLOW_ZOOM_MAX;
    float factor = bigPixelSize * 0.5; // the big pixel is N across, we must move by this much to move 1px... but if we start center pixel its half that much
    if( factor < 1 ) factor = 1.0; // basically negates the normal effects of this block to slow things, when this is 1.0 we move 1px per px movement


    if( openglContext->fishEyeScale < FISHEYE_SLOW_ZOOM_MAX ){
        float intensity = 1.0 - ((openglContext->fishEyeScale-FISHEYE_SLOW_ZOOM_THRESHOLD) / (FISHEYE_SLOW_ZOOM_MAX-FISHEYE_SLOW_ZOOM_THRESHOLD));
        factor /= (25.0 * intensity);
    }

//    if( openglContext->fishEyeScale <= FISHEYE_SLOW_ZOOM_THRESHOLD ){
//        //factor = (0.05 - openglContext->fishEyeScalePct) * 80;
//        factor /= 25.0;
//    }
//    SDL_Log("bigPixelSize          %i" , bigPixelSize);
//    SDL_Log("computed factor %f" , factor);
    return factor;
}

void OpenGLContext::triggerMovement(){
    //    SDL_Log("fishEyeScale    %f" , fishEyeScale);
    //    SDL_Log("fishEyeScalePct %f" , fishEyeScalePct);

    float pxFactor = getPixelMovementFactor();

    //SDL_Log("Velocity is: %f %f", openglContext->pixelInteraction.vx, openglContext->pixelInteraction.vy);
    accumulated_movement_x += openglContext->pixelInteraction.rx;
    accumulated_movement_y += openglContext->pixelInteraction.ry;

//    if( false && openglContext->fishEyeScale <= FISHEYE_SLOW_ZOOM_THRESHOLD /*openglContext->fishEyeScalePct < 0.05*/ ){
//        // to move faster when zoomed out (sensible)
////        colorPickState->mmovex = accumulated_movement_x *( 1 + (factor));
////        colorPickState->mmovey = accumulated_movement_y *( 1 + (factor));
//        colorPickState->mmovex = accumulated_movement_x / (factor);
//        colorPickState->mmovey = accumulated_movement_y / (factor);
//        accumulated_movement_x=0;
//        accumulated_movement_y=0; // fully applied
//
//    }else if( openglContext->fishEyeScale > FISHEYE_SLOW_ZOOM_THRESHOLD){
//        colorPickState->mmovex = accumulated_movement_x;
//        colorPickState->mmovey = accumulated_movement_y;
//        accumulated_movement_x=0;
//        accumulated_movement_y=0; // fully applied

    //SDL_Log("max zoom time: current:%f max:%f threshold:%f range:%f computed factor: %f", openglContext->fishEyeScale, MAX_FISHEYE_ZOOM, FISHEYE_SLOW_ZOOM_THRESHOLD, FISHEYE_SLOW_ZOOM_MAX, factor);

    int resultx = (int) accumulated_movement_x / pxFactor;
    int resulty = (int) accumulated_movement_y / pxFactor;

    colorPickState->mmovex = resultx;
    colorPickState->mmovey = resulty;

    accumulated_movement_x-=resultx * pxFactor;
    accumulated_movement_y-=resulty * pxFactor; // at least partially applied, but also a good chance result x/y is 0

#ifndef OMIT_SCROLLY_ARROWS
    generalUx->movementArrows->indicateVelocity(pixelInteraction.vx, pixelInteraction.vy);
    //generalUx->movementArrows->indicateVelocity(openglContext->generalUx->currentInteraction.vx, openglContext->generalUx->currentInteraction.vy);
#endif

    //SDL_Log("MOUSE xy %d %d", colorPickState->mmovex,colorPickState->mmovey);
    openglContext->renderShouldUpdate = true;

}

void OpenGLContext::triggerVelocity(float x, float y){

    accumulated_velocity_y=0;
    accumulated_velocity_x=0;
    has_velocity = true;

    indicateHighSpeed();
}

void OpenGLContext::indicateHighSpeed(){
    if( generalUx->runner->uiObjectItself->doesInFactRender ){
        generalUx->uxAnimations->scale_bounce(generalUx->runner->runner, 0.002);
    }
}

void OpenGLContext::clearVelocity(){
    has_velocity = false;
    accumulated_movement_x = 0; // also called when beginning a new move
    accumulated_movement_y = 0;
}

//void OpenGLContext::updateFrame(Uint32 elapsedMs) {
//    generalUx->updateAnimations(elapsedMs);
//}

/**
 renderScene will contain all our rendering code.
 Next up we are going to clear our COLOR, DEPTH and STENCIL buffers to avoid overlapping of renders.
 Any of your other rendering code will go here. Finally we are going to swap buffers.
 */
#define VELOCITY_MIN 0.0000001f


void OpenGLContext::renderZoomedPickerBg(void) { // update and render....

    debugGLerror("renderZoomedPickerBg begin");


    Uint32 ticks = SDL_GetTicks();

    if( keyInteractions.hasPressedKeys() ){
        int moveSpeed = 1;

        if( keyInteractions.zoomIn->isPressed(ticks) ){
            setFishScale(1.0, 0.10f);
        }else if( keyInteractions.zoomOut->isPressed(ticks) ){
            setFishScale(-1.0, 0.10f);
        }

        if( fishEyeScale < FISHEYE_SLOW_ZOOM_MAX ){
            float intensity = 1.0 - ((openglContext->fishEyeScale-FISHEYE_SLOW_ZOOM_THRESHOLD) / (FISHEYE_SLOW_ZOOM_MAX-FISHEYE_SLOW_ZOOM_THRESHOLD));
            SDL_Log("fishhy %f intensity %f", fishEyeScale, intensity);
            moveSpeed += 64 * intensity;
        }

#define dirKeyPressedApplicationMacro(directionOfEffect, signOfEffect, keyIdentifier) \
        colorPickState->directionOfEffect=signOfEffect(keyInteractions.keyIdentifier->was_new ? 1 : moveSpeed); \
        indicateHighSpeed();

        if( keyInteractions.up->isPressed(ticks) ){
            dirKeyPressedApplicationMacro(mmovey, +, up)
        }
        if( keyInteractions.down->isPressed(ticks) ){
            dirKeyPressedApplicationMacro(mmovey, -, down)
        }
        if( keyInteractions.right->isPressed(ticks) ){
            dirKeyPressedApplicationMacro(mmovex, -, right)
        }
        if( keyInteractions.left->isPressed(ticks) ){
            dirKeyPressedApplicationMacro(mmovex, +, left)
        }

    }

    if( has_velocity ){

        float pxFactor = getPixelMovementFactor();

        accumulated_velocity_y += pixelInteraction.vy;
        accumulated_velocity_x += pixelInteraction.vx;
        pixelInteraction.update(ticks);

        // we now take any full integer amounts visible in the accumulated velocity....
        /// todo does this work good for negative numbers?? ANSWER : no  X: -1.267014 -2 Y: -0.894823 -1 -- it always rounds towards negative even for negativess
        //        int acu_v_y_int = SDL_floorf(accumulated_velocity_y);
        //        int acu_v_x_int = SDL_floorf(accumulated_velocity_x);
        /// todo does this work good for negative numbers?? ANSWER : yes  X: -9.183308 -9 Y: -5.968337 -5    X: -9.247559 -9 Y: -6.010095 -6
        int acu_v_y_int = (int)(accumulated_velocity_y) / pxFactor;
        int acu_v_x_int = (int)(accumulated_velocity_x) / pxFactor;
        //SDL_Log("Pixel Velocity is: %f %f", pixelInteraction.vx, pixelInteraction.vy);
        //SDL_Log("Velocity is: X: %f %i Y: %f %i", accumulated_velocity_x, acu_v_x_int, accumulated_velocity_y, acu_v_y_int);
        colorPickState->mmovex = acu_v_x_int;
        colorPickState->mmovey = acu_v_y_int;
        accumulated_velocity_x -= acu_v_x_int * pxFactor; // subtract the velocity we "already" applied, remainder will continue to be tracked and might still apply...
        accumulated_velocity_y -= acu_v_y_int * pxFactor;

        // set movex and movey accordingly
        if( fabs(pixelInteraction.vy) < VELOCITY_MIN && fabs(pixelInteraction.vx) < VELOCITY_MIN ){
            has_velocity = false; // never reached ????
            //renderShouldUpdate=false;
        }
    }

    // update
    if(colorPickState->mmovex != 0 || colorPickState->mmovey != 0){
        position_x += colorPickState->mmovex;
        colorPickState->mmovex=0;
        position_y += colorPickState->mmovey;
        colorPickState->mmovey=0;

        generalUx->hideHistoryPalleteIfShowing(); // panning background...

        //SDL_Log("position before %i %i" , position_x, position_y);0 1535

        // pass a reference to the posiiton - then use that, rather than above
        // since textureId_default is not changing..... we should omit it here... (we keep moving it, but we can move it in shader...) // maths
        // can also just omit when zoomed. - excep when u can see it... so near edges it does not work // fishEyeScale < 3.0f ? textureId_default : textureNone
        //oggLoadTextureSized(fullPickImgSurface, textureId_default, textureId_pickImage, textureSize, &position_x, &position_y, lastHue);


        // quandry: do we move this logic into oggLoadTextureSized ? probably.... but it would have to be a mode we only "enable" from here(only if we call form here) - normally it is to be very deterministic (resetting these vars when called)
        position_offset_x = position_cropped_x-position_x;
        position_offset_y = position_cropped_y-position_y;

        if( fabs(position_offset_x) > 128 || fabs(position_offset_y) > 128 ){
            oggLoadTextureSized(fullPickImgSurface, textureNone, textureId_pickImage, textureSize, &position_x, &position_y);
        }else{
            // just lazy move it... no new texture to stream to GPU, we just move what we have there already...
            textures->UpdateSelectedColorForOffset(fullPickImgSurface, &position_x, &position_y);
            position_offset_x = position_cropped_x-position_x;
            position_offset_y = position_cropped_y-position_y;
        }

        //


        if( has_velocity && position_x_was == position_x && position_y_was == position_y ){
            // we have velocity but we are not moving (reached corner) save some battery...  we could let it try to go a few frames maybe.... for accumulator sake...
            has_velocity=false;
        }

        position_x_was = position_x;
        position_y_was = position_y;

        // lets figure out how to maintain this position into the shader for the full image texture...

        updateColorPreview();

    }else if( !has_velocity ){

        if( !keyInteractions.hasPressedKeys() ){
            renderShouldUpdate=false;
        }
        //generalUx->movementArrows->indicateVelocity(0, 0);

        // if we didn't move, then do not do this :)
        //oggLoadTextureSized(fullPickImgSurface, textureNone, textureId_pickImage, textureSize, &position_x, &position_y);

        //has_velocity = false;
    }
    // then re-crop our source texture properly




    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);//  | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //    glDisable(GL_DEPTH_TEST);
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




    glUniform1f(uniformLocations->fishScale, fishEyeScale);
    glUniform1f(uniformLocations->fishScalePct, fishEyeScalePct);
    glUniform1f(uniformLocations->textureWidth, (float)textureSize);

    glUniform2f(uniformLocations->positionOffset, (float)position_x / -loadedImageMaxSize, (float)position_y/ -loadedImageMaxSize);

    glUniform2f(uniformLocations->zoomedPositionOffset, position_offset_x, position_offset_y);
    
    //glUniform2f(uniformLocations->positionOffset, 0.1f, 0.1f);

    //    glUniform4f(uniformLocations->ui_foreground_color,
    //                renderObj->foregroundColor.r/255.0,
    //                renderObj->foregroundColor.g/255.0,
    //                renderObj->foregroundColor.b/255.0,
    //                renderObj->foregroundColor.a/255.0
    //                );

    glUniform1f(uniformLocations->widthHeightRatio, colorPickState->viewport_ratio);

    //    glUniformMatrix4fv(uniformLocations->modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
    //    glUniformMatrix4fv(uniformLocations->projectionMatrixLocation,    1, GL_FALSE, &projectionMatrix[0][0]); // Send our projection matrix to the shader

    glUniform1i(uniformLocations->textureSampler, 0);
    glUniform1i(uniformLocations->textureSampler2, 1);
    //glUniform1i(uniformLocations->textureSampler3, 3);

    debugGLerror("renderScene glUniform done");


    glActiveTexture( GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, textureId_pickImage);

    glActiveTexture( GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D,  textureId_default);


    debugGLerror("renderScene textuers bound");


    glDisable(GL_BLEND);

    debugGLerror("renderScene glDisable(GL_BLEND");

    square_mesh->bind(); // Bind our Vertex Array Object GL_INVALID_OPERATION (except android?)
    debugGLerror("renderScene glBindVertexArray(rect_vaoID");


    glDrawArrays(GL_TRIANGLES, 0, 6);

    debugGLerror("renderScene our renderinng done");

}

void OpenGLContext::renderUi(void) {
    // needs GL_Blend
    /* NEXT UP: RENDER UI */
    uniformLocations = shader_ui_shader_default->bind(); // Bind our shader


    debugGLerror("renderScene ui shader bound");


    glUniform1i(uniformLocations->textureSampler, 0);
    //glUniform1i(uniformLocations->textureSampler2, 1);
    // glUniform1i(uniformLocations->textureSampler3, 1);

    debugGLerror("renderScene ui texture uniform set");


    glActiveTexture( GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D,  textureId_fonts);

    debugGLerror("renderScene ui textureId_fonts bound");


//    glBindBuffer(GL_ARRAY_BUFFER, rect_vboID[0]);

    square_mesh->bind();


    generalUx->renderObject(uniformLocations); // renders all obj

//    generalUx->renderObjects(uniformLocations, generalUx->addHistoryBtn, glm::mat4(1.0f));
//    generalUx->renderObjects(uniformLocations, generalUx->addHistoryBtn, glm::mat4(1.0f));
//    generalUx->renderObjects(uniformLocations, generalUx->addHistoryBtn, glm::mat4(1.0f));
//    generalUx->renderObjects(uniformLocations, generalUx->zoomSlider, glm::mat4(1.0f));
//    generalUx->renderObjects(uniformLocations, generalUx->addHistoryBtn, glm::mat4(1.0f));
//    generalUx->renderObjects(uniformLocations, generalUx->zoomSlider, glm::mat4(1.0f));
//    generalUx->renderObjects(uniformLocations, generalUx->zoomSlider, glm::mat4(1.0f));
//    generalUx->renderObjects(uniformLocations, generalUx->zoomSlider, glm::mat4(1.0f));
//    generalUx->renderObjects(uniformLocations, generalUx->zoomSlider, glm::mat4(1.0f));

    debugGLerror("generalUx->renderObject ending");

}



void OpenGLContext::renderScene(void) {

    debugGLerror("renderScene begin");
    //SDL_Log("renderScene begin");

#if USE_FBO_FOR_RENDERING
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // note binding buffer 0 (when its already bound) will break IOS....
#endif

    if( isMinigameMode() ){

        glClear(GL_COLOR_BUFFER_BIT);//  | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        debugGLerror("renderScene minigame glClear");

        minigames->update();
        minigames->render();

        glEnable(GL_BLEND);  //this enables alpha blending

        renderUi();

    }else{
        renderZoomedPickerBg();

//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glEnable(GL_BLEND);  //this enables alpha blending
//        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//        // some animations will go before or after the UI!!!!  depends on which animation....

//        glClear(GL_COLOR_BUFFER_BIT);
//        renderUi();


        if( animationDropper3dId == ANIMATION_ZOOM_INTO_DROPPER || animationDropper3dId == ANIMATION_ZOOM_INTO_BULB ){

            // HMM for zoom into bulb... we want to switch rendeirng half way through....

            if( animationDropper3dId == ANIMATION_ZOOM_INTO_BULB && SDL_GetTicks() - animationDropper3dStartTime < 375 ){ // really 375 but whatev.. ( for 1500 duration and 0.25 begin duration )
                render3dDropperAnimation();
                renderUi();

            }else{
                renderUi();
                render3dDropperAnimation();
            }
        }else{
            render3dDropperAnimation();
            renderUi();
        }


    }

#if USE_FBO_FOR_RENDERING

    glBindFramebuffer(GL_FRAMEBUFFER, 0);  // note binding buffer 0 (when its already bound) will break IOS....
    glClear(GL_COLOR_BUFFER_BIT);

    uniformLocations = shader_ui_shader_default->bind();

    glUniform1i(uniformLocations->textureSampler, 0);
    //glUniform1i(uniformLocations->textureSampler2, 1);
    // glUniform1i(uniformLocations->textureSampler3, 1);

    debugGLerror("renderScene FBO texture uniform set");

    glActiveTexture( GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D,  texColorBuffer);

    debugGLerror("renderScene FBO textureId_fonts bound");

    glBindVertexArray(rect_vaoID[0]);

    // the next 3 lines could be one probably.... or two...
    generalUx->renderObjects(uniformLocations, generalUx->screenRenderQuadObj, glm::mat4(1.0f));

//    glUniformMatrix4fv(uniformLocations->ui_modelMatrix, 1, GL_FALSE, &glm::mat4(1.0f)[0][0]);
//    generalUx->screenRenderQuadObj->setUniformsForRender(uniformLocations);
//    glDrawArrays(GL_TRIANGLES, 0, 6);



#endif


}


void OpenGLContext::begin3dDropperAnimation(int aDropperAnimation, SDL_Color* aColor){
    animationDropper3dStartTime = SDL_GetTicks();
    animationDropper3dId = aDropperAnimation; // see DROPPER_ANIMATION_ENUM
    if( aColor != nullptr ){
        animationDropper3dColor = *aColor; // todo rm unneeded var animationDropper3dColor ?
        eyedropper_fill->color_additive = glm::vec4(aColor->r / 255.0,
                                                    aColor->g / 255.0,
                                                    aColor->b / 255.0,
                                                    1.0);
    }
}

void OpenGLContext::begin3dDropperAnimation(int aDropperAnimation) {
    begin3dDropperAnimation(aDropperAnimation, nullptr);
}

void OpenGLContext::begin3dDropperAnimation(void) {
    begin3dDropperAnimation(DROPPER_ANIMATION_ENUM::ANIMATION_DEFAULT);
}

void OpenGLContext::eyedropperTestMatrix(float progress){
    matrixModel = glm::mat4(1.0f);
    float rotation = progress * 90.0f;
//    float position = progress * 10.0f;
//    float scale = 1.0 - progress;
    // try translating Z.. compare with matrixViews eye
    //       matrixModel = glm::translate(matrixModel, glm::vec3(0.0f, 0.0f, -10.0f + position));
    // so +z moves towards screen..
    matrixModel = glm::translate(matrixModel, glm::vec3(0.0f, 0.0f, -20.0f));
    //    matrixModel = glm::translate(matrixModel, glm::vec3(0.0f, 0.0f, -10.0f + position));
    //    matrixModel = glm::scale(matrixModel, glm::vec3(1.0f, 1.0f, 0.5));
    //   matrixModel = glm::translate(matrixModel, glm::vec3(0.0f, 0.0f, -20.0f));
    // matrixModel = glm::translate(matrixModel, glm::vec3(0.0f, 0.0f, -1.0f)); // in bulb
    //    matrixModel = glm::translate(matrixModel, glm::vec3(0.0f, 0.0f, 0.0f)); // base
    //matrixModel = glm::translate(matrixModel, glm::vec3(0.0f, 0.0f, 1.0f));
    //matrixModel = glm::rotate(matrixModel, 180.0f, glm::vec3(1.0f, 0.0f, 0.0f)); // pointed at camera
    //  matrixModel = glm::rotate(matrixModel, -90.0f, glm::vec3(1.0f, 0.0f, 0.0f)); // vertical (point down)
    //    matrixModel = glm::scale(matrixModel, glm::vec3(1.0f, 1.0f, scale)); // if we are already looking down the stem, this collapses the stem.... if we rotated it first we will see this from the side...
    //    SDL_Log("scale %f " , scale);
    matrixModel = glm::rotate(matrixModel, rotation*4.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    matrixModel = glm::rotate(matrixModel, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f)); // upside down (point up)
    matrixModel = glm::rotate(matrixModel, -rotation*4, glm::vec3(1.0f, 0.0f, 0.0f));
    matrixModel = glm::rotate(matrixModel, rotation, glm::vec3(0.0f, 1.0f, 0.0f));
    //  matrixModel = glm::translate(matrixModel, glm::vec3(0.0f, 0.0f, 8.0f));
    //matrixModel = glm::translate(matrixModel, glm::vec3(0.0f, 0.0f, 9.0f));
}

void OpenGLContext::eyedropperAddColorMatrix(float progress){
    matrixModel = glm::mat4(1.0f);
    float rotation = progress * 90.0f;
    bool endProgressActive = false;
    float endProgress=0.0f;
    if( progress > 0.9 ){
        endProgress = 1.0 - ((1.0 - progress) / 0.1);
        matrixModel = glm::translate(matrixModel, glm::vec3(0.0f, endProgress * -3.0f, endProgress * 20.0f));
        endProgressActive=true;

    }else if( progress < 0.1 ){
        float beginProgress = 1.0 - (progress / 0.1);
        matrixModel = glm::translate(matrixModel, glm::vec3(beginProgress * 2.0f, beginProgress * -2.0f, beginProgress * 40.0f));
    }
    matrixModel = glm::translate(matrixModel, glm::vec3(0.0f, 0.0f, -20.0f)); // how far away is dropper
    matrixModel = glm::rotate(matrixModel, 45.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    matrixModel = glm::rotate(matrixModel, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    matrixModel = glm::rotate(matrixModel, -progress*25.0f, glm::vec3(1.0f, 0.0f, 0.0f));

    if( endProgressActive ){
        // some random thing ? we need to roll above though.
        matrixModel = glm::rotate(matrixModel, endProgress * -90.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    }
}

void OpenGLContext::eyedropperZoomDropperMatrix(float progress){
    matrixModel = glm::mat4(1.0f);
    float rotation = progress * 90.0f;
    float scale = 1.0f+ (progress * 2.0f); // 1.0 - progress;
    // need an exponentail slow down as we approach the end of the dropper...
    float invProg = 1.0 - progress;
    float scaler = invProg * 100;
    float invProgResult = (scaler * scaler) / 10000;
    // 1.0 puts the entire dropper behind the camera, -15 lets us see the back of the dropper
    matrixModel = glm::translate(matrixModel, glm::vec3(0.0f, 0.0f, 1.0 + (invProgResult * -13.0f)));
    matrixModel = glm::rotate(matrixModel, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    matrixModel = glm::scale(matrixModel, glm::vec3(scale,scale,scale));
}

void OpenGLContext::eyedropperZoomDropperBulbMatrix(float progress){
    matrixModel = glm::mat4(1.0f);
    float scaler = progress * 100;
    float progResult = (scaler * scaler) / 10000;
    float rotation = progress * 90.0f;
    float position = progResult * 30.0f;
    float scale = 1.0f+ (progress * 4.0f);
    matrixModel = glm::translate(matrixModel, glm::vec3(0.0f, 0.0f, -20.0f + position));
    if( progress < 0.25 ){
        float beginProgress = 1.0 - (progress / 0.25);
        matrixModel = glm::translate(matrixModel, glm::vec3(beginProgress * 2.0f, beginProgress * -2.0f, 0.0f));
        matrixModel = glm::rotate(matrixModel, beginProgress * -90, glm::vec3(1.0f, 1.0f, 0.0f));
    }
    matrixModel = glm::rotate(matrixModel, 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    matrixModel = glm::rotate(matrixModel, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    matrixModel = glm::scale(matrixModel, glm::vec3(scale,scale,1.0));
}

void OpenGLContext::render3dDropperAnimation(void) {
    if( !meshes->mesh3d_enabled ){
        return;
    }
    if( animationDropper3dId == DROPPER_ANIMATION_ENUM::NO_ANIMATION ){return;}
    // presumably this/each animation knows when it's done... based on some duration of the specified animation..... for now we will say 4 seconds

    float progress=1.0;

    if( animationDropper3dId == ANIMATION_ADD_COLOR ){
        progress = (SDL_GetTicks() - animationDropper3dStartTime) / 1200.0f;
        eyedropperAddColorMatrix(progress);
        render3dDropper(progress);
    } else if( animationDropper3dId == ANIMATION_ZOOM_INTO_DROPPER ){
        progress = (SDL_GetTicks() - animationDropper3dStartTime) / 3000.0f;
        eyedropperZoomDropperMatrix(progress);
        render3dDropper(0.0);
    } else if( animationDropper3dId == ANIMATION_ZOOM_INTO_BULB ){
        progress = (SDL_GetTicks() - animationDropper3dStartTime) / 1500.0f;
        if( progress <= 1.0 ){
            eyedropperZoomDropperBulbMatrix(progress);
            render3dDropper(0.0);
        }else{
            animationDropper3dId = DROPPER_ANIMATION_ENUM::NO_ANIMATION;
            minigames->beginNextGame();
            renderUi(); // we ALREADY rendered the UI, but it was underneath the dropper and the minigame...
        }
        return; // custom progress handling.... for end events...
    } else if( animationDropper3dId == ANIMATION_DEFAULT ){
        //SDL_Log("Default debug animation rendering...");
        progress = (SDL_GetTicks() - animationDropper3dStartTime) / 8000.0f;
        if( progress < 8.0 ){
            eyedropperTestMatrix(progress);
            render3dDropper(0.0f);
        }else{
            // animation is "done" lets mark it as such
            animationDropper3dId = DROPPER_ANIMATION_ENUM::NO_ANIMATION;
            // bounce the add icon when our animation is done....
            generalUx->uxAnimations->scale_bounce(generalUx->addHistoryBtn, 0.005);
        }
        return; // custom progress handling.... for repeat of animations
    }

    if( progress >= 1.0 ){
        animationDropper3dId = DROPPER_ANIMATION_ENUM::NO_ANIMATION;
        //SDL_Log("3d animation done");
    }
}

void OpenGLContext::setLight(){
    // we could pass in our own uniformLocations (this is using the class member OpenGLContext::uniformLocations )
    // we could pass in our own vec3 for global light
    if( uniformLocations->globalAmbientLight < 0 ){ return; } // even when it is zero, many times its not needed....
    float globalLightAmt = 0.15;
    float ambient_light = 0.00005;
    glUniform4f(uniformLocations->globalAmbientLight,
                globalLightAmt, // x
                globalLightAmt, // y
                globalLightAmt, // z
                ambient_light); // ambient
}

void OpenGLContext::render3dDropper(float colorFillPercent){ // todo: color arg is going to be unneded I think...

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE); // GL_TRUE enables writes to depth buffer....
    glClear(GL_DEPTH_BUFFER_BIT); // THIS SHOULD MOVE OUT OF HERE FOR SURE JUST ABOUT>>>>> (we can't clear the depth buffer though when GL_DEPTH_TEST is not enabled - perhaps we should always have it on though and set the prop where we don't write to the buffer?

    uniformLocations = shader_3d->bind(); // Bind our shader
    setLight();

    glUniformMatrix4fv(uniformLocations->modelMatrixLocation, 1, GL_FALSE, &matrixModel[0][0]); // Send our model matrix to the shader
    glUniformMatrix4fv(uniformLocations->viewMatrixLocation, 1, GL_FALSE, &matrixViews[0][0]); // Send our model matrix to the shader
    glUniformMatrix4fv(uniformLocations->projectionMatrixLocation, 1, GL_FALSE, &matrixPersp[0][0]); // Send our model matrix to the shader


    colorFillPercent = (colorFillPercent - 0.1) / 0.9; // the first 0.1 are skipped


    if( colorFillPercent > fill_requirement ){
        // we make some preliminary write op to the stencil buffer...

        glUniform4f(uniformLocations->color_additive, 0.0,0,0,0.0); // transparent draw so no visible output....  to debug you might use  eyedropper_stem->applyUniforms(uniformLocations);  (glass default)
        eyedropper_stem->render();
        //we write the depth buffer only ^

        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 1, 0xFF); // to clear buffer, we need something like this....
        glStencilMask(0xFF);
        glClear(GL_STENCIL_BUFFER_BIT);
        //glStencilMask(0x00); // disables writes to stencil buffer
        //glStencilFunc(GL_EQUAL, 1, 0xFF);  // only where our buffer is filled in...
       // glStencilFunc(GL_EQUAL, 1, 0xFF);  // only where our buffer is filled in...


        glDepthMask(GL_FALSE); // GL_TRUE enables writes to depth buffer....
        glDepthFunc(GL_GREATER);  // we only want to draw parts of the dropper that are on the INSIDE

        glStencilMask(0xFF); // enable writes to stencil buffer
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // really write to the stencil buffer?
        //glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should update the stencil buffer

        glUniform4f(uniformLocations->color_additive, 1.0,0,0,0.0); // transparent draw so no visilble output (we write to stencil only right now) you might try red to visualize/debug the stencil buffer... 1.0,0,0,1.0
        //glUniform4f(uniformLocations->color_additive, 1.0,0,0,1.0);
        eyedropper_stem->render();


        glStencilMask(0x00); // disables writes to stencil buffer
        glDisable(GL_STENCIL_TEST);

        //our use of depth buffer wass temporary...
        glDepthFunc(GL_LESS);  // set defaults
        glDepthMask(GL_TRUE);
        glClear(GL_DEPTH_BUFFER_BIT); // our use was temporary....

        // this is test stuff so lets reset/exit now..
//        glEnable(GL_CULL_FACE);
//        glDisable(GL_DEPTH_TEST);
//        renderShouldUpdate=true; // keep animation going...
//        return;
    }

    //debugGLerror("render3dDropperAnimation glDisable glEnable");

//    glUniform1i(uniformLocations->textureSampler, 0);
//    glActiveTexture( GL_TEXTURE0 + 0);
//    glBindTexture(GL_TEXTURE_2D,  textureId_fonts);

    eyedropper_bulb->applyUniforms(uniformLocations);
    eyedropper_bulb->render();

    uniformLocations = shader_3d_Glass->bind(); // Bind our shader
    setLight();

    glUniform1i(uniformLocations->textureSampler, 0);
    glActiveTexture( GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D,  textureId_fonts);

    glUniform1i(uniformLocations->textureSampler, 1);
    glActiveTexture( GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D,  textureId_pickImage);

    glUniformMatrix4fv(uniformLocations->modelMatrixLocation, 1, GL_FALSE, &matrixModel[0][0]); // Send our model matrix to the shader
    glUniformMatrix4fv(uniformLocations->viewMatrixLocation, 1, GL_FALSE, &matrixViews[0][0]); // Send our model matrix to the shader
    glUniformMatrix4fv(uniformLocations->projectionMatrixLocation, 1, GL_FALSE, &matrixPersp[0][0]); // Send our model matrix to the shader

    //SDL_Log("FISHY SCALE %f %f", fishEyeScale, fishEyeScalePct);
    // the refraction of textureId_pickImage is variable.... TODO rename these uniforms to be generic... or if it is fish scale be fishy about it (not linear)
    glUniform1f(uniformLocations->fishScale, fishEyeScale);
    glUniform1f(uniformLocations->fishScalePct, fishEyeScalePct);

    glDepthMask(GL_FALSE); // GL_TRUE enables writes to depth buffer....
    glDisable(GL_CULL_FACE);

    eyedropper_stem->applyUniforms(uniformLocations);
    eyedropper_stem->render();

    if( colorFillPercent > fill_requirement ){

        glEnable(GL_CULL_FACE); // no sense rendering back of our clolr block....

        glEnable(GL_STENCIL_TEST);  // our stencil shows the "inside" of the dropper
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // not modifying buffer
        glStencilMask(0x00); // disables writes to stencil buffer
        glStencilFunc(GL_EQUAL, 1, 0xFF);  // only where our buffer is filled in...

        uniformLocations = shader_3d_unlit->bind(); // Bind our shader

        glm::mat4 matrixModelInk = glm::scale(matrixModel, glm::vec3(1.0f, 1.0f, colorFillPercent * 1.0));

        glUniformMatrix4fv(uniformLocations->modelMatrixLocation, 1, GL_FALSE, &matrixModelInk[0][0]); // Send our model matrix to the shader
        glUniformMatrix4fv(uniformLocations->viewMatrixLocation, 1, GL_FALSE, &matrixViews[0][0]); // Send our model matrix to the shader
        glUniformMatrix4fv(uniformLocations->projectionMatrixLocation, 1, GL_FALSE, &matrixPersp[0][0]); // Send our model matrix to the shader

        eyedropper_fill->applyUniforms(uniformLocations);
        eyedropper_fill->render();


        // now we are still rendering the stenciled glass that is over top of our color sample.....
        // with the depth check...  see the alpha value below?  fix this... and maybe light too...

        uniformLocations = shader_3d->bind(); // Bind our shader
        setLight();

        glUniformMatrix4fv(uniformLocations->modelMatrixLocation, 1, GL_FALSE, &matrixModelInk[0][0]); // Send our model matrix to the shader
        glUniformMatrix4fv(uniformLocations->viewMatrixLocation, 1, GL_FALSE, &matrixViews[0][0]); // Send our model matrix to the shader
        glUniformMatrix4fv(uniformLocations->projectionMatrixLocation, 1, GL_FALSE, &matrixPersp[0][0]); // Send our model matrix to the shader

        // depth mask already off...
        //glDepthMask(GL_FALSE); // GL_TRUE enables writes to depth buffer....
        //glDisable(GL_CULL_FACE);

        //eyedropper_stem->applyUniforms(uniformLocations);
        //eyedropper_fill->applyUniforms(uniformLocations);
        glUniform4f(uniformLocations->color_additive,
                    eyedropper_fill->color_additive.r,
                    eyedropper_fill->color_additive.g,
                    eyedropper_fill->color_additive.b,
                    0.1);
        eyedropper_stem->render();

        glDisable(GL_STENCIL_TEST);

        // some 3d ux experiment...
    }
    if( minigames->minigamesEnabled() && (colorFillPercent > fill_requirement || animationDropper3dId == ANIMATION_ZOOM_INTO_BULB) ){


        // UI needs square....
        square_mesh->bind();

        //glEnable(GL_CULL_FACE);
        //glDisable(GL_DEPTH_TEST);
        uniformLocations = shader_3d_ui->bind(); // Bind our shader

        glUniform1i(uniformLocations->textureSampler, 0);
        glActiveTexture( GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D,  textureId_fonts);

        glm::mat4 counterMat = matrixModel * minigameCounterMatrix;
        glUniformMatrix4fv(uniformLocations->modelMatrixLocation, 1, GL_FALSE, &counterMat[0][0]); // Send our model matrix to the shader
        glUniformMatrix4fv(uniformLocations->viewMatrixLocation, 1, GL_FALSE, &matrixViews[0][0]); // Send our model matrix to the shader
        glUniformMatrix4fv(uniformLocations->projectionMatrixLocation, 1, GL_FALSE, &matrixPersp[0][0]); // Send our model matrix to the shader

        // this renderes the ENTIRE ui on our "quad"
        //generalUx->renderObject(uniformLocations);
        // the quad is not the same aspect ratiou as the screen

        // the render is always within the root render space where sizes rae computed...  with knowldge of that we can (painfluly?) scale it to the ful quad
        glm::mat4 uiSizerMat = glm::mat4(1.0f);
        Ux::uiObject* toRender; // = generalUx->curerntColorPreview->uiObjectItself;
        // realistically any object destined to take up the full quad, should just be added to root, or orphaned adn updated independently.
        toRender = generalUx->minigameCounterText;

        generalUx->renderObjects(uniformLocations, toRender, uiSizerMat);
    }

    // "reset" the following (arguably this should just move out of here?)
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    renderShouldUpdate=true; // keep animation going...
    debugGLerror("render3dDropperAnimation ending");
}


#ifdef DEVELOPER_TEST_MODE
void OpenGLContext::TEST_BEGIN(void) {
    int my_timer_id = SDL_AddTimer(6000, TEST_CALLBACK, nullptr);
}

Uint32 OpenGLContext::TEST_CALLBACK(Uint32 interval, void* parm){
    OpenGLContext* ogg=OpenGLContext::Singleton();
    SDL_Log("WE GOT THE CALLBACK");
    ogg->TEST_CAN_VIEW_ANY_COLOR();
    return 0; // callback won't fire at same interval exactly... we cancel it here
}

// this may just trigger the test?????  a thread should run this though....
void OpenGLContext::TEST_CAN_VIEW_ANY_COLOR(void) {
    OpenGLContext* ogg=OpenGLContext::Singleton();
    SDL_Color aColor;
    for( int r=1; r<256; r+=5 ){
        for( int g=0; g<256; g+=5 ){
            for( int b=0; b<256; b+=5 ){
                aColor.r=r;
                aColor.g=g;
                aColor.b=b;
                SDL_Event event;
                SDL_UserEvent userevent;
                userevent.type = SDL_USEREVENT;
                userevent.code = USER_EVENT_ENUM::GENERIC_ARBITRARY_CALL;
                userevent.data1 = (void*)&ogg->generalUx->hueClickedPickerHsv;
                userevent.data2 = &aColor;
                event.type = SDL_USEREVENT;
                event.user = userevent;
                SDL_PushEvent(&event);
                //ogg->generalUx->hueClickedPickerHsv(&aColor);
                SDL_Delay(100);
            }
        }
    }
}
#endif

bool OpenGLContext::canMesh(){
#ifdef __ANDROID__
    return enoughMemoryForMeshes();
#else
    return true;
#endif
}

// todo: return mesh* instead?
Mesh* OpenGLContext::createSquare(void) {
    Mesh* mesh = new Mesh("square");

    // we could allocate the temp ones on the mesh* instead...
    float *vertices = new float[18];	// Vertices for our square
    float *colors = new float[18]; // Colors for our vertices
    float* texCoord = new float[12]; // Colors for our vertices
    float* normals = new float[18]; // Colors for our vertices

//    float *vertices = (float*)SDL_malloc( sizeof(float) * 18 );// new float[18];    // Vertices for our square
//    float *colors = (float*)SDL_malloc( sizeof(float) * 18 );//new float[18]; // Colors for our vertices
//    float* texCoord = (float*)SDL_malloc( sizeof(float) * 12 );//new float[12]; // Colors for our vertices
//    float* normals = (float*)SDL_malloc( sizeof(float) * 18 );//new float[18]; // Colors for our vertices

    float c1=0.00001f;//(float)(rand() % 10);
    float c2=0.00001f;//(float)(rand() % 10);
    float c3=0.00001f;
    float c4=0.00001f;//(float)(rand() % 10);

    //todo move to mesh...
//    squareTriangleStripIndicies[0]=0;
//    squareTriangleStripIndicies[1]=4;
//    squareTriangleStripIndicies[2]=2;
//    squareTriangleStripIndicies[3]=1;

//    squareTriangleIndicies[0]=0;
//    squareTriangleIndicies[1]=1;
//    squareTriangleIndicies[2]=3;
//    squareTriangleIndicies[3]=2;
//    squareTriangleIndicies[2]=2;
//    squareTriangleIndicies[3]=3;
//    squareTriangleIndicies[4]=4;
//    squareTriangleIndicies[5]=5;

//    glGenBuffers(1, &rect_triangleStripIndexBuffer); // when using glDrawElements on a "core" context you can't store the indicies in ram
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rect_triangleStripIndexBuffer);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareTriangleStripIndicies), squareTriangleStripIndicies, GL_STATIC_DRAW);
//    debugGLerror("indicies bound....");


    int n=-1;
    float sq_size = 0.725;
    //sq_size = 0.985;
    //sq_size = 0.998;
    sq_size = 1.0;

    int vIdx = 0;
    int cIdx = 0;
    int tIdx = 0;


    vertices[vIdx++] = -sq_size; vertices[vIdx++] = -sq_size; vertices[vIdx++] = c1; // Bottom left corner
    colors[cIdx++] = 1.0; colors[cIdx++] = 1.0; colors[cIdx++] = 1.0; // Bottom left corner
    texCoord[tIdx++] = 0.0; texCoord[tIdx++] = 1.0;
    normals[++n] = 0.0;normals[++n] = 0.0;normals[++n] = -1.0;

    vertices[vIdx++] = sq_size; vertices[vIdx++] = sq_size; vertices[vIdx++] = c2; // Top Right corner
    colors[cIdx++] = 0.0; colors[cIdx++] = 1.0; colors[cIdx++] = 0.0; // Top Right corner
    texCoord[tIdx++] = 1.0; texCoord[tIdx++] = 0.0;
    normals[++n] = 0.0;normals[++n] = 0.0;normals[++n] = -1.0;

    vertices[vIdx++] = -sq_size; vertices[vIdx++] = sq_size; vertices[vIdx++] = c3; // Top left corner
    colors[cIdx++] = 1.0; colors[cIdx++] = 0.0; colors[cIdx++] = 0.0; // Top left corner
    texCoord[tIdx++] = 0.0; texCoord[tIdx++] = 0.0;
    normals[++n] = 0.0;normals[++n] = 0.0;normals[++n] = -1.0;

    vertices[vIdx++] = -sq_size; vertices[vIdx++] = -sq_size; vertices[vIdx++] = c1; // Bottom left corner
    colors[cIdx++] = 1.0; colors[cIdx++] = 1.0; colors[cIdx++] = 1.0; // Bottom left corner
    texCoord[tIdx++] = 0.0; texCoord[tIdx++] = 1.0;
    normals[++n] = 0.0;normals[++n] = 0.0;normals[++n] = -1.0;

    vertices[vIdx++] = sq_size; vertices[vIdx++] = -sq_size; vertices[vIdx++] = c4; // Bottom right corner
    colors[cIdx++] = 0.0; colors[cIdx++] = 0.0; colors[cIdx++] = 1.0; // Bottom right corner
    texCoord[tIdx++] = 1.0; texCoord[tIdx++] = 1.0;
    normals[++n] = 0.0;normals[++n] = 0.0;normals[++n] = -1.0;

    vertices[vIdx++] = sq_size; vertices[vIdx++] = sq_size; vertices[vIdx++] = c2; // Top Right corner
    colors[cIdx++] = 0.0; colors[cIdx++] = 1.0; colors[cIdx++] = 0.0; // Top Right corner
    texCoord[tIdx++] = 1.0; texCoord[tIdx++] = 0.0;
    normals[++n] = 0.0;normals[++n] = 0.0;normals[++n] = -1.0;

#ifndef COLORPICK_OPENGL_ES2
    glGenVertexArrays(1, &mesh->vertex_array[0]); // Create our Vertex Array Object
    glBindVertexArray(mesh->vertex_array[0]); // Bind our Vertex Array Object so we can use it
#endif

    glGenBuffers(4, &mesh->buffers[0]); // Generate our two Vertex Buffer Object

    glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[SHADER_POSITION]); // Bind our Vertex Buffer Object
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), vertices, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
    glVertexAttribPointer((GLuint)SHADER_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
    glEnableVertexAttribArray(SHADER_POSITION); // Enable the first our Vertex Array Object

    //you can comment out this block, we dont use colors for rects
//    glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[SHADER_COLOR]); // Bind our second Vertex Buffer Object
//    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), colors, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
//    glVertexAttribPointer((GLuint)SHADER_COLOR, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
//    glEnableVertexAttribArray(SHADER_COLOR); // Enable the second vertex attribute array

    glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[SHADER_TEXTURE]); // Bind our second Vertex Buffer Object
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), texCoord, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
    glVertexAttribPointer((GLuint)SHADER_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
    glEnableVertexAttribArray(SHADER_TEXTURE); // Enable the second vertex attribute array

    //you can comment out this block we dn't use normals for rendering rects
//    glBindBuffer(GL_ARRAY_BUFFER,  mesh->buffers[SHADER_NORMAL]); // Bind our second Vertex Buffer Object
//    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), normals, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
//    glVertexAttribPointer((GLuint)SHADER_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
//    glEnableVertexAttribArray(SHADER_NORMAL); // Enable the second vertex attribute array


    //glBindBuffer(GL_ARRAY_BUFFER, 0);
#ifndef COLORPICK_OPENGL_ES2
    glBindVertexArray(0); // Disable our Vertex Buffer Object
#endif
    delete [] vertices; // Delete our vertices from memory
    delete [] colors; // Delete our vertices from memory 
    delete [] texCoord;
    delete [] normals;

    mesh->vertex_count = 6;  // 3 componenets (xyz) per vertex, so the count is....
    mesh->is_fully_loaded = true;

    return mesh;
}

void OpenGLContext::doOpenURL(char* url){ // note: any spaces in the URL will cause this to not work (osx)... replace with + or encode to %20 ?

    char * i;
    for (i=url; *i; i++) {
        if( *i == ' ' ){
            *i = '+'; // space to +
        }else if( *i == '\n' ){
            *i = '~'; // newline to ~
        }
    }
    SDL_Log("now opening %s", url);
    openURL(url); // uses platform specific version from FileChooser.h
}

void OpenGLContext::doRequestReview(){
    SDL_Log("calling request review...");
    // TODO: we will also check the score here... to see if they are also successful overall at using the app....
    requestReview(); // uses platform specific version from FileChooser.h
}
