//
//
//  ColorPick SDL
//
//
//

// YOU MAY HAVE TO DELETE THIS FILE FROM OSX PROJECT - IT WILL CONFUSE XCODE
#include <windows.h>


//#if defined(__WINDOWS__)

#define PICK_UPDATE_DELAY_MS 16 //render delay in picking thread

#include <math.h>

#include <algorithm>
int op_increase (int i) { return i*1.25; }

//#define _AFXDLL
//#include <AFXWIN.H>



#include <process.h>


#include <wtypes.h>
#include <objidl.h>
#include <stdio.h>
#include <stdlib.h>
// evaluate - many above includes are NOT NEEDED AT ALL TODO TODO TODO

#include "WinFileChooser.h" //doens't work right... we have to add our includes below...


#include "../../ColorPick.h"
#include "external_static/portable-file-dialogs.h"

#include "SDL.h" // redundant?
//#include "stdlib.h" // for system() call

// read form ColorPickWinClr (dll)


// connect dll

typedef BOOL(__stdcall* pBegin_Monitor_Mouse_Position)(pt_type*);
typedef BOOL(__stdcall* pEnd_Monitor_Mouse_Position)();
//typedef pt_type* (__stdcall* pGet_Mouse_Position)();
typedef bool(__stdcall* p_color_pick_win_api_getstatus)(pt_type* info);
typedef bool(__stdcall* p_color_pick_win_api_starturl)(char*);
typedef bool(__stdcall* p_color_pick_win_api_toggle_picking)();
typedef void(__stdcall* p_color_pick_win_api_get_screen_size)(pt_type* info);
typedef void(__stdcall* p_color_pick_win_api_screen_to_bitmap)(void* ret_bitm, int* ret_size, pt_type* info);


pBegin_Monitor_Mouse_Position Begin_Monitor_Mouse_Position;
pEnd_Monitor_Mouse_Position End_Monitor_Mouse_Position;
//pGet_Mouse_Position Get_Mouse_Position;
p_color_pick_win_api_getstatus color_pick_win_api_getstatus;
p_color_pick_win_api_starturl color_pick_win_api_starturl;
p_color_pick_win_api_toggle_picking color_pick_win_api_toggle_picking;
p_color_pick_win_api_get_screen_size color_pick_win_api_get_screen_size;
p_color_pick_win_api_screen_to_bitmap color_pick_win_api_screen_to_bitmap;

//see also, regDllFunctionsIfNotRegistered()



static HINSTANCE hDLL = NULL;

//typedef unsigned int CWnd; // suppress errors TEMPORARY!!

static pt_type* m_clr_status = new pt_type(); // not allocated...

//static HWND pick_from_hwnd=NULL;
//static HWND preview_hwnd=NULL;
//static HWND pick_zoom_hwnd=NULL;
//static CWnd* preview_cwnd=NULL;
//static CWnd* pick_from_cwnd=NULL;

//static UINT uMyTimerId;

static bool regDllFunctionsIfNotRegistered() {
	if (hDLL == NULL) {

		hDLL = LoadLibrary("colorpick_main_win_clr.dll");
		//hDLL = LoadLibrary("../Platform/WIN/COLORPICK_WIN_CLR/Debug/colorpick_main_win_clr.dll");

		if (hDLL == NULL) {
			SDL_Log("Fatal Error: Could not load colorpick_main_win_clr.dll");
			//System::Windows::Forms::MessageBox::Show("Fatal Error: Could not load colorpick_main_win_clr lib", "ColorPick");
			return false;
		}
		else {
			color_pick_win_api_getstatus = (p_color_pick_win_api_getstatus)GetProcAddress(hDLL, "color_pick_win_api_getstatus");
			color_pick_win_api_starturl = (p_color_pick_win_api_starturl)GetProcAddress(hDLL, "color_pick_win_api_starturl");
			color_pick_win_api_toggle_picking = (p_color_pick_win_api_toggle_picking)GetProcAddress(hDLL, "color_pick_win_api_toggle_picking");
			color_pick_win_api_get_screen_size = (p_color_pick_win_api_get_screen_size)GetProcAddress(hDLL, "color_pick_win_api_get_screen_size");
			color_pick_win_api_screen_to_bitmap = (p_color_pick_win_api_screen_to_bitmap)GetProcAddress(hDLL, "color_pick_win_api_screen_to_bitmap");
			return true;
		}

	}
}

SDL_Surface* CopyEntireScreenToSurfaceWin()
{
	//HGDIOBJ* hBitmap = p_color_pick_win_api_screen_to_bitmap();

	//ColorPickWinClr* colorPickWinClr = nullptr;

	//color_pick_win_api_get(colorPickWinClr);

	pt_type sizeInfo; // juse reuse m_clr_status ??

	// first see what size bmp we need to allocate...

	color_pick_win_api_get_screen_size(&sizeInfo);

	UINT32 pixelFootprint = 0;

	pixelFootprint = sizeInfo.W * sizeInfo.H;

	auto depth = 32;
	auto comppp = 4;

	//HGDIOBJ ret_bitm;
	Uint8* ret_bitm;
	Uint8* ret_bitm_flipped;

	int size = 0;


	// one problem, we need to allocate enough memory for our screen object...

	ret_bitm = (Uint8*)SDL_malloc(sizeof(Uint32) * pixelFootprint); // over allocated?? or actualy now it should always be allocated exactly correct...

	SDL_Log("before call to dll... s: %i width: %i height: %i ", size, sizeInfo.W, sizeInfo.H);

	color_pick_win_api_screen_to_bitmap(ret_bitm, &size, &sizeInfo);

	SDL_Log("Recieved information from dll... s: %i width: %i height: %i ", size, sizeInfo.W, sizeInfo.H);

	//SDL_Log("Recieved bitmap info.. %i, %i, %i, %i, %i, %i ", ret_bitm[0], ret_bitm[1], ret_bitm[2], ret_bitm[3], ret_bitm[4], ret_bitm[5]);

	//SDL_memcpy(ret_bitm_flipped, ret_bitm, sizeof(Uint32) * SHMEMSIZE);

	// allocated properly (for desktop image recieved...)
	ret_bitm_flipped = (Uint8*)SDL_malloc(sizeof(Uint32) * size);

	int row_len = sizeof(Uint32) * sizeInfo.W;
	int rr = 0;
	for (int r = 0; r < sizeInfo.H; r++) {
		rr = sizeInfo.H - r - 1;
		SDL_memcpy(&ret_bitm_flipped[(rr * row_len)], &ret_bitm[(r * row_len)], row_len);
	}

    // TODO: fix me (not tested) chances are does notw ork...
	SDL_Surface* srf = SDL_CreateRGBSurfaceFrom(ret_bitm_flipped,
		sizeInfo.W,
		sizeInfo.H,
       depth,
       comppp * sizeInfo.W, // pitch isn't stride, its stride /8 (eg width*4, when 4*8=32pbb)
       0x00FF0000,
       0x0000FF00,
       0x000000FF,
       0x00000000
   );

	SDL_free(ret_bitm);
	//SDL_free(ret_bitm_flipped); // whatever we give surface, cannot free it!

   return srf;
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

static Uint32 check_active_picking_activities(Uint32 interval, void* parm) {

	color_pick_win_api_getstatus(m_clr_status);
	OpenGLContext* openglContext = OpenGLContext::Singleton();

	if (m_clr_status->picking_active) {

		if (m_clr_status->refresh_requested) {
			beginScreenshotSeleced(); // first call: toggles picking OFF!
			SDL_AddTimer(250, pick_again_soon, nullptr);
			//beginScreenshotSeleced(); // if we tried it again now, from this thread, everything gets really broken!
			return 0;
		}

		ColorPickState* colorPickState = ColorPickState::Singleton();

		// the position is pretty wierd sometimes (bg in correct position, fg not... I think I know WHY possibly???) we have to move a certian amount to fix the panning....
		openglContext->position_x = (openglContext->fullPickImgSurface->clip_rect.w - (int)m_clr_status->X) - (openglContext->fullPickImgSurface->clip_rect.w / 2);
		openglContext->position_y = (openglContext->fullPickImgSurface->clip_rect.h - (int)m_clr_status->Y) - (openglContext->fullPickImgSurface->clip_rect.h / 2);

		colorPickState->movedxory = true;
		openglContext->renderShouldUpdate = true; // do not call renderScene from timer thread!


	} else {

		// if we ended, depending on how (cancel vs click) we should push the color to history!s
		if (!m_clr_status->picking_canceled) {
			OpenGLContext* openglContext = OpenGLContext::Singleton();
			openglContext->generalUx->addCurrentToPickHistory();
		}
		SDL_RaiseWindow(openglContext->getSdlWindow()); // timeout??? this seems to only work when using escape (eg picking_canceled)

		return 0; // end timer
	}

	return interval;
}


static void win_TogglePicking(){

	if (color_pick_win_api_toggle_picking()) { // invalidates m_clr_status...
		// picking was activated!  monitor it...
		SDL_AddTimer(33, check_active_picking_activities, nullptr);
	}
}


void beginScreenshotSeleced(){
	regDllFunctionsIfNotRegistered();

	color_pick_win_api_getstatus(m_clr_status);

    if(m_clr_status->picking_active){
		win_TogglePicking();
        return;
    }

	// by passing 0,0 we will ensure that we get the right snap... trust me...  without it, there are some issues retrunign to pick mode, or even panning quick and ending up in the wrong place
	// there are some alternate soltuions, to try to get the CORRECT mouse position (eg maybe we could pass in screen coord of the click that triggered this function call...)
	openglContext->imageWasSelectedCb(CopyEntireScreenToSurfaceWin(), false, 0, 0);

    if( !m_clr_status->picking_active){
		win_TogglePicking();
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


bool openURL(char* url)
{
	regDllFunctionsIfNotRegistered();
	color_pick_win_api_starturl(url);
    return true;
}


void requestReview(){

}

void getImagePathFromMainThread(){

}

//#endif
