//
//  NSOverlayWindow.m
//  ColorPick
//
//  Created by Rahul on 6/8/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import "NSOverlayWindow.h"


@implementation NSOverlayWindow

- (id)initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)windowStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)deferCreation screen:(NSScreen*)selScreen {
	
	if ((self = [super initWithContentRect:contentRect
								 styleMask:windowStyle
								   backing:bufferingType
									 defer:deferCreation screen:selScreen])) {
		[self setOpaque:YES];
		[self setIgnoresMouseEvents:NO];
		[self setAlphaValue:0.0];
		[self setLevel:CGShieldingWindowLevel()];
		[self makeKeyAndOrderFront:nil];
	}
	return self;
}


// Windows created with NSBorderlessWindowMask normally can't be key, but we want ours to be
- (BOOL)canBecomeKeyWindow {
    return YES;
}

- (void)keyUp:(NSEvent*)event 
{ 
	[self willChangeValueForKey:@"shield_keyboard"];
	lastKeycode=[event keyCode];
	[self didChangeValueForKey:@"shield_keyboard"];
}

- (int)getLastKeycode{
	return lastKeycode;
}


@end
