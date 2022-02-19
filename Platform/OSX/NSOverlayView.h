//
//  NSOverlayView.h
//  ColorPick
//
//  Created by Rahul on 6/8/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>



@interface NSOverlayView : NSView {
	//NSPoint currentLocation;
	//NSPoint downLocation;
	//BOOL drawing;
	NSCursor* custom_cursor;
	//BOOL has_been_clicked_on;
}

//setter
//@property (assign) BOOL *has_been_clicked_on;

//- (BOOL)getHasBeenClickedOn;
- (void)releaseResources;

@end
