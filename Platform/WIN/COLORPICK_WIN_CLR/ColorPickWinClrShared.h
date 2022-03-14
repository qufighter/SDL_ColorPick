//
//  FileChooser.h
//  ColorPick SDL
//
//  Created by Sam Larison on 8/13/16.
//
//

// USED BY LINUX / UNIX

#ifndef ColorPick_windiows_clrshared__h
#define ColorPick_windiows_clrshared__h

#define SHMEMSIZE 33177600 
// 1920*1080 = 2073600
// 4k: 2073600 * 4 = 8294400
// quad 4k: 33177600
// really we support only squares, so it is possible to offset 2 4k monitors sch that the single desktop image square that contains both is roughly quad 4k...

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


#endif
