//
//  FileChooser.h
//  ColorPick SDL
//
//  Created by Sam Larison on 8/13/16.
//
//

// USED BY LINUX / UNIX

#ifndef ColorPick_windiows_clrmain__h
#define ColorPick_windiows_clrmain__h


struct pt_type
{
	int	X, Y, W, H;
	bool m1, m2;
	pt_type() {
		X = 0, Y = 0, W = 0, H = 0, m1 = false, m2 = false;
	}
	pt_type(int pX, int pY, bool pm1, bool pm2) {
		X = pX, Y = pY, W = 0, H = 0, m1 = pm1, m2 = pm2;
	}
};

#define DECLARE_HANDLE(name) struct name##__{int unused;}; typedef struct name##__ *name
typedef void * HGDIOBJ;
DECLARE_HANDLE(HDCtype);

class ColorPickWinClr
{
private:
	HDCtype     hScrDC, hMemDC;         // screen DC and memory DC
	HGDIOBJ     hBitmap;


public:
	int         nWidth, nHeight;        // DIB width and height


	static ColorPickWinClr* Singleton();
	void winTogglePicking();
	bool openURL(char*& url);

	static bool ms_bInstanceCreated;
	static ColorPickWinClr* pInstance;

	void ColorPickWinClr::ColorPickWinClrCopyEntireScreenToBitmapWin(void* ret_bitm, int* ret_size, pt_type* info);
	void ColorPickWinClr::FreeLastBitmapWin();

};


static ColorPickWinClr* colorPickWinClr = ColorPickWinClr::Singleton();


/*
to call in y our app, use

//http://www.cplusplus.com/forum/general/8090/
//  typedef LONG (__stdcall *pCreateCompression)(LONG *context, LONG flags);
typedef BOOL (__stdcall *pBegin_Monitor_Mouse_Position)(pt_type*);
typedef BOOL (__stdcall *pEnd_Monitor_Mouse_Position)();
typedef pt_type* (__stdcall *pGet_Mouse_Position)();

pBegin_Monitor_Mouse_Position Begin_Monitor_Mouse_Position;
pEnd_Monitor_Mouse_Position End_Monitor_Mouse_Position;
pGet_Mouse_Position Get_Mouse_Position;



*/

////Calling this function from VB simply ensures winMain gets called properly
//extern "C" __declspec(dllexport) int  Begin_Monitor_Mouse_Position(pt_type* mpt);
//
////Calling this function from VB simply ensures winMain gets called properly
//extern "C" __declspec(dllexport) int  End_Monitor_Mouse_Position(void);
//
////Calling this function from VB simply ensures winMain gets called properly
//extern "C" __declspec(dllexport) pt_type* Get_Mouse_Position(void);
//
//extern "C" __declspec(dllexport) bool color_pick_win_api_starturl(char*& url);


#endif
