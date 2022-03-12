https://vidsbee.com/ColorPick/Mobile/

This will also complile into a:

~100% working Mac OS app
~100% wokring Linux app (for gtkmm-3.0 so far)
~95% working/stable windows app (missking key screen feature)
~IOS
~Android
~Emscripten
... more?

Before you contriubte, I suggest looking for forks of the project, to see if anyone else has move on with the work since I've left off.


Check the readme in the "external" fodler though for instrucitons ot staic link SDL and SDL Image libraries.

The windows visual studio project can be generated from CMAKE!  Have tested the llvm build too which is probably more stable (better threads and no cashes from using the worng free function, even though the one key feature won't work there it's not quite done yet form VS either, easy fix probably)

The mac OS project is hte most up to date and is the most used development project.

Just read CMakeLists.txt for details...

Future work: probably won't be done
Future publishing: probably won't be done

TODO: bundling app bundles from cmake isn't working just yet, maybe soemeone can figure it out.  See add_subdirectory( cmake_install ) for details.

I do have a script to bundle up the osx app...

Just wanted to try to get this out there just in case!  You never know and I'm not getting the installs to ever reach those goals.

see todo for some work...

more TODO that are more on the top of mind:
1) test if clone from here works (really can't say)
2) move the 2 SDL library folders permanently into correct paths inside external folder
3) other bookkeeping and organizing (a new directory external_static is planned)(wip)

Hopefully someone in the future finds hte project usefulf or personal use or decides to coontinue development/code cleanup/etc!,
much love (and etc)

Sam

PS:

see also, the chrome extension src:
https://github.com/qufighter/ColorPick
