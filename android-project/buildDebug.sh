#!/bin/sh

source buildUtils.sh
install_assets
#clean_intermediates

# adds the test textures to the bundle...
# install_test_assets

echo "Please run the correct buildIcons script for your build before continuing!!!!"
# ./buildIcons.sh
# TODO: need a way to copy the BASIC icons instead....

#./gradlew tasks
./gradlew installDebug
if [[ $? -ge 1 ]]; then
	exit 1
fi

echo "arg $1"

~/Library/Android/sdk/platform-tools/adb shell pm revoke com.vidsbee.colorpicksdl android.permission.READ_EXTERNAL_STORAGE
~/Library/Android/sdk/platform-tools/adb shell pm revoke com.vidsbee.colorpickbasicsdl android.permission.READ_EXTERNAL_STORAGE

echo "watching logs"

#we can watch the logs on device..
# removed adb -d (usb only)
~/Library/Android/sdk/platform-tools/adb logcat | grep "$1"
