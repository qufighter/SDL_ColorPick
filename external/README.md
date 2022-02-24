# Bypass Static Linking

These libraries are only required to static link, using cmake build.

You can alwasy build and install the libraries and link dynamically.

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

If you do use git clones, you can update using the usual commands

```
cd <repo-folder>
git pull origin master
```

# Future

Other external libs may also live here... 
although external_always_static may be more sensible name

```
glm
portable-file-dialogs-master
```

We may integrate those into the build process with
cmake (although since just headers, seems potentially not necessary...)
