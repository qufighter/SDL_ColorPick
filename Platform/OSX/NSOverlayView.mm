//
//  NSOverlayView.m
//  ColorPick
//
//  Created by Rahul on 6/8/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import "NSOverlayView.h"
#import <Cocoa/Cocoa.h>

@implementation NSOverlayView

- (BOOL)acceptsFirstResponder {return YES;}
- (BOOL)becomeFirstResponder {return YES;}
- (BOOL)canBecomeKeyWindow {return YES;}

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
	
	NSImage* cursorImage = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"crosshair" ofType:@"png"]]; 
	custom_cursor = [[NSCursor alloc] initWithImage:cursorImage
											  hotSpot:NSMakePoint(16,16)];
	
	//[cursorImage release];
	//has_been_clicked_on=NO;
    return self;
}

- (void)releaseResources {
	[custom_cursor release];
}

- (void)viewDidLoad {
    [NSCursor hide];
}

- (void)mouseEntered:(NSEvent *)theEvent {
	//[[NSCursor crosshairCursor] push];
    [custom_cursor push];
}

- (void)scrollWheel:(NSEvent *)event {
    [self willChangeValueForKey:@"shield_scrollywheel"];
    if( [event scrollingDeltaY] > 0.0 ){
        lastScrollWheelDirection = 1;
    }else{
        lastScrollWheelDirection = -1;
    }
    [self didChangeValueForKey:@"shield_scrollywheel"];
}

- (void)mouseExited:(NSEvent *)theEvent {
	[custom_cursor pop];
    //[[NSCursor crosshairCursor] pop];
}

- (void)mouseMoved:(NSEvent *)theEvent {
	//    [[NSCursor crosshairCursor] set];
    
}
-(void)cursorUpdate:(NSEvent *)theEvent
{
	[custom_cursor set];
    //[[NSCursor crosshairCursor] set];
}

- (void) resetCursorRects
{
	//    [super resetCursorRects];
	[self addCursorRect: [self bounds] cursor:custom_cursor];
    //[self addCursorRect: [self bounds] cursor: [NSCursor crosshairCursor]];
    
} 


- (void)drawRect:(NSRect)rect {
	
	//here we may perform our display of a screenshot, if such implementation exists
	
}

- (void)mouseUp:(NSEvent *)event {
	/*
    NSLog(@"<%p>%s:", self, __PRETTY_FUNCTION__);
    [self setNeedsDisplay:YES];    
    drawing = NO;
	 */
	[self willChangeValueForKey:@"shield_mouseup"];
	//has_been_clicked_on=YES;  // this belongs in the WINDOW not the VIEW!?
	//[self orderOut:self];
	[self didChangeValueForKey:@"shield_mouseup"];
}

- (void)mouseDown:(NSEvent *)event{
	/*
    downLocation = [self convertPoint:[event locationInWindow] fromView:[[self window] contentView]];
    currentLocation = [self convertPoint:[event locationInWindow] fromView:[[self window] contentView]];
    drawing = YES;
	[self setNeedsDisplay:YES];
	 */

}

-(void)mouseDragged:(NSEvent *)event {   
    //currentLocation = [self convertPoint:[event locationInWindow] fromView:[[self window] contentView]];
	//[self setNeedsDisplay:YES];
}

/*
- (BOOL)getHasBeenClickedOn{
	return has_been_clicked_on;
	//[self orderOut:self]; - 
}
*/

- (int)getLastScrollDir{
    return lastScrollWheelDirection;
}

@end
