# Platform Checklists

What *should* work on each platform...

## All

 * The upload button for selected pallete colors, and other buttons which are links (settings -> bug / help) should launch default system web browser
 * the open file button should work to allow image selection

## Desktop

 * scroll wheel should zoom

### PICK_FROM_SCREEN_ENABLED / CHAR_MOUSE_CURSOR

ordered from most to less important...

 * screenshot and mouse position should always work for any resolution/(number of displays) and any mouse position, including 2x (retina) displays or other OS zoom levels... (every other pixel may not be accessible via mouse when 2x...)
 * quick movements should always land in correct spot ( as we do not always create a new zoom image, see PAN_RECENTER_SNAP_AT_OFFSET)
 * click anywhere (on invisible shield window) should add the current color
 * `esc` key should cancel and NOT add current color
 * `r` or `j` keys should refresh the snap AND provide a screenshot of the interaction you hover
 * mousewheel anywhere on shield should zoom
 * `enter` key anywhere should add current color and exit
 * arrow keys can and should pan and allow selection in conjunction with enter, as long as mouse is not moved (can always just click or esc, then drag view)

## Mobile

 * pinch should zoom

## What about build

Wrong folder, see external folder for dependency and platform instructions...
Also, See CMakeLists.txt for more info... on how to build and set build arguments...
