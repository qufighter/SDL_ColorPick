cmake_minimum_required(VERSION 3.7)
project(ColorPick VERSION 1.00 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 11)

#steps are somewhat like this
# 1) mkdir BUILD
# 2) cd BUILD
# 3) cmake .. #OR: cmake .. -D COLORPICK_DYNAMIC_LINK=1 -D CMAKE_BUILD_TYPE=Release
# 3) make VERBOSE=1
# of course, a better way might be to use the CMAKE gui to configure everything (set directories, click configure, click geneerate, then run make --VERBOSE from teh build folder...
# 4) ./colorpick_main (or run the tests...)

#known issue: enabling SUPPORT_WEBP seems to cause problems bundling GLUT on OSX, still working out the details...
#make[2]: *** No rule to make target `/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.15.sdk/System/Library/Frameworks/GLUT.framework/GLUT', needed by `external/SDL2_Image/external/libwebp-1.0.3/vwebp'.  Stop.
#make[1]: *** [external/SDL2_Image/external/libwebp-1.0.3/CMakeFiles/vwebp.dir/all] Error 2
# more specific work around may be to disable WEBP_BUILD_VWEBP
set(SUPPORT_WEBP)
unset(WEBP_BUILD_VWEBP)

#Be aware that this cmake script can fail if the path to project contains spaces (at least on Mac OS). Other than that, indeed it is an easy way to statically link to SDL2. –

# UNIX WIN32 APPLE CYGWIN MSVC MSVC80 MSVC_IDE iOS  CMAKE_SYSTEM_NAME
# CMAKE_CL_64 #Using the 64 bit compiler from Microsoft

# !!! FIXME: this should probably do "MACOSX_RPATH ON" as a target property
# !!! FIXME:  for the SDL2 shared library (so you get an
# !!! FIXME:  install_name ("soname") of "@rpath/libSDL-whatever.dylib"
# !!! FIXME:  instead of "/usr/local/lib/libSDL-whatever.dylib"), but I'm
# !!! FIXME:  punting for now and leaving the existing behavior. Until this
# !!! FIXME:  properly resolved, this line silences a warning in CMake 3.0+.
# !!! FIXME:  remove it and this comment entirely once the problem is
# !!! FIXME:  properly resolved.
#cmake_policy(SET CMP0042 OLD)

set( CMAKE_OSX_DEPLOYMENT_TARGET "10.10" )
# TODO: consider mulitple ARCH /abi build...

set(CMAKE_OSX_ARCHITECTURES "i386;x86_64;arm64" CACHE STRING "")
#set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD)" CACHE STRING "") # xcode expanded

# todo: target correct OSX sdk... cursor strangeness noted
# signing


if(NOT APPLE AND UNIX AND NOT DEFINED COLORPICK_DYNAMIC_LINK)
    set( COLORPICK_DYNAMIC_LINK "1" )
    if( NOT DEFINED CMAKE_BUILD_TYPE )
        set( CMAKE_BUILD_TYPE "Release" )
    endif()
endif()

file(REMOVE BUILD/colorpick_main)

file(COPY textures DESTINATION .)
file(COPY shaders DESTINATION .)
#file(COPY textures/crosshair.png DESTINATION .)

if(WIN32)
	file(COPY textures DESTINATION ./${CMAKE_BUILD_TYPE})
	file(COPY shaders DESTINATION ./${CMAKE_BUILD_TYPE})
	#file(COPY textures/crosshair.png DESTINATION ./${CMAKE_BUILD_TYPE})

	file(COPY textures DESTINATION ./Debug)
	file(COPY shaders DESTINATION ./Debug)
	#file(COPY textures/crosshair.png DESTINATION ./Debug)

endif()

find_package(OpenGL REQUIRED)
#find_package(GLUT) # we don't need vwebp...


message(STATUS " OPENGL_INCLUDE_DIR:  ${OPENGL_INCLUDE_DIR}")
message(STATUS " OPENGL_LIBRARIES:    ${OPENGL_LIBRARIES}")

include_directories(${OPENGL_INCLUDE_DIR})

# if(GLUT_FOUND)
# message(STATUS " GLUT_INCLUDE_DIR:   ${GLUT_INCLUDE_DIR}")
# message(STATUS " GLUT_LIBRARIES:     ${GLUT_LIBRARIES}")
# include_directories(${GLUT_INCLUDE_DIR})
# endif()


if(COLORPICK_DYNAMIC_LINK)

    if(COLORPICK_KDE_QT)
        add_compile_definitions(COLORPICK_X11_QT=1)
        add_compile_definitions(COLORPICK_USE_XCB=1)
    else()
        # This creates the C++ defines needed presently for compilation on linux
        add_compile_definitions(COLORPICK_X11_GTK=1)
        add_compile_definitions(COLORPICK_USE_XCB=1)
    endif()

    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
    find_package(XCB)

    if(XCB_FOUND)
    message(STATUS " XCB_INCLUDE_DIR:    ${XCB_INCLUDE_DIR}")
    message(STATUS " XCB_LIBRARIES:     ${XCB_LIBRARIES}")
    include_directories(${XCB_INCLUDE_DIR})
    endif()


    if(COLORPICK_KDE_QT)
        message(STATUS " IS COLORPICK_KDE_QT (not gtk, not gnome)")

        set(QT_MIN_VERSION "6.6.0")
        #set(KF_MIN_VERSION "5.240.0")
        find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)

        find_package(ECM REQUIRED NO_MODULE)
        set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

        # set up standard kde build settings

        include(KDEInstallDirs)
        include(KDECMakeSettings)
        include(KDECompilerSettings NO_POLICY_SCOPE)

        message(STATUS " IS COLORPICK_KDE_QT (not gtk, not gnome)")

        # omiting ${QT_MIN_VERSION} as it not doing MIN match
        find_package(
            Qt6 CONFIG REQUIRED
                Core
                Concurrent
                #Qml
                Quick
                #QuickControls2
                #QuickTemplates2
                Widgets
                #DBus
                #PrintSupport
                #Test
                #WaylandClient
                #Multimedia
        )
        # qt_policy(SET QTP0001 NEW)

        pkg_check_modules(Qt6 REQUIRED)

        message(STATUS " QT_INCLUDE_DIRS:    ${QT_INCLUDE_DIRS}")
        message(STATUS " QT_LIBRARIES:     ${QT_LIBRARIES}")


    else()
        message(STATUS " NOT COLORPICK_KDE_QT (GTKMM Gnome)")
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
        if(GTKMM_FOUND)
        message(STATUS " GTKMM_INCLUDE_DIRS:    ${GTKMM_INCLUDE_DIRS}")
        message(STATUS " GTKMM_LIBRARIES:     ${GTKMM_LIBRARIES}")
        include_directories(${GTKMM_INCLUDE_DIRS})
        else()
        message(STATUS " PROBMEM: no GTKMM_LIBRARIES:     ${GTKMM_LIBRARIES}")
        endif()
    endif()


    message(STATUS " COLORPICK_DYNAMIC_LINK was detected!")
    set( SDL_STATIC OFF CACHE BOOL "" FORCE )
    set( SDL_SHARED ON CACHE BOOL "" FORCE )
    set( BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE )

    #TODO: configure to use shared library? COLORPICK_DYNAMIC_LINK

    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)

	#todo use system library directories instead..
   # include_directories(SDL/library SDLImage/library)
    include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
    
else()
    message(STATUS " COLORPICK_DYNAMIC_LINK disabled STATIC LINK!")

    set( SDL_STATIC ON CACHE BOOL "" FORCE )
    set( SDL_SHARED OFF CACHE BOOL "" FORCE )
    set( BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE )
    add_subdirectory( external/SDL2 )
    add_subdirectory( external/SDL2_image )

    if(WIN32)
        add_subdirectory( external/glew/build/cmake )
        message(STATUS " GLEW_LIBRARIES ${GLEW_LIBRARIES}")
        message(STATUS " GLEW_LIB_NAME ${GLEW_LIB_NAME}")

		add_subdirectory( Platform/WIN/COLORPICK_WIN_CLR )


    endif()
endif()


# especially nice for windows visual studio project generatoin...
FILE(GLOB MY_HEADERS "*.h")
#FILE(GLOB_RECURSE MY_HEADERS "*.h") # could lload to another list and append??

include_directories(.)
include_directories(minigames)

FILE(GLOB EXTRA_MINIGAME_H_FILES "minigames/**/*.h")
FILE(GLOB EXTRA_MINIGAME_DIRECTORIES "minigames/**/")

message(STATUS " EXTRA_MINIGAME_H_FILES ${EXTRA_MINIGAME_H_FILES}")
message(STATUS " EXTRA_MINIGAME_DIRECTORIES (fixme... has non direcotires...) ${EXTRA_MINIGAME_DIRECTORIES}")

include_directories(${EXTRA_MINIGAME_H_FILES})


if(APPLE)
    include_directories(Platform/OSX)

    list(APPEND EXTRA_SRC_FILES
      Platform/OSX/FileChooser.mm
      Platform/OSX/ColorPickOsx.mm
      Platform/OSX/NSOverlayView.mm
      Platform/OSX/NSOverlayWindow.mm
    )

	FILE(GLOB EXTRA_HEADER_FILES "Platform/OSX/*.h")

elseif(UNIX)
    include_directories(Platform/LINUX)
    
    list(APPEND EXTRA_SRC_FILES
      Platform/LINUX/FileChooser.cpp
    )
	FILE(GLOB EXTRA_HEADER_FILES "Platform/LINUX/*.h")


elseif(WIN32)
    include_directories(Platform/WIN)

    list(APPEND EXTRA_SRC_FILES
      Platform/WIN/FileChooser.cpp
    )
	FILE(GLOB EXTRA_HEADER_FILES "Platform/WIN/*.h")
endif()

message(STATUS " EXTRA_SRC_FILES:    ${EXTRA_SRC_FILES}")

add_executable(colorpick_main main.cpp
                    ColorPick.cpp
                    ColorPickState.cpp
                    shader.cpp
                    meshes.cpp
                    ux.cpp
                    textures.cpp
                    ${EXTRA_SRC_FILES} ${EXTRA_HEADER_FILES}
					${MY_HEADERS}
					minigames/_minigames.h
					${EXTRA_MINIGAME_H_FILES}
               )




set( SDL_VERSION_STATIC_OR_NOT "SDL2" )


if(COLORPICK_DYNAMIC_LINK)
    if(COLORPICK_KDE_QT)
        target_link_libraries(colorpick_main SDL2main SDL2 SDL2_image ${OPENGL_LIBRARIES} ${XCB_LIBRARIES} Qt6::Widgets Qt6::Quick)
    else()
        target_link_libraries(colorpick_main SDL2main SDL2 SDL2_image ${OPENGL_LIBRARIES} ${XCB_LIBRARIES} ${GTKMM_LIBRARIES})
    endif()
else()
    set( SDL_VERSION_STATIC_OR_NOT "SDL2-static" )

    if(WIN32)
        target_link_libraries(colorpick_main SDL2main SDL2-static SDL2_image ${OPENGL_LIBRARIES} glew_s colorpick_main_win_clr) # pretty strange, since we dynamic link colorpick_main_win_clr, but this ensures the dependency is built... 
        # target_link_libraries(colorpick_main SDL2main SDL2-static SDL2_image ${OPENGL_LIBRARIES} glew_s  )
		#target_link_libraries(colorpick_main PUBLIC  colorpick_main_win_clr)
    else()
        target_link_libraries(colorpick_main SDL2main SDL2-static SDL2_image ${OPENGL_LIBRARIES} )
    endif()
endif()

# tests...
add_executable(         colorpick_test_dirscan test/directionalScanTest.cpp)
target_link_libraries(  colorpick_test_dirscan SDL2main ${SDL_VERSION_STATIC_OR_NOT})

add_executable(         colorpick_test_uilist test/uiListTest.cpp)
target_link_libraries(  colorpick_test_uilist SDL2main ${SDL_VERSION_STATIC_OR_NOT})



if(APPLE) #  AND NOT APPLE) #temporary BYPASS, testing cmake_install feature (not working?)

    add_custom_command(TARGET colorpick_main POST_BUILD
        COMMAND ./cmake/osx_bundle_app.sh colorpick_main
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Bundling into app bundle..."
    )

endif()


if(WIN32)

	# set(LIBS_PATH colorpick_main_win_clr) # strange effect, copying the debug dll is a broken debugging experience......

endif()


set(MAIN_APP colorpick_main)
set(DIRS_PATH "textures;shaders")

add_subdirectory( cmake_install )
