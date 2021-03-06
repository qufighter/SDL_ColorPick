#!/bin/sh

# WARNING: this deletes "app/src/main/java/org/libsdl/app/" so if you have any non standard
#          files there please move them to your own directory before using this script.

# USAGE:
# we expect 2 args, first the LIB name, which should be the SYMLINK name... which should already exist
# second the FULL PATH to the new lib version, which should exist

# this documents the old manual steps
# ALSO: don't forget to update the .java files - these can change in subsequent releases and sometimes fail silently.... or loudly as the case may be
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

    rm -fr $existingJavaFilesPath

    echo "=== existing files after rm... ===="
    ls $existingJavaFilesPath

    cp -r $newJavaFilesPath $existingJavaFilesPath

    echo "=== existing files after copy... ===="
    ls $existingJavaFilesPath

    yourProjectFilePath="app/src/main/AndroidManifest.xml"
    theirProjectFilePath=$newLibPath"/android-project/"$yourProjectFilePath
    echo "\n\nFINALLY => please compare the "$yourProjectFilePath" and merge any relevant changes\n\n < YOURS "$yourProjectFilePath" \n > THEIRS "$theirProjectFilePath"\n\n"
    diff $yourProjectFilePath $theirProjectFilePath

    yourProjectFilePath="build.gradle"
    theirProjectFilePath=$newLibPath"/android-project/"$yourProjectFilePath
    echo "\n\nFINALLY => please compare the "$yourProjectFilePath" and merge any relevant changes\n\n < YOURS "$yourProjectFilePath" \n > THEIRS "$theirProjectFilePath"\n\n"
    diff $yourProjectFilePath $theirProjectFilePath

    yourProjectFilePath="app/build.gradle"
    theirProjectFilePath=$newLibPath"/android-project/"$yourProjectFilePath
    echo "\n\nFINALLY => please compare the "$yourProjectFilePath" and merge any relevant changes\n\n < YOURS "$yourProjectFilePath" \n > THEIRS "$theirProjectFilePath"\n\n"
    diff $yourProjectFilePath $theirProjectFilePath

    yourProjectFilePath="app/jni/Application.mk"
    theirProjectFilePath=$newLibPath"/android-project/"$yourProjectFilePath
    echo "\n\nFINALLY => please compare the "$yourProjectFilePath" and merge any relevant changes\n\n < YOURS "$yourProjectFilePath" \n > THEIRS "$theirProjectFilePath"\n\n"
    diff $yourProjectFilePath $theirProjectFilePath

    echo "\n\nPlease examine all the DIFFs above and merge any necessary changes manually.  Some of these changes may be ctritical for certain functionalities.  The files diff'd above may not be exhaustive."

    echo "\n\nNOTE: please keep in mind your NDK bundle version has a big effect on how compliation will go and what devices are supported... when you upgrade this I suggest making a backup of the old one just in case!  Its easy to upgrade using Android Studio but it won't make backups for you.  IME support for older devices may be dropped in newer versions but you also get the latest patches."

fi


