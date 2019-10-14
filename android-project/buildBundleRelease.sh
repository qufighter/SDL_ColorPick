#!/bin/sh

source buildUtils.sh
install_assets
#clean_intermediates

#./gradlew tasks
#./gradlew build
./gradlew bundleRelease
if [[ $? -ge 1 ]]; then
	exit 1
fi

echo "finally, check in app/build/outputs/ -- really don't use this script, use buildAllRelease.sh"

# to build a bundle instead.. try ./gradlew bundleRelease

#install to device?
# adb -d install path/to/your_app.apk
