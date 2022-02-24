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

#include "LinuxFileChooser.h"

#include   <X11/Xlib.h>
//#include <xcb/xcb.h>

void getImagePathFromMainThread(){

}

//std::vector<uint8_t> PixelsToBitmap(std::vector<uint8_t> pixels, size_t width, size_t height, int16_t depth)
//{
//    BITMAPFILEHEADER bmpFileHeader = {};
//    bmpFileHeader.bfType = kBitmapHeaderMagic;
//    bmpFileHeader.bfReserved1 = bmpFileHeader.bfReserved2 = 0;
//    bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
//    bmpFileHeader.bfSize = bmpFileHeader.bfOffBits + pixels.size();
//
//    BITMAPINFOHEADER bmpInfoHeader = {};
//    bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
//    bmpInfoHeader.biWidth = width;
//    bmpInfoHeader.biHeight = -height;
//    bmpInfoHeader.biPlanes = 1;
//    bmpInfoHeader.biBitCount = static_cast<int16_t>(depth);
//    bmpInfoHeader.biClrUsed = 0;
//    bmpInfoHeader.biClrImportant = 0;
//
//    std::vector<uint8_t> bitmap;
//    bitmap.reserve(bmpFileHeader.bfSize);
//
//    bitmap.insert(bitmap.end(),
//        reinterpret_cast<uint8_t*>(&bmpFileHeader),
//        reinterpret_cast<uint8_t*>(&bmpFileHeader) + sizeof(BITMAPFILEHEADER));
//    bitmap.insert(bitmap.end(),
//        reinterpret_cast<uint8_t*>(&bmpInfoHeader),
//        reinterpret_cast<uint8_t*>(&bmpInfoHeader) + sizeof(BITMAPINFOHEADER));
//    bitmap.insert(bitmap.end(), pixels.begin(), pixels.end());
//    return bitmap;
//} It was originally published on https://www.apriorit.com/

// withxlib...
//std::vector<uint8_t> pixels(
//image->data
//, image->data + image->bytes_per_line * image->height
//);
//auto bitmap = PixelsToBitmap(
//pixels
//, width
//, height
//, image->bits_per_pixel
//);
//
//std::ofstream file(
//"xlib.bmp"
//, std::ios::binary
//);
//file.write(
//reinterpret_cast<const char*>(bitmap.data())
//, bitmap.size()
//);


//Now it’s time to explore the same operation with the XCB library:
//
//std::vector<uint8_t> pixels(
//    xcb_get_image_data(image.get())
//    , xcb_get_image_data(image.get()) +
//xcb_get_image_data_length(image.get())
//);
//auto depth = pixels.size() / width / height * 8;
//auto bitmap = PixelsToBitmap(pixels, width, height, depth);
//
//std::ofstream file("xcb.bmp", std::ios::binary);
//file.write(
//reinterpret_cast<const char*>(bitmap.data())
//, bitmap.size()
//); It was originally published on https://www.apriorit.com/

static SDL_Surface* Create_SDL_Surface_From_XImage(XImaeg* img, XWindowAttributes attr){

    SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, at.width, at.height, 32, img->red_mask, img->green_mask, img->blue_mask, 0);
    int x, y;
    SDL_LockSurface(tmp);
    // holy slow shit batman... we can do better than this!
    for (y = 0; y < at.height; y++) {
        for (x = 0; x < at.width; x++) {
            ((unsigned *)tmp->pixels)[x + y * at.width] = XGetPixel(img, x,  y);
        }
    }
    SDL_UnlockSurface(tmp);
    return tmp
    //SDL_SaveBMP(tmp, filename);
    //SDL_FreeSurface(tmp);
}

#define COLORPICK_USE_XCB 1
void beginScreenshotSeleced(){

    #ifndef COLORPICK_USE_XCB
        //Xlib.h
        Display* dsp = XOpenDisplay(nullptr);
        Window root = RootWindow(dsp, DefaultScreen(dsp));

        XWindowAttributes attr = {};
        XGetWindowAttributes(dsp, root, &attr);

        auto width = attr.width;
        auto height = attr.height;

        XImage* image = XGetImage(dsp, root, 0, 0, width, height, AllPlanes, ZPixmap);

        // now conveert and load...

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

        uint8_t *xcb_get_image_data(const xcb_get_image_request_t *reply);

        // next, process the data

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

    char * sysLaunchCmd = "xdg-open "
    size_t len = SDL_strlen(sysLaunchCmd) + SDL_strlen(url) + 4;
    char* fullCmdDest = (char*)SDL_malloc( sizeof(char) * len );
    SDL_snprintf(*fullCmdDest, len, "%s%s", sysLaunchCmd, url);
    system(fullCmdDest);
    SDL_Free(fullCmdDest);

    return true;
}

void requestReview(){
    
}

#endif
