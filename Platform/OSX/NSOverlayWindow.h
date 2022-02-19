//
//  NSOverlayWindow.h
//  ColorPick
//
//  Created by Rahul on 6/8/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface NSOverlayWindow : NSWindow {
	int lastKeycode;
}

- (int)getLastKeycode;

@end
