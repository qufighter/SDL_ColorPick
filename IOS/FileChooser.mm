//
//  FileChooser.mm
//  ColorPick iOS SDL
//
//  Created by Sam Larison on 8/13/16.
//
//

#import "FileChooser.h"
//#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

// USED BY IOS EXCLUSIVELY MOVE TO IOS FOLDER

/*
 Following was coied from SDL2_image-2.0.1/IMG_UIImage.m
 curiously this does not seem to be an export of SDL_image.h
 IMG_UIImage.m also defines this function, presumably for non ios devices
 */

#import <MobileCoreServices/MobileCoreServices.h> // for UTCoreTypes.h

// Once we have our image, we need to get it into an SDL_Surface
static SDL_Surface* Create_SDL_Surface_From_CGImage_RGB(CGImageRef image_ref)
{
    /* This code is adapted from Apple's Documentation found here:
     * http://developer.apple.com/documentation/GraphicsImaging/Conceptual/OpenGL-MacProgGuide/index.html
     * Listing 9-4††Using a Quartz image as a texture source.
     * Unfortunately, this guide doesn't show what to do about
     * non-RGBA image formats so I'm making the rest up.
     * All this code should be scrutinized.
     */

    size_t w = CGImageGetWidth(image_ref);
    size_t h = CGImageGetHeight(image_ref);
    CGRect rect = {{0, 0}, {w, h}};

    CGImageAlphaInfo alpha = CGImageGetAlphaInfo(image_ref);
    //size_t bits_per_pixel = CGImageGetBitsPerPixel(image_ref);
    size_t bits_per_component = 8;

    SDL_Surface* surface;
    Uint32 Amask;
    Uint32 Rmask;
    Uint32 Gmask;
    Uint32 Bmask;

    CGContextRef bitmap_context;
    CGBitmapInfo bitmap_info;
    CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();

    if (alpha == kCGImageAlphaNone ||
        alpha == kCGImageAlphaNoneSkipFirst ||
        alpha == kCGImageAlphaPremultipliedFirst || // added
        alpha == kCGImageAlphaPremultipliedLast || // added
        alpha == kCGImageAlphaNoneSkipLast) {
        bitmap_info = kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Host; /* XRGB */
        Amask = 0x00000000;
    } else {
        /* kCGImageAlphaFirst isn't supported */
        //bitmap_info = kCGImageAlphaFirst | kCGBitmapByteOrder32Host; /* ARGB */
        if( alpha == kCGImageAlphaFirst ){
            bitmap_info = kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Host; /* ARGB */
        }else{
            bitmap_info = kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Host; /* ARGB */
        }
        Amask = 0xFF000000;
    }

    Rmask = 0x00FF0000;
    Gmask = 0x0000FF00;
    Bmask = 0x000000FF;

    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, Rmask, Gmask, Bmask, Amask);
    if (surface)
    {
        // Sets up a context to be drawn to with surface->pixels as the area to be drawn to
        bitmap_context = CGBitmapContextCreate(
                                               surface->pixels,
                                               surface->w,
                                               surface->h,
                                               bits_per_component,
                                               surface->pitch,
                                               color_space,
                                               bitmap_info
                                               );

        // Draws the image into the context's image_data
        CGContextDrawImage(bitmap_context, rect, image_ref);

        CGContextRelease(bitmap_context);

        // FIXME: Reverse the premultiplied alpha
        if ((bitmap_info & kCGBitmapAlphaInfoMask) == kCGImageAlphaPremultipliedFirst) {
            int i, j;
            Uint8 *p = (Uint8 *)surface->pixels;
            for (i = surface->h * surface->pitch/4; i--; ) {
#if __LITTLE_ENDIAN__
                Uint8 A = p[3];
                if (A) {
                    for (j = 0; j < 3; ++j) {
                        p[j] = (p[j] * 255) / A;
                    }
                }
#else
                Uint8 A = p[0];
                if (A) {
                    for (j = 1; j < 4; ++j) {
                        p[j] = (p[j] * 255) / A;
                    }
                }
#endif /* ENDIAN */
                p += 4;
            }
        }
    }

    if (color_space)
    {
        CGColorSpaceRelease(color_space);
    }

    return surface;
}
static SDL_Surface* Create_SDL_Surface_From_CGImage_Index(CGImageRef image_ref)
{
    size_t w = CGImageGetWidth(image_ref);
    size_t h = CGImageGetHeight(image_ref);
    size_t bits_per_pixel = CGImageGetBitsPerPixel(image_ref);
    size_t bytes_per_row = CGImageGetBytesPerRow(image_ref);

    SDL_Surface* surface;
    SDL_Palette* palette;
    CGColorSpaceRef color_space = CGImageGetColorSpace(image_ref);
    CGColorSpaceRef base_color_space = CGColorSpaceGetBaseColorSpace(color_space);
    size_t num_components = CGColorSpaceGetNumberOfComponents(base_color_space);
    size_t num_entries = CGColorSpaceGetColorTableCount(color_space);
    uint8_t *entry, entries[num_components * num_entries];

    /* What do we do if it's not RGB? */
    if (num_components != 3) {
        SDL_SetError("Unknown colorspace components %lu", num_components);
        return NULL;
    }
    if (bits_per_pixel != 8) {
        SDL_SetError("Unknown bits_per_pixel %lu", bits_per_pixel);
        return NULL;
    }

    CGColorSpaceGetColorTable(color_space, entries);
    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, bits_per_pixel, 0, 0, 0, 0);
    if (surface) {
        uint8_t* pixels = (uint8_t*)surface->pixels;
        CGDataProviderRef provider = CGImageGetDataProvider(image_ref);
        NSData* data = (id)CGDataProviderCopyData(provider);
        [data autorelease];
        const uint8_t* bytes = (const unsigned char*)[data bytes];
        size_t i;

        palette = surface->format->palette;
        for (i = 0, entry = entries; i < num_entries; ++i) {
            palette->colors[i].r = entry[0];
            palette->colors[i].g = entry[1];
            palette->colors[i].b = entry[2];
            entry += num_components;
        }

        for (i = 0; i < h; ++i) {
            SDL_memcpy(pixels, bytes, w);
            pixels += surface->pitch;
            bytes += bytes_per_row;
        }
    }
    return surface;
}
static SDL_Surface* Create_SDL_Surface_From_CGImage(CGImageRef image_ref)
{
    CGColorSpaceRef color_space = CGImageGetColorSpace(image_ref);
    CGColorSpaceModel color_space_model = CGColorSpaceGetModel(color_space);

    if (color_space_model == kCGColorSpaceModelIndexed) {
        return Create_SDL_Surface_From_CGImage_Index(image_ref);
    } else {
        return Create_SDL_Surface_From_CGImage_RGB(image_ref);
    }
}






UIViewController *top;


// normally goes in header file, but we cant include this elsewhere
@interface FileChooser : UIViewController <UIImagePickerControllerDelegate, UINavigationControllerDelegate>

@property (strong, nonatomic) IBOutlet UIImageView *imageView;

- (void)takePhoto;
- (void)selectPhoto;

@end





@interface FileChooser ()

@end

@implementation FileChooser

- (void)viewDidLoad {

    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.

}

- (void)viewDidAppear:(BOOL)animated {

    [super viewDidAppear:animated];
    
}

- (void)presentSelf {
    top = [UIApplication sharedApplication].keyWindow.rootViewController; // think we can use view controller from sdl window instead
    [top presentViewController:self animated:NO completion: nil];
}
- (void)dismissSelf {
    [self dismissViewControllerAnimated:NO completion:NULL]; // dismiss myFc
}

- (void)takePhoto {

    if (![UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera]) {

        UIAlertView *myAlertView = [[UIAlertView alloc] initWithTitle:@"Error"
                                                              message:@"Device has no camera"
                                                             delegate:nil
                                                    cancelButtonTitle:@"OK"
                                                    otherButtonTitles: nil];
        [myAlertView show];

    }else{
        [self presentSelf];

        UIImagePickerController *picker = [[UIImagePickerController alloc] init];
        picker.delegate = self;
        picker.allowsEditing = YES;
        picker.sourceType = UIImagePickerControllerSourceTypeCamera;

        [self presentViewController:picker animated:NO completion:NULL];
    }



}

- (void)selectPhoto {


    [self presentSelf];

    UIImagePickerController *picker = [[UIImagePickerController alloc] init];
    picker.delegate = self;
    picker.allowsEditing = YES;
    picker.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;

    [self presentViewController:picker animated:NO completion:NULL];


}

#pragma mark - Image Picker Controller delegate methods

- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info {

    UIImage *chosenImage = info[UIImagePickerControllerOriginalImage];
    //self.imageView.image = chosenImage;

    CGImageRef imageRef = chosenImage.CGImage;

    SDL_Surface *myCoolSurface = Create_SDL_Surface_From_CGImage(imageRef);

    //http://stackoverflow.com/questions/20595227/how-do-i-get-an-image-from-the-ios-photo-library-and-display-it-in-in-uiwebview
    //You can retrieve the actual UIImage
//    UIImage *image = [info valueForKey:UIImagePickerControllerOriginalImage];
    //Or you can get the image url from AssetsLibrary
//    NSURL *path = [info valueForKey:UIImagePickerControllerReferenceURL];
//    NSString *pathString = path.absoluteString;
//    const char *cString = [pathString UTF8String];

    [picker dismissViewControllerAnimated:NO completion:NULL];


    [self dismissSelf]; // dismiss myFc

    openglContext->imageWasSelectedCb(myCoolSurface);

}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker {

    [picker dismissViewControllerAnimated:NO completion:NULL];


    [self dismissSelf]; // dismiss myFc

}


@end


FileChooser *myFc = [[FileChooser alloc] init];
// http://stackoverflow.com/a/21903675/2905353

void beginImageSelector()
{
    [myFc selectPhoto];
}

