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


//http://www.cplusplus.com/forum/general/8090/
//  typedef LONG (__stdcall *pCreateCompression)(LONG *context, LONG flags);
typedef BOOL(__stdcall* pBegin_Monitor_Mouse_Position)(pt_type*);
typedef BOOL(__stdcall* pEnd_Monitor_Mouse_Position)();
//typedef pt_type* (__stdcall* pGet_Mouse_Position)();
typedef bool(__stdcall* p_color_pick_win_api_starturl)(char*);
typedef void(__stdcall* p_color_pick_win_api_toggle_picking)();
typedef void(__stdcall* p_color_pick_win_api_screen_to_bitmap)(void* ret_bitm, int* ret_size, pt_type* info);

//typedef void(__stdcall* p_color_pick_win_api_get)(ColorPickWinClr*); // will not wrok this way

pBegin_Monitor_Mouse_Position Begin_Monitor_Mouse_Position;
pEnd_Monitor_Mouse_Position End_Monitor_Mouse_Position;
//pGet_Mouse_Position Get_Mouse_Position;
p_color_pick_win_api_starturl color_pick_win_api_starturl;
p_color_pick_win_api_toggle_picking color_pick_win_api_toggle_picking;
p_color_pick_win_api_screen_to_bitmap color_pick_win_api_screen_to_bitmap;
//p_color_pick_win_api_get color_pick_win_api_get;

/*

// we may be missing something here...
private: void ColorPick::loadMouseListener(){
				HINSTANCE hDLL = NULL;
				hDLL = LoadLibrary(L"ColorPickMouse.dll");

				if( hDLL == NULL ){
					System::Windows::Forms::MessageBox::Show("Fatal Error: Could not load ColorPickMouse.dll","ColorPick");
					exit(0);
					return;
				}else{
					Begin_Monitor_Mouse_Position = (pBegin_Monitor_Mouse_Position)GetProcAddress(hDLL, "Begin_Monitor_Mouse_Position");
					End_Monitor_Mouse_Position = (pEnd_Monitor_Mouse_Position)GetProcAddress(hDLL, "End_Monitor_Mouse_Position");
					Get_Mouse_Position = (pGet_Mouse_Position)GetProcAddress(hDLL, "Get_Mouse_Position");
					//mpt = Get_Mouse_Position();
					//Begin_Monitor_Mouse_Position(mpt);
				}
		 }

*/

static HINSTANCE hDLL = NULL;

static bool pick_mode_enabled=false;
static bool pick_from_wnd_created=false;

typedef unsigned int CWnd; // suppress errors TEMPORARY!!

static pt_type* mouse_point;

#define DISABLE_WIN32_CODEBLOCKS 1

static HWND pick_from_hwnd=NULL;
static HWND preview_hwnd=NULL;
static HWND pick_zoom_hwnd=NULL;
static CWnd* preview_cwnd=NULL;
static CWnd* pick_from_cwnd=NULL;

static UINT uMyTimerId;

static bool regDllFunctionsIfNotRegistered() {
	if (hDLL == NULL) {

		HINSTANCE hDLL = NULL;
		hDLL = LoadLibrary("colorpick_main_win_clr.dll");
		//hDLL = LoadLibrary("../Platform/WIN/COLORPICK_WIN_CLR/Debug/colorpick_main_win_clr.dll");


		if (hDLL == NULL) {
			SDL_Log("Fatal Error: Could not load colorpick_main_win_clr.dll");
			//System::Windows::Forms::MessageBox::Show("Fatal Error: Could not load colorpick_main_win_clr lib", "ColorPick");
			return false;
		}
		else {
			color_pick_win_api_starturl = (p_color_pick_win_api_starturl)GetProcAddress(hDLL, "color_pick_win_api_starturl");
			color_pick_win_api_toggle_picking = (p_color_pick_win_api_toggle_picking)GetProcAddress(hDLL, "color_pick_win_api_toggle_picking");
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

	auto depth = 32;
	auto comppp = 4;

	//HGDIOBJ ret_bitm;
	Uint8* ret_bitm;
	Uint8* ret_bitm_flipped;

	int size = 0;
	pt_type sizeInfo;


	// one problem, we need to allocate enough memory for our screen object...

	ret_bitm = (Uint8*)SDL_malloc(sizeof(Uint32) * SHMEMSIZE); // over allocated....

	SDL_Log("before call to dll... s: %i width: %i height: %i ", size, sizeInfo.W, sizeInfo.H);


	color_pick_win_api_screen_to_bitmap(ret_bitm, &size, &sizeInfo);

	SDL_Log("Recieved information from dll... s: %i width: %i height: %i ", size, sizeInfo.W, sizeInfo.H);

	SDL_Log("Recieved bitmap info.. %i, %i, %i, %i, %i, %i ", ret_bitm[0], ret_bitm[1], ret_bitm[2], ret_bitm[3], ret_bitm[4], ret_bitm[5]);

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


   //colorPickWinClr->FreeLastBitmapWin();

   return srf;
}


static void win_TogglePicking(){
    #ifndef DISABLE_WIN32_CODEBLOCKS
   
    #else

		//color_pick_win_api_toggle_picking();

	#endif
}


void beginScreenshotSeleced(){
	regDllFunctionsIfNotRegistered();

    if( pick_mode_enabled ){
		win_TogglePicking();
        return;
    }
	
    openglContext->imageWasSelectedCb(CopyEntireScreenToSurfaceWin(), false);

    if( !pick_mode_enabled ){
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
    #ifndef DISABLE_WIN32_CODEBLOCKS
    System::Diagnostics::Process::Start(url);
    #endif

	regDllFunctionsIfNotRegistered();


	color_pick_win_api_starturl(url);
    return true;
}


void requestReview(){

}

void getImagePathFromMainThread(){

}

//#endif
