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
#include "portable-file-dialogs.h"

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
typedef HGDIOBJ*(__stdcall* p_color_pick_win_api_screen_to_bitmap)();

typedef void(__stdcall* p_color_pick_win_api_get)(ColorPickWinClr*);

pBegin_Monitor_Mouse_Position Begin_Monitor_Mouse_Position;
pEnd_Monitor_Mouse_Position End_Monitor_Mouse_Position;
//pGet_Mouse_Position Get_Mouse_Position;
p_color_pick_win_api_starturl color_pick_win_api_starturl;
p_color_pick_win_api_toggle_picking color_pick_win_api_toggle_picking;
p_color_pick_win_api_screen_to_bitmap color_pick_win_api_screen_to_bitmap;
p_color_pick_win_api_get color_pick_win_api_get;


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


SDL_Surface* CopyEntireScreenToSurfaceWin()
{
	//HGDIOBJ* hBitmap = p_color_pick_win_api_screen_to_bitmap();

	ColorPickWinClr* colorPickWinClr = nullptr;

	color_pick_win_api_get(colorPickWinClr);

	auto depth = 32;
	auto comppp = 4;

    // TODO: fix me (not tested) chances are does notw ork...
	SDL_Surface* srf = SDL_CreateRGBSurfaceFrom(colorPickWinClr->ColorPickWinClrCopyEntireScreenToBitmapWin(),
		colorPickWinClr->nWidth,
		colorPickWinClr->nHeight,
       depth,
       comppp * colorPickWinClr->nWidth, // pitch isn't stride, its stride /8 (eg width*4, when 4*8=32pbb)
       0x00FF0000,
       0x0000FF00,
       0x000000FF,
       0x00000000
   );

   colorPickWinClr->FreeLastBitmapWin();

   return srf;
}


static void win_TogglePicking(){
    #ifndef DISABLE_WIN32_CODEBLOCKS
   
    #else

		color_pick_win_api_toggle_picking();

	#endif
}


void beginScreenshotSeleced(){
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

	color_pick_win_api_starturl(url);
    return true;
}


void requestReview(){

}

void getImagePathFromMainThread(){

}

//#endif
