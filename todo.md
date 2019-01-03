# notes
DEVELOPER_TEST_MODE


#  todo


movement velocity platform diffences (android) (dupe)

add version string somewhere (do not let it look like an IP address!)  maybe this is plat specifc/bundle specific?
 > AndroidManifest.xml

achievements

settings

pre-open dialogue (stock gradientz?)

sort history colors "rewrite history"  (no index, pretty easy)
  sort dialogue initial text -> "Sort?"
  
copy (dupe) SDL_SetClipboardText
   rClickMenu uiToolMenu rClickMenu->addMenuItem
   note: check widescreen - and responsive....
   note: check if we remove items, they are not clickable later
   -- important: we should have some user feedback when copy is successful --
   -- vertical alignment of menu uiToolMenu - maybe just avoid being under cursor though???


## bugs
osx/ios jpg rotation not respected (is this exif rotation???)
3up pallete drag it down where it should crop - observe artifacts (seems to be the position reset delete button... of the valid items, this is likely a crop logic skipped in shader?)

##  can do

score_size_scaling > score_position->origBoundryRect.w ){ // enforce "max" (default) size
  for score the above will be overwritten right??? 
  what this does is effectively keep things from ever getting larger than the smallest size ever reached..... strange right?  maybe needs testing with SDL_MAX_SINT32

HMM this is really something totally different than containText which is used for responsive.....

"%cAll" really many/several or all....

a way to expand collision bounds outside object bounds automatically to contain child objects??????
  or just use testChildCollisionIgnoreBounds ??

squarilate scroller

unable to find some colors precisely (pink ff00fc > "works" while ff00fd des not)
-> android -> ff00fc resolevs to fe00fd
-> desktop osx -> ff00fc resolves to ff00fe

arrow interaction repeat....

home pressed - maybe ios screenshot? time for gimicky marketing ploy

pallete swatch color -> huePicker for adjustment ( see cp_set_from_hsv )
^ above is basically "done" but further optimizations could be good

sort pallete colors
^ (web interface? / later - watch out for index?)

text selection and or copy ~~paste~~ ease (desktop, mobile)             SDL_Log("Double touched color preview......");
^ copy menu, modal-ish (any interaction shall call dismiss modals function)
^ SDL_SetClipboardText makes this easy..... 

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

# platform specific todo

> rate it 5 stars
  > pallete upload referrer ???????? (maybe not unless it can be consumed) - or just after this is clicked? "generated using" - could be good
  > see SDL_ShowMessageBox > allocate buttons once....
> check update policy 
> hide scrolly arrows on touchy devices OMIT_SCROLLY_ARROWS OMIT_SCROLLBAR_ARROWS

## desktop any
desktop scroll wheel in pallete (modal)
other keypress support (modal confirm enter, ...)
click bg to dismiss any modal such as historyPalleteHolder, yesno?, etc
clock bar hide (enabled now for dev) (dev cleanup) (seperate testing constant to trigger it? DEVELOPER_TEST_MODE )
SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK < - do we want a right click menu?
handle when save of history data occurs (specifically desktop plat?) (program exit)
    see SDL_APP_WILLENTERBACKGROUND < this is where we currently write state, works good on mobile clients where there is no fixed quit command...
    SDL_WINDOWEVENT_LEAVE,
    SDL_WINDOWEVENT_FOCUS_LOST, 
    SDL_WINDOWEVENT_CLOSE
hold mouse down on scroll arrow, expect continuous scroll (also nav arrow?)
drop image when history holder present - we should hide it (other modals should be canceled too?)

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
