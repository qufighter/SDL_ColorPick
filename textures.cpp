#include "textures.h"  
#include "ColorPick.h"

bool Textures::ms_bInstanceCreated = false;
Textures* Textures::pInstance = NULL;

Textures* Textures::Singleton() {
    if(!ms_bInstanceCreated){
        pInstance = new Textures();
        ms_bInstanceCreated=true;
    }
    return pInstance;
}

/**
Default constructor
*/
Textures::Textures(void) {


    directionalScan = new DirectionalScan<SDL_Point, glm::vec3>(25, &Textures::vectorIsComponent);

}
/**
Destructor
*/
Textures::~Textures(void) {
	
}


void Textures::replaceTexture(GLuint textureid){
    LoadTexture("textures/dynamictexture.tga", textureid);
    //Uint32 delay = 250;
   // Uint32 my_timer_id;
    //my_timer_id = SDL_AddTimer(delay, my_callbackfunc, &my_timer_id);




}

//Uint32 my_callbackfunc(Uint32 interval, void *param)
//{
//    SDL_Event event;
//    SDL_UserEvent userevent;
//
//    /* In this example, our callback pushes an SDL_USEREVENT event
//     into the queue, and causes our callback to be called again at the
//     same interval: */
//
//    userevent.type = SDL_USEREVENT;
//    userevent.code = 0;
//    userevent.data1 = NULL;
//    userevent.data2 = NULL;
//
//    event.type = SDL_USEREVENT;
//    event.user = userevent;
//
//    SDL_PushEvent(&event);
//    return(interval);
//
//    SDL_RemoveTimer(*param);
//};

GLuint Textures::GenerateTexture(){
    GLuint textureid;
    glGenTextures(1, &textureid);
    return textureid;
}

//GLuint Textures::LoadTexture(SDL_Surface *surface, GLuint& textureid) {
//    GLuint textureid;
//    LoadTextureFromSdlSurface( surface,  textureid);
//    return textureid;
//}

GLuint Textures::LoadTextureSized(SDL_Surface *surface, GLuint& contained_in_texture_id, GLuint& textureid, int size, int *x, int *y) {
    LoadTextureSizedFromSdlSurface( surface, size, x, y, contained_in_texture_id, textureid);
    return textureid;
}


GLuint Textures::LoadTexture(const char* filename) {
    GLuint textureid;
    LoadTexture( filename,  textureid);
    return textureid;

}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{


    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
        case 1:
            return *p;
            break;

        case 2:
            return *(Uint16 *)p;
            break;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
            break;

        case 4:
            return *(Uint32 *)p;
            break;
            
        default:
            return 0;       /* shouldn't happen, but avoids warnings */
    }
}

SDL_Surface* Textures::ConvertSurface(SDL_Surface *origSurface) {
    return ConvertSurface(origSurface, nullptr);
}

SDL_Surface* Textures::ConvertSurface(SDL_Surface *origSurface, SDL_Color* backgroundColor) {

    if( origSurface == NULL ) return origSurface;

    SDL_Log("orig imgs pixel format name: %s", SDL_GetPixelFormatName(origSurface->format->format));

//    SDL_Log("orig MASKS for image as follows %i %i %i %i",
//            origSurface->format->Rmask == 0x000000ff,
//            origSurface->format->Gmask == 0x000000ff,
//            origSurface->format->Bmask == 0x000000ff,
//            origSurface->format->Amask == 0x000000ff
//            );

    bool isProbablyAndroid = origSurface->format->Rmask == 0x000000ff;

#if __ANDROID__
    isProbablyAndroid = true;
#endif


    if( backgroundColor!=nullptr ){

        //SDL_Surface *surface = SDL_DuplicateSurface(origSurface);

//        SDL_Surface *surface = SDL_CreateRGBSurface(0, origSurface->w, origSurface->h, origSurface->format->BitsPerPixel,
//                                                    origSurface->format->Rmask,
//                                                    origSurface->format->Gmask,
//                                                    origSurface->format->Bmask,
//                                                    origSurface->format->Amask);

        SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, origSurface->w, origSurface->h, origSurface->format->BitsPerPixel, isProbablyAndroid ? SDL_PIXELFORMAT_BGR888 : SDL_PIXELFORMAT_RGB888);


        SDL_FillRect(surface, &surface->clip_rect, SDL_MapRGB(surface->format, backgroundColor->r, backgroundColor->g, backgroundColor->b) );

        //int didBlit = SDL_BlitScaled(origSurface,NULL/* NULL src rect entire surface*/,surface,&surface->clip_rect);


        //SDL_SetSurfaceBlendMode(origSurface, SDL_BLENDMODE_BLEND );

        int didBlit = SDL_BlitSurface(origSurface,
                                      NULL, // src rect entire surface
                                      surface,
                                      &surface->clip_rect);

        SDL_Surface *surface2 = SDL_CreateRGBSurfaceWithFormat(0, origSurface->w, origSurface->h, origSurface->format->BitsPerPixel, isProbablyAndroid ? SDL_PIXELFORMAT_ABGR8888 : SDL_PIXELFORMAT_ARGB8888);

        SDL_BlitSurface(surface,
                        NULL, // src rect entire surface
                        surface2,
                        &surface->clip_rect);

        SDL_FreeSurface(surface);

        surface = SDL_CreateRGBSurfaceWithFormat(0, origSurface->w, origSurface->h, origSurface->format->BitsPerPixel, isProbablyAndroid ? SDL_PIXELFORMAT_BGR888 : SDL_PIXELFORMAT_RGB888);
        SDL_BlitSurface(surface2,
                        NULL, // src rect entire surface
                        surface,
                        &surface->clip_rect);

        SDL_FreeSurface(surface2);

//        int didBlit = SDL_LowerBlit(origSurface,
//                                      &surface->clip_rect,
//                                      surface,
//                                      &surface->clip_rect);

        SDL_FreeSurface(origSurface);
        origSurface = surface;

    }



    //        Uint32 curFormat = SDL_MasksToPixelFormatEnum(origSurface->format->BitsPerPixel,
    //                                                                  origSurface->format->Rmask,
    //                                                                  origSurface->format->Gmask,
    //                                                                  origSurface->format->Bmask,
    //                                                                  origSurface->format->Amask);
    //
    //        SDL_Log("images pixel format name: %s", SDL_GetPixelFormatName(curFormat));
    SDL_Log("images pixel format name: %s", SDL_GetPixelFormatName(origSurface->format->format));


//    SDL_Log("MASKS for image as follows %i %i %i %i",
//            origSurface->format->Rmask == 0x000000ff,
//            origSurface->format->Gmask == 0x000000ff,
//            origSurface->format->Bmask == 0x000000ff,
//            origSurface->format->Amask == 0x000000ff
//            );

    SDL_Log("images bpp: %i", origSurface->format->BytesPerPixel );

    // if we are android we really need to force 4bpp.. other plat at least 3bpp


    if (origSurface->format->BytesPerPixel == 3){

        SDL_Surface *surface;
        surface = SDL_ConvertSurfaceFormat(origSurface, isProbablyAndroid ? SDL_PIXELFORMAT_ABGR8888 : SDL_PIXELFORMAT_RGB888, 0);
        SDL_FreeSurface(origSurface);
        return surface;
    }else if (origSurface->format->BytesPerPixel == 1){ // gif may be 1bpp... (observed on android)
        SDL_Surface *surface;
        surface = SDL_ConvertSurfaceFormat(origSurface, isProbablyAndroid ? SDL_PIXELFORMAT_ABGR8888 : SDL_PIXELFORMAT_RGB888, 0);
        SDL_FreeSurface(origSurface);
        return surface;
    }









    return origSurface;
//
//    SDL_Surface *surface;
//

//
//    // work out what format to tell glTexImage2D to use...
//    if (origSurface->format->BytesPerPixel == 4 && origSurface->format->Amask > 0) { // RGB 24bit
//        //mode = GL_RGB;
//        // shall we check current format first?  or otherwise update origSurface elsewhere? // maths codeword
//        // TODO leeaking memory !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! freee surface
//        surface = SDL_ConvertSurfaceFormat(origSurface, curFormat, 0);
//        //GL_APPLE_texture_format_BGRA8888
//
//    } else /*if (origSurface->format->BytesPerPixel == 3) */{ // RGBA 32bit
//
//        //mode = GL_RGBA;
//        // TODO leeaking memory !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! freee surface
//
//        surface = SDL_ConvertSurfaceFormat(origSurface, curFormat, 0);
//        //GL_APPLE_texture_format_BGRA8888
//    }
////    else {
////        SDL_Log("Possible ERROR or otherwise %d BytesPerPixel not supported", surface->format->BytesPerPixel);
////        //SDL_FreeSurface(surface);
////    }
//
//    SDL_FreeSurface(origSurface);
//
//    return surface;
}



SDL_bool ModeForSurface(SDL_Surface *surface, GLint* internalFormat, GLenum* format){
    *format = GL_BGRA;

    bool isProbablyAndroid = surface->format->Rmask == 0x000000ff;

#if __ANDROID__
    isProbablyAndroid = true;
#endif

//    SDL_Log("MASKS for image as follows %i %i %i %i",
//            surface->format->Rmask == 0x000000ff,
//            surface->format->Gmask == 0x000000ff,
//            surface->format->Bmask == 0x000000ff,
//            surface->format->Amask == 0x000000ff
//            );

    if (surface->format->BytesPerPixel == 3  /*surface->format->format == SDL_PIXELFORMAT_RGB888 (not necessarily RGB order)*/ ) { // RGB 24bit
        *internalFormat = GL_RGB; // always keep alpah

        if (isProbablyAndroid){
            *format = GL_RGB;
        }
    } else if (surface->format->BytesPerPixel == 4 /*surface->format->format == SDL_PIXELFORMAT_ARGB8888 (not necessarily ARGB order) */) { // RGBA 32bit
        *internalFormat = GL_RGBA;

        if (isProbablyAndroid){
            *format = GL_RGBA;
        }
    } else {
        SDL_Log("Possible ERROR or otherwise %d BytesPerPixel not supported", surface->format->BytesPerPixel);

        //SDL_FreeSurface(surface);
        return SDL_FALSE;
    }

    return SDL_TRUE;
}


GLuint Textures::LoadTextureSizedFromSdlSurface(SDL_Surface *surface, int widthHeight, int *x, int *y, GLuint& contained_in_texture_id, GLuint& textureid){
    GLint mode;
    GLenum surfaceFmt;

    debugGLerror("LoadTextureSizedFromSdlSurface start - pre existing error");


    //SDL_Surface *surface;

    ModeForSurface(surface, &mode, &surfaceFmt);


    //    *textw=surface->w;
    //    *texth=surface->h;
    // create one texture name




    // this indented block can be normalized and shared between next fn and this one
        //SDL_Log("making 2048 from Width height %d %d !!!!", surface->w, surface->h );

        //shader currently set at 2048
        int hwh = widthHeight / 2;


    // SDL_CreateRGBSurface(<#Uint32 flags#>, <#int width#>, <#int height#>, <#int depth#>, <#Uint32 Rmask#>, <#Uint32 Gmask#>, <#Uint32 Bmask#>, <#Uint32 Amask#>)
        SDL_Surface* newSurface = SDL_CreateRGBSurface(0, widthHeight, widthHeight, surface->format->BitsPerPixel,
                                                       surface->format->Rmask,
                                                       surface->format->Gmask,
                                                       surface->format->Bmask,
                                                       surface->format->Amask);





    //SDL_SetSurfacePalette(newSurface, surface->format->palette);


    //SDL_CreateRGBSurfaceWithFormat(<#Uint32 flags#>, <#int width#>, <#int height#>, <#int depth#>, <#Uint32 format#>)

    //SDL_ConvertSurface

        float hswf = (surface->w * 0.5);
        float hshf = (surface->h * 0.5);
        int hsw = SDL_floorf(hswf);
        int hsh = SDL_floorf(hshf);
//        int x = *xp;
//        int y = *yp;

    //SDL_Log("pincking xy %i %i hsw %i hsh %i", *x, *y, hsw, hsh);

        // boundaries for x,y position
        if(*x >= hsw) *x= hsw - 1;
        else if(*x < -hsw) *x= -SDL_ceilf(hswf);//-hsw;// - 1;
        if(*y >= hsh) *y= hsh - 1;
        else if(*y < -hsh) *y= -SDL_ceilf(hshf);//-hsh;// - 1;

//
//    xp = &x;
//    yp = &y;

        SDL_Rect rect;


        if( contained_in_texture_id > 0 ){
            SDL_Log("Creating the \"contain in\" surface");

            // also second surface to process... todo move to helper function???
            // we will default to strecth
            SDL_Surface* new_contain_in_surface = SDL_CreateRGBSurface(0, widthHeight, widthHeight, surface->format->BitsPerPixel,
                                                           surface->format->Rmask,
                                                           surface->format->Gmask,
                                                           surface->format->Bmask,
                                                           surface->format->Amask);


            //when creating new 2048 from image... I have observed non black background, but picked color for background.... seems wrong...  (should we explicity SDL_FillRect black ??)
            SDL_FillRect(new_contain_in_surface, &new_contain_in_surface->clip_rect, SDL_MapRGB(new_contain_in_surface->format, 0, 0, 0) );


//            SDL_Rect srcRect;
//            srcRect.x = -*x;
//            srcRect.y = -*y;
//            srcRect.w = surface->w;
//            srcRect.h = surface->h;

            rect.x = 0.0f;
            rect.y = 0.0f;
            rect.w = widthHeight;//*2;
            rect.h = widthHeight;//*2;

            //SDL_Log("OUR SCALED BLIT RECT %i %i %i %i",rect.x,rect.y,rect.w,rect.h);

            

            //  rect.x += *x;
            //  rect.y += *y;


            float ratio = float(surface->w) / surface->h;  //ratio
            if( surface->w > surface->h ){
                rect.h = widthHeight / ratio;
                rect.y += (widthHeight - rect.h) * 0.5;

                //srcRect.w = widthHeight;

            }else{
                rect.w = widthHeight * ratio;
                rect.x += (widthHeight - rect.w) * 0.5;

                //srcRect.h = widthHeight;
            }

            //todo scale these
            // todo moving these to become defunct, move it in vert shader //maths
//            float xsc = (float(rect.w) / surface->w);
//            float ysc = (float(rect.h) / surface->h);
//            rect.x += *x * xsc;
//            rect.y += *y * ysc;// * ysc;


            //rect.h += *y * ysc;

            //SDL_Log("OUR SCALED BLIT RECT %i %i %i %i",rect.x,rect.y,rect.w,rect.h);


            //int didBlit = SDL_BlitSurface(surface,NULL/* src rect entire surface*/,new_contain_in_surface,&rect);
            //int didBlit = SDL_SoftStretch(surface,NULL/* src rect entire surface*/,new_contain_in_surface,&rect);
            int didBlit = SDL_BlitScaled(surface,NULL/* NULL src rect entire surface*/,new_contain_in_surface,&rect);
            

            if( didBlit != 0 ){
                SDL_Log("Blit contain in problem");
                SDL_Log("%s", SDL_GetError());
                //return 0;
            }

            // end block
            // tell opengl to use the generated texture name
            glBindTexture(GL_TEXTURE_2D, contained_in_texture_id);

//new_contain_in_surface->form
 //           SDL_PixelFormat

            /*typedef struct SDL_PixelFormat
             {
             Uint32 format;
             SDL_Palette *palette;
             Uint8 BitsPerPixel;
             Uint8 BytesPerPixel;
             Uint8 padding[2];
             Uint32 Rmask;
             Uint32 Gmask;
             Uint32 Bmask;
             Uint32 Amask;
             Uint8 Rloss;
             Uint8 Gloss;
             Uint8 Bloss;
             Uint8 Aloss;
             Uint8 Rshift;
             Uint8 Gshift;
             Uint8 Bshift;
             Uint8 Ashift;
             int refcount;
             struct SDL_PixelFormat *next;
             } SDL_PixelFormat;
             */

           // SDL_ConvertSurfaceFormat(<#SDL_Surface *src#>, <#Uint32 pixel_format#>, <#Uint32 flags#>)

            // this reads from the sdl surface and puts it into an opengl texture
            //glTexImage2D(GL_TEXTURE_2D, 0, mode, newSurface->w, newSurface->h, 0, mode,    GL_UNSIGNED_BYTE, newSurface->pixels);
            glTexImage2D(GL_TEXTURE_2D, 0, mode, widthHeight, widthHeight, 0, surfaceFmt, GL_UNSIGNED_BYTE, new_contain_in_surface->pixels);

            //SDL_Log("w:%d h:%d BytesPerPixel:%d textureId:%d ", newSurface->w, newSurface->h, newSurface->format->BytesPerPixel, textureid);

            // these affect how this texture is drawn later on... but mostly teh shader picks how pixly it gets
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);//GL_NEAREST
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,(GLfloat)( GL_CLAMP_TO_EDGE ) );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,(GLfloat)( GL_CLAMP_TO_EDGE ) );
//
//                glGenerateMipmap(GL_TEXTURE_2D);
//                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
//                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // just experimenting...
            //    debugGLerror();
            //    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            //    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            //    //glEnable(GL_TEXTURE_2D);
            //    debugGLerror();
//                glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,(GLfloat)(GL_REPEAT) );
//                glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,(GLfloat)(GL_REPEAT) );
            //    debugGLerror();
            //    glGenerateMipmap(GL_TEXTURE_2D);
            //    debugGLerror();

//            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,(GLfloat)( GL_MIRRORED_REPEAT ) );
//            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,(GLfloat)( GL_MIRRORED_REPEAT ) );

            //glGenerateMipmap(GL_TEXTURE_2D);

            debugGLerror("new_contain_in Texture Load Debug --->:");

            SDL_FreeSurface(new_contain_in_surface); // free temp surface


            //return textureid;
        }//end new_contain_in_surface

//        rect.x = 0.0f;
//        rect.y = 0.0f;


        //        // centering image in wh space
        //        // we will try to use same calcuation for both < or > or == really
        rect.x = hwh - hsw;
        rect.y = hwh - hsh;
        // centering complete, now offset
        rect.x += *x;
        rect.y += *y;


        rect.w = widthHeight;
        rect.h = widthHeight;

        int didBlit = SDL_BlitSurface(surface,
                                      NULL, // src rect entire surface
                                      newSurface,
                                      &rect);

    
        if( didBlit != 0 ){
            SDL_Log("Blit problem");
            SDL_Log("%s", SDL_GetError());
            //return 0;
        }


// get the pixels!!!
    colorFromSurface(newSurface, hwh-1, hwh-1, &selectedColor);



    // end block
    // tell opengl to use the generated texture name
    glBindTexture(GL_TEXTURE_2D, textureid);

    // this reads from the sdl surface and puts it into an opengl texture
    //glTexImage2D(GL_TEXTURE_2D, 0, mode, newSurface->w, newSurface->h, 0, mode,    GL_UNSIGNED_BYTE, newSurface->pixels);
    glTexImage2D(GL_TEXTURE_2D, 0, mode, newSurface->w, newSurface->h, 0, surfaceFmt, GL_UNSIGNED_BYTE, newSurface->pixels);

    //SDL_Log("w:%d h:%d BytesPerPixel:%d textureId:%d ", newSurface->w, newSurface->h, newSurface->format->BytesPerPixel, textureid);


    /* TextureWrapMode
     #define GL_REPEAT                                        0x2901
     #define GL_CLAMP_TO_EDGE                                 0x812F
     #define GL_MIRRORED_REPEAT                               0x8370
     */
    //    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,(GLfloat)( GL_MIRRORED_REPEAT ) );
    //    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,(GLfloat)( GL_MIRRORED_REPEAT ) );
//    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,(GLfloat)( GL_CLAMP_TO_EDGE ) );
//    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,(GLfloat)( GL_CLAMP_TO_EDGE ) );

        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,(GLfloat)( GL_REPEAT ) );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,(GLfloat)( GL_REPEAT ) );



    // these affect how this texture is drawn later on... but mostly teh shader picks how pixly it gets
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);//GL_LINEAR GL_NEAREST
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);// GL_NEAREST
    //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);// GL_NEAREST

//    glGenerateMipmap(GL_TEXTURE_2D);
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // just experimenting...
    //    debugGLerror();
    //    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    //    //glEnable(GL_TEXTURE_2D);
    //    debugGLerror();
    //    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,(GLfloat)(GL_REPEAT) );
    //    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,(GLfloat)(GL_REPEAT) );
    //    debugGLerror();
    //    glGenerateMipmap(GL_TEXTURE_2D);
    //    debugGLerror();



    //glGenerateMipmap(GL_TEXTURE_2D);

    debugGLerror("Texture Load Debug --->:");

    SDL_FreeSurface(newSurface); // free temp surface

    return textureid;
}

glm::vec3 Textures::getVec3ColorForPoint(SDL_Point point){
    Textures* self = Textures::Singleton();
    bool inBounds = colorFromSurface(self->scanSurface, self->scanOrigin.x + point.x, self->scanOrigin.y + point.y, &self->tmpColor);
    if( !inBounds ){
        return glm::vec3(99999.0, 99999.0, 99999.0);
    }
    return glm::vec3(self->tmpColor.r, self->tmpColor.g, self->tmpColor.b);
}

// too many args..
bool Textures::searchSurfaceForColor(SDL_Surface *newSurface, SDL_Color* seekClr, int x, int y, int* outx, int* outy){
// return true if we found a better match that requires a pixel movement

    // todo call
    // SDL_Point result = myDirectionalScan->getOffsetForBestMatch(dest, &getDistanceForVec3s, &getItemForPointTest2);
    // originx = hwh - x;
    // originy = hwh - y;
    // SDL_Point origin; // use this for our lookup function
    // whatever SDL_Point result we get out... we can just add it to x,y and return those (outx, outy)
    // FINALLY - evaluate how this fn is called, it can probably just be a void*... otherwise return true if hte point is non zero,zero
    /*
         *outx = x + result.x;
         *outy = y + result.y;
    */

    //  directionalScan = new DirectionalScan<SDL_Point, glm::vec3>(25, &Textures::vectorIsComponent);
    //searchSurfaceForColor
    int hwh = SDL_min(newSurface->w, newSurface->h) / 2;
    hwh-=1;
    glm::vec3 dest = glm::vec3(seekClr->r, seekClr->g, seekClr->b);
    scanOrigin.x = hwh - x;
    scanOrigin.y = hwh - y;
    scanSurface = newSurface;
    SDL_Point result = directionalScan->getOffsetForBestMatch(dest, &Textures::getDistanceForVec3s, &Textures::getVec3ColorForPoint);

    *outx = x - result.x;
    *outy = y - result.y;

    if( result.x != 0 || result.y != 0 ){
        return true;
    }
    return false;
}


bool Textures::colorFromSurface(SDL_Surface *newSurface, int x, int y, SDL_Color* outColor){

    // getting pixels code should move from here
    //Uint32 temp;
    Uint8 red, green, blue, alpha;

    if( x < 0 || y < 0 || x > newSurface->w || y > newSurface->h ){
        SDL_Log("ERROR/WARNING: Getting OOB pixel");
        return false;
    }

    bool mustLock = SDL_MUSTLOCK( newSurface );
    if(  mustLock ){
        SDL_LockSurface( newSurface );
    }
    Uint32 pix = getpixel(newSurface, x, y);
    if( mustLock ){
        SDL_UnlockSurface( newSurface );
    }

    if( newSurface->format->Rmask == 0 && newSurface->format->Gmask == 0 && newSurface->format->Bmask == 0){
        // this is an indexed color image not true color
        //https://wiki.libsdl.org/SDL_PixelFormat

        //TODO - try indexed color img!
        //outColor=&newSurface->format->palette->colors[pix];

        outColor->r = newSurface->format->palette->colors[pix].r;
        outColor->g = newSurface->format->palette->colors[pix].g;
        outColor->b = newSurface->format->palette->colors[pix].b;
        outColor->a = newSurface->format->palette->colors[pix].a;

        SDL_Log("Pixel Color-> Red: %d, Green: %d, Blue: %d. Index: %d\n",
               outColor->r, outColor->g, outColor->b, pix);
    }else{


        //            /* Get Red component */
        //            temp = pix & newSurface->format->Rmask;  /* Isolate red component */
        //            temp = temp >> newSurface->format->Rshift; /* Shift it down to 8-bit */
        //            temp = temp << newSurface->format->Rloss;  /* Expand to a full 8-bit number */
        //            red = (Uint8)temp;
        //
        //            /* Get Green component */
        //            temp = pix & newSurface->format->Gmask;  /* Isolate green component */
        //            temp = temp >> newSurface->format->Gshift; /* Shift it down to 8-bit */
        //            temp = temp << newSurface->format->Gloss;  /* Expand to a full 8-bit number */
        //            green = (Uint8)temp;
        //
        //            /* Get Blue component */
        //            temp = pix & newSurface->format->Bmask;  /* Isolate blue component */
        //            temp = temp >> newSurface->format->Bshift; /* Shift it down to 8-bit */
        //            temp = temp << newSurface->format->Bloss;  /* Expand to a full 8-bit number */
        //            blue = (Uint8)temp;
        //
        //            /* Get Alpha component */
        //            temp = pix & newSurface->format->Amask;  /* Isolate alpha component */
        //            temp = temp >> newSurface->format->Ashift; /* Shift it down to 8-bit */
        //            temp = temp << newSurface->format->Aloss;  /* Expand to a full 8-bit number */
        //            alpha = (Uint8)temp;

        SDL_GetRGBA(pix, newSurface->format, &red, &green, &blue, &alpha);

        outColor->r = red;
        outColor->g = green;
        outColor->b = blue;
        outColor->a = alpha;
        //SDL_Log("Pixel Color -> R: %d,  G: %d,  B: %d,  A: %d\n", red, green, blue, alpha);

    }

    return true; // guess the pixel is valid!  cool....
}

///* Determine the corresponding GLES texture format params */
//switch (texture->format)
//{
//    case SDL_PIXELFORMAT_ARGB8888:
//    case SDL_PIXELFORMAT_ABGR8888:
//    case SDL_PIXELFORMAT_RGB888:
//    case SDL_PIXELFORMAT_BGR888:
//        format = GL_RGBA;
//        type = GL_UNSIGNED_BYTE;
//        break;
//    case SDL_PIXELFORMAT_IYUV:
//    case SDL_PIXELFORMAT_YV12:
//    case SDL_PIXELFORMAT_NV12:
//    case SDL_PIXELFORMAT_NV21:
//        format = GL_LUMINANCE;
//        type = GL_UNSIGNED_BYTE;
//        break;
//#ifdef GL_TEXTURE_EXTERNAL_OES
//    case SDL_PIXELFORMAT_EXTERNAL_OES:
//        format = GL_NONE;
//        type = GL_NONE;
//        break;
//#endif
//    default:
//        return SDL_SetError("Texture format not supported");
//}

//SDL_FORCE_INLINE SDL_bool
//convert_format(GL_RenderData *renderdata, Uint32 pixel_format,
//               GLint* internalFormat, GLenum* format, GLenum* type)
//{
//    switch (pixel_format) {
//        case SDL_PIXELFORMAT_ARGB8888:
//            *internalFormat = GL_RGBA8;
//            *format = GL_BGRA;
//            *type = GL_UNSIGNED_INT_8_8_8_8_REV;
//            break;
//        case SDL_PIXELFORMAT_YV12:
//        case SDL_PIXELFORMAT_IYUV:
//        case SDL_PIXELFORMAT_NV12:
//        case SDL_PIXELFORMAT_NV21:
//            *internalFormat = GL_LUMINANCE;
//            *format = GL_LUMINANCE;
//            *type = GL_UNSIGNED_BYTE;
//            break;
//#ifdef __MACOSX__
//        case SDL_PIXELFORMAT_UYVY:
//            *internalFormat = GL_RGB8;
//            *format = GL_YCBCR_422_APPLE;
//            *type = GL_UNSIGNED_SHORT_8_8_APPLE;
//            break;
//#endif
//        default:
//            return SDL_FALSE;
//    }
//    return SDL_TRUE;
//}


// defunct ?? (used for ascii)
GLuint Textures::LoadTextureFromSdlSurface(SDL_Surface *surface, GLuint& textureid){
    GLint mode;
    GLenum surfaceFmt = GL_BGRA;

    debugGLerror("LoadTextureFromSdlSurface start - pre existing error");

    SDL_Log("LoadTextureFromSdlSurface images pixel format name: %s", SDL_GetPixelFormatName(surface->format->format));


    ModeForSurface(surface, &mode, &surfaceFmt);


    // create one texture name
    glGenTextures(1, &textureid);
    debugGLerror("LoadTextureFromSdlSurface glGenTextures");

    // tell opengl to use the generated texture name
    glBindTexture(GL_TEXTURE_2D, textureid);

    debugGLerror("LoadTextureFromSdlSurface glBindTexture");


    //    *textw=surface->w;
    //    *texth=surface->h;
    if( surface->w != surface->h || surface->w > 2048 ){
        SDL_Log("WARNIGN Width height are not matched %d %d making 2048 !!!!", surface->w, surface->h );

        int wh = max(surface->w, surface->h);
        wh = 2048; //2048 max ios (older device)
        //shader currently set at 256
        int hwh = wh / 2;

        SDL_Surface* newSurface = SDL_CreateRGBSurface(0, wh, wh, surface->format->BitsPerPixel,
                                                       surface->format->Rmask,
                                                       surface->format->Gmask,
                                                       surface->format->Bmask,
                                                       surface->format->Amask);

        SDL_Rect rect;
        rect.x = 0.0f;
        rect.y = 0.0f;
        rect.w = wh;
        rect.h = wh;

//        // centering image in wh space
//        // we will try to use same calcuation for both < or > or == really
            rect.x = hwh - (surface->w / 2);
            rect.y = hwh - (surface->h / 2);


        // SDL_BlitScaled ?SDL_BlitSurface
        int didBlit = SDL_BlitScaled(surface,
                                      NULL, // src rect entire surface
                                      newSurface,
                                      &rect);

        if( didBlit != 0 ){
            SDL_Log("Blit problem");
            SDL_Log("%s", SDL_GetError());
        }

        //SDL_FreeSurface(surface);
        //surface = newSurface;
        glTexImage2D(GL_TEXTURE_2D, 0, mode, newSurface->w, newSurface->h, 0, surfaceFmt, GL_UNSIGNED_BYTE, newSurface->pixels);
        debugGLerror("LoadTextureFromSdlSurface resized/shrunk texture glTexImage2D");


        SDL_FreeSurface(newSurface);
    }else{
        glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, surfaceFmt, GL_UNSIGNED_BYTE, surface->pixels);
        debugGLerror("LoadTextureFromSdlSurface vanilla glTexImage2D");
    }


    // this reads from the sdl surface and puts it into an opengl texture
    //glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode,    GL_UNSIGNED_BYTE, surface->pixels);

    //SDL_Log("w:%d h:%d BytesPerPixel:%d textureId:%d ", surface->w, surface->h, surface->format->BytesPerPixel, textureid);

/*
 
 // TextureMagFilter
#define GL_NEAREST                                       0x2600
#define GL_LINEAR                                        0x2601

//
         GL_NEAREST
         GL_LINEAR
#define GL_NEAREST_MIPMAP_NEAREST                        0x2700
#define GL_LINEAR_MIPMAP_NEAREST                         0x2701
#define GL_NEAREST_MIPMAP_LINEAR                         0x2702
#define GL_LINEAR_MIPMAP_LINEAR                          0x2703
 */

    // these affect how this texture is drawn later on... including ascii, etc
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    debugGLerror("LoadTextureFromSdlSurface GL_TEXTURE_MIN_FILTER GL_TEXTURE_MAG_FILTER");


    glGenerateMipmap(GL_TEXTURE_2D);
    debugGLerror("LoadTextureFromSdlSurface glGenerateMipmap");

//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    //
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    debugGLerror("LoadTextureFromSdlSurface GL_LINEAR_MIPMAP_NEAREST");


    // just experimenting...
    //    debugGLerror();
    //    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    //    //glEnable(GL_TEXTURE_2D);
    //    debugGLerror();
    //    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,(GLfloat)(GL_REPEAT) );
    //    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,(GLfloat)(GL_REPEAT) );
    //    debugGLerror();
    //    glGenerateMipmap(GL_TEXTURE_2D);
    //    debugGLerror();

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,(GLfloat)( GL_REPEAT ) );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,(GLfloat)( GL_REPEAT ) );


    //glGenerateMipmap(GL_TEXTURE_2D);

    debugGLerror("Texture Load Debug LoadTextureFromSdlSurface (surface/ascii/etc) --->:");

    return textureid;
}

GLuint Textures::LoadTexture(const char* filename, GLuint& textureid) {
    SDL_Surface* surface = ConvertSurface(LoadSurface(filename));
    LoadTextureFromSdlSurface(surface, textureid);
    // clean up
    SDL_FreeSurface(surface);
    return textureid;
}

SDL_Surface* Textures::LoadSurface(const char* filename) {

    SDL_Surface *surface = IMG_Load(filename); // this probably does the below... in one line
//    SDL_RWops* fileref = SDL_RWFromFile(filename, "r");
//    SDL_Surface *surface = IMG_Load_RW(fileref, 0); // it can be done in two lines
//    SDL_RWclose(fileref);

    if (!surface) {
        SDL_Log("Cannot find file %s", filename);
        return 0;
    }

    return surface;
}


GLuint Textures::LoadTexture(unsigned char *data, int width, int height) {
	GLuint texture_id;
	//int wrap = 1;

	
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	
	debugGLerror();
	
		/*void glTexImage2D(	GLenum  	target,
 	GLint  	level,
 	GLint  	internalFormat,
 	GLsizei  	width,a
 	GLsizei  	height,
 	GLint  	border,
 	GLenum  	format,
 	GLenum  	type,
 	const GLvoid *  	data);*/
	DebugMessage(("HELLO---"));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);


	debugGLerror();
	DebugMessage(("HELLO---glTexImage2D done"));
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	//glEnable(GL_TEXTURE_2D);

//	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,(GLfloat)(wrap ? GL_REPEAT : GL_CLAMP) );
//	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,(GLfloat)(wrap ? GL_REPEAT : GL_CLAMP) );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,(GLfloat)( GL_REPEAT ) );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,(GLfloat)( GL_REPEAT ) );

	glGenerateMipmap(GL_TEXTURE_2D);

	debugGLerror();
	//glDrawPixels(treeTexture.width, treeTexture.height,treeTexture.type, GL_UNSIGNED_BYTE,treeTexture.imageData);

	return texture_id;
}
/*
//similar to http://en.wikibooks.org/wiki/OpenGL_Programming/Intermediate/Textures
GLuint Textures::LoadTexture(const char * filename) {
	GLuint texture;
    int width, height;
	int wrap = 0;
    BYTE * data;
    FILE * file;

    // open texture data
    file = fopen( filename, "rb" );
    if ( file == NULL ) return 0;

    // allocate buffer
    width = 256;
    height = 256;
    data = (BYTE*)malloc( width * height * 3 );

    // read texture data
    fread( data, width * height * 3, 1, file );
    fclose( file );

    // allocate a texture name
    glGenTextures( 1, &texture );

    // select our current texture
    glBindTexture( GL_TEXTURE_2D, texture );

	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB,
				GL_UNSIGNED_BYTE, (const GLvoid *)data);

    // select modulate to mix texture with color for shading
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                     GL_LINEAR_MIPMAP_NEAREST );
    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                     (GLfloat)(wrap ? GL_REPEAT : GL_CLAMP) );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                     (GLfloat)(wrap ? GL_REPEAT : GL_CLAMP) );

    // build our texture mipmaps
    //gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data );
	glGenerateMipmap(texture);

    // free buffer
    SDL_free( data );

    return texture;
}*/


void Textures::screenshot(char* filename,int x, int y)//width height
{
    /*
    SDL_Surface* saveSurface = NULL;
    SDL_Surface* infoSurface = SDL_GetWindowSurface(openglContext->sdlWindow);
    //IMG_SavePNG(infoSurface, filename);

    if (infoSurface == NULL) {
        std::cerr << "Failed to create info surface from window in saveScreenshotBMP(string), SDL_GetError() - " << SDL_GetError() << "\n";
    } else {
        unsigned char * pixels = new (std::nothrow) unsigned char[infoSurface->w * infoSurface->h * infoSurface->format->BytesPerPixel];
        if (pixels == 0) {
            std::cerr << "Unable to allocate memory for screenshot pixel data buffer!\n";
            return;
        } else {


            saveSurface = SDL_CreateRGBSurfaceFrom(pixels, infoSurface->w, infoSurface->h, infoSurface->format->BitsPerPixel, infoSurface->w * infoSurface->format->BytesPerPixel, infoSurface->format->Rmask, infoSurface->format->Gmask, infoSurface->format->Bmask, infoSurface->format->Amask);
            if (saveSurface == NULL) {
                std::cerr << "Couldn't create SDL_Surface from renderer pixel data. SDL_GetError() - " << SDL_GetError() << "\n";
                return;
            }
            IMG_SavePNG(saveSurface, filename);
            SDL_FreeSurface(saveSurface);
            saveSurface = NULL;

            delete[] pixels;
        }
        SDL_FreeSurface(infoSurface);
        infoSurface = NULL;
    }*/



     // upside down
    int bpp = 4;
    unsigned char * pixels = new unsigned char[x*y*bpp]; // 4 bytes for RGBA
    unsigned char * pixels2 = new unsigned char[x*y*bpp];

    glReadPixels(0,0,x, y, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    //flip
    int stride = x*bpp;
    for(int row =0; row<y; row++){
        std::memcpy(&pixels2[row*stride],&pixels[(y-row)*stride],stride);
    }

    SDL_Surface * surf = SDL_CreateRGBSurfaceFrom(pixels2, x, y, 8*4, x*bpp, 0,0,0,0);
    IMG_SavePNG(surf, filename);
    SDL_FreeSurface(surf);
    delete [] pixels;
    delete [] pixels2;

}

void Textures::dataToTgaFile(char filename[160],unsigned char *data,int x, int y)//width height
{
	// get the image data
	long imageSize = x * y * 4;

	// split x and y sizes into bytes
	int xa= x % 256;
	int xb= (x-xa)/256;

	int ya= y % 256;
	int yb= (y-ya)/256;

	//assemble the header
	unsigned char header[18]={0,0,2,0,0,0,0,0,0,0,0,0,static_cast<unsigned char>((char)xa),static_cast<unsigned char>((char)xb),static_cast<unsigned char>((char)ya),static_cast<unsigned char>((char)yb),32,0};

	// write header and data to file
	ofstream File(filename, ios::out | ios::binary);
	//File << "This is another line.\n";
	File.write(reinterpret_cast<const char *>(header), sizeof(char)*18);
	File.write(reinterpret_cast<const char *>(data), sizeof(char)*imageSize);
	File.close();
}

//void Textures::screenToTexture(int x, int y, int width, int height)
//{
//	lower left hand corner
//}
//GLuint Textures::screenToTexture(int x, int y, int width, int height, GLuint targetTexture)
//{
//    return screenToTexture(x, y, width, height, GL_BACK, targetTexture);
//    /*
//    long imageSize = width * height * 4;
//    unsigned char *data = new unsigned char[imageSize];
//    glReadPixels(x,y,width, height, GL_BGRA,GL_UNSIGNED_BYTE,data);
//    GLuint result = LoadTexture(data, width, height);
//
//    if(data){
//        delete[] data;
//        data=NULL;
//    }
//    return result;
//*/
//    //DebugMessage(("screenToTexture"));
//    //debugGLerror();
//
//        /*void glTexImage2D(    GLenum      target,
//     GLint      level,
//     GLint      internalFormat,
//     GLsizei      width,a
//     GLsizei      height,
//     GLint      border,
//     GLenum      format,
//     GLenum      type,
//     const GLvoid *      data);*/
//
//
//
//
//    //DebugMessage(("screenToTexture glReadPixels"));
//    //debugGLerror();
//
////debug only
////dataToTgaFile("textures/dynamictexture.tga", data, width,  height);
//
//
//}

//GLuint Textures::screenToTexture(int x, int y, int width, int height, GLuint bufferToRead, GLuint targetTexture){
//    glReadBuffer(bufferToRead);
//    glBindTexture(GL_TEXTURE_2D, targetTexture);
//    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, width, height, 0);
//    return targetTexture;
//}
//glDeleteTextures( 1, &texture );
