#!/bin/sh

# todo: move following to a shared assets script we'll call here?

rm -fr app/src/main/assets

mkdir app/src/main/assets

cp -R ../shaders app/src/main/assets
cp -R ../textures app/src/main/assets

cp -f ../icons/icon72.png app/src/main/res/mipmap-hdpi/ic_launcher.png
cp -f ../icons/icon48.png app/src/main/res/mipmap-mdpi/ic_launcher.png
cp -f ../icons/icon96.png app/src/main/res/mipmap-xhdpi/ic_launcher.png
cp -f ../icons/icon144.png app/src/main/res/mipmap-xxhdpi/ic_launcher.png
cp -f ../icons/icon192.png app/src/main/res/mipmap-xxxhdpi/ic_launcher.png

#./gradlew tasks
./gradlew installDebug
if [[ $? -ge 1 ]]; then
	exit 1
fi

echo "arg $1"

~/Library/Android/sdk/platform-tools/adb shell pm revoke com.vidsbee.colorpicksdl android.permission.READ_EXTERNAL_STORAGE

echo "watching logs"

#we can watch the logs on device..
~/Library/Android/sdk/platform-tools/adb -d logcat | grep "$1"
