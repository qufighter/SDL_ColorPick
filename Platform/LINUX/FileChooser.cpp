//
//  FileChooser.mm
//  ColorPick SDL
//
//  Created by Sam Larison on 8/13/16.
//
//

// YOU MAY HAVE TO DELETE THIS FILE FROM OSX PROJECT - IT WILL CONFUSE XCODE


#if defined(__ANDROID__) | defined(__MACOSX__) // etc?
    // skip, wrong platform!  we tried #defiend(__LINUX__) here but for some reason the constant is not defined...?
#else

#include "LinuxFileChooser.h" //doens't work right...


#include "../../ColorPick.h"
#include "portable-file-dialogs.h"



#include "SDL.h" // redundant?
#include "stdlib.h"

#define COLORPICK_USE_XCB 1
#ifndef COLORPICK_USE_XCB
#include   <X11/Xlib.h>
#else
#include <xcb/xcb.h>
#endif

void getImagePathFromMainThread(){

}

void beginScreenshotSeleced(){

    #ifndef COLORPICK_USE_XCB
        //Xlib.h // not fully implemented! see below...
        Display* dsp = XOpenDisplay(nullptr);
        Window root = RootWindow(dsp, DefaultScreen(dsp));

        XWindowAttributes attr = {};
        XGetWindowAttributes(dsp, root, &attr);

        auto width = attr.width;
        auto height = attr.height;

        XImage* image = XGetImage(dsp, root, 0, 0, width, height, AllPlanes, ZPixmap);

        // now conveert and load... TODO (not implemented!!)

        XDestroyImage(image);

        XCloseDisplay(dsp);

        //xcb.h
    #else // ifdef COLORPICK_USE_XCB
        xcb_connection_t* dsp = xcb_connect(nullptr, nullptr);
        xcb_window_t root =
        xcb_setup_roots_iterator(xcb_get_setup(dsp)).data->root;

        std::shared_ptr<xcb_get_geometry_reply_t> geometry(
            xcb_get_geometry_reply(
                dsp
                , xcb_get_geometry(dsp, root)
                , nullptr
            )
            , free
        );
        auto width = geometry->width;
        auto height = geometry->height;

        std::shared_ptr<xcb_get_image_reply_t> image(
            xcb_get_image_reply(
                dsp
                , xcb_get_image(
                    dsp
                    , XCB_IMAGE_FORMAT_Z_PIXMAP
                    , root
                    , 0
                    , 0
                    , width
                    , height
                    , static_cast<uint32_t>(~0)
                )
                , nullptr
            )
            , free
        );

        auto comppp = xcb_get_image_data_length(image.get()) / width / height;
        auto depth = comppp * 8;

        SDL_Surface* srf = SDL_CreateRGBSurfaceFrom(xcb_get_image_data(image.get()),
            width,
            height,
            depth,
            comppp * width, // pitch isn't stride, its stride /8
            0x00FF0000,
            0x0000FF00,
            0x000000FF,
            0x00000000
        );


        openglContext->imageWasSelectedCb(srf, false);

        // TODO: MOUSE SHILED
        // TODO: FOOLLOW MOUSE
        // TODO: CATCH CLICK


        xcb_disconnect(dsp);
    #endif


}

void beginImageSelector()
{
    // GENERIC CROSS PLATFORM FILE DIALOGUE!!! (cool :)
    // https://github.com/samhocevar/portable-file-dialogs/blob/master/doc/open_file.md
    auto selection = pfd::open_file("").result();
    if (!selection.empty()){
        openglContext->imageWasSelectedCb(openglContext->textures->LoadSurface(selection[0].c_str()), true);
        SDL_RaiseWindow(openglContext->getSdlWindow());
    }else{
        SDL_RaiseWindow(openglContext->getSdlWindow());
    }
}

bool openURL(char* &url)
{
    //url
    //system("x-www-browser http://www.google.com");
    //SDL_Log("%s", url);
    
    char * sysLaunchCmd = "xdg-open ";
    size_t len = SDL_strlen(sysLaunchCmd) + SDL_strlen(url) + 4;
    char* fullCmdDest = (char*)SDL_malloc( sizeof(char) * len );
    SDL_snprintf(fullCmdDest, len, "%s%s", sysLaunchCmd, url);
    //SDL_Log("%s", fullCmdDest);
    system(fullCmdDest);
    SDL_free(fullCmdDest);

    return true;
}

void requestReview(){
    
}

#endif
