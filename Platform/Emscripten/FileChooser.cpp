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
1) evaluate preferencesPath = (char*)"/vidsbeecolorpickdata as once we pick a path we can never change it later..., 
1a) fix loading screen init to always occur to to avoid platform bugs later...
2) we have detected BAD ANIMATOINS can occur if the app is in the background and not focused... leading to timing anomolies... (eg pick button can go to orbit if you BG the app before/around with dropper goes away, or maybe just after interacting, maybe other times) I love these and notfixing them (msot of the real fun in the app is based on these combinable animations)... however when the app is in the bg we need to just count that time too and subtrract it from the animation time... (IF the bg time span is after the animation start time and animation is not ended)

releasing the capture from screenshot button on Firefox feels so hollow thouhg because its totally broken
suppose we could run a javascript check to suppress the button or find an alternative (create <video> element and check for capture interfaces)

    disabled From version 35: this feature is behind the dom.imagecapture.enabled preference (needs to be set to true). To change preferences in Firefox, visit about:config.
https://developer.mozilla.org/en-US/docs/Web/API/ImageCapture
*/


void getImagePathFromMainThread(){


}

/*
need to look.. we cann call this but its odddddd

 Module.cwrap('_Z11testversionv', 'number', [])()
 55

 or

 __Z11testversionv()
 55

but this isn't like the tutorial...... so what gives with the name here??

 TODO: test with and without cwrap export

 all this is find-able in hello.js (cwrap,testversion etc)

 also see if this needs to be in the .h or not... lots to check...

 */

// TODO:
// regarding the EMSCRIPTEN_KEEPALIVE and AKA's below...
// FYI the correct way is to read https://emscripten.org/docs/getting_started/FAQ.html?highlight=exported_runtime_methods#why-do-i-get-typeerror-module-something-is-not-a-function


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
    //document.body.appendChild(i); // << this may be optional - plus we need to clean up if we leave these in the doc...

    i.addEventListener('change', function(ev){

//        console.log('3gotten', i, i.value);
//
//        console.log('3gotten', i.data);

        var fauxPath = "customfiles/"+i.files[0].name;

        var img = new Image;
        // todo: error handling??? maybe it already is...
        img.onload = function() {

            var cvs = document.createElement('canvas');
            cvs.width = img.naturalWidth;
            cvs.height= img.naturalHeight;
            var ctx = cvs.getContext('2d');
            ctx.drawImage(img, 0,0);
            //alert('the image is drawn');

            //Module["preloadedImages"][fauxPath] = cvs;
            //Module["preloadedImages"]['/latest-custom-img'] = cvs;

            //  hmmm above now seems broken, see maybe Module.FS_createPreloadedFile  ???
            //FS.createDataFile
            //function createDataFile(parent, name, data, canRead, canWrite, canOwn)
            // actually it's still there and still seems to be the most direct way...
            preloadedImages['/latest-custom-img'] = cvs;

            //alert('now handoff to sdl IMG_Load and cross fingers...');

            //document.body.appendChild(cvs);

//            const p = __Z11filenameBufi(fauxPath.length);
//            Module.HEAP8.set(fauxPath, p);
//
//            __Z19load_img_canvas_nowPKc(p);
            __Z19load_img_canvas_nowii();
        };
        img.src = URL.createObjectURL(i.files[0]);


//        var reader = new FileReader();
//        reader.onload = function(){
//
//            //
//
//            //console.log('this, i, reader', this, i, reader);
//
//            var arrayBuffer = reader.result;
//            var array = new Uint8Array(arrayBuffer);
//
//            console.log(array);
//
//
//            const p = __Z14create8_bufferi(array.length);
//            Module.HEAPU8.set(array, p);
//            //Module.HEAP8.set(array, p);
//
//            __Z33load_8buffer_to_image_and_destroyPhi(p, array.length);
//
//            //var binaryString = String.fromCharCode.apply(null, array);
//            //console.log(binaryString); // for fancy visual repr... not actually useful?....
//
//
//            console.log(__Z11testversionv());
//
//        };
//        reader.readAsArrayBuffer(i.files[0]);

    });
    i.click();

//    setTimeout(function(){
//        console.log('gotten', i, i.value);
//
//        console.log('gotten', i.data);
//
//    },250);
//
//    console.log('2gotten', i, i.value);
//
//    console.log('2gotten', i.data);


    //return i.value;

    // maybe this function is bool instead?  and we return true if we got a file's data ready to go, otherwise false
    // note - click() doens't block... so it can't return crap
});



void beginImageSelector()
{
    em_get_file();
}


EM_JS(void, em_screenshot_as_file, (), {

    var track = null;

    var controller = null;

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
        selfBrowserSurface: "exclude",
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

        imageCapture = new ImageCapture(track);

        imageCapture.grabFrame().then(function(imgBmp){
            do_cleanup(); // video no longer needed, sorry JS engine garbage collector.. would pass track, capture as args but that will probably interfere with a differnt garbage collector...

            var cvs = document.createElement('canvas');
            cvs.width = imgBmp.width;
            cvs.height= imgBmp.height;
            var ctx = cvs.getContext('2d');
            ctx.drawImage(imgBmp, 0,0);
            preloadedImages['/latest-custom-img'] = cvs;
            __Z19load_img_canvas_nowii();


        }).catch(function(error){
            do_cleanup();
            console.error("grabFrame() error: ", error);
            alert(error + " \n\n See also, ImageCapture.grabFrame browser compatibility chart: \n https://developer.mozilla.org/en-US/docs/Web/API/ImageCapture/grabFrame#browser_compatibility")
        });
        //return imageCapture.getPhotoCapabilities();
    }).catch(function(error){
        do_cleanup();
        console.error("getDisplayMedia() error: ", error);
        alert(error + " \n\n See also, ImageCapture browser compatibility chart: \n https://developer.mozilla.org/en-US/docs/Web/API/ImageCapture#browser_compatibility")

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
