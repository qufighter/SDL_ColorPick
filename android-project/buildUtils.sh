#!/bin/sh

exit_if_untracked_changes () {
	git diff-index --quiet HEAD -- || (echo "untracked changes present" && exit 1)
	if [[ $? -ge 1 ]]; then
		echo "above bundle error occured; WARNING: DO NOT WORK WHILE THIS SCRIPT RUNS - YOU COULD LOOSE WORK OR BREAK BASIC BUILD"
		exit 1
	fi
}

install_assets () {
	rm -fr app/src/main/assets
	mkdir app/src/main/assets
	cp -R ../shaders app/src/main/assets
	cp -R ../textures app/src/main/assets
	echo "assets replaced"
}

install_test_assets () {
	cp -R ../test_textures app/src/main/assets/textures
	echo "test assets added"
}

clean_intermediates () {
	# this one is a bit nasty, we need to clear the build folder complete
	rm -fr app/build/generated
	rm -fr app/build/intermediates
	rm -fr app/build/tmp
	echo "intermediates clared"
}

# LESS GENERIC FUNCTIONS.....

revoke_app_permissions () {
	~/Library/Android/sdk/platform-tools/adb shell pm revoke com.vidsbee.colorpicksdl android.permission.READ_EXTERNAL_STORAGE
	~/Library/Android/sdk/platform-tools/adb shell pm revoke com.vidsbee.colorpickbasicsdl android.permission.READ_EXTERNAL_STORAGE
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
