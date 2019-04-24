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
EMSCRIPTEN_KEEPALIVE
int testversion() {
    return 55;
}

/*
 when working, we have the time for this next:

 see "Compiling an Existing C Module to WebAssembly"

 1) call a function that gives JS the buffer it needs
 2) JS can write our data to the buffer
 3) tell our program to LOAD IT UP TO A SURFACE (THE DATAS)
 4) profit?

 */

EM_JS(const char*, get_file, (), {
    //alert('hai');
    //alert('bai');
    var i=document.createElement('input');
    i.type='file';
    document.body.appendChild(i); // << this may be optional - plus we need to clean up if we leave these in the doc...

    i.addEventListener('change', function(ev){

        console.log('3gotten', i, i.value);

        console.log('3gotten', i.data);



        var reader = new FileReader();
        reader.onload = function(){

            //console.log('this, i, reader', this, i, reader);

            var arrayBuffer = reader.result;
            var array = new Uint8Array(arrayBuffer);
            var binaryString = String.fromCharCode.apply(null, array);

            console.log(binaryString); // for fancy visual repr... not actually useful?....

        };
        reader.readAsArrayBuffer(i.files[0]);


    });
    i.click();

    setTimeout(function(){
        console.log('gotten', i, i.value);

        console.log('gotten', i.data);

    },250);

    console.log('2gotten', i, i.value);

    console.log('2gotten', i.data);


    return i.value;

    // maybe this function is bool instead?  and we return true if we got a file's data ready to go, otherwise false
    // note - click() doens't block... so it can't return crap
});



void beginImageSelector()
{
    get_file();

    // next we need to push an event if we got something???
}

bool openURL(char* &url)
{

    return true;
}

void requestReview(){
    
}


#endif
