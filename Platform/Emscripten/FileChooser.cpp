//
//  FileChooser.mm
//  ColorPick SDL
//
//  Created by Sam Larison on 8/13/16.
//
//

// YOU MAY HAVE TO DELETE THIS FILE FROM OSX PROJECT - IT WILL CONFUSE XCODE


#ifdef __EMSCRIPTEN__
#import "EmscriptenFileChooser.h"

/*
Next fixes:

4) keyboard mode game usability issue needs to be fixed, we know the giltch CAN OCCUR and logic dictates it must be fixable given what it was when it happened...
        suspect moved item to exactly the wrong pixel so that it was exactly dead on the >< match and missed the object?? @navigateControllerCursor

6) finish review of https://github.com/qufighter/SDL_ColorPick/commit/3258cf8e5993e61e76ae9a613165ccd385ff345a
    -> ux constructor -> does too much stuff !!!!! now delay that stuff?

    disabled From version 35: this feature is behind the dom.imagecapture.enabled preference (needs to be set to true). To change preferences in Firefox, visit about:config.
https://developer.mozilla.org/en-US/docs/Web/API/ImageCapture
 actualy did test and still doesn not work, but maybe takePhoto() does... 

*/


void getImagePathFromMainThread(){


}



// regarding the EMSCRIPTEN_KEEPALIVE and AKA's below...
// read https://emscripten.org/docs/getting_started/FAQ.html?highlight=exported_runtime_methods#why-do-i-get-typeerror-module-something-is-not-a-function
// but its named deterministically I think this is wontfix and arguably in testing there is nootherway actually.. remaping the name is even less intuitive as tracing the code becomes thatmuchmore confusing

EMSCRIPTEN_KEEPALIVE
void load_img_canvas_now(int x, int y){ // AKA __Z19load_img_canvas_nowii
    SDL_Surface* fancyImg = IMG_Load("/latest-custom-img");
    if( !fancyImg ){
        SDL_Log("no suface..");
        SDL_Log("%s", SDL_GetError());
    }

    if( x > 0 || y > 0 ){
        openglContext->imageWasSelectedCb(fancyImg, true, x, y);
    }else{
        openglContext->imageWasSelectedCb(fancyImg, true);
    }

}


EMSCRIPTEN_KEEPALIVE
void resize_img_canvas_now(int w, int h){
    SDL_SetWindowSize(openglContext->sdlWindow, w, h);
}
//__Z21resize_img_canvas_nowii


EMSCRIPTEN_KEEPALIVE
bool is_program_booted(){

    return openglContext->isProgramBooted();
}

EMSCRIPTEN_KEEPALIVE
void try_reading_prefs_now(int x, int y){ // AKA __Z21try_reading_prefs_nowii 

    //openglContext->generalUx->readInState();
    // this has to occur in main thread again... 
    //openglContext->setupScene(); // this ultimately calls the above...  creates the UX and reads prefs... 
    // todo: we can refactor this so we can use the UX to show a loading screen though... in the meantime

    SDL_Event event;
    SDL_UserEvent userevent;
    userevent.type = SDL_USEREVENT;
    userevent.code = USER_EVENT_ENUM::IDBFS_INITIAL_SYNC_COMPLETED;
    event.type = SDL_USEREVENT;
    event.user = userevent;
    SDL_PushEvent(&event);

}
//
//EMSCRIPTEN_KEEPALIVE int testversion() {
//    return 55;
//}
//
//EMSCRIPTEN_KEEPALIVE
//uint8_t* create8_buffer(int count) {
//    return (uint8_t*)malloc(count * sizeof(uint8_t));
//}
//
//EMSCRIPTEN_KEEPALIVE
//void load_8buffer_to_image_and_destroy(uint8_t* p, int count) {
//
//    SDL_Log("We got the buffer now!! %i", count);
//
//    // next - can we use.... IMG_Load_RW(SDL_RWops *src, int freesrc); ?
//    // or is thre some other magic we can use in JS land to instead just get a SDL_Surface* ?
//
//    SDL_RWops* memref = SDL_RWFromMem(p, count * sizeof(uint8_t));
//
////    Sint64 filesize = SDL_RWsize(memref);
////    Sint64 start = SDL_RWtell(memref);
////    SDL_Log("we got filesize %i and start %i", filesize, start);
//
//    SDL_Log("some u8s... %i %i %i", (int)SDL_ReadU8(memref), (int)SDL_ReadU8(memref), (int)SDL_ReadU8(memref));
//
//    SDL_Log("before error? %s", SDL_GetError());
//    SDL_SetError("huh...");
//
//    //SDL_Surface* fancyImg = IMG_Load_RW(memref, 0);
//    SDL_Surface* fancyImg = IMG_LoadPNG_RW(memref);
//
//    if( !fancyImg ){
//        SDL_Log("no suface..");
//        SDL_Log("%s", SDL_GetError());
//    }
//
//    openglContext->imageWasSelectedCb(fancyImg);
//
//    SDL_RWclose(memref);
//
//    free(p);
//}
//
//EMSCRIPTEN_KEEPALIVE
//void destroy8_buffer(uint8_t* p) {
//    free(p);
//}

/*
 when working, we have the time for this next:

 see "Compiling an Existing C Module to WebAssembly"

 1) call a function that gives JS the buffer it needs
 2) JS can write our data to the buffer
 3) tell our program to LOAD IT UP TO A SURFACE (THE DATAS)
 4) profit? (use whatever hacks avaialble in emscripten SDL image loader to hopefully make sense of the datas)

 */

EM_JS(void, em_get_file, (), {
    //alert('hai');
    //alert('bai');
    var i=document.createElement('input');
    i.type='file';
    i.setAttribute('accept', 'image/jpg, image/jpeg, image/png');
    //document.body.appendChild(i); // << this may be optional - plus we need to clean up if we leave these in the doc...

    i.addEventListener('change', function(ev){
        em_screenshot_load_from_url(URL.createObjectURL(i.files[0]));
    });
    i.click();

});



void beginImageSelector()
{
    em_get_file();
}



EM_JS(void, em_screenshot_load_from_url, (int* data), {
    //console.log(data); // when we call from JS data will just be a simple string datatype
    var img = new Image;
    // todo: error handling??? maybe it already is...
    img.onload = function() {
        em_screenshot_load_from_img(img);
        URL.revokeObjectURL(data); // cleanup/release object url
    };
    img.src = data;
});

EM_JS(void, em_screenshot_load_from_img, (int* imgOrImgBmp), {
    var cvs = document.createElement('canvas');
    cvs.width = imgOrImgBmp.naturalWidth || imgOrImgBmp.width;
    cvs.height= imgOrImgBmp.naturalHeight || imgOrImgBmp.height;
    var ctx = cvs.getContext('2d');
    ctx.drawImage(imgOrImgBmp, 0,0);
    preloadedImages['/latest-custom-img'] = cvs;
    __Z19load_img_canvas_nowii();
});



EM_JS(void, em_screenshot_as_file, (), {

    var track = null;
    var controller = null;
    var imageCapture = null;

    try{
        controller = new CaptureController();
    }catch(controllerEx){}

    var do_cleanup_focus_beh = function(){
        //console.log(track, controller); // DEBUG!!!
        if( controller != null && track && (!track.label || track.label.substring(0, 5) != 'scree')){
            //console.warn(controller, track); // ONGOING DEBUG!!!  (magic property hunt to avoid/suppress uncatchable error below!)
            try{
                if(track.readyState != "ended"){
                    controller.setFocusBehavior("no-focus-change");
                }
                // this is sort of dumb/broken, setFocusBehavior triggers error on the getDisplayMedia promise !(instead of being handled/caught here ^^) ..., so we scree the error out above
            }catch(invalidStateEx){}
        }
    };
    // so the CaptureController interface is also causing a lot of problematic testing being needed, and throws unnecessary erros, while not revealing it's state very clearly to avoid those; when it should just work...

    var do_cleanup = function(){
        if( track != null){
            track.stop();
        }
        do_cleanup_focus_beh();
    };

    navigator.mediaDevices.getDisplayMedia({
        video: true,
        controller: controller,
        preferCurrentTab: false,
        surfaceSwitching: "exclude",
        monitorTypeSurfaces: "include",
    }).then(function(mediaStream){

        //document.querySelector("video").srcObject = mediaStream; // fake fullscreen video window for interactivity??? maybe???
        // probelm being it would only work for "fullscreen" type, do we know what type we got here? (sort of; see track.label tests above)
        track = mediaStream.getVideoTracks()[0];

        //console.log(track, controller); //DEBUG!!!

        // well this is embarrasing to use, unimplementable race condition of an interface... like focus ALREADY CHANGED now right? (test it?)
        //alert('the kracken arrived just now instead'); // so yes in firefox the focus changed on the platform testing...
        do_cleanup_focus_beh(); // maybe quickly undoes the focus change needed for screenshot?  anyway it'd be better to see if getDisplayMedia options support a keep focus as an alternative...

        if( typeof(window.ImageCapture) == "function" ){
            imageCapture = new ImageCapture(track);
        }else{
            imageCapture = {
                grabFrame: function(){return new Promise(function(resolve, reject){
                    var videoElm = document.createElement('video');
                    var canvas = document.createElement('canvas');
                    var context = canvas.getContext('2d');

                    videoElm.addEventListener('loadeddata', async function(){
                        canvas.width = videoElm.videoWidth;
                        canvas.height = videoElm.videoHeight;
                        try{
                            await videoElm.play();
                            context.drawImage(videoElm, 0, 0, videoElm.videoWidth, videoElm.videoHeight);
                            canvas.toBlob(function(blb){resolve(createImageBitmap(blb));}, 'image/png');
                        }catch(er){
                            reject(er);
                        }
                    });
                    videoElm.srcObject = mediaStream; // but not track !?!?! tbd learn how to assing or construct from track... 
                })}
            }
        }

        imageCapture.grabFrame().then(function(imgBmp){
            do_cleanup(); // video no longer needed, sorry JS engine garbage collector.. would pass track, capture as args but that will probably interfere with a differnt garbage collector...
            em_screenshot_load_from_img(imgBmp);
        }).catch(function(error){
            do_cleanup();
            console.error("grabFrame() error: ", error);
            alert(error + " \n\n See also, ImageCapture.grabFrame browser compatibility chart: \n https://developer.mozilla.org/en-US/docs/Web/API/ImageCapture/grabFrame#browser_compatibility \n\n Try taking the screenshot with a different program instead and open it with the other button.")
        });


        //return imageCapture.getPhotoCapabilities(); // to chain promises...
    }).catch(function(error){
        do_cleanup();
        console.error("getDisplayMedia() error: ", error);
        alert(error + " \n\n See also, ImageCapture browser compatibility chart: \n https://developer.mozilla.org/en-US/docs/Web/API/ImageCapture#browser_compatibility \n\n Try taking the screenshot with a different program instead and open it with the other button.")

    });
    // TBD if error; under no condition will we be able to determine that we can defnitely now remove this completely broken button form the UX on this platform...
    // in theory a property on track or capture could allow us to detect a "maybe retry will work" state?
    // if track and capture are both undefined, that COULD VERY WELL BE a "this ain't gonna work" state... we should handle with button removal event/call
});


void beginScreenshotSeleced(){
    // tbd the icon that triggers this needs to be unique to the platform as it does not opperate like others
    // just opens a screenshot more like beginImageSelector
    // no cursor on icon...??? actually cursor may be appropriate, crosshair is removed...
    em_screenshot_as_file();
}


EM_JS(void, em_open_url, (char* url), {
    var str = UTF8ToString(url);
    setTimeout(function(){
        console.log('Navigating to the following URL in a new tab...', str);
        window.open(str);
    }, 500); // we give the time for our animation engine to catch up... the first part of scale UP needs to complete BEFORE time starts accumulating.....
});


bool openURL(char* &url)
{
    /*
     in JS land we can use this helper to fix our cstring into a proper javascript string!
     SEE: function _emscripten_get_preloaded_image_data(path, w, h) {
     if ((path | 0) === path) path = UTF8ToString(path);
    */
    em_open_url(url);
    return true;
}

void requestReview(){
    
}


EM_JS(void, em_copy_to_clippy, (char* url), {
    var str = UTF8ToString(url);
    console.log('Copying the following to clipboard...', str);
    var n=document.createElement('input');document.body.appendChild(n);
    n.value=str;n.select();document.execCommand('copy');n.parentNode.removeChild(n);
});

void emscripen_copy_to_clipboard(char* url){
    // todo eventually SDL_SetClipboardText will probably just work on emscripten...
    // we could contribut our working "fix" to this....

    em_copy_to_clippy(url);

}

#endif
