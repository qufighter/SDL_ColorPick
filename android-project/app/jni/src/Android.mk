LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

# the following symlinks exist...
SDL_PATH := ../SDL
SDL_IMG_PATH := ../SDL_Image

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
