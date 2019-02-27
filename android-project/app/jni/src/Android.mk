LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

# the following symlinks exist...
SDL_PATH := ../SDL
SDL_IMG_PATH := ../SDL_Image

# NOTE: the above symlinks - when you modify them - you should delete the app/build/intermediates/
#     rm -fr app/build/intermediates
# so the above is the last step... first... check your symlinks:
#     ls -al app/jni
# to update the symlink, just remove the old one fist, eg:
#     rm app/jni/SDL_Image
#     ln -s /full/path/to/lib/SDL2_image-2.0.3 app/jni/SDL_Image
# then, you guessed it... check your symlinks:
#     ls -al app/jni
# if you do not remove the old one then a new file is created inside of the old link instead
# and to reiterate, finally:
#     rm -fr app/build/intermediates

# BY THE WAY: you should also update the following files if you are updating SDL...
# SDL.java
# SDLActivity.java
# etc...


LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/$(SDL_IMG_PATH)/

# Add your application source files here...
# LOCAL_SRC_FILES := \
#	../SDL/test/testgles.c

LOCAL_SRC_FILES := \
	../../../../main.cpp \
	../../../../ColorPickState.cpp \
	../../../../ColorPick.cpp \
	../../../../ux.cpp \
	../../../../textures.cpp \
	../../../../shader.cpp \
    ../../../../meshes.cpp \
	../../../../Platform/Android/FileChooser.cpp



LOCAL_SHARED_LIBRARIES := \
	SDL2 \
	SDL2_image

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
