#!/bin/sh

source buildUtils.sh
install_assets
#clean_intermediates

#./gradlew tasks
#./gradlew installDebug
./gradlew installRelease
if [[ $? -ge 1 ]]; then
	exit 1
fi

echo "arg $1"

revoke_app_permissions

echo "watching logs"

#we can watch the logs on device..
~/Library/Android/sdk/platform-tools/adb -d logcat | grep "$1"
