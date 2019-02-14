/*
 */

#include "SDL.h"

#include <stdio.h>
#include <stdlib.h>

#include <iostream> // cpp functioanlity, not .c

#include "uiList.h"


// GLM include files
#include "../glm/glm.hpp"
#include "../glm/gtx/projection.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtx/fast_square_root.hpp"
#include "../glm/gtx/perpendicular.hpp"

static void setColorRgb(SDL_Color * color, Uint8 r, Uint8 g, Uint8 b, Uint8 a){
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = a;
}

static bool colorEquals(SDL_Color * color, SDL_Color * bcolor){
    //return SDL_memcmp(color, bcolor, sizeof(SDL_Color)) == 0; // there is no short circut with memcmp....
    return color->r == bcolor->r && color->g == bcolor->g && color->b == bcolor->b;
}

static int indexForColor(SDL_Color* c){
    return (c->r*65536) + (c->g*256) + (c->b);
}
#define COLOR_INDEX_MAX 16777217 //  256^3 +1

int
main(int argc, char *argv[])
{

    SDL_Log("Hello world list test");

    uiList<SDL_Color, Uint8>* myColorList = new uiList<SDL_Color, Uint8>(254);
    myColorList->index(COLOR_INDEX_MAX, indexForColor);

    SDL_Color blue;
    setColorRgb(&blue, 0, 0, 255, 255);
    SDL_Color green;
    setColorRgb(&green, 0, 255, 0, 255);
    SDL_Color red;
    setColorRgb(&red, 255, 0, 0, 255);
    SDL_Color black;
    setColorRgb(&black, 0, 0, 0, 255);
    SDL_Color white;
    setColorRgb(&black, 255, 255, 255, 255);

    if(myColorList->total() != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected total(): %i\n",
                     myColorList->total() );
        return 1;
    }

    myColorList->add(blue);

    if(myColorList->total() != 1){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected total(): %i\n",
                     myColorList->total() );
        return 1;
    }

    if(myColorList->locate(blue) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected locate(blue): %i\n",
                     myColorList->locate(blue) );
        return 1;
    }

    myColorList->add(blue);

    if(myColorList->locate(blue) != 1){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected locate(blue): %i\n",
                     myColorList->locate(blue) );
        return 1;
    }

    // could / should the index have blocked the dupe item?  it now points at the last one added (1) above and we now have (2) items.... in practice we use locate before we use add if we want to avoid dupes right now...
    if(myColorList->total() != 2){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected total(): %i\n",
                     myColorList->total() );
        return 1;
    }

    myColorList->add(red);

    if(myColorList->locate(red) != 2){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected locate(red): %i\n",
                     myColorList->locate(red) );
        return 1;
    }

    // could / should the index have blocked the dupe item?  it now points at the last one added (1) above and we now have (2) items.... in practice we use locate before we use add if we want to avoid dupes right now...
    // right now you will break settings - certain items don't need to be in the index and its okay if they have the same index
    if(myColorList->total() != 3){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected total(): %i\n",
                     myColorList->total() );
        return 1;
    }


    uiListIterator<uiList<SDL_Color, Uint8>, SDL_Color>* myColorListIterator = myColorList->iterate();
    SDL_Color* tmp;

    tmp = myColorListIterator->next();
    if( !colorEquals(tmp, &blue) ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected color, expected blue: %i %i %i\n",
                     tmp->r, tmp->g, tmp->b );
        return 1;
    }

    tmp = myColorListIterator->next();
    if( !colorEquals(tmp, &blue) ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected color, expected blue: %i %i %i\n",
                     tmp->r, tmp->g, tmp->b );
        return 1;
    }

    tmp = myColorListIterator->next();
    if( !colorEquals(tmp, &red) ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected color, expected red: %i %i %i\n",
                     tmp->r, tmp->g, tmp->b );
        return 1;
    }

    tmp = myColorListIterator->next();
    if( tmp != nullptr ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected color, expected nullptr: %i %i %i\n",
                     tmp->r, tmp->g, tmp->b );
        return 1;
    }

    SDL_free(myColorListIterator);
    SDL_free(myColorList);


    // non indexed....
    myColorList = new uiList<SDL_Color, Uint8>(254);

    if(myColorList->total() != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected total(): %i\n",
                     myColorList->total() );
        return 1;
    }

    myColorList->add(blue);
    myColorList->add(red);
    myColorList->add(green);

    if(myColorList->total() != 3){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected total(): %i\n",
                     myColorList->total() );
        return 1;
    }


    uiList<SDL_Color, Uint8>* myListClone = myColorList->clone();

    if(myListClone->total() != 3){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected total(): %i\n",
                     myListClone->total() );
        return 1;
    }

    tmp = myListClone->get(0);
    if( !colorEquals(tmp, &blue) ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected color, expected blue: %i %i %i\n",
                     tmp->r, tmp->g, tmp->b );
        return 1;
    }

    tmp = myListClone->get(1);
    if( !colorEquals(tmp, &red) ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected color, expected red: %i %i %i\n",
                     tmp->r, tmp->g, tmp->b );
        return 1;
    }

    tmp = myListClone->get(2);
    if( !colorEquals(tmp, &green) ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected color, expected green: %i %i %i\n",
                     tmp->r, tmp->g, tmp->b );
        return 1;
    }

    SDL_free(myColorList);
    SDL_free(myListClone);


    SDL_Log("All list tests passed.");
}
