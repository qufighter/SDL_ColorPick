# Bypass Static Linking

These libraries are only required to static link, using cmake build.

You can always build and install the libraries and link dynamically.

The cmake argument `COLORPICK_DYNAMIC_LINK` controls this.

By default this is only enabled on Linux platforms...

# External Libraries

to build static library version of SDL Color Pick using cmake, 
we require 2 libraries in this directory.

The best way to get them MIGHT be to clone from source...

The libs should be in the following named folders:

 * SDL2
 * SDL2_image

A quick and easy way to get the ABSOLTUE LATEST is:

```
cd external
git clone git@github.com:libsdl-org/SDL_image.git SDL2_image
git clone git@github.com:libsdl-org/SDL.git SDL2
```

In practice it may be better to choose an actual release, YMMV.

```
cd <repo-folder>
git tag # shows list of releases
git checkout release-2.0.5 # or whatever release...
git branch # show list of branches
git checkout main # return to main branch
```

If you do use git clones, you can update using the usual commands

```
cd <repo-folder>
git checkout main # see above to select correct branch name...
git pull origin main # branch name should match
git fetch -a # should get latest tags
```

# Other folder: external_static

Other external libs live nearby... 
see the external_static folder

```
glm
portable-file-dialogs
```

We may integrate those into the build process with cmake instead...

# Linux platform COLORPICK_DYNAMIC_LINK requirements...

Specific library names may vary by distribution... generally:

```
sudo apt-get update
apt list | grep sdl | grep dev
apt list | grep gtk | grep mm
```

If you don't see results you are missing the required packages, select the correct packages:
```
apt-cache search sdl | grep 2 | grep dev
apt-cache search gtk | grep mm | grep dev
# once you identify the correct names for the 3 packages needed...
sudo apt-get install <package1> <package2> <pacakge3>
```

## Packages

In summary SDL2 dev, SDL2 image dev, and gtkmm-3.0 dev are the 3 main requirements...

XCB is also currently required although generally already available... if not install it!

XCB was chosen with hopes of backwards compatibility in the future of X... see COLORPICK_USE_XCB

We only use gtk for an invisible window (shield to intercept movement and click)... see COLORPICK_X11_GTK as a port for alternatives is planned!

