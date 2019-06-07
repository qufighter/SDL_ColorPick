#!/bin/sh

# this one is a bit nasty, we need to clear teh buidl folder complete;y or the bundle will have duplicate symbols?
rm -fr app/build/generated
rm -fr app/build/intermediates
rm -fr app/build/tmp

rm -fr app/src/main/assets

mkdir app/src/main/assets

cp -R ../shaders app/src/main/assets
cp -R ../textures app/src/main/assets

#./gradlew tasks
#./gradlew build
./gradlew bundleRelease
if [[ $? -ge 1 ]]; then
	exit 1
fi

echo "finally, check in app/build/outputs/"

# to build a bundle instead.. try ./gradlew bundleRelease

#install to device?
# adb -d install path/to/your_app.apk
