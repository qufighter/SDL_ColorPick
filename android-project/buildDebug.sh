#!/bin/sh

# this is not necessary...
#cd app
#~/Library/Android/sdk/ndk-bundle/ndk-build
#cd ..

rm -fr app/src/main/assets

mkdir app/src/main/assets

cp -R ../shaders app/src/main/assets
cp -R ../textures app/src/main/assets

#./gradlew tasks
./gradlew installDebug

if [[ $? -ge 1 ]]; then
	exit 1
fi

echo "arg $1"

#we can watch the logs on device..
~/Library/Android/sdk/platform-tools/adb -d logcat | grep "$1"
