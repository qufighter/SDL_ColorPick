

#ifndef ColorPick_windiows_clrshared__h
#define ColorPick_windiows_clrshared__h


struct pt_type
{
	// has become monitor of state of the CLR instance...
	int	X, Y, W, H;
	bool m1, m2;
	bool picking_active;
	bool refresh_requested;
	bool picking_canceled;
	bool shield_created;
	pt_type() {
		X = 0, Y = 0, W = 0, H = 0, m1 = false, m2 = false, picking_active = false, refresh_requested = false, shield_created = false, picking_canceled = false;
	}
	pt_type(int pX, int pY, bool pm1, bool pm2) {
		X = pX, Y = pY, W = 0, H = 0, m1 = pm1, m2 = pm2;
	}
};

#endif
