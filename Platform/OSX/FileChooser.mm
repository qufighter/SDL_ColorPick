//
//  FileChooser.mm
//  ColorPick SDL
//
//  Created by Sam Larison on 8/13/16.
//
//

//ARE YOU GETTNG ODD ERARORS / FAILUERS BUILDING / LINKING ??? IF SO DELETE ANDROID VERSIO OF ATHIS FIL FROM PROJECT


#import "FileChooser.h"
#include "ColorPickOsx.h"

#import <Cocoa/Cocoa.h>
//#import <Foundation/Foundation.h>
//#import <UIKit/UIKit.h>

static ColorPickOsx* colorPickOsx_Window = nullptr;

void beginScreenshotSeleced()
{
    if( colorPickOsx_Window == nullptr ){
        colorPickOsx_Window = [[ColorPickOsx alloc] init];
    }
    if(colorPickOsx_Window->is_picking){
        return; // cancel? impossible to reach here... next click WILL cancel right???
    }

    [colorPickOsx_Window takeScreenshot];

    if(!colorPickOsx_Window->is_picking){
        [colorPickOsx_Window togglePicking];
    }
}

void beginImageSelector()
{

    NSOpenPanel* openDlg = [NSOpenPanel openPanel];

    [openDlg setCanChooseFiles:YES];
    [openDlg setCanChooseDirectories:NO];

//    [openDlg setPrompt:@"Select an Image"];
//    [openDlg setTitle: @"Choose an image to pick colors from"];
    if ( [openDlg runModal] == NSModalResponseOK ) // NSOKButton
    {
        NSArray* files = [openDlg URLs];
        for( int i = 0; i < [files count]; /*i++*/ )
        {
            NSString* fileName = [[files objectAtIndex:i] path];
            NSLog(@"file: %@", fileName);

            openglContext->imageWasSelectedCb(openglContext->textures->LoadSurface([fileName cStringUsingEncoding:NSUTF8StringEncoding]), true);

            //SDL_ShowWindow(window);
            SDL_RaiseWindow(openglContext->getSdlWindow());
            
            // OUR mAIN window did not regain focus!!!
            break; // one image supported.... we do not even increment our loop...
        }
    }else{
        SDL_RaiseWindow(openglContext->getSdlWindow());
    }

}

bool openURL(const std::string &url)
{
    bool success = false;

    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@(url.c_str())]];



//    @autoreleasepool
//    {
//        UIApplication *app = [UIApplication sharedApplication];
//        NSURL *nsurl = [NSURL URLWithString:@(url.c_str())];
//
//        if ([app canOpenURL:nsurl])
//            success = [app openURL:nsurl];
//    }

    return success;
}

void requestReview(){

}
