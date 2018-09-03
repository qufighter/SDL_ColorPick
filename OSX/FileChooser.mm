//
//  FileChooser.mm
//  ColorPick iOS SDL
//
//  Created by Sam Larison on 8/13/16.
//
//

#import "FileChooser.h"
#import <Cocoa/Cocoa.h>
//#import <Foundation/Foundation.h>
//#import <UIKit/UIKit.h>



void beginImageSelector()
{
    //[myFc selectPhoto];
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
