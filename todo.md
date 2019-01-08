# notes
DEVELOPER_TEST_MODE


#  todo


movement velocity platform diffences (android) (dupe)

add version string somewhere (do not let it look like an IP address!)  maybe this is plat specifc/bundle specific?
 > AndroidManifest.xml

achievements

settings -
  - if we disable scoring - some or all of the displayed explanations should still be preserved.... (eg "out of space!")
  implement: bool game_enabled = settingsScroller->getBooleanSetting(uiSettingsScroller::UI_SETTING_GAME_ON);


pre-open dialogue (stock gradientz?)

sort history colors "rewrite history"  (no index, pretty easy)
  sort dialogue initial text -> "Sort?"
  
  SDL_ShowMessageBox > allocate buttons once....
  > check update policy

  // lets treat it as if they stopped interacting....
    -- we can zoom the scrollers?
  

## bugs
osx/ios jpg rotation not respected (is this exif rotation???)
3up pallete drag it down where it should crop - observe artifacts (seems to be the position reset delete button... of the valid items, this is likely a crop logic skipped in shader?)

##  can do

collisionRect; // this rect has good w/h that we can use (its scaled to boundary space)
^ while true, when is it scaled?  we may use this sometimes before it is scaled.

this helper goes elsewhere?? static int compareColor

we need test colors like:
0,7,255
7,0,255
0,85,255 (256*86)
85,15,15 (256*86)
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

unable to find some colors precisely (pink ff00fc > "works" while ff00fd des not)
-> android -> ff00fc resolevs to fe00fd
-> desktop osx -> ff00fc resolves to ff00fe

arrow interaction repeat....

home pressed - maybe ios screenshot? time for gimicky marketing ploy

pallete swatch color -> huePicker for adjustment ( see cp_set_from_hsv )
^ above is basically "done" but further optimizations could be good

sort pallete colors
^ (web interface? / later - watch out for index?)

dedupe history (when out of space?)

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
desktop scroll wheel in pallete (modal)
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
//TODO recall window position?  useful possibly for desktop platforms....

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


### ios
anything using main loop timers (processed on main loop) are not processed....
  ^ uncomment lines above pickerForPercentV(&percent); to test it out... eg USER EVENT 1
  ^ this is at least addressed / worked around.... potential for future issues though since these custom events are just not processed
10 phone special stuffs?
  ^ there is "no clock bar" on iphone 10

# tests
test history overwrote final color in index that it is released from the index (pretty sure it is...)

