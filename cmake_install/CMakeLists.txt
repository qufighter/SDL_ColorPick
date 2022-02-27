cmake_minimum_required(VERSION 3.7)
project(ColorPickInstall VERSION 1.00 LANGUAGES CXX)

# https://cmake.org/pipermail/cmake/2019-February/069023.html

message(STATUS " CMAKE_BINARY_DIR:   ${CMAKE_BINARY_DIR}")
message(STATUS " MAIN_APP:   ${MAIN_APP}")
message(STATUS " LIBS_PATH:   ${LIBS_PATH}")
message(STATUS " DIRS_PATH:   ${DIRS_PATH}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#exclude from bundle as follows?
# add_subdirectory(3rdparty/googletest/ EXCLUDE_FROM_ALL)

install(CODE "\

    include(BundleUtilities)

    # set(gp_tool \"${GP_TOOL}\")

    fixup_bundle(\"\${MAIN_APP}\" \"${LIBS_PATH}\" \"${DIRS_PATH}\")

" COMPONENT Runtime)
