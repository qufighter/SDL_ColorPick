cmake_minimum_required(VERSION 3.7)
project(ColorPickWinClr VERSION 1.00 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 11)

# UNIX WIN32 APPLE CYGWIN MSVC MSVC80 MSVC_IDE iOS  CMAKE_SYSTEM_NAME
# CMAKE_CL_64 #Using the 64 bit compiler from Microsoft


set( CMAKE_DOTNET_TARGET_FRAMEWORK "4.0" )

message(STATUS " EXTRA_SRC_FILES:    ${EXTRA_SRC_FILES}")

FILE(GLOB EXTRA_HEADER_FILES "*.h")


#add_executable(colorpick_main_win_clr ColorPickWinClr.cpp)

add_library(colorpick_main_win_clr SHARED ColorPickWinClr.cpp  ${EXTRA_HEADER_FILES})

include_directories(.)

if(WIN32)
    # target_compile_options(colorpick_main_win_clr PRIVATE /clr)
    # target_compile_options(colorpick_main_win_clr PRIVATE /fp:precise)
    # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /clr")  
    # target_compile_options(colorpick_main_win_clr PRIVATE "/FU${asmPath}")

	# RC_INVOKED  WEBP_HAVE_JUST_SDL_H 1, 1
    set_target_properties(colorpick_main_win_clr PROPERTIES COMPILE_FLAGS "/clr /EHa")

    set_target_properties(colorpick_main_win_clr PROPERTIES COMMON_LANGUAGE_RUNTIME "")

    set_property(TARGET colorpick_main_win_clr PROPERTY VS_GLOBAL_ROOTNAMESPACE ${project_name})

    set_property(TARGET colorpick_main_win_clr PROPERTY VS_GLOBAL_KEYWORD "ManagedCProj")

    set_property(TARGET colorpick_main_win_clr PROPERTY VS_GLOBAL_CLRSupport "true")

    set_property(TARGET colorpick_main_win_clr PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.0")

    set_property(TARGET colorpick_main_win_clr PROPERTY VS_DOTNET_REFERENCES "System;System.Windows;System.Windows.Forms;System.Drawing;System.Net")

endif()

if(WIN32)

 #seems to be copied automatically per target_link_libraries in the main (parent) cmakelists.txt HOWEVER running from VS and attempting to reach any DLL code will not work....

 # added back anyway since when only the dll is rebuilt, copy seems to NOT occur... we don't need all these files though and release is not considered here...
     add_custom_command(TARGET colorpick_main_win_clr POST_BUILD
         COMMAND  ${CMAKE_COMMAND} -E copy_directory Debug ../../../Debug/
         WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
         COMMENT "copying in colorpick_main_win_clr.dll library..."
		 MAIN_DEPENDENCY Debug/colorpick_main_win_clr.dll
     )

endif()
