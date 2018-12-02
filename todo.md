#  todo

movement speed for full zoom (moving logic out of main... using accumulator)
movement velocity platform diffences (android)

pallete swatch color -> huePicker for adjustment ( see cp_set_from_hsv )
     ^ above is basically "done" but further refinements would be good
sort pallete colors
    ^ (web interface? / later)

feedback when dupe selected (color in pallete) needs some refinement now....
    ^ see calculateCropParentOrig
    setCropModeOrigPosition
    etc...
    
recall delete states in scrollers (dont't keep just a list of SDL Color but keep uiState too... )
  -> see ColorList
  -> problems: 
     -> readInState newHistoryList
     -> writeOutState
  -> solution - use a second list of bool or a second list of state objects (good idea - annoying to manage though)

    pickHistoryListState = new uiList<ColorListState, Uint8>(pickHistoryMax);
    palleteListState = new uiList<ColorListState, Uint8>(palleteMax);


disable builtin image list looping/defaulting when nothign opened... (dev cleanup)
disable adding random history colors (dev cleanup)

remove unused textures from bundle (dev cleanup?)

## bugs
osx/ios jpg rotation not respected (is this exif rotation???)
scroll scroller part way OOB (dragging content) then right click or middle click without releasing lclick

##  can do

remove myScrollController just use myUiController instead of the 1off
remove hasInteraction hasInteractionCb just use nullptr check...
fix crop of rounded corners in scrollers
close button for historyPalleteHolder ???  
ux feedback when nav arrow is pressed
   -see setInteractionBegin should work now....
ux refactor splitup files (for responsive?)
text selection and or copy ~~paste~~ ease (desktop, mobile)
main thread animations for less jitter

maths keyworkd
leak / leaking keyword
 ^ new Float_Rect
 
 lookup trilinear or aniostropic filteirng 
 
cleanup shader code

add version string somewhere

# platform specific todo

## desktop any
esc key exit modals (desktop platforms)
desktop scroll wheel in pallete (modal)
other keypress support (modal confirm enter, ...)
click bg to dismiss any modal such as historyPalleteHolder, yesno?, etc
clock bar hide (enabled now for dev)
handle when save of history data occurs (specifically desktop plat?)
    see SDL_APP_WILLENTERBACKGROUND < this is where we currently write state, works good on mobile clients where there is no fixed quit command...
    SDL_WINDOWEVENT_LEAVE,
    SDL_WINDOWEVENT_FOCUS_LOST, 
    SDL_WINDOWEVENT_CLOSE


### osx
osx application bundle (see readme and  see   Use pbxbuild in the same directory as your .pbproj file)
energy usage
window border mouse drag listen limit

### win
win impl

## mobile any

### android
resources into android bundle... ? (done?)
rotation of screen - render again??
there is no clock bar - why leave space for it
timer keeps firign in background if file chooser is left open... // we tried this but we can't pick an image... not quite sure why yet... compare logs?
movement velocity platform diffences (android)

### ios
anything using main loop timers (processed on main loop) are not processed....
  ^ uncomment lines above pickerForPercentV(&percent); to test it out... eg USER EVENT 1
  ^ this is at least addressed / worked around.... potential for future issues though since these custom events are just not processed
10 phone special stuffs?
