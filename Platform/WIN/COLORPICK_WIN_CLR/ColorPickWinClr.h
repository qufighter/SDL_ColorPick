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
	int	X, Y, W;
	bool m1, m2;
	pt_type() {
		X = 0, Y = 0, W = 0, m1 = false, m2 = false;
	}
	pt_type(int pX, int pY, bool pm1, bool pm2) {
		X = pX, Y = pY, W = 0, m1 = pm1, m2 = pm2;
	}
};

class ColorPickWinClr
{
public:
	void winTogglePicking();
	bool openURL(char*& url);

};


#endif
