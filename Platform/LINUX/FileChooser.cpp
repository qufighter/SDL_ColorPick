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

#include "LinuxFileChooser.h" //doens't work right... we have to add our includes below...


#include "../../ColorPick.h"
#include "portable-file-dialogs.h"


#include "SDL.h" // redundant?
#include "stdlib.h" // for system() call

#define COLORPICK_USE_XCB 1
#ifndef COLORPICK_USE_XCB
#include   <X11/Xlib.h>
#else
#include <xcb/xcb.h>
#endif

static bool pick_mode_enabled=false;

// TODO: we need non GTK variant support...
#define COLORPICK_X11_GTK 1
#ifdef COLORPICK_USE_XCB
#include <gtk/gtk.h>

typedef struct {
  GdkRectangle  rect;
  gboolean      button_pressed;
  GtkWidget    *window;

  gboolean      aborted;
} select_area_filter_data;

static gboolean
select_area_button_press (GtkWidget               *window,
                          GdkEventButton          *event,
                          select_area_filter_data *data)
{
  if (data->button_pressed)
    return TRUE;

  data->button_pressed = TRUE;
  data->rect.x = event->x_root;
  data->rect.y = event->y_root;

  return TRUE;
}

static gboolean
select_area_motion_notify (GtkWidget               *window,
                           GdkEventMotion          *event,
                           select_area_filter_data *data)
{


    OpenGLContext* openglContext = OpenGLContext::Singleton();
    ColorPickState* colorPickState = ColorPickState::Singleton();

    //openglContext->position_x =(openglContext->fullPickImgSurface->clip_rect.w - (int)mloc.x) - (openglContext->fullPickImgSurface->clip_rect.w / 2);
    //openglContext->position_y =((int)mloc.y - openglContext->fullPickImgSurface->clip_rect.h) + (openglContext->fullPickImgSurface->clip_rect.h / 2);
    //colorPickState->movedxory = true;

    openglContext->renderShouldUpdate = true; // do not call renderScene from timer thread!

    return TRUE; // remove dead code below???

  GdkRectangle draw_rect; // not used

  if (!data->button_pressed)
    return TRUE;


  draw_rect.width = ABS (data->rect.x - event->x_root);
  draw_rect.height = ABS (data->rect.y - event->y_root);
  draw_rect.x = MIN (data->rect.x, event->x_root);
  draw_rect.y = MIN (data->rect.y, event->y_root);

  if (draw_rect.width <= 0 || draw_rect.height <= 0)
    {
      gtk_window_move (GTK_WINDOW (window), -100, -100);
      gtk_window_resize (GTK_WINDOW (window), 10, 10);
      return TRUE;
    }

  gtk_window_move (GTK_WINDOW (window), draw_rect.x, draw_rect.y);
  gtk_window_resize (GTK_WINDOW (window), draw_rect.width, draw_rect.height);

  /* We (ab)use app-paintable to indicate if we have an RGBA window */
  if (!gtk_widget_get_app_paintable (window))
    {
      GdkWindow *gdkwindow = gtk_widget_get_window (window);

      /* Shape the window to make only the outline visible */
      if (draw_rect.width > 2 && draw_rect.height > 2)
        {
          cairo_region_t *region;
          cairo_rectangle_int_t region_rect = {
            0, 0,
            draw_rect.width, draw_rect.height
          };

          region = cairo_region_create_rectangle (&region_rect);
          region_rect.x++;
          region_rect.y++;
          region_rect.width -= 2;
          region_rect.height -= 2;
          cairo_region_subtract_rectangle (region, &region_rect);

          gdk_window_shape_combine_region (gdkwindow, region, 0, 0);

          cairo_region_destroy (region);
        }
      else
        gdk_window_shape_combine_region (gdkwindow, NULL, 0, 0);
    }

  return TRUE;
}

static gboolean
select_area_button_release (GtkWidget               *window,
                            GdkEventButton          *event,
                            select_area_filter_data *data)
{
  if (!data->button_pressed)
    return TRUE;

  data->rect.width  = ABS (data->rect.x - event->x_root);
  data->rect.height = ABS (data->rect.y - event->y_root);
  data->rect.x = MIN (data->rect.x, event->x_root);
  data->rect.y = MIN (data->rect.y, event->y_root);

  if (data->rect.width == 0 || data->rect.height == 0)
    data->aborted = TRUE;

  gtk_main_quit ();
        // stop picking...
    //    OpenGLContext* openglContext = OpenGLContext::Singleton();
    //    openglContext->generalUx->addCurrentToPickHistory();
  pick_mode_enabled = false;

  return TRUE;
}

static gboolean
select_area_key_press (GtkWidget               *window,
                       GdkEventKey             *event,
                       select_area_filter_data *data)
{
  if (event->keyval == GDK_KEY_Escape)
    {
      data->rect.x = 0;
      data->rect.y = 0;
      data->rect.width  = 0;
      data->rect.height = 0;
      data->aborted = TRUE;

      gtk_main_quit ();
      pick_mode_enabled = false;

    }

  return TRUE;
}


void begin_pick_mode_linux(){ // mode_x11_gtk
    if( pick_mode_enabled ) return;

    g_autoptr(GdkCursor) cursor = NULL;
    GdkDisplay *display;
    select_area_filter_data  data;
    GdkSeat *seat;

    data.rect.x = 0;
    data.rect.y = 0;
    data.rect.width  = 0;
    data.rect.height = 0;
    data.button_pressed = FALSE;
    data.aborted = FALSE;
    data.window = create_select_window();

    g_signal_connect (data.window, "key-press-event", G_CALLBACK (select_area_key_press), &data);
    g_signal_connect (data.window, "button-press-event", G_CALLBACK (select_area_button_press), &data);
    g_signal_connect (data.window, "button-release-event", G_CALLBACK (select_area_button_release), &data);
    g_signal_connect (data.window, "motion-notify-event", G_CALLBACK (select_area_motion_notify), &data);

    display = gtk_widget_get_display (data.window);
    cursor = gdk_cursor_new_for_display (display, GDK_CROSSHAIR);
    seat = gdk_display_get_default_seat (display);

    gdk_seat_grab (seat,
                   gtk_widget_get_window (data.window),
                   GDK_SEAT_CAPABILITY_ALL,
                   FALSE,
                   cursor,
                   NULL,
                   NULL,
                   NULL);

    gtk_main ();

    gdk_seat_ungrab (seat);

    gtk_widget_destroy (data.window);

    cb_data->aborted = data.aborted;
    cb_data->rectangle = data.rect;

    pick_mode_enabled = true;
}

#endif // COLORPICK_USE_XCB

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
        begin_pick_mode_linux();

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

void getImagePathFromMainThread(){

}

#endif
