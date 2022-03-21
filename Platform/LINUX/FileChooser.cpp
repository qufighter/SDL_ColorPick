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
#include "external_static/portable-file-dialogs.h"


#include "SDL.h" // redundant?
#include "stdlib.h" // for system() call

#define COLORPICK_USE_XCB 1
#ifndef COLORPICK_USE_XCB
#include   <X11/Xlib.h>
#else
#include <xcb/xcb.h>
#endif

static bool pick_mode_enabled=false;
static bool gtk_init_complete=false;

// TODO: we need non GTK variant support...
#define COLORPICK_X11_GTK 1
#ifdef COLORPICK_X11_GTK
#include <gtk/gtk.h>

typedef struct {
  GdkRectangle  rect;
  gboolean      button_pressed;
  GtkWidget    *window;

  gboolean      aborted;
} select_area_filter_data;


static void stop_picking_mode(){
    gtk_main_quit ();
    pick_mode_enabled = false;
}

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
                           GdkEventMotion          *g_event,
                           select_area_filter_data *data)
{

//    OpenGLContext* openglContext = OpenGLContext::Singleton();
//    ColorPickState* colorPickState = ColorPickState::Singleton();
//
//    openglContext->position_x =(openglContext->fullPickImgSurface->clip_rect.w - (int)g_event->x_root) - (openglContext->fullPickImgSurface->clip_rect.w / 2);
//    openglContext->position_y =(openglContext->fullPickImgSurface->clip_rect.h - (int)g_event->y_root) - (openglContext->fullPickImgSurface->clip_rect.h / 2);
//
//    // todo: note; theory; all 3 platforms suffer from the fact that this is occuring in another thread...
//    // which means our position_x  position_y updates may or may not be consumed, or may change mid move...
//    // as a better approach lets 1) push an event 2) let those be consumed by the main thread
//
//    colorPickState->movedxory = true;
//    openglContext->renderShouldUpdate = true; // do not call renderScene from timer thread!


    SDL_Event event;
    SDL_UserEvent userevent;
    SDL_Point* mmevent = new SDL_Point(); // note: we deallocate this on main thread...

    mmevent->x = (openglContext->fullPickImgSurface->clip_rect.w - (int)g_event->x_root) - (openglContext->fullPickImgSurface->clip_rect.w / 2);
    mmevent->y = (openglContext->fullPickImgSurface->clip_rect.h - (int)g_event->y_root) - (openglContext->fullPickImgSurface->clip_rect.h / 2);

    userevent.type = SDL_USEREVENT;
    userevent.code = USER_EVENT_ENUM::PICK_AT_POSITION;
    userevent.data1 = mmevent;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    //SDL_Log("mm event pos type %i", USER_EVENT_ENUM::PICK_AT_POSITION);
    SDL_PushEvent(&event);


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

    stop_picking_mode();

        // stop picking...
        OpenGLContext* openglContext = OpenGLContext::Singleton();
        openglContext->generalUx->addCurrentToPickHistory();

  return TRUE;
}

static Uint32 pick_again_soon(Uint32 interval, void* parm) {
    // to make just this timer, fire on main thread - push event!
    SDL_Event event;
    SDL_UserEvent userevent;
    userevent.type = SDL_USEREVENT;
    userevent.code = USER_EVENT_ENUM::PICK_AGAIN_NOW;
    event.type = SDL_USEREVENT;
    event.user = userevent;
    SDL_PushEvent(&event);
    return 0;
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

        stop_picking_mode();

    }
  if (event->keyval == GDK_KEY_r || event->keyval == GDK_KEY_j){
  	// TODO: a better method might be, drop out of pick mode, to give some time, then screenshot
  	// TODO: also note, when we ener pick mode OR here, we loose some position information...
  	//beginScreenshotSeleced();

      stop_picking_mode();
      SDL_AddTimer(250, pick_again_soon, nullptr);
  }
  return TRUE;
}


static gboolean
select_window_draw (GtkWidget *window, cairo_t *cr, gpointer unused)
{
  return TRUE; // why draw at all?? ?curious... guess we don't show a selection rect, so why bother?
  /*
  GtkStyleContext *style;

  style = gtk_widget_get_style_context (window);

  if (gtk_widget_get_app_paintable (window))
    {
      cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
      cairo_set_source_rgba (cr, 255, 0, 0, 0);
      cairo_paint (cr);

      gtk_style_context_save (style);
      gtk_style_context_add_class (style, GTK_STYLE_CLASS_RUBBERBAND);

      gtk_render_background (style, cr,
                             0, 0,
                             gtk_widget_get_allocated_width (window),
                             gtk_widget_get_allocated_height (window));
      gtk_render_frame (style, cr,
                        0, 0,
                        gtk_widget_get_allocated_width (window),
                        gtk_widget_get_allocated_height (window));

      gtk_style_context_restore (style);
    }
*/
  return TRUE;
}

static GtkWidget *
create_select_window (void)
{
  GtkWidget *window;
  GdkScreen *screen;
  GdkVisual *visual;

  screen = gdk_screen_get_default ();
  visual = gdk_screen_get_rgba_visual (screen);

  window = gtk_window_new (GTK_WINDOW_POPUP);
  if (gdk_screen_is_composited (screen) && visual)
    {
      gtk_widget_set_visual (window, visual);
      gtk_widget_set_app_paintable (window, TRUE);
    }

  g_signal_connect (window, "draw", G_CALLBACK (select_window_draw), NULL);

  gtk_window_move (GTK_WINDOW (window), -100, -100);
  gtk_window_resize (GTK_WINDOW (window), 10, 10);
  gtk_widget_show (window);

  return window;
}

                    
gboolean begin_pick_mode_linux(gpointer user_data){ // mode_x11_gtk'[
	SDL_Log("begin pick mode actally called");
    if( pick_mode_enabled ) return FALSE;
   if( !gtk_init_complete ){gtk_init(NULL, NULL);gtk_init_complete=true;}
   	
   	
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
                   
    pick_mode_enabled = true;
    
    gtk_main (); // locks everythign up!

    gdk_seat_ungrab (seat);

    gtk_widget_destroy (data.window);

    return TRUE;
}


static int thread_begin_pck_mode_linux(void* data){
	begin_pick_mode_linux(NULL);
	return 0;
}           

#endif // COLORPICK_X11_GTK

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

        //SDL_Log("screen image dimensions: %i %i", width, height);

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

        //SDL_Log("srf bpp %i", srf->format->BitsPerPixel);

        // NOTE while the above would be detected later as SDL_PIXELFORMAT_RGB888 srf->format->BitsPerPixel is (correctly for below?) 32...
        // code to change the format should *probably* be common however picking the correct format for the platform would be a good plan.... see also isProbablyAndroid
        SDL_Surface *surface2 = SDL_CreateRGBSurfaceWithFormat(0, srf->w, srf->h, srf->format->BitsPerPixel, SDL_PIXELFORMAT_ABGR8888);
        int didBlit = SDL_BlitSurface(srf, NULL /*src rect entire surface*/, surface2, &srf->clip_rect);
        if( didBlit != 0 ){
            SDL_Log("Blit problem");
            SDL_Log("%s", SDL_GetError());
        }
        SDL_FreeSurface(srf);

		// by passing 0,0 we will ensure that we get the right snap... trust me...  without it, there are some issues retrunign to pick mode, or even panning quick and ending up in the wrong place
		// there are some alternate soltuions, to try to get the CORRECT mouse position (eg maybe we could pass in screen coord of the click that triggered this function call...)

    SDL_Point gm_result = {0,0};
    SDL_GetGlobalMouseState(&gm_result.x, &gm_result.y);
    openglContext->imageWasSelectedCb(surface2, false, gm_result.x, gm_result.y);
        xcb_disconnect(dsp);
    #endif

    // TODO: TEST MULTIPLE MONITORS!!! (likely broken... event x/y is screen or desktop?)
    if( !pick_mode_enabled ){
	SDL_Thread* thread = SDL_CreateThread(thread_begin_pck_mode_linux, "thread_begin_pck_mode_linux", nullptr);
	SDL_DetachThread(thread);
    }

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
