#!/bin/sh

echo "WARNING: DO NOT WORK WHILE THIS IS RUNNING - UNTRACKED CHANGES COULD BE LOST OR CHERY PICK MAY NOT WORK!!!!"

echo "WARNING: UNPLUG DEVICE OR IT WILL TRY TO INSTALL THE APK BUILDS TO IT"

exit_if_untracked_changes () {
	git diff-index --quiet HEAD -- || (echo "untracked changes present" && exit 1)
	if [[ $? -ge 1 ]]; then
		echo "bundle error occured WARNING: DO NOT WORK WHILE THIS SCRIPT RUNS - YOU COULD LOOSE WORK OR BREAK BASIC BUILD"
		exit 1
	fi
}
exit_if_untracked_changes

buildCode=`cat app/build.gradle | grep -E "versionCode \d" | grep -Eo "\d+"`
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
	echo "bundle exists, exiting"
	exit 1
fi

ls "app/builds-archive/app"$buildCode"basic.apk"
if [[ $? -eq 0 ]]; then
	echo "bundle exists, exiting"
	exit 1
fi

rm -fr app/src/main/assets
mkdir app/src/main/assets
cp -R ../shaders app/src/main/assets
cp -R ../textures app/src/main/assets

clean_intermediates () {
	# this one is a bit nasty, we need to clear teh buidl folder complete
	rm -fr app/build/generated
	rm -fr app/build/intermediates
	rm -fr app/build/tmp
	echo "intermediates clared"
}

enable_basic_mode () {
	git cherry-pick -n color-pick-basic-mode-enable # (-n is --no-commit).
	if [[ $? -ge 1 ]]; then
		echo "ERROR: chery pick of color-pick-basic-mode-enable failed"
		exit 1
	fi
	./buildIconsBasic.sh
}

disable_basic_mode () {
	git revert -n color-pick-basic-mode-enable
		if [[ $? -ge 1 ]]; then
		echo "ERROR: revert of color-pick-basic-mode-enable failed"
		exit 1
	fi
	git revert --abort
	./buildIcons.sh
}

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
