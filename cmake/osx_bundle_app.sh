# Type a script or drag a script file from your workspace to insert its path.

# we made this script to craete a bundle... from our debug or release build...
cd BUILD

rm -fr ColorPickSDL.app
rm -fr ColorPick.app

mkdir -p ColorPickSDL.app/Contents/MacOS
mkdir -p ColorPickSDL.app/Contents/Resources


cp "$1" ColorPickSDL.app/Contents/MacOS/ColorPickSDL

cp -r textures ColorPickSDL.app/Contents/Resources/
cp -r shaders ColorPickSDL.app/Contents/Resources/
cp -r shaders ColorPickSDL.app/Contents/Resources/
cp textures/crosshair.png ColorPickSDL.app/Contents/Resources/
cp textures/crosshair.png .

cp ../Info-macosx.plist ColorPickSDL.app/Contents/Info.plist

echo "APPL????" > ColorPickSDL.app/Contents/PkgInfo
#cp PkgInfo ColorPickSDL.app/Contents/

rm -fr ColorPick.iconset
mkdir ColorPick.iconset

cp ../icons/icon16.png ColorPick.iconset/icon_16x16.png
cp ../icons/icon32.png ColorPick.iconset/icon_16x16@2x.png
cp ../icons/icon32.png ColorPick.iconset/icon_32x32.png
cp ../icons/icon64.png ColorPick.iconset/icon_32x32@2x.png
cp ../icons/icon128.png ColorPick.iconset/icon_128x128.png
cp ../icons/icon256.png ColorPick.iconset/icon_128x128@2x.png
cp ../icons/icon256.png ColorPick.iconset/icon_256x256.png
cp ../icons/icon512.png ColorPick.iconset/icon_256x256@2x.png
cp ../icons/icon512.png ColorPick.iconset/icon_512x512.png
cp ../icons/icon1024.png ColorPick.iconset/icon_512x512@2x.png

iconutil -c icns ColorPick.iconset

mv ColorPick.icns ColorPickSDL.app/Contents/Resources/ColorPick.icns
rm -fr ColorPick.iconset


mv ColorPickSDL.app ColorPick.app

exit 0 # in case of error... ignore it! (ohterwise build failure will be really myserious...)


