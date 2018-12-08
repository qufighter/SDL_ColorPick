#  todo

movement speed for full zoom (moving logic out of main... using accumulator)
movement velocity platform diffences (android) (dupe)

pallete swatch color -> huePicker for adjustment ( see cp_set_from_hsv )
     ^ above is basically "done" but further refinements would be good
sort pallete colors
    ^ (web interface? / later)


recall delete states in scrollers (dont't keep just a list of SDL Color but keep uiState too... )
  -> see ColorList
  -> problems: 
     -> readInState newHistoryList
     -> writeOutState
  -> solution - use a second list of bool or a second list of state objects (good idea - annoying to manage though)

    pickHistoryListState = new uiList<ColorListState, Uint8>(pickHistoryMax);
    palleteListState = new uiList<ColorListState, Uint8>(palleteMax);

-> solution2
   transform the list after read and before write into a new list instead......

    uiListIterator<uiList<SDL_Color, Uint8>, SDL_Color>* myIterator = myUxRef->palleteList->iterate();
    SDL_Color *color = myIterator->next();
    while(color != nullptr){
        //SDL_Log("%i %i %i", color->r, color->g, color->b);
        color = myIterator->next();
    }
    SDL_free(myIterator);
    // except use ColorList struct and build a new list of SDL_Color for save, or reverse for load....


make delete all more clear (pallete can mislead to think only one row delete?)  (maybe scroll the view to illustrate the effect?  although reset of scroll on cancel would be good..... or otherwise end anim at current position?)

text selection and or copy ~~paste~~ ease (desktop, mobile)
main thread animations for less jitter

disable builtin image list looping/defaulting when nothign opened... (dev cleanup)
disable adding random history colors (dev cleanup)
remove unused textures from bundle (dev cleanup?)

## bugs
osx/ios jpg rotation not respected (is this exif rotation???)
scroll scroller part way OOB (dragging content) then right click or middle click without releasing lclick

##  can do

remove myScrollController just use myUiController instead of the 1off (not necessarily removable when child elements have different controllers)
remove hasInteraction hasInteractionCb just use nullptr check... (meh)
fix crop of rounded corners in scrollers (see cleanup shader code)

close button for historyPalleteHolder ???  

ux feedback when nav arrow is pressed
  -see setInteractionBegin should work now....
  ^ improve it further?!  uiAminChain* scale  -> really scaleBounce - and really we should reset it when interaction lost or complete... hmmm

ux refactor splitup files (for responsive?) meh

maths keyworkd
leak / leaking keyword (done 2019)
    ^  "maybe we should copy it instead "
    
 lookup trilinear or aniostropic filteirng 
 
cleanup shader code

add version string somewhere (do not let it look like an IP address!)  maybe this is plat specifc/bundle specific?

# platform specific todo

> rate it 5 stars
  > pallete upload referrer ???????? (maybe not unless it can be consumed) - or just after this is clicked? "generated using" - could be good
  > see SDL_ShowMessageBox > allocate buttons once....
> check update policy 


## desktop any
esc key exit modals (desktop platforms)
desktop scroll wheel in pallete (modal)
other keypress support (modal confirm enter, ...)
click bg to dismiss any modal such as historyPalleteHolder, yesno?, etc
clock bar hide (enabled now for dev)
SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK < - do we want a right click menu?
SDL_HINT_VIDEO_ALLOW_SCREENSAVER <- we should allow it
handle when save of history data occurs (specifically desktop plat?)
    see SDL_APP_WILLENTERBACKGROUND < this is where we currently write state, works good on mobile clients where there is no fixed quit command...
    SDL_WINDOWEVENT_LEAVE,
    SDL_WINDOWEVENT_FOCUS_LOST, 
    SDL_WINDOWEVENT_CLOSE


### osx
osx application bundle (see readme and  see   Use pbxbuild in the same directory as your .pbproj file)
energy usage -> SDL_HINT_MAC_BACKGROUND_APP ??
window border mouse drag listen limit

### win
win impl

## mobile any

### android
resources into android bundle... ? (done?)
there is no clock bar - why leave space for it
// (done?) timer keeps firign in background if file chooser is left open... // we tried this but we can't pick an image... not quite sure why yet... compare logs?
movement velocity platform diffences (android) (dupe)

### ios
anything using main loop timers (processed on main loop) are not processed....
  ^ uncomment lines above pickerForPercentV(&percent); to test it out... eg USER EVENT 1
  ^ this is at least addressed / worked around.... potential for future issues though since these custom events are just not processed
10 phone special stuffs?
