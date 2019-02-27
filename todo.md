# notes
DEVELOPER_TEST_MODE

# Checklist
   platform version number
   settings displayed version number
   re-enter EACH completed minigame

#  todo

achievements
can only achieve when points enabled? (at least for points based achievements...)
   - huzzah (3rd button) meh achievement?  training achievement?  need ponts for clicking button though...,
   - 13 points from color 5d5a4b - its improved.... but not an achievement
   - the +1 "no scope"  mathmagical (color exact found without scan?) < TODO: show > < or some other char based message, see generalUx->defaultScoreDisplay->displayExplanation(" [ ] ");

when points disabled, they still accumulate and yuou can still loose yer points.... (is this a bug?)
   - add lock points feature.... ?

ios - loading image progress bar ?  no refresh? < - refresh is done (todo:test)
   - android loading image has not progress bar either... blank screen.... maybe ok for now...

hide announcmenet texts (achivements ?) option - again need to be careful here...explanations will still show
slow announcments texts option

REALLY reset the chain multiplier (the 2 times it gets reset accidentally to 1, and should be set to 0)

// some random thing ? we need to roll above though.

pigcel

drag scroller edge bg beyond end/start is actually annoying

KEYBOARD INPUT NOT sent to MINIGAME!!!
multigesture input not sent to minigame
mouse wheel/pinch not intercepted during minigame

edge rounding tuned for circles, make dynamic

disable 3d anim (when modal present?) or and optionally ? (no?)

distance based movement time for moveTo used during games (even when its really close, it still takes 1 second to move...) ?

update icons

verify logging in release mode

## bugs
osx/ios jpg rotation not respected (is this exif rotation???)
images can have a color space????
3up pallete drag it down where it should crop - observe artifacts (seems to be the position reset delete button... of the valid items, this is likely a crop logic skipped in shader?)

##  can do

convert or crop cubic coord

more tests for uiList needed...
1) remove the last item then try to locate it and other items


additional sorts (dark->light->dark) (dull -> vivid -> dull)

the adjustments histogram/curves/gamma correction/etc for image
   the rotation of the image

- URL shorteners - how friendly with location hash ?
        most are ok with it....  web solution seems plausible....

SDL_ShowMessageBox > allocate buttons once....
> check update policy

// UX read settings begin....

when color is selected in pallete - indicate in history view if visible??????? 
(really only good for first 2 -3 rows of history..... has to be reset any time the view color is hidden, more of a can do)
IMO this become part of ColorList item itself?  but we don't need to scan the full color list except to reset it?
Arguably we should have an index of our list just to help find this.... - or shader bool ikr

send accumulated_movement_x/pxFactor  accumulated_movement_y/pxFactor to the shader
  this represents the approach to 1.0 in each cardinal direction before we move a full pixel over....

stencil dropper bulb when viewed from bottom, so it may be removed from refraction?  the stencil is scaled up?

interactionAddHistory -> animation scale_bounce - how about squeeze?

Uint32 getpixel - non member of textures?  similar fn's in meshes...

todo: keep moving (non int) stuff to vsh???

// TODO - if you use setAnimation on the ui object then you will automatically track and cancel the chains
/// so there is no need to track ANY chain in this file :)

ERROR::: Max Animation Chains 128 Exceeded !!!!!!!!
  ^ simply do not allow this to occur?

add all button cannot achieve achievement (due to mulitple achievement of achievement...)
    addColorToPallete 
    if( standardScoring ) myUxRef->defaultScoreDisplay->displayAchievement(Ux::uiSettingsScroller::UI_ACHEIVEMENT_NOSPACE);
    we could handle the scoring a single time outside of this function... wehn  !standardScoring
    see TODO handle scoring todo note here...


running out of animation chains can break some ux expectations.... (dismisssal of modals, etc...)... git show 772a29a4a46f66c9fc689d2c61f2535cc9478baf

// so if we are showing removeButton and our color changed, we know that something changed and we should hide our deleteX simple as that 

sort dialogue initial text -> "Sort?" <- add more sort choices here 
goto history style

// lets treat it as if they stopped interacting....
-- we can zoom the scrollers?   

settings -
- goto history style
- open imgs style

rclick menu is not responsive (its only sized once)

show alpha value when < 255, rgba hex too?ååå

"deselect all" 3rd option on bulk delete (restart app to deselect all)

renames:
myUxRef -> uxInstance
defaultScoreDisplay -> scoreDisplay
uiSettingsScroller -> uiSettings
settingsScroller -> settings ?
myUiController -<> no change?

use something better (a struct) rather than raw char* ? new Ux::uiList<const char*, Uint8>(25); 
   ^ could include default position?
   eg // position_y= (fullPickImgSurface->h * 0.5) - 513; <<< replicated code btw...

   blend transparency????? (no?)

collisionRect; // this rect has good w/h that we can use (its scaled to boundary space)
^ while true, when is it scaled?  we may use this sometimes before it is scaled.

this helper goes elsewhere?? static int compareColor (it was once moved....) probably not to final place

we need test colors like:
0,7,255
7,0,255
0,85,255 (256*86)
85,15,15 (256*86)
// as far as platform android seek color differences...
// why not try out SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 256); < -- does not work on android

unable to find some colors precisely (pink ff00fc > "works" while ff00fd des not)
-> android -> ff00fc resolevs to fe00fd
-> desktop osx -> ff00fc resolves to ff00fe


good test color:
10,11,13  0a0b0d (hsv picker scan crash fixed...) test all possible color?
add c test for //here is an index test, albeit in javascript:

hisotry sorted like
#255,95,0
#255,96,0
#255,95,0
does it dedupe (yes now, it sorts)

score explanation - bg for readability?

score_size_scaling > score_position->origBoundryRect.w ){ // enforce "max" (default) size
  for score the above will be overwritten right??? 
  what this does is effectively keep things from ever getting larger than the smallest size ever reached..... strange right?  maybe needs testing with SDL_MAX_SINT32

HMM this is really something totally different than containText which is used for responsive.....

"%cAll" really many/several or all....

a way to expand collision bounds outside object bounds automatically to contain child objects??????
  or just use testChildCollisionIgnoreBounds ??

squarilate scroller

zoom scrollers?  maybe both would zoom at same time??? not sure...


arrow interaction repeat....

home pressed - maybe ios screenshot? time for gimicky marketing ploy

pallete swatch color -> huePicker for adjustment ( see cp_set_from_hsv )
^ above is basically "done" but further optimizations could be good

sort pallete colors
^ (web interface? / later - watch out for index?)

dedupe history (when out of space?) (occurs on sort)

remove myScrollController just use myUiController instead of the 1off (not necessarily removable when child elements have different controllers)
remove hasInteraction hasInteractionCb just use nullptr check... (meh)
fix crop of rounded corners in scrollers (see cleanup shader code)

maths keyworkd
leak / leaking keyword (done 2019)
    ^  "maybe we should copy it instead "

 lookup trilinear or aniostropic filteirng (better texture scaling??)

 main thread animations for less jitter see MAIN_THREAD_ANIMATIONS (meh?)

cleanup shader code

perhaps one must swipe too far left to show delete button // swipe left (on desktop this is fine though)

NOTE: just using  currentInteraction here could have SIDE EFFECTS

throw out of view - initial velocity maintain... if over threshold?

// TODO maybe just call resize (without update render position) on hueGradient->resize() instead of handling it here???

128; //derp
  // use const static int then you don't need to derp!

# platform specific todo

> rate it 5 stars
  > pallete upload referrer ???????? (maybe not unless it can be consumed) - or just after this is clicked? "generated using" - could be good
  > see SDL_ShowMessageBox > allocate buttons once....
> check update policy 
> hide scrolly arrows on touchy devices OMIT_SCROLLY_ARROWS OMIT_SCROLLBAR_ARROWS

## desktop any
desktop scroll wheel in pallete (modal)  - scroll wheel improvements possible
other keypress support (modal confirm enter, ...)
clock bar hide (enabled now for dev) (dev cleanup) (seperate testing constant to trigger it? DEVELOPER_TEST_MODE )
SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK < - do we want a right click menu?
- copy menu on right click....
handle when save of history data occurs (specifically desktop plat?) (program exit)
    see SDL_APP_WILLENTERBACKGROUND < this is where we currently write state, works good on mobile clients where there is no fixed quit command...
    SDL_WINDOWEVENT_LEAVE,
    SDL_WINDOWEVENT_FOCUS_LOST, 
    SDL_WINDOWEVENT_CLOSE
      ^ currently saving when program exits (window closed) only
hold mouse down on scroll arrow, expect continuous scroll (also nav arrow?)
drop image when history holder present - we should hide it (other modals should be canceled too?)
//TODO recall window position?  useful possibly for desktop platforms.... ( dubious ) see win_pos_x

### osx
osx application bundle (see readme and  see   Use pbxbuild in the same directory as your .pbproj file)
energy usage -> we can improve this if we allow nap or disable the main loop when the app enters the background (somehow!) SDL_Delay (33 works good desktop but poor on android )
window border mouse drag listen limit

### win
win impl

## mobile any
> check update policy 

### android
// (done?) timer keeps firign in background if file chooser is left open... // we tried this but we can't pick an image... not quite sure why yet... compare logs?
contribute to sdl multi-window support, (see multi-window html) and android:resizeableActivity="false"

### ios
anything using main loop timers (processed on main loop) are not processed....
  ^ uncomment lines above pickerForPercentV(&percent); to test it out... eg USER EVENT 1
  ^ this is at least addressed / worked around.... potential for future issues though since these custom events are just not processed
10 phone special stuffs?
  ^ there is "no clock bar" on iphone 10

# tests
test history overwrote final color in index that it is released from the index (pretty sure it is...)
test with vanilla (renamed) settings files
add some automatic test to verify each color can be viewed in the HSV? (done)

full history list (8192) out of space achieved
- before adding a color observe the last color in the history (topmost color)
- and final color in history (bottom color)
- then delete any colory, the top and bottom most  should not change, all colors should be in position
- add new color to history, it should appear at the top and the colors should all shift by one, all colors should be in position

# sotry

I remember the good old days
when all the colors were vibrant
the hues were saturated with luminosity
and I could pick all the colors I ever wanted
with zero consequences.

Now the world has changed, and
You can't double pick the same color twice anymore
this might not be such a big deal
but my entire life I have double clicked to pick colors
this change has been devestating.

There are still vibrante hues out there
waiting to be discovered and shared with the world;
a world that has become bitter
and obsessed with points... as if that even matters.
The discovery of color, tantalizes.

I will bring color to the world
And in doing so will strive to teach the truth.
That points are not reality.
That the score does not matter.
I will find the hue you seek and set us free...
