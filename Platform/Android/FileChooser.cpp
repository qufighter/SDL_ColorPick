//
//  FileChooser.mm
//  ColorPick SDL
//
//  Created by Sam Larison on 8/13/16.
//
//

// YOU MAY HAVE TO DELETE THIS FILE FROM OSX PROJECT - IT WILL CONFUSE XCODE


#ifdef __ANDROID__
#include <jni.h>
#import "AndroidFileChooser.h"


//com_vidsbee_colorpicksdl
//public class SDLColorPickActivity

//#define SDL_JAVA_PREFIX                                 com_vidsbee_colorpicksdl
//#define CONCAT1(prefix, class, function)                CONCAT2(prefix, class, function)
//#define CONCAT2(prefix, class, function)                Java_ ## prefix ## _ ## class ## _ ## function
//#define SDL_JAVA_INTERFACE(function)                    CONCAT1(SDL_JAVA_PREFIX, SDLColorPickActivity, function)
////#define SDL_JAVA_AUDIO_INTERFACE(function)              CONCAT1(SDL_JAVA_PREFIX, SDLAudioManager, function)
////#define SDL_JAVA_CONTROLLER_INTERFACE(function)         CONCAT1(SDL_JAVA_PREFIX, SDLControllerManager, function)
////#define SDL_JAVA_INTERFACE_INPUT_CONNECTION(function)   CONCAT1(SDL_JAVA_PREFIX, SDLInputConnection, function)
//
//JNIEXPORT void JNICALL SDL_JAVA_INTERFACE(loadThisImagePath)(
//                                                            JNIEnv* env, jclass cls,
//                                                            jstring path);
//
//
//JNIEXPORT void JNICALL SDL_JAVA_INTERFACE(loadThisImagePath)(
//                                                            JNIEnv* env, jclass cls,
//                                                            jstring path)
//{
//
//    const char *cpath = env->GetStringUTFChars(path, NULL);
//    SDL_Log("We got the path! %s" , cpath);
//
//    env->ReleaseStringUTFChars(path, cpath);
//}


/*

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



 */

int queued_img_proc_events = 0;
SDL_TimerID my_android_fileopen_timer_id;
static Uint32 my_test_if_image_selected(Uint32 interval, void* parm){

    SDL_Log("Our check done timer is firing a lot"); // useful to see that animations DO stop...

    if( queued_img_proc_events > 0 ) return interval;

    SDL_Event event;
    SDL_UserEvent userevent;

    /* In this example, our callback pushes an SDL_USEREVENT event
     into the queue, and causes our callback to be called again at the
     same interval: */

    
    userevent.type = SDL_USEREVENT;
    userevent.code = USER_EVENT_ENUM::IMAGE_SELECTOR_READY;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    queued_img_proc_events++;

    //UxAnim* self = (UxAnim*)parm; // param was nullptr

    if( !colorPickState->appInForeground ) { // we tried this but we can't pick an image... not quite sure why yet... compare logs?
        return 0; // app is in background, kill the timer...
    }

    return interval;
}


bool enoughMemoryForMeshes(){


    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject activity = (jobject)SDL_AndroidGetActivity();
    jclass clazz(env->GetObjectClass(activity));

    
//    jmethodID method_id = env->GetMethodID(clazz, "getTotalMemory", "()J");
//    jlong result = env->CallLongMethod(activity, method_id);
//    SDL_Log("okay we got a long here, maybe, %i", result);


    jmethodID method_id = env->GetMethodID(clazz, "getHasEnoughMeshMemory", "()Z");
    jboolean result = env->CallBooleanMethod(activity, method_id);
    //SDL_Log("okay we got a bool here, maybe, %i", result);




    // clean up the local references.
    env->DeleteLocalRef(activity);
    env->DeleteLocalRef(clazz);

    //return true;

    return (bool)result;
}


void getImagePathFromMainThread(){

    SDL_Log("Our process img call from main is firing a lot"); // useful to see that animations DO stop...

    queued_img_proc_events -= 1;

    if( my_android_fileopen_timer_id == -1 ){
        return; // already got an image and canceled timer....
    }

    SDL_Log("We are looking to see if we should load an image..."); // useful to see that animations DO stop...


    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject activity = (jobject)SDL_AndroidGetActivity();
    jclass clazz(env->GetObjectClass(activity));

    jmethodID method_id = env->GetStaticMethodID(clazz, "checkForImageSelectorCompleted", "()Ljava/lang/String;");

    jstring path = (jstring)env->CallStaticObjectMethod(clazz, method_id);

    // if we get null -> we are still waiting for a result.. maybe set another timer?
    // if we get "" empty string - they either canceled the selector or it didn't retrurn a path with length... (no permissions)
    // otherwise we got a path presumably...

    SDL_RemoveTimer(my_android_fileopen_timer_id);
    my_android_fileopen_timer_id = -1;

    if( path != nullptr ){


        const char *cpath = env->GetStringUTFChars(path, NULL);
        SDL_Log("We got the path `%s`" , cpath);

        if( SDL_strlen(cpath) > 0 ){
            openglContext->imageWasSelectedCb(openglContext->textures->LoadSurface(cpath), true);
        }

        env->ReleaseStringUTFChars(path, cpath);
    }else{
        my_android_fileopen_timer_id = SDL_AddTimer(1000, my_test_if_image_selected, nullptr);
    }

    // clean up the local references.
    env->DeleteLocalRef(path);   /// IS ATHIS REDUNDANT????
    env->DeleteLocalRef(activity);
    env->DeleteLocalRef(clazz);
}

void beginImageSelector()
{
    //[myFc selectPhoto];


    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject activity = (jobject)SDL_AndroidGetActivity();
    jclass clazz(env->GetObjectClass(activity));
    jmethodID method_id = env->GetMethodID(clazz, "beginImageSelector", "()V");
    env->CallVoidMethod(activity, method_id);

    // clean up the local references.
    env->DeleteLocalRef(activity);
    env->DeleteLocalRef(clazz);

    // Warning (and discussion of implementation details of SDL for Android):
    // Local references are automatically deleted if a native function called
    // from Java side returns. For SDL this native function is main() itself.
    // Therefore references need to be manually deleted because otherwise the
    // references will first be cleaned if main() returns (application exit).

    my_android_fileopen_timer_id = SDL_AddTimer(1000, my_test_if_image_selected, nullptr);


}

bool openURL(char* &url)
{
    bool success = false;

    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject activity = (jobject)SDL_AndroidGetActivity();
    jclass clazz(env->GetObjectClass(activity));
    jmethodID method_id = env->GetMethodID(clazz, "openWebsiteUrl", "(Ljava/lang/String;)V");

    jstring jstrUrl = env->NewStringUTF(url);
    env->CallVoidMethod(activity, method_id, jstrUrl);

    env->DeleteLocalRef(activity);
    env->DeleteLocalRef(clazz);
    env->DeleteLocalRef(jstrUrl);

    success = true;

    return success;
}

void requestReview(){
    
}

/*
 "JNI tips  |  Android NDK  |  Android Developers"
https://developer.android.com/training/articles/perf-jni#primitive-arrays
 To see if two references refer to the same object, you must use the IsSameObject function.
 Never compare references with == in native code.
 */



// This example requires C++ and a custom Java method named "void showHome()"

// Calls the void showHome() method of the Java instance of the activity.
//void showHome(void)
//{
//    // retrieve the JNI environment.
//    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
//
//    // retrieve the Java instance of the SDLActivity
//    jobject activity = (jobject)SDL_AndroidGetActivity();
//
//    // find the Java class of the activity. It should be SDLActivity or a subclass of it.
//    jclass clazz(env->GetObjectClass(activity));
//
//    // find the identifier of the method to call
//    jmethodID method_id = env->GetMethodID(clazz, "showHome", "()V");
//
//    // effectively call the Java method
//    env->CallVoidMethod(activity, method_id);
//
//    // clean up the local references.
//    env->DeleteLocalRef(activity);
//    env->DeleteLocalRef(clazz);
//
//    // Warning (and discussion of implementation details of SDL for Android):
//    // Local references are automatically deleted if a native function called
//    // from Java side returns. For SDL this native function is main() itself.
//    // Therefore references need to be manually deleted because otherwise the
//    // references will first be cleaned if main() returns (application exit).
//}
#endif
