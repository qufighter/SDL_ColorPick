#!/bin/sh

echo "WARNING: DO NOT WORK WHILE THIS IS RUNNING - UNTRACKED CHANGES COULD BE LOST OR CHERY PICK MAY NOT WORK!!!!"

echo "WARNING: UNPLUG DEVICE OR IT WILL TRY TO INSTALL THE APK BUILDS TO IT"

source buildUtils.sh

exit_if_untracked_changes () {
	git diff-index --quiet HEAD -- || (echo "untracked changes present" && exit 1)
	if [[ $? -ge 1 ]]; then
		echo "above bundle error occured; WARNING: DO NOT WORK WHILE THIS SCRIPT RUNS - YOU COULD LOOSE WORK OR BREAK BASIC BUILD"
		exit 1
	fi
}
exit_if_untracked_changes

buildCode=`cat app/build.gradle | grep -Eo "versionCode \d+" | grep -Eo "\d+"`
echo "Now buildilng "$buildCode

ls "app/builds-archive/app"$buildCode"full.aab"
if [[ $? -eq 0 ]]; then
	echo "bundle exists, exiting"
	exit 1
fi

ls "app/builds-archive/app"$buildCode"basic.aab"
if [[ $? -eq 0 ]]; then
	echo "bundle exists, exiting"
	exit 1
fi

ls "app/builds-archive/app"$buildCode"full.apk"
if [[ $? -eq 0 ]]; then
	echo "apk exists, exiting"
	exit 1
fi

ls "app/builds-archive/app"$buildCode"basic.apk"
if [[ $? -eq 0 ]]; then
	echo "apk exists, exiting"
	exit 1
fi

build_bundle () {
	./gradlew bundleRelease
	if [[ $? -ge 1 ]]; then
		echo "bundle error occured"
		exit 1
	fi
	echo "finally, check in app/build/outputs/bundle/release"
}

build_apk () {
	./gradlew installRelease
	echo "finally, check in app/build/outputs/apk/release"
}

install_assets

clean_intermediates

build_bundle
mv app/build/outputs/bundle/release/app.aab "app/builds-archive/app"$buildCode"full.aab"
build_apk
mv app/build/outputs/apk/release/app-release.apk "app/builds-archive/app"$buildCode"full.apk"

enable_basic_mode
build_bundle
mv app/build/outputs/bundle/release/app.aab "app/builds-archive/app"$buildCode"basic.aab"
build_apk
mv app/build/outputs/apk/release/app-release.apk "app/builds-archive/app"$buildCode"basic.apk"
disable_basic_mode


echo "

Ignore Above Device Error

All results should be in the app/builds-archive for release "$buildCode"

"

ls app/builds-archive | grep $buildCode

#install to device?
# adb -d install path/to/your_app.apk
