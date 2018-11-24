#  todo

movement speed for full zoom (moving logic out of main... using accumulator)
movement velocity platform diffences (android)
handle when save of history data occurs (specifically desktop plat?)
pallete swatch color -> huePicker for adjustment ( see cp_set_from_hsv )
     ^ above is basically "done" but further refinements would be good
sort pallete colors
    ^ (web interface? / later)
crop/scale issues
widescreen pallete up-ness

disable builtin image list looping/defaulting when nothign opened...

myUxRef->interactionTogglePalletePreview(myUxRef->palleteSelectionPreviewHolder, delta); more often

## bugs
osx/ios jpg rotation not respected (is this exif rotation???)
scroll scroller part way OOB (dragging content) then right click or middle click without releasing lclick
default window size hue pos render artifact (desktop) font render artifact...

##  can do

close button for historyPalleteHolder ???  
ux feedback when nav arrow is pressed
ux refactor splitup files (for responsive?)
text selection and or copy ~~paste~~ ease (desktop, mobile)

# platform specific todo

## desktop any
esc key exit modals (desktop platforms)
desktop scroll wheel in pallete (modal)
other keypress support (modal confirm enter, ...)
click bg to dismiss any modal such as historyPalleteHolder, yesno?, etc

### osx
osx application bundle (see readme and  see   Use pbxbuild in the same directory as your .pbproj file)
energy usage

### win
win impl

## mobile any
rotation of screen - clock bar?

### android
resources into android bundle... ? (done?)
rotation of screen - render again??
there is no clock bar - why leave space for it
timer keeps firign in background if file chooser is left open... // we tried this but we can't pick an image... not quite sure why yet... compare logs?

### ios
anything using main loop timers (processed on main loop) are not processed....
  ^ uncomment lines above pickerForPercentV(&percent); to test it out... eg USER EVENT 1
  ^ this is at least addressed / worked around.... potential for future issues though since these custom events are just not processed
10 phone special stuffs?
