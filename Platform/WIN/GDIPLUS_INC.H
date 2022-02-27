#ifndef cpickGDIPINC_h
#define cpickGDIPINC_h

// NOTE: determine if this file is really needed
// I think it is perhaps NOT needed at all ( we can avoid GDI completely )


/**********************************************************************/
/*     						GDI + FILES     				  	  	  */
/**********************************************************************/
#include <string>
//#include <windows.h>
#include <wtypes.h>
#include <objidl.h>
#include <algorithm>
typedef ULONG ULONG_PTR;
#include <gdiplus.h>
using namespace Gdiplus;
//used by gdi to save images to disk
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
wchar_t* string_to_wchar(std::string instring);
static CLSID bmpClsid;
static bool gdi_plus_started = false;

#ifdef USING_GDIPLUS_SHARED_FUNCTIONS 
	//GDI+ Token
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

	void GDI_START(){
		// Initialize GDI+.
		if(gdi_plus_started)return;
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		GetEncoderClsid(L"image/bmp", &bmpClsid);
		gdi_plus_started=true;
		//LogStr( "DLL: DllMain: GDI Started" );
	}
	void GDI_END(){
		//shutdown GDI+
		Gdiplus::GdiplusShutdown(gdiplusToken);
		gdi_plus_started=false;
		//LogStr( "DLL: DllMain: GDI Shutdown" );
	}

	//string to wchar
	wchar_t* string_to_wchar(std::string instring){
		wchar_t *wfilePathStr = (wchar_t *)malloc( 255 );
		MultiByteToWideChar(CP_ACP,MB_COMPOSITE,instring.c_str(),-1,wfilePathStr,255);
		return wfilePathStr;
	}
	//used by gdi to save images to disk
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
	{
	   UINT  num = 0;          // number of image encoders
	   UINT  size = 0;         // size of the image encoder array in bytes

	   ImageCodecInfo* pImageCodecInfo = NULL;

	   GetImageEncodersSize(&num, &size);
	   if(size == 0)
		  return -1;  // Failure

	   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	   if(pImageCodecInfo == NULL)
		  return -1;  // Failure

	   GetImageEncoders(num, size, pImageCodecInfo);

	   for(UINT j = 0; j < num; ++j)
	   {
		  if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		  {
			 *pClsid = pImageCodecInfo[j].Clsid;
			 free(pImageCodecInfo);
			 return j;  // Success
		  }    
	   }

	   free(pImageCodecInfo);
	   return -1;  // Failure
	}

	//snapshot entire screen
	//taken from CapIT_src.zip from http://www.codeproject.com/KB/GDI/barry_s_screen_capture.aspx
	Gdiplus::Bitmap* CopyEntireScreenToBitmap(Gdiplus::Rect &screenRect)
	{
		HDC         hScrDC, hMemDC;         // screen DC and memory DC     
	//	HBITMAP     hBitmap; //, 
	//	HBITMAP     hBitmap;
	//	HBITMAP     hOldBitmap;    // handles to deice-dependent bitmaps     
		int         nX, nY, nX2, nY2;       // coordinates of rectangle to grab     
		int         nWidth, nHeight;        // DIB width and height     
		int         xScrn, yScrn;           // screen resolution      

		HGDIOBJ     hOldBitmap , hBitmap;
			

		LONG num_monitors=((LONG)::GetSystemMetrics(SM_CMONITORS));
		LONG same_fmt=((LONG)::GetSystemMetrics(SM_SAMEDISPLAYFORMAT));
		//oddly these do not seem to update correctly until application restarts..??
		LONG virtual_start_x=((LONG)::GetSystemMetrics(SM_XVIRTUALSCREEN));
		LONG virtual_start_y=((LONG)::GetSystemMetrics(SM_YVIRTUALSCREEN));
		LONG virtual_end_x=((LONG)::GetSystemMetrics(SM_CXVIRTUALSCREEN));
		LONG virtual_end_y=((LONG)::GetSystemMetrics(SM_CYVIRTUALSCREEN));

		//SIZE* s = new SIZE();

		//total width across all monitors (size of bitmap)
		int scx = abs(virtual_start_x-virtual_end_x);
		int scy = abs(virtual_start_y-virtual_end_y);
		//s->cx = (LONG)::GetSystemMetrics( SM_CXFULLSCREEN );//SM_CXSCREEN
		//s->cy = (LONG)::GetSystemMetrics( SM_CYSCREEN );

		
		//s->cx
		//s->cy

		/*LPRECT lpRect;
		lpRect->top = 0;
		lpRect->left = 0;
		lpRect->bottom = s->cy;
		lpRect->right = s->cx;
		*/
			// check for an empty rectangle 
		//if (IsRectEmpty(lpRect))       
		//   return NULL;      
		   // create a DC for the screen and create     
		   // a memory DC compatible to screen DC          

		//http://stackoverflow.com/questions/576476/get-devicecontext-of-entire-screen-with-multiple-montiors
	   //hScrDC = ::GetDC(0);//CreateDC("DISPLAY", NULL, NULL, NULL);   
	   hScrDC = ::GetDC(0);//CreateDC(TEXT("DISPLAY"),NULL,NULL,NULL);
	   hMemDC = CreateCompatibleDC(hScrDC);      // get points of rectangle to grab  
   
	   screenRect.X=virtual_start_x;
	   screenRect.Y=virtual_start_y;
		screenRect.Width=scx;
		screenRect.Height=scy;

	   nX = virtual_start_x;//0;//lpRect->left;     
	   nY = virtual_start_y;//lpRect->top;     
//	   nX2 = virtual_end_x;//s->cx;//lpRect->right;     
//	   nY2 = virtual_end_y;//s->cy;//lpRect->bottom;      // get screen resolution      
   
	   xScrn = GetDeviceCaps(hScrDC, HORZRES);     
	   yScrn = GetDeviceCaps(hScrDC, VERTRES);      

	   //make sure bitmap rectangle is visible      
   
/*	   if (nX < virtual_start_x)         
		  nX = 0;     
   
	   if (nY < virtual_start_y)         
		  nY = 0;     
   
	   if (nX2 > xScrn)         
		  nX2 = xScrn;     
   
	   if (nY2 > yScrn)         
		  nY2 = yScrn;   */   

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
   
	   Gdiplus::Bitmap* gbmp = new Bitmap((HBITMAP)hBitmap, NULL);

	   // clean up      
   
	   //DeleteDC(hScrDC);     
	   //DeleteDC(hMemDC);      
		DeleteObject(hMemDC);

		DeleteObject(hBitmap);

	   ::ReleaseDC(0,hScrDC);

	   // return handle to the bitmap      
   
	   return gbmp; 

	}

	//adapted
	Gdiplus::Bitmap* CopyRegionToBitmap(int* x, int* y, int width)
	{
		HDC         hScrDC, hMemDC;         // screen DC and memory DC     
	//	HBITMAP     hBitmap; //, 
	//	HBITMAP     hBitmap;
	//	HBITMAP     hOldBitmap;    // handles to deice-dependent bitmaps     
		int         nX, nY, nX2, nY2;       // coordinates of rectangle to grab     
		int         nWidth, nHeight;        // DIB width and height     
		int         xScrn, yScrn;           // screen resolution      

		int hwidth = width/2;

		HGDIOBJ     hOldBitmap , hBitmap;
			
		LONG virtual_start_x=((LONG)::GetSystemMetrics(SM_XVIRTUALSCREEN));
		LONG virtual_start_y=((LONG)::GetSystemMetrics(SM_YVIRTUALSCREEN));
		LONG virtual_end_x=((LONG)::GetSystemMetrics(SM_CXVIRTUALSCREEN));
		LONG virtual_end_y=((LONG)::GetSystemMetrics(SM_CYVIRTUALSCREEN));

		if( *x < virtual_start_x )*x=virtual_start_x;
		if( *y < virtual_start_y )*y=virtual_start_y;
		if( *x > virtual_end_x )*x=virtual_end_x;
		if( *y > virtual_end_y )*y=virtual_end_y;

		//int scx = (LONG)::GetSystemMetrics( SM_CXFULLSCREEN );//SM_CXSCREEN
		//int scy = (LONG)::GetSystemMetrics( SM_CYSCREEN );

		/*LPRECT lpRect;
		lpRect->top = 0;
		lpRect->left = 0;
		lpRect->bottom = s->cy;
		lpRect->right = s->cx;
		*/
			// check for an empty rectangle 
		//if (IsRectEmpty(lpRect))       
		//   return NULL;      
		   // create a DC for the screen and create     
		   // a memory DC compatible to screen DC          

	   hScrDC = ::GetDC(0);//CreateDC("DISPLAY", NULL, NULL, NULL);     
	   hMemDC = CreateCompatibleDC(hScrDC);      // get points of rectangle to grab  
   
	   nX = *x - hwidth;//lpRect->left;     
	   nY = *y - hwidth;//lpRect->top;    
	   nX2 = nX + width;//s->cx;//lpRect->right;     
	   nY2 = nY + width;//s->cy;//lpRect->bottom;      // get screen resolution      
   
	   xScrn = GetDeviceCaps(hScrDC, HORZRES);     
	   yScrn = GetDeviceCaps(hScrDC, VERTRES);      
   
	   //make sure bitmap rectangle is visible      
   
/*	   if (nX < 0)         
		  nX = 0;     
   
	   if (nY < 0)         
		  nY = 0;     
   
	   if (nX2 > xScrn)         
		  nX2 = xScrn;     
   
	   if (nY2 > yScrn)         
		  nY2 = yScrn;      
*/
	   nWidth = nX2 - nX;     
	   nHeight = nY2 - nY;
   
	   // create a bitmap compatible with the screen DC     
   
	   hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);      
   
	   // select new bitmap into memory DC     
   
	   hOldBitmap =   SelectObject (hMemDC, hBitmap);      
   
	   // bitblt screen DC to memory DC     
   
	   BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);     
   
	   // select old bitmap back into memory DC and get handle to     
	   // bitmap of the screen          
   
	   hBitmap = SelectObject(hMemDC, hOldBitmap);      
   
	   Gdiplus::Bitmap* gbmp = new Bitmap((HBITMAP)hBitmap, NULL);

	   // clean up      
   
	   //DeleteDC(hScrDC);     
	   //DeleteDC(hMemDC);      
		DeleteObject(hMemDC);

		DeleteObject(hBitmap);

	   ::ReleaseDC(0,hScrDC);

	   // return handle to the bitmap      
   
	   return gbmp; 

	}
#endif//end USING_GDIPLUS_SHARED_FUNCTIONS
#endif//end cpickGDIPINC_h
