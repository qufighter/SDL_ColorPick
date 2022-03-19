# Bypass Static Linking

These libraries are only required to static link, using cmake build.

You can always build and install the libraries and link dynamically.

A planned argument to dynamic link will be:

`COLORPICK_DYNAMIC_LINK`

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
git pull origin master
```

# Other folder: external_static

Other external libs live nearby... 
see the external_static folder

```
glm
portable-file-dialogs
```

We may integrate those into the build process with cmake instead...
