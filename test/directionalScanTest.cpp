/*
 Copyright (C) 1997-2018 Sam Lantinga <slouken@libsdl.org>

 This software is provided 'as-is', without any express or implied
 warranty.  In no event will the authors be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely.
 */

/* Simple test of the SDL threading code and error handling */

#include "SDL.h"

#include <stdio.h>
#include <stdlib.h>

#include <iostream> // cpp functioanlity, not .c

#include "directionalScan.h"


// GLM include files
#include "../glm/glm.hpp"
#include "../glm/gtx/projection.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtx/fast_square_root.hpp"
#include "../glm/gtx/perpendicular.hpp"


int
main(int argc, char *argv[])
{

    SDL_Log("Hello directional scan world");

    DirectionalScan<SDL_Point, glm::vec3>* myDirectionalScan = new DirectionalScan<SDL_Point, glm::vec3>(2);

    if(myDirectionalScan->_maxSize != 2){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected _maxSize: %i\n",
                     myDirectionalScan->_maxSize );
        return 1;
    }

    if(myDirectionalScan->_startIndex != -2){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected _startIndex: %i\n",
                     myDirectionalScan->_startIndex );
        return 1;
    }

    if(myDirectionalScan->_endIndex != 3){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected _endIndex: %i\n",
                     myDirectionalScan->_endIndex );
        return 1;
    }

    if(myDirectionalScan->_rowLen != 5){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected _rowLen: %i\n",
                     myDirectionalScan->_rowLen );
        return 1;
    }

    if(myDirectionalScan->_total_size != 25){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected _total_size: %i\n",
                     myDirectionalScan->_total_size );
        return 1;
    }
/*
this is basically our 4x4 grid...
   * * # * *
   * * # * *
   # # ~ # #
   * * # * *
   * * # * *

 the offset to center point is quad offset... this way we can use a 0,0 array, the index offset by _quad_offset
*/
    if(myDirectionalScan->_quad_offset != 12){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected _quad_offset: %i\n",
                     myDirectionalScan->_quad_offset );
        return 1;
    }

/*
     this is basically our 4x4 grid...
 -2,-2 * # * *
     * * # * *
     # # ~ # #
     * * # * *
     * * # * 2,2

 coordinateIndex is a 1d array from -2,-2 to 2,2 with 0,0 falling at _quad_offset
 */

    SDL_Point* coordinateIndex = myDirectionalScan->coordinateIndex;
    int textIndex = 0;

    if(coordinateIndex[textIndex].x != -2 || coordinateIndex[textIndex].y != -2){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected coordinateIndex[%i].x: %i coordinateIndex[%i].y: %i\n",
                     textIndex, coordinateIndex[textIndex].x, textIndex, coordinateIndex[textIndex].y );
        return 1;
    }

    textIndex = 1;
    if(coordinateIndex[textIndex].x != -1 || coordinateIndex[textIndex].y != -2){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected coordinateIndex[%i].x: %i coordinateIndex[%i].y: %i\n",
                     textIndex, coordinateIndex[textIndex].x, textIndex, coordinateIndex[textIndex].y );
        return 1;
    }

    textIndex = 2;
    if(coordinateIndex[textIndex].x != 0 || coordinateIndex[textIndex].y != -2){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected coordinateIndex[%i].x: %i coordinateIndex[%i].y: %i\n",
                     textIndex, coordinateIndex[textIndex].x, textIndex, coordinateIndex[textIndex].y );
        return 1;
    }

    textIndex = 3;
    if(coordinateIndex[textIndex].x != 1 || coordinateIndex[textIndex].y != -2){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected coordinateIndex[%i].x: %i coordinateIndex[%i].y: %i\n",
                     textIndex, coordinateIndex[textIndex].x, textIndex, coordinateIndex[textIndex].y );
        return 1;
    }

    textIndex = 4;
    if(coordinateIndex[textIndex].x != 2 || coordinateIndex[textIndex].y != -2){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected coordinateIndex[%i].x: %i coordinateIndex[%i].y: %i\n",
                     textIndex, coordinateIndex[textIndex].x, textIndex, coordinateIndex[textIndex].y );
        return 1;
    }

    textIndex = 5;
    if(coordinateIndex[textIndex].x != -2 || coordinateIndex[textIndex].y != -1){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected coordinateIndex[%i].x: %i coordinateIndex[%i].y: %i\n",
                     textIndex, coordinateIndex[textIndex].x, textIndex, coordinateIndex[textIndex].y );
        return 1;
    }

    textIndex = 10;
    if(coordinateIndex[textIndex].x != -2 || coordinateIndex[textIndex].y != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected coordinateIndex[%i].x: %i coordinateIndex[%i].y: %i\n",
                     textIndex, coordinateIndex[textIndex].x, textIndex, coordinateIndex[textIndex].y );
        return 1;
    }

    textIndex = myDirectionalScan->_quad_offset;
    if(coordinateIndex[textIndex].x != 0 || coordinateIndex[textIndex].y != 0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected coordinateIndex[%i].x: %i coordinateIndex[%i].y: %i\n",
                     textIndex, coordinateIndex[textIndex].x, textIndex, coordinateIndex[textIndex].y );
        return 1;
    }

    textIndex = 14;
    if(coordinateIndex[textIndex].x != 2 || coordinateIndex[textIndex].y != 0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected coordinateIndex[%i].x: %i coordinateIndex[%i].y: %i\n",
                     textIndex, coordinateIndex[textIndex].x, textIndex, coordinateIndex[textIndex].y );
        return 1;
    }

    textIndex = 15;
    if(coordinateIndex[textIndex].x != -2 || coordinateIndex[textIndex].y != 1 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected coordinateIndex[%i].x: %i coordinateIndex[%i].y: %i\n",
                     textIndex, coordinateIndex[textIndex].x, textIndex, coordinateIndex[textIndex].y );
        return 1;
    }

    textIndex = 24;
    if(coordinateIndex[textIndex].x != 2 || coordinateIndex[textIndex].y != 2 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected coordinateIndex[%i].x: %i coordinateIndex[%i].y: %i\n",
                     textIndex, coordinateIndex[textIndex].x, textIndex, coordinateIndex[textIndex].y );
        return 1;
    }


    /*
     this is basically our 4x4 grid...
     * * 2 * *
     * * 1 * *
     2 1 0 1 2
     * * 1 * *
     * * 2 * *

     distances are float distances from center
     */

    float* distances = myDirectionalScan->distances;

    textIndex = myDirectionalScan->_quad_offset;
    if( distances[textIndex] != 0.0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected distances[%i]:  %f\n",
                     textIndex, distances[textIndex]);
        return 1;
    }

    textIndex = 2;
    if( distances[textIndex] != 2.0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected distances[%i]:  %f\n",
                     textIndex, distances[textIndex]);
        return 1;
    }

    textIndex = 10;
    if( distances[textIndex] != 2.0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected distances[%i]:  %f\n",
                     textIndex, distances[textIndex]);
        return 1;
    }

    textIndex = 14;
    if( distances[textIndex] != 2.0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected distances[%i]:  %f\n",
                     textIndex, distances[textIndex]);
        return 1;
    }

    textIndex = 22;
    if( distances[textIndex] != 2.0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected distances[%i]:  %f\n",
                     textIndex, distances[textIndex]);
        return 1;
    }

    textIndex = 7;
    if( distances[textIndex] != 1.0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected distances[%i]:  %f\n",
                     textIndex, distances[textIndex]);
        return 1;
    }

    textIndex = 11;
    if( distances[textIndex] != 1.0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected distances[%i]:  %f\n",
                     textIndex, distances[textIndex]);
        return 1;
    }

    textIndex = 13;
    if( distances[textIndex] != 1.0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected distances[%i]:  %f\n",
                     textIndex, distances[textIndex]);
        return 1;
    }

    textIndex = 17;
    if( distances[textIndex] != 1.0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected distances[%i]:  %f\n",
                     textIndex, distances[textIndex]);
        return 1;
    }


    /*
     this is basically our 4x4 grid...
     * * 2 * *
     * * 1 * *
     2 1 0 1 2
     * * 1 * *
     * * 2 * *

     distances are float distances from center
     this (or above) is perhaps redundant, we just use the helper function for these seemingly
     but this tests coordinateSpace which is not hte same as distances
     however the above tests should probably be removed as redundant from the test and program code
     */


    //DirectionalElement* elements  = ;

    textIndex = myDirectionalScan->_quad_offset;
    if( myDirectionalScan->getDistance(textIndex) != 0.0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected myDirectionalScan->getDistance(%i):  %f\n",
                     textIndex, myDirectionalScan->getDistance(textIndex));
        return 1;
    }

    textIndex = 2;
    if( myDirectionalScan->getDistance(textIndex) != 2.0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected myDirectionalScan->getDistance(%i):  %f\n",
                     textIndex, myDirectionalScan->getDistance(textIndex));
        return 1;
    }

    textIndex = 10;
    if( myDirectionalScan->getDistance(textIndex) != 2.0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected myDirectionalScan->getDistance(%i):  %f\n",
                     textIndex, myDirectionalScan->getDistance(textIndex));
        return 1;
    }

    textIndex = 14;
    if( myDirectionalScan->getDistance(textIndex) != 2.0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected myDirectionalScan->getDistance(%i):  %f\n",
                     textIndex, myDirectionalScan->getDistance(textIndex));
        return 1;
    }

    textIndex = 22;
    if( myDirectionalScan->getDistance(textIndex) != 2.0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected myDirectionalScan->getDistance(%i):  %f\n",
                     textIndex, myDirectionalScan->getDistance(textIndex));
        return 1;
    }

    textIndex = 7;
    if( myDirectionalScan->getDistance(textIndex) != 1.0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected myDirectionalScan->getDistance(%i):  %f\n",
                     textIndex, myDirectionalScan->getDistance(textIndex));
        return 1;
    }

    textIndex = 11;
    if( myDirectionalScan->getDistance(textIndex) != 1.0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected myDirectionalScan->getDistance(%i):  %f\n",
                     textIndex, myDirectionalScan->getDistance(textIndex));
        return 1;
    }

    textIndex = 13;
    if( myDirectionalScan->getDistance(textIndex) != 1.0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected myDirectionalScan->getDistance(%i):  %f\n",
                     textIndex, myDirectionalScan->getDistance(textIndex));
        return 1;
    }

    textIndex = 17;
    if( myDirectionalScan->getDistance(textIndex) != 1.0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected myDirectionalScan->getDistance(%i):  %f\n",
                     textIndex, myDirectionalScan->getDistance(textIndex));
        return 1;
    }

    // all above tests might fail if we sort durign init... unless we copy to a sorted coordinateIndex
    // computing vectors is good because we can rule them out and whole sets of destinations
    // however at best each coordinateIndex may merely belong to a vector that is eitehr still under examination
    // or already ruled out
    // its is the list of vectors that are ruled out that we'll need to reset - the search space is the same


    SDL_free(myDirectionalScan);
    SDL_Log("All directional scan tests passed.");
}
