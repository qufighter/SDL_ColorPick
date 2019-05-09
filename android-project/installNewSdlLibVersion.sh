#!/bin/sh

# USAGE:
# we expect 2 args, first the LIB name, which should be the SYMLINK name... which should already exist
# second the FULL PATH to the new lib version, which should exist

# ALSO: don't forget to update the .java files - these can change in subsequent releases and sometimes fail silently.... or loudly as the condition dictates
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


echo "===== links before chagnes ====="
# list links
ls -al app/jni | grep -e ^l

linkName="app/jni/"$1

echo "upgrading "$linkName

test -h $linkName
if [[ $? -ge 1 ]]; then
    echo "First argument LIB NAME is not found in "$linkName" ... we cannot upgrade a lib that is non existent"
    exit 1
fi

newLibPath=$2

echo "to point at >"$newLibPath"<"

test -n ""$newLibPath
if [[ $? -ge 1 ]]; then
    echo "Second argument NEW LIB PATH is not found or non directory... "$newLibPath" ..."
    exit 1
fi

test -d $newLibPath
if [[ $? -ge 1 ]]; then
    echo "Second argument NEW LIB PATH is not found or non directory... "$newLibPath" ..."
    exit 1
fi

rm -fr app/build/intermediates

rm -fr ""$linkName

echo "===== cleared old link ====="
# list links
ls -al app/jni | grep -e ^l

ln -s ""$newLibPath ""$linkName

echo "===== created new link ====="
# list links
ls -al app/jni | grep -e ^l


test $1 == "SDL"
if [[ $? -eq 0 ]]; then
    echo "We are updating SDL< lets copy the JAVA files too!!!!"

    newJavaFilesPath=$newLibPath"/android-project/app/src/main/java/org/libsdl/app/"

    test -d $newJavaFilesPath
    if [[ $? -ge 1 ]]; then
        echo "newJavaFilesPath is not right... ... "$newJavaFilesPath" ..."
        exit 1
    fi
    echo "=== new files ===="
    ls $newJavaFilesPath

    existingJavaFilesPath="app/src/main/java/org/libsdl/app/"

    test -d $existingJavaFilesPath
    if [[ $? -ge 1 ]]; then
        echo "existingJavaFilesPath is not right... ... "$existingJavaFilesPath" ..."
        exit 1
    fi
    echo "=== existing files ===="
    ls $existingJavaFilesPath

    pwd

    rm -fr `pwd`"/"$existingJavaFilesPath

    echo "=== existing files after rm... ===="
    ls $existingJavaFilesPath

    cp -r $newJavaFilesPath $existingJavaFilesPath

    echo "=== existing files after copy... ===="
    ls $existingJavaFilesPath

fi
