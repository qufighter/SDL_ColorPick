//
//
//  ColorPick SDL
//
//
//

// YOU MAY HAVE TO DELETE THIS FILE FROM OSX PROJECT - IT WILL CONFUSE XCODE
#include <ColorPickWinClr.h>


//#if defined(__WINDOWS__)
//using namespace System; FIX THIS!
//using namespace System::ComponentModel;
//using namespace System::Collections;
//using namespace System::Windows::Forms;
//using namespace System::Drawing;
//using namespace System::Security::Cryptography;
//using namespace System::Net;
//using namespace System::Threading;

#define PICK_UPDATE_DELAY_MS 16 //render delay in picking thread



#include <math.h>
#include <algorithm>
//int op_increase (int i) { return i*1.25; }

#define _AFXDLL 1
#include <AFXWIN.H>



#include <process.h>
//
//
#include <wtypes.h>
#include <objidl.h>
#include <stdio.h>
#include <stdlib.h>
// evaluate - many above includes are NOT NEEDED AT ALL TODO TODO TODO


static bool pick_mode_enabled=false;
static bool pick_from_wnd_created=false;

//#define DISABLE_WIN32_CODEBLOCKS 1

static HWND pick_from_hwnd=NULL;
static HWND preview_hwnd=NULL;
static HWND pick_zoom_hwnd=NULL;
static CWnd* preview_cwnd=NULL;
static CWnd* pick_from_cwnd=NULL;

static UINT uMyTimerId;

static pt_type* m_data;// = new pt_type(); //variable is defined elsewhere and passed into dll

#define SHMEMSIZE 4096

static LPVOID lpvMem = NULL;      // pointer to shared memory
static HANDLE hMapObject = NULL;  // handle to file mapping

void ColorPickWinClr::ColorPickWinClrCopyEntireScreenToBitmapWin(void* ret_bitm, int* ret_size, pt_type* info)
{
	//TODO: ghastly but maybe we cAN pass something back?  so far no luck, see CopyEntireScreenToSurfaceWin
	// there must be a way to 1) load the dll into shared (non protected) memory (probably preferred way?
	//						  2) pass a (fixed length?) string of some tpye? (nasty solution...) but we know struct of fixed len works?! 

    //HDC         hScrDC, hMemDC;         // screen DC and memory DC
    int         nX, nY, nX2, nY2;       // coordinates of rectangle to grab
    int         xScrn, yScrn;           // screen resolution

	HGDIOBJ     hOldBitmap;//, hBitmap;
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
   hScrDC = (HDCtype)::GetDC(0);//CreateDC(TEXT("DISPLAY"),NULL,NULL,NULL);
   hMemDC = (HDCtype)CreateCompatibleDC((HDC)hScrDC);      // get points of rectangle to grab


   nX = virtual_start_x;//0;//lpRect->left;
   nY = virtual_start_y;//lpRect->top;

   xScrn = GetDeviceCaps((HDC)hScrDC, HORZRES);
   yScrn = GetDeviceCaps((HDC)hScrDC, VERTRES);

   nWidth = scx;//nX2 - nX;
   nHeight = scy;//nY2 - nY;

   // create a bitmap compatible with the screen DC
   hBitmap = CreateCompatibleBitmap((HDC)hScrDC, nWidth, nHeight);

   // select new bitmap into memory DC
   hOldBitmap =   SelectObject ((HDC)hMemDC, hBitmap);

   // bitblt screen DC to memory DC
   BitBlt((HDC)hMemDC, 0, 0, nWidth, nHeight, (HDC)hScrDC, nX, nY, SRCCOPY);

   // select old bitmap back into memory DC and get handle to
   // bitmap of the screen

   hBitmap = SelectObject((HDC)hMemDC, hOldBitmap);

    // TODO: fix me (not tested) chances are does notw ork...
   /*
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
   */

   // clean up
 //   DeleteObject(hMemDC); // TODO
 //   DeleteObject(hBitmap); // TODO
 //  ::ReleaseDC(0,hScrDC); // TODO

   // some bookkeeeping here... we need to cpy the bitmap to shared memory
   // then, arguablly, we coudl free it right now...


   int copySize = nWidth * nHeight;
   if (copySize > SHMEMSIZE) {
	   copySize = SHMEMSIZE;
   }

   memcpy(lpvMem, hBitmap, copySize);

   ret_bitm = &lpvMem;
   *ret_size = nWidth * nHeight;

   info->W = nWidth;
   info->H = nHeight;

   FreeLastBitmapWin();

//return &hBitmap;
 //  return srf;
}

void ColorPickWinClr::FreeLastBitmapWin() {
    DeleteObject((HDC)hMemDC); // TODO
   DeleteObject(hBitmap); // TODO
   ::ReleaseDC(0,(HDC)hScrDC); // TODO
}

//this is run by the dialog color pick preview class cpick_prev_class
//defines message loop for the pickable area that shows the crosshair cursor
LRESULT CALLBACK CPick_Preview_WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam )
{
  //  OpenGLContext* openglContext;
    
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
       //      openglContext = OpenGLContext::Singleton();
       //      openglContext->choosePickFromScreen(); //?
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

bool ColorPickWinClr::ms_bInstanceCreated = false;
ColorPickWinClr* ColorPickWinClr::pInstance = NULL;

ColorPickWinClr* ColorPickWinClr::Singleton() {
	if (!ms_bInstanceCreated) {
		pInstance = new ColorPickWinClr();
		ms_bInstanceCreated = true;
	}
	return pInstance;
}

void ColorPickWinClr::winTogglePicking(){
    if(pick_mode_enabled){

        preview_cwnd->SetActiveWindow();

        //if(pickUpdateThread && pickUpdateThread->IsAlive )pickUpdateThread->Abort();
        pick_mode_enabled=false;
        //threadLoopCount = 0;
        KillTimer(pick_from_hwnd, uMyTimerId);

        //End_Monitor_Mouse_Position();
        //mpt->m1=false;//intercepted event

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
}



bool ColorPickWinClr::openURL(char* &url)
{
    #ifndef DISABLE_WIN32_CODEBLOCKS
	System::String^ temp = gcnew System::String(url);

	
    System::Diagnostics::Process::Start(temp);
    #endif
    return true;
}


//#endif



//Calling this function from VB simply ensures winMain gets called properly
extern "C" __declspec(dllexport) int  Begin_Monitor_Mouse_Position(pt_type* mpt) {
	m_data = mpt;
	//m_data->X=-1,
	//m_data->Y=-1,
	m_data->m1 = false;

	//if (!watchingMousePosition) {
	//	if (colorPickExistsAndRunning()) {
	//		//LogStr( "MOUSE.DLL starting to monitor mouse position" );
	//		attachMouseHook(myInstance);
	//	}
	//}
	//MessageBox(NULL, "hello", "bonjour(s)", MB_OK);
	return 0;
}

//Calling this function from VB simply ensures winMain gets called properly
extern "C" __declspec(dllexport) int  End_Monitor_Mouse_Position(void) {
	//detachMouseHook();

	//MessageBox(NULL, "goodbye", "bonjour(s)", MB_OK);
	return 0;
}

//Calling this function from VB simply ensures winMain gets called properly
extern "C" __declspec(dllexport) pt_type* Get_Mouse_Position(void) {
	//m_data->X = mx;
	//m_data->Y = my;
	//m_data->m1 = m1;
	//m_data->m2 = m2;
	//m1=false,m2=false;//click has been intercepted
	return m_data;

}
extern "C" __declspec(dllexport) void color_pick_win_api_toggle_picking() {
	colorPickWinClr->winTogglePicking();
}

//extern "C" __declspec(dllexport) HGDIOBJ* color_pick_win_api_screen_to_bitmap() {
//	return colorPickWinClr->ColorPickWinClrCopyEntireScreenToBitmapWin();
//}

extern "C" __declspec(dllexport) void color_pick_win_api_screen_to_bitmap(void* bitm, int* &size, pt_type* &info) {
	return colorPickWinClr->ColorPickWinClrCopyEntireScreenToBitmapWin(bitm, size, info);
}

extern "C" __declspec(dllexport) void color_pick_win_api_get(ColorPickWinClr* t) {
	t = ColorPickWinClr::Singleton();
}

extern "C" __declspec(dllexport) bool color_pick_win_api_starturl(char* url) {
	System::String^ str = gcnew System::String(url);
	System::Diagnostics::Process::Start(str);
	return true;
}



//main entry point for the application, when VB makes its first function call in this DLL
BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD  fdwReason, LPVOID lpReserved)
{
	BOOL fInit, fIgnore;


	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		//myInstance = hModule;

		// Create a named file mapping object
		hMapObject = CreateFileMapping(
			INVALID_HANDLE_VALUE,   // use paging file
			NULL,                   // default security attributes you may choose to use a SECURITY_ATTRIBUTES structure to provide additional security.
			PAGE_READWRITE,         // read/write access
			0,                      // size: high 32-bits
			SHMEMSIZE,              // size: low 32-bits
			TEXT("colorpickfilemap")); // name of map object
		if (hMapObject == NULL)
			return FALSE;

		// The first process to attach initializes memory

		fInit = (GetLastError() != ERROR_ALREADY_EXISTS);

		// Get a pointer to the file-mapped shared memory

		lpvMem = MapViewOfFile(
			hMapObject,     // object to map view of
			FILE_MAP_WRITE, // read/write access
			0,              // high offset:  map from
			0,              // low offset:   beginning
			0);             // default: map entire file
		if (lpvMem == NULL)
			return FALSE;

		// Initialize memory if this is the first process

		if (fInit)
			memset(lpvMem, '\0', SHMEMSIZE);


	};

	if (fdwReason == DLL_PROCESS_DETACH)
	{
		//LogStr( "MOUSE.DLL stopping monitoring mouse position" );
		//detachMouseHook();

		// Unmap shared memory from the process's address space
		fIgnore = UnmapViewOfFile(lpvMem);

		// Close the process's handle to the file-mapping object
		fIgnore = CloseHandle(hMapObject);
	}

	return 0;
}
