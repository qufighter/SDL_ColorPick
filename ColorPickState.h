//
//  ColorPickState.h
//  ___PROJECTNAME___
//
//  Created by Sam Larison on 8/24/15.
//
//

//data:text/html;plain,<b>bold</b>

#ifndef _____PROJECTNAME_____ColorPickState__
#define _____PROJECTNAME_____ColorPickState__

//class ColorPickState; /* forward decl ???? */



class ColorPickState {

public:
    static ColorPickState* Singleton();

    // probably should not be public but easier this way!
    int mmovex, mmovey; /// needs to be ux accessible singleton.......


private:

protected:


    ColorPickState(void); // Default constructor
    ~ColorPickState(void); // Destructor for cleaning up our application

    
};
//


static bool cps_ms_bInstanceCreated=false;
static ColorPickState* cps_pInstance=0;


//ColorPickState::ms_bInstanceCreated = false;
//ColorPickState::pInstance = nullptr;

//ColorPickState* ColorPickState::Singleton() {
//    if(!cps_ms_bInstanceCreated){
//        cps_pInstance = new ColorPickState();
//        cps_ms_bInstanceCreated=true;
//    }
//    return cps_pInstance;
//}
//
//ColorPickState::ColorPickState(void) {
//    //constructor
//    mmovex=0;
//    mmovey=0;
//
//}




//ends up mulitply defined (?? in future?)
static ColorPickState* colorPickState = ColorPickState::Singleton();



#endif /* defined(_____PROJECTNAME_____ColorPickState__) */
