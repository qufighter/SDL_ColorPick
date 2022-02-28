//
//
//  ColorPick SDL
//
//
//

// YOU MAY HAVE TO DELETE THIS FILE FROM OSX PROJECT - IT WILL CONFUSE XCODE
#include <windows.h>


//#if defined(__WINDOWS__)
// using namespace System;
// using namespace System::ComponentModel;
// using namespace System::Collections;
// using namespace System::Windows::Forms;
// using namespace System::Drawing;
// using namespace System::Security::Cryptography;
// using namespace System::Net;
// using namespace System::Threading;

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

static bool pick_mode_enabled=false;
static bool pick_from_wnd_created=false;

typedef unsigned int CWnd; // suppress errors TEMPORARY!!
#define DISABLE_WIN32_CODEBLOCKS 1

static HWND pick_from_hwnd=NULL;
static HWND preview_hwnd=NULL;
static HWND pick_zoom_hwnd=NULL;
static CWnd* preview_cwnd=NULL;
static CWnd* pick_from_cwnd=NULL;

static UINT uMyTimerId;


SDL_Surface* CopyEntireScreenToSurfaceWin()
{
    HDC         hScrDC, hMemDC;         // screen DC and memory DC
    int         nX, nY, nX2, nY2;       // coordinates of rectangle to grab
    int         nWidth, nHeight;        // DIB width and height
    int         xScrn, yScrn;           // screen resolution

    HGDIOBJ     hOldBitmap , hBitmap;
    LONG num_monitors=((LONG)::GetSystemMetrics(SM_CMONITORS));
    LONG same_fmt=((LONG)::GetSystemMetrics(SM_SAMEDISPLAYFORMAT));

    LONG virtual_start_x=((LONG)::GetSystemMetrics(SM_XVIRTUALSCREEN));
    LONG virtual_start_y=((LONG)::GetSystemMetrics(SM_YVIRTUALSCREEN));
    LONG virtual_end_x=((LONG)::GetSystemMetrics(SM_CXVIRTUALSCREEN));
    LONG virtual_end_y=((LONG)::GetSystemMetrics(SM_CYVIRTUALSCREEN));

    //total width across all monitors (size of bitmap)
    int scx = abs(virtual_start_x-virtual_end_x);
    int scy = abs(virtual_start_y-virtual_end_y);
    //s->cx = (LONG)::GetSystemMetrics( SM_CXFULLSCREEN );//SM_CXSCREEN
    //s->cy = (LONG)::GetSystemMetrics( SM_CYSCREEN );

    //http://stackoverflow.com/questions/576476/get-devicecontext-of-entire-screen-with-multiple-montiors
   //hScrDC = ::GetDC(0);//CreateDC("DISPLAY", NULL, NULL, NULL);
   hScrDC = ::GetDC(0);//CreateDC(TEXT("DISPLAY"),NULL,NULL,NULL);
   hMemDC = CreateCompatibleDC(hScrDC);      // get points of rectangle to grab


   nX = virtual_start_x;//0;//lpRect->left;
   nY = virtual_start_y;//lpRect->top;

   xScrn = GetDeviceCaps(hScrDC, HORZRES);
   yScrn = GetDeviceCaps(hScrDC, VERTRES);

   nWidth = scx;//nX2 - nX;
   nHeight = scy;//nY2 - nY;

   // create a bitmap compatible with the screen DC
   hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);

   // select new bitmap into memory DC
   hOldBitmap =   SelectObject (hMemDC, hBitmap);

   // bitblt screen DC to memory DC
   BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);

   // select old bitmap back into memory DC and get handle to
   // bitmap of the screen

   hBitmap = SelectObject(hMemDC, hOldBitmap);

   auto depth = 32;
   auto comppp = 4;

    // TODO: fix me (not tested) chances are does notw ork...
   SDL_Surface* srf = SDL_CreateRGBSurfaceFrom(hBitmap,
       nWidth,
       nHeight,
       depth,
       comppp * nWidth, // pitch isn't stride, its stride /8 (eg width*4, when 4*8=32pbb)
       0x00FF0000,
       0x0000FF00,
       0x000000FF,
       0x00000000
   );

   // clean up
    DeleteObject(hMemDC);
    DeleteObject(hBitmap);
   ::ReleaseDC(0,hScrDC);
   return srf;
}

//this is run by the dialog color pick preview class cpick_prev_class
//defines message loop for the pickable area that shows the crosshair cursor
LRESULT CALLBACK CPick_Preview_WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam )
{
    OpenGLContext* openglContext;
    
    switch( message )
    {
    case WM_CREATE:
        //Beep( 50, 10 );
        break;
    //case WM_CHAR:
    //    switch( wparam )
    //    {
    //    case 'G':   // make ghostly
    //    case 'g':
    //        // maintain old style, turn on WS_EX_LAYERED bits on.
    //        SetWindowLongPtr(   hwnd,
    //            GWL_EXSTYLE,
    //            GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);

    //        SetLayeredWindowAttributes( hwnd,
    //            0,
    //            85,  /* "amount of solidness" 0=transparent, 255=solid*/
    //            LWA_ALPHA);
    //        // Everytime you make a window ghostly,
    //        // you MUST call SetLayeredWindowAttributes(),
    //        // otherwise it won't work properly.
    //        break;

    //    case 'S':   // make solid
    //    case 's':
    //        // Remove WS_EX_LAYERED from this window's style
    //        SetWindowLongPtr(   hwnd,
    //            GWL_EXSTYLE,    // set the EX_STYLE of this window
    //            GetWindowLong(hwnd, GWL_EXSTYLE) &  // GET old style first
    //            ~WS_EX_LAYERED);  // turn WS_EX_LAYERED bits off

    //        // Note:  Use SetWindowLongPtr (NOT SetWindowLong()!)
    //        // to write code that'll work
    //        // on both 32-bit and 64-bit windows!
    //        // http://msdn2.microsoft.com/en-us/library/ms644898(VS.85).aspx
    //    }
    //    return 0;
    //    break;
    case WM_PAINT:
        {
            HDC hdc;
            PAINTSTRUCT ps;
            hdc = BeginPaint( hwnd, &ps );

            EndPaint( hwnd, &ps );
        }
        break;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
            // no instance?? we may need to use singleton to reach into here... possibly simply calling beginScreenshotSeleced() again would disable...
//        if(pick_mode_enabled) winTogglePicking();
             openglContext = OpenGLContext::Singleton();
             openglContext->choosePickFromScreen(); //?
//ShowWindow(pick_from_hwnd,SW_HIDE);//if we click on it, well we shouldn't click on it!  that is all - the window should never have focus

        break;
    case WM_KEYDOWN:

        switch( wparam )
        {
        case VK_ESCAPE:
            PostQuitMessage( 0 );
            break;
        default:
            break;
        }
        break;
    case WM_DESTROY:
        //PostQuitMessage( 0 ) ;
        break;
    }
    return DefWindowProc( hwnd, message, wparam, lparam );
}


static void initWinDesktopScreenshotPreviewWindow(){
    if(pick_from_wnd_created)return;
    WNDCLASSEX wcx;
        wcx.cbClsExtra = 0;
        wcx.cbSize = sizeof( WNDCLASSEX );  // 1.  NEW!  must know its own size.
        wcx.cbWndExtra = 0;
        wcx.hbrBackground = (HBRUSH)GetStockObject( LTGRAY_BRUSH ); //HOLLOW_BRUSH option prevent screen flash
        wcx.hCursor = NULL; // LoadCursor( NULL, MAKEINTRESOURCE(IDC_CROSS) ); // TODO: fix the cursor here !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        wcx.hIcon = NULL;//LoadIcon( NULL, IDI_APPLICATION );
        wcx.hIconSm = NULL;                 // 2.  NEW!!  Can specify small icon.
        wcx.hInstance = NULL;//GetModuleHandle(0);//hInstance;
        wcx.lpfnWndProc = CPick_Preview_WndProc;
        wcx.lpszClassName = TEXT("cpick_prev_class");
        wcx.lpszMenuName = 0;
        wcx.style = CS_HREDRAW | CS_VREDRAW;
    RegisterClassEx( &wcx );

    LONG virtual_start_x=((LONG)::GetSystemMetrics(SM_XVIRTUALSCREEN));
    LONG virtual_start_y=((LONG)::GetSystemMetrics(SM_YVIRTUALSCREEN));
    LONG virtual_end_x=((LONG)::GetSystemMetrics(SM_CXVIRTUALSCREEN));
    LONG virtual_end_y=((LONG)::GetSystemMetrics(SM_CYVIRTUALSCREEN));

    int scx = abs(virtual_start_x-virtual_end_x);
    int scy = abs(virtual_start_y-virtual_end_y);

    //static Gdiplus::Rect* screenRect;
    //screenRect=new Gdiplus::Rect(virtual_start_x,virtual_start_y,scx,scy);

    //Window Styles http://msdn.microsoft.com/en-US/library/ms632600(v=vs.80).aspx
    //Window Styles Extended http://msdn.microsoft.com/en-us/library/ff700543(v=vs.85).aspx
    //WC_DIALOG - WC_STATIC - TEXT("cpick_prev_class")
    pick_from_hwnd = CreateWindowEx(
            WS_EX_PALETTEWINDOW /*WS_EX_TOPMOST*/,
            TEXT("cpick_prev_class"),
            "colorpick_preview_window",
            WS_POPUP /*WS_POPUP*/,
            virtual_start_x,virtual_start_y,scx,scy,
            NULL,NULL,GetModuleHandle(0),NULL
        );
    #ifndef DISABLE_WIN32_CODEBLOCKS
    pick_from_cwnd = CWnd::FromHandle(pick_from_hwnd);
    #endif
    pick_from_wnd_created=true;
}

static void winTogglePicking(){
    #ifndef DISABLE_WIN32_CODEBLOCKS
    if(pick_mode_enabled){

        preview_cwnd->SetActiveWindow();

        //if(pickUpdateThread && pickUpdateThread->IsAlive )pickUpdateThread->Abort();
        pick_mode_enabled=false;
        //threadLoopCount = 0;
        KillTimer(pick_from_hwnd, uMyTimerId);

        //End_Monitor_Mouse_Position();
        mpt->m1=false;//intercepted event

        if(pick_from_wnd_created) ShowWindow(pick_from_hwnd,SW_HIDE);

        //ShowWindow(preview_hwnd,SW_RESTORE);
        //preview_cwnd->SetActiveWindow();
        //preview_cwnd->ActivateTopParent();


        //Sleep(100);
        //this->TopMost=false;
        //sendPickedColorToHistory();

    }else{
        //this->TopMost=true;
        //sendPickedColorToHistory();

        //Begin_Monitor_Mouse_Position(mpt);
        //mpt = Get_Mouse_Position();/*!*/
        pick_mode_enabled=true;

        initWinDesktopScreenshotPreviewWindow();


        //if(usingThreadedPicking){
        //    pickUpdateThread = gcnew Thread(gcnew ThreadStart(this, &ColorPick::threadUpdatePickPreview));
        //    pickUpdateThread->Start();
        //}else{
            //threadLoopCount = 0;
            uMyTimerId = SetTimer(pick_from_hwnd, NULL, PICK_UPDATE_DELAY_MS * 2, NULL);
        //}


            ShowWindow(pick_from_hwnd,SW_HIDE);

            // we'd snap here...??? if we have not already...

            ShowWindow(pick_from_hwnd,SW_SHOW);
            // next up? pick_mode_enabled

            // lets not draw to the "shield" window??? if possible??
//            Gdiplus::Graphics* bmp_screen = new Gdiplus::Graphics(GetDC(pick_from_hwnd));
//            Sleep(5);
//            bmp_screen->DrawImage(b,0,0);


//        ShowWindow(preview_hwnd,SW_RESTORE);
//        preview_cwnd->SetActiveWindow();
//        preview_cwnd->ActivateTopParent();
    }
    #endif
}


void beginScreenshotSeleced(){

    if( pick_mode_enabled ){
        winTogglePicking();
        return;
    }

    openglContext->imageWasSelectedCb(CopyEntireScreenToSurfaceWin(), false);

    if( !pick_mode_enabled ){
        winTogglePicking();
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
    #ifndef DISABLE_WIN32_CODEBLOCKS
    System::Diagnostics::Process::Start(url);
    #endif
    return true;
}


void requestReview(){

}

void getImagePathFromMainThread(){

}

//#endif
