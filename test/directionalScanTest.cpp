/*


 */

#include "SDL.h"

#include <stdio.h>
#include <stdlib.h>

#include <iostream> // cpp functioanlity, not .c

// GLM include files
#include "../external_static/glm/glm.hpp"
#include "../external_static/glm/gtx/projection.hpp"
#include "../external_static/glm/gtc/matrix_transform.hpp"
#include "../external_static/glm/gtx/fast_square_root.hpp"
#include "../external_static/glm/gtx/perpendicular.hpp"

#include "directionalScan.h"

bool floatsUnequal( float test, float expectation ){
    float diff = test - expectation;
    if( SDL_fabs(diff) > 0.00001 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected floatsApproxEq(%f, %f):  %f\n",
                     test, expectation, SDL_fabs(diff));
        return true;
    }

    return false;
}

bool vectorExpectationPasses( SDL_Point vector, int interval, SDL_Point expVector, int expInterval ){

    if( vector.x != expVector.x || vector.y != expVector.y ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected vector (%i, %i):  wanted (%i, %i)\n",
                     vector.x, vector.y, expVector.x, expVector.y);
        return false;
    }

    if( interval != expInterval ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected vector interval (%i):  wanted (%i)\n",
                     interval, expInterval);
        return false;
    }

    return true;

}

glm::vec3 getItemForPoint(SDL_Point point){
    if( point.x == 1 && point.y == 2 ){
        return glm::vec3(13, 23, 56); // winning point....
    }
    return glm::vec3(12,24,57);
}

glm::vec3 getItemForPointTest2(SDL_Point point){
    if( point.x == 2 && point.y == 2 ){
        return glm::vec3(13, 23, 55); // winning point.... its 1 off
    }
    return glm::vec3(12,24,57);
}

int
main(int argc, char *argv[])
{

    SDL_Log("Hello directional scan world");

    int tempInt=0;
    SDL_Point a = {134,421};
    SDL_Point b = {85,12};
    a = b;
    b.x = 134;
    if(a.x != 85){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected struct assignment behavior: %i\n",
                     a.x );
        return 1;
    }

    SDL_Point tempPointA;
    SDL_Point tempPointB;

    tempPointA = {1,1};
    tempPointB = {1,1};
    tempInt = vectorIsComponent(tempPointA, tempPointB);
    if( tempInt != 1 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected vector component (%i,%i), total (%i,%i) interval (%i):  wanted (%i)\n",
                     tempPointA.x, tempPointA.y,
                     tempPointB.x, tempPointB.y,
                     tempInt, 1);
        return 1;
    }

    tempPointA = {1,1};
    tempPointB = {2,2};
    tempInt = vectorIsComponent(tempPointA, tempPointB);
    if( tempInt != 2 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected vector component (%i,%i), total (%i,%i) interval (%i):  wanted (%i)\n",
                     tempPointA.x, tempPointA.y,
                     tempPointB.x, tempPointB.y,
                     tempInt, 2);
        return 1;
    }

    tempPointA = {1,1};
    tempPointB = {-2,-2};
    tempInt = vectorIsComponent(tempPointA, tempPointB);
    if( tempInt != 0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected vector component (%i,%i), total (%i,%i) interval (%i):  wanted (%i)\n",
                     tempPointA.x, tempPointA.y,
                     tempPointB.x, tempPointB.y,
                     tempInt, 0);
        return 1;
    }

    tempPointA = {1,1};
    tempPointB = {-2,-2};
    tempInt = vectorIsComponent(tempPointA, tempPointB);
    if( tempInt != 0 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected vector component (%i,%i), total (%i,%i) interval (%i):  wanted (%i)\n",
                     tempPointA.x, tempPointA.y,
                     tempPointB.x, tempPointB.y,
                     tempInt, 0);
        return 1;
    }

    tempPointA = {1,-1};
    tempPointB = {2,-2};
    tempInt = vectorIsComponent(tempPointA, tempPointB);
    if( tempInt != 2 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected vector component (%i,%i), total (%i,%i) interval (%i):  wanted (%i)\n",
                     tempPointA.x, tempPointA.y,
                     tempPointB.x, tempPointB.y,
                     tempInt, 2);
        return 1;
    }

    tempPointA = {-1,-1};
    tempPointB = {-2,-2};
    tempInt = vectorIsComponent(tempPointA, tempPointB);
    if( tempInt != 2 ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected vector component (%i,%i), total (%i,%i) interval (%i):  wanted (%i)\n",
                     tempPointA.x, tempPointA.y,
                     tempPointB.x, tempPointB.y,
                     tempInt, 2);
        return 1;
    }

    /*

     now the "real" tests start


     */

    DirectionalScan<SDL_Point, glm::vec3>* myDirectionalScan = new DirectionalScan<SDL_Point, glm::vec3>(2, &vectorIsComponent);

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
     todo remove    float* distances
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

    /*
     this is basically our 4x4 grid... once sorted by distance, so the distances are:
     0.0  1.0  1.0  1.0  1.0
     1.4  1.4  1.4  1.4  2.0
     2.0  2.0  2.0  2.2  2.2
     2.2  2.2  2.2  2.2  2.2
     2.2  2.8  2.8  2.8  2.8

     this is basically our 4x4 grid... of float distances
     2.8  2.2  2.0  2.2  2.8
     2.2  1.4  1.0  1.4  2.2
     2.0  1.0  0.0  1.0  2.0
     2.2  1.4  1.0  1.4  2.2
     2.8  2.2  2.0  2.2  2.8

     */

    textIndex = 0;
    if( floatsUnequal( myDirectionalScan->getItemDistanceAtDistanceIndex(textIndex), 0.0 ) ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected myDirectionalScan->getItemDistanceAtDistanceIndex(%i):  %f\n",
                     textIndex, myDirectionalScan->getItemDistanceAtDistanceIndex(textIndex));
        return 1;
    }

    while( ++textIndex < 5 ){
        if( floatsUnequal( myDirectionalScan->getItemDistanceAtDistanceIndex(textIndex), 1.0 ) ){
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected myDirectionalScan->getItemDistanceAtDistanceIndex(%i):  %f\n",
                         textIndex, myDirectionalScan->getItemDistanceAtDistanceIndex(textIndex));
            return 1;
        }
    }

    while( ++textIndex < 9 ){
        if( floatsUnequal( myDirectionalScan->getItemDistanceAtDistanceIndex(textIndex), 1.414214 ) ){
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected myDirectionalScan->getItemDistanceAtDistanceIndex(%i):  %f\n",
                         textIndex, myDirectionalScan->getItemDistanceAtDistanceIndex(textIndex));
            return 1;
        }
    }

    while( ++textIndex < 13 ){
        if( floatsUnequal( myDirectionalScan->getItemDistanceAtDistanceIndex(textIndex), 2.0 ) ){
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected myDirectionalScan->getItemDistanceAtDistanceIndex(%i):  %f\n",
                         textIndex, myDirectionalScan->getItemDistanceAtDistanceIndex(textIndex));
            return 1;
        }
    }

    while( ++textIndex < 21 ){
        if( floatsUnequal( myDirectionalScan->getItemDistanceAtDistanceIndex(textIndex), 2.236068 ) ){
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected myDirectionalScan->getItemDistanceAtDistanceIndex(%i):  %f\n",
                         textIndex, myDirectionalScan->getItemDistanceAtDistanceIndex(textIndex));
            return 1;
        }
    }

    while( ++textIndex < myDirectionalScan->_total_size ){
        if( floatsUnequal( myDirectionalScan->getItemDistanceAtDistanceIndex(textIndex), 2.828427 ) ){
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected myDirectionalScan->getItemDistanceAtDistanceIndex(%i):  %f\n",
                         textIndex, myDirectionalScan->getItemDistanceAtDistanceIndex(textIndex));
            return 1;
        }
    }



    /*
     recalling      this is basically our 4x4 grid...
 -2,-2 * # * *
     * * # * *
     # # ~ # #
     * * # * *
     * * # * 2,2

     next up: vectors, so for our orig grid vectors should be like this, since each position may be a unit of some other vector:
     -1,-1  -1,-2  +0,-1  +1,-2  +1,-1
     -2,-1  -1,-1  +0,-1  +1,-1  +2,-1
     -1,+0  -1,+0  +0,+0  +1,+0  +1,+0
     -2,+1  -1,+1  +0,+1  +1,+1  +2,+1
     -1,+1  -1,+2  +0,+1  +1,+2  +1,+1

     the unique vectors are tough to see... so we will veriy the total is the expected total 8 + 8 + 1 in this case

     since each location also has a vectorInterval (unit of orig vector) we anticipate this
     2 * 2 * 2
     * 1 1 1 *
     2 1 0 1 2
     * 1 1 1 *
     2 * 2 * 2

     where any * is also 1 (left empty to illustrate teh vectors that have only one instance)
     so this is cleary unique from the float distance, its the interval of some arbitarary vector needed to reach a pixel
     */

    if(myDirectionalScan->_possible_vector_total != 17){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected myDirectionalScan->_possible_vector_total: %i\n",
                     myDirectionalScan->_possible_vector_total );
        return 1;
    }

    textIndex = myDirectionalScan->indexGet2d(0,0); // center point
    if( !vectorExpectationPasses( *myDirectionalScan->getVector(textIndex),
                                 myDirectionalScan->getVectorInterval(textIndex),
                                SDL_Point{0,0}, 1 ) ){return 1;}


    textIndex = myDirectionalScan->indexGet2d(1,1);
    if( !vectorExpectationPasses( *myDirectionalScan->getVector(textIndex),
                                 myDirectionalScan->getVectorInterval(textIndex),
                                 SDL_Point{1,1}, 1 ) ){return 1;}
    textIndex = myDirectionalScan->indexGet2d(2,2);
    if( !vectorExpectationPasses( *myDirectionalScan->getVector(textIndex),
                                 myDirectionalScan->getVectorInterval(textIndex),
                                 SDL_Point{1,1}, 2 ) ){return 1;}


    textIndex = myDirectionalScan->indexGet2d(-1,-1);
    if( !vectorExpectationPasses( *myDirectionalScan->getVector(textIndex),
                                 myDirectionalScan->getVectorInterval(textIndex),
                                 SDL_Point{-1,-1}, 1 ) ){return 1;}
    textIndex = myDirectionalScan->indexGet2d(-2,-2);
    if( !vectorExpectationPasses( *myDirectionalScan->getVector(textIndex),
                                 myDirectionalScan->getVectorInterval(textIndex),
                                 SDL_Point{-1,-1}, 2 ) ){return 1;}


    textIndex = myDirectionalScan->indexGet2d(-1,1);
    if( !vectorExpectationPasses( *myDirectionalScan->getVector(textIndex),
                                 myDirectionalScan->getVectorInterval(textIndex),
                                 SDL_Point{-1,1}, 1 ) ){return 1;}
    textIndex = myDirectionalScan->indexGet2d(-2,2);
    if( !vectorExpectationPasses( *myDirectionalScan->getVector(textIndex),
                                 myDirectionalScan->getVectorInterval(textIndex),
                                 SDL_Point{-1,1}, 2 ) ){return 1;}


    textIndex = myDirectionalScan->indexGet2d(1,-1);
    if( !vectorExpectationPasses( *myDirectionalScan->getVector(textIndex),
                                 myDirectionalScan->getVectorInterval(textIndex),
                                 SDL_Point{1,-1}, 1 ) ){return 1;}
    textIndex = myDirectionalScan->indexGet2d(2,-2);
    if( !vectorExpectationPasses( *myDirectionalScan->getVector(textIndex),
                                 myDirectionalScan->getVectorInterval(textIndex),
                                 SDL_Point{1,-1}, 2 ) ){return 1;}

    textIndex = myDirectionalScan->indexGet2d(1,0);
    if( !vectorExpectationPasses( *myDirectionalScan->getVector(textIndex),
                                 myDirectionalScan->getVectorInterval(textIndex),
                                 SDL_Point{1,0}, 1 ) ){return 1;}
    textIndex = myDirectionalScan->indexGet2d(2,0);
    if( !vectorExpectationPasses( *myDirectionalScan->getVector(textIndex),
                                 myDirectionalScan->getVectorInterval(textIndex),
                                 SDL_Point{1,0}, 2 ) ){return 1;}


    textIndex = myDirectionalScan->indexGet2d(-1,0);
    if( !vectorExpectationPasses( *myDirectionalScan->getVector(textIndex),
                                 myDirectionalScan->getVectorInterval(textIndex),
                                 SDL_Point{-1,0}, 1 ) ){return 1;}
    textIndex = myDirectionalScan->indexGet2d(-2,0);
    if( !vectorExpectationPasses( *myDirectionalScan->getVector(textIndex),
                                 myDirectionalScan->getVectorInterval(textIndex),
                                 SDL_Point{-1,0}, 2 ) ){return 1;}


    textIndex = myDirectionalScan->indexGet2d(0,1);
    if( !vectorExpectationPasses( *myDirectionalScan->getVector(textIndex),
                                 myDirectionalScan->getVectorInterval(textIndex),
                                 SDL_Point{0,1}, 1 ) ){return 1;}
    textIndex = myDirectionalScan->indexGet2d(0,2);
    if( !vectorExpectationPasses( *myDirectionalScan->getVector(textIndex),
                                 myDirectionalScan->getVectorInterval(textIndex),
                                 SDL_Point{0,1}, 2 ) ){return 1;}


    textIndex = myDirectionalScan->indexGet2d(0,-1);
    if( !vectorExpectationPasses( *myDirectionalScan->getVector(textIndex),
                                 myDirectionalScan->getVectorInterval(textIndex),
                                 SDL_Point{0,-1}, 1 ) ){return 1;}
    textIndex = myDirectionalScan->indexGet2d(0,-2);
    if( !vectorExpectationPasses( *myDirectionalScan->getVector(textIndex),
                                 myDirectionalScan->getVectorInterval(textIndex),
                                 SDL_Point{0,-1}, 2 ) ){return 1;}


    // and one of the lone ones for good measure...
    textIndex = myDirectionalScan->indexGet2d(1,2);
    if( !vectorExpectationPasses( *myDirectionalScan->getVector(textIndex),
                                 myDirectionalScan->getVectorInterval(textIndex),
                                 SDL_Point{1,2}, 1 ) ){return 1;}

    //DirectionalElement* elements  =
    // computing vectors is good because we can rule them out and whole sets of destinations
    // however at best each coordinateIndex may merely belong to a vector that is eitehr still under examination
    // or already ruled out
    // its is the index of vectors that are ruled out that we'll need to reset - the search space is the same


    glm::vec3 dest = glm::vec3(13, 23, 56); // we are looking for this vec3 in the hayfield
    SDL_Point result;

    result = myDirectionalScan->getOffsetForBestMatch(dest, &getDistanceForVec3s, &getItemForPoint);
    if( result.x != 1 || result.y != 2){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected myDirectionalScan->getOffsetForBestMatch wanted 1,2:  got (%i, %i)\n",
                     result.x, result.y);
        return 1;
    }

    result = myDirectionalScan->getOffsetForBestMatch(dest, &getDistanceForVec3s, &getItemForPointTest2);
    if( result.x != 2 || result.y != 2){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected myDirectionalScan->getOffsetForBestMatch (closest match non exact) wanted 1,2:  got (%i, %i)\n",
                     result.x, result.y);
        return 1;
    }

    myDirectionalScan->_vector_search_cutoff = -1; // this would only work for 2 steps or greater as it is now... since the first step is always the best match for that vector, and once we take the second step we find our match
    result = myDirectionalScan->getOffsetForBestMatch(dest, &getDistanceForVec3s, &getItemForPointTest2);
    if( result.x != 2 || result.y != 2){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected myDirectionalScan->getOffsetForBestMatch (_vector_search_cutoff) wanted 1,2:  got (%i, %i)\n",
                     result.x, result.y);
        return 1;
    }

    SDL_free(myDirectionalScan);

    // todo try to allocate a larger scan and perform some cursory evaluations about it....

    SDL_Log(" ----------- Now reachign test part B - DirectionalScan 5 ----------- ");

    myDirectionalScan = new DirectionalScan<SDL_Point, glm::vec3>(5, &vectorIsComponent);

    
    if(myDirectionalScan->_maxSize != 5){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected _maxSize: %i\n",
                     myDirectionalScan->_maxSize );
        return 1;
    }

    if(myDirectionalScan->_startIndex != -5){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected _startIndex: %i\n",
                     myDirectionalScan->_startIndex );
        return 1;
    }

    if(myDirectionalScan->_endIndex != 6){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected _endIndex: %i\n",
                     myDirectionalScan->_endIndex );
        return 1;
    }

    if(myDirectionalScan->_rowLen != 11){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected _rowLen: %i\n",
                     myDirectionalScan->_rowLen );
        return 1;
    }

    if(myDirectionalScan->_total_size != 121){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected _total_size: %i\n",
                     myDirectionalScan->_total_size );
        return 1;
    }

    if(myDirectionalScan->_quad_offset != 60){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected _quad_offset: %i\n",
                     myDirectionalScan->_quad_offset );
        return 1;
    }

    if(myDirectionalScan->_possible_vector_total != 81){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected myDirectionalScan->_possible_vector_total: %i\n",
                     myDirectionalScan->_possible_vector_total );
        return 1;
    }

    SDL_Log("All directional scan tests passed.");
}


/*

 colorFromSurface(newSurface, hwh -x , hwh -y , &tmpColor);

 glm::vec3 current;
 float dist;
 float lastDist, bestDist;

 // test?
 current = glm::vec3(tmpColor.r, tmpColor.g, tmpColor.b);
 lastDist = glm::distance(dest,current);
 bestDist = lastDist;
 SDL_Log("starting distance is %f", lastDist);

 //glm::fastDistance(<#const detail::tvec4<valType> &x#>, <#const detail::tvec4<valType> &y#>)

 int bestx = x;
 int besty = y;

 int dir_x = 0;
 int dir_y = 0;
 int next_dir_x=0;
 int next_dir_y=0;
 int counter=0;


 int sideLen = 1;
 int sideCtr = 0;
 int loopCtr = 0;

 bool inBounds = false;

 float upDist;
 float dnDist;
 float lfDist;
 float rtDist;
 // sideLength = 1

 while( counter < 100 ){
 counter++;

 inBounds = colorFromSurface(newSurface, hwh -x , hwh -(y - 1), &tmpColor);
 if( inBounds ){
 current = glm::vec3(tmpColor.r, tmpColor.g, tmpColor.b);
 upDist = glm::distance(dest,current);
 }else{
 upDist = 999999999.0;
 }
 inBounds = colorFromSurface(newSurface, hwh -x , hwh -(y + 1), &tmpColor);
 if( inBounds ){
 current = glm::vec3(tmpColor.r, tmpColor.g, tmpColor.b);
 dnDist = glm::distance(dest,current);
 }else{
 dnDist = 999999999.0;
 }
 inBounds = colorFromSurface(newSurface, hwh -(x - 1), hwh -y, &tmpColor);
 if( inBounds ){
 current = glm::vec3(tmpColor.r, tmpColor.g, tmpColor.b);
 lfDist = glm::distance(dest,current);
 }else{
 lfDist = 999999999.0;
 }
 inBounds = colorFromSurface(newSurface, hwh -(x + 1), hwh -y, &tmpColor);
 if( inBounds ){
 current = glm::vec3(tmpColor.r, tmpColor.g, tmpColor.b);
 rtDist = glm::distance(dest,current);
 }else{
 rtDist = 999999999.0;
 }

 // 2 fn, bestDir and dirDist

 float min_ud = SDL_min(upDist, dnDist);
 float min_lr = SDL_min(lfDist, rtDist);
 float min_dis =SDL_min(min_ud, min_lr);

 if( min_dis == upDist ){
 y-=1;
 next_dir_x = 0;
 next_dir_y = -1;
 }else if(min_dis == dnDist ){
 y+=1;
 next_dir_x = 0;
 next_dir_y = 1;
 }else if(min_dis == lfDist ){
 x-=1;
 next_dir_x = -1;
 next_dir_y = 0;
 }else if(min_dis == rtDist ){
 x+=1;
 next_dir_x = 1;
 next_dir_y = 0;
 }

 dist = min_dis;

 SDL_Log(" closest this round %f ?", dist);

 if( dist < bestDist ){
 SDL_Log(" and it is better than %f ?", bestDist);
 bestx = x;
 besty = y;
 bestDist = dist;
 }

 if( dist == 0.0 ){
 SDL_Log("Looks like we found it!");
 SDL_Log(" ----------- A BEST MATCH WAS FOUND!!! ------------ ");
 *outx = bestx;
 *outy = besty;
 return true;
 }

 if( next_dir_x == dir_x * -1 && next_dir_y == dir_y * -1 ){
 SDL_Log(" ----------- LOOKS LIKE REVERSI... lets abort the looping... ------------ ");
 // in some cases a better match CAN be found if we look further or chagne the rules bout which direction we move when the distance is the same...


 loopCtr++;
 if( loopCtr > 5 ){
 break;
 }
 }

 dir_x = next_dir_x;
 dir_y = next_dir_y;

 }

 if( bestx != *outx || besty != *outy ){
 SDL_Log(" ----------- A BETTER (closer) MATCH WAS FOUND!!! ------------ ");
 *outx = bestx;
 *outy = besty;
 return true;

 }

 return false;
 int distanceToMove = 1;
 int directionsLooped = 0;
 while(counter < 100){ // max moves

 y+= dir_y;
 x+= dir_x;

 colorFromSurface(newSurface, hwh -x , hwh -y , &tmpColor);

 current = glm::vec3(tmpColor.r, tmpColor.g, tmpColor.b);

 dist = glm::fastDistance(dest,current);

 SDL_Log("distance is %f", dist);

 if( dist < bestDist ){
 SDL_Log(" and it is better than %f ?", bestDist);
 bestx = x;
 besty = y;
 bestDist = dist;
 }

 if( dist == 0.0 ){
 SDL_Log("Looks like we found it!");
 SDL_Log(" ----------- A BETTER MATCH WAS FOUND!!! ------------ ");
 *outx = bestx;
 *outy = besty;
 return true;
 }

 if( dist <= lastDist ){
 // getting closer (keep going?)

 }else{
 // getting further
 // first undo our last move....
 y-= dir_y;
 x-= dir_x;

 if( dir_y == distanceToMove ){
 if( dir_x == 0 ){
 dir_x = distanceToMove;
 }else if( dir_x == distanceToMove ){
 dir_x = -distanceToMove;
 }else{
 dir_y = 0;
 }
 }

 if( dir_x == -distanceToMove ){
 if( dir_y == 0 ){
 dir_y = distanceToMove;
 }else if( dir_y == distanceToMove ){
 dir_y = -distanceToMove;
 }else{
 dir_x = 0;
 }
 }

 if( dir_y == -distanceToMove ){
 if( dir_x == 0 ){
 dir_x = -distanceToMove;
 }else if( dir_x == -distanceToMove ){
 dir_x = distanceToMove;
 }else{
 dir_y = 0;
 }
 }

 if( dir_x == distanceToMove ){
 if( dir_y == 0 ){
 dir_y = -distanceToMove;
 }else if( dir_y == -distanceToMove ){
 dir_y = distanceToMove;
 }else{
 dir_x = 0;
 distanceToMove+=2;
 }
 }
 }

 lastDist = dist;
 counter++;

 }


 if( bestx != *outx || besty != *outy ){
 SDL_Log(" ----------- A BETTER (closer) MATCH WAS FOUND!!! ------------ ");
 *outx = bestx;
 *outy = besty;
 return true;

 }


 // impossible goal:
 //SDL_assert(seekClr->r == tmpColor.r && seekClr->r == tmpColor.r && seekClr->r == tmpColor.r);

 return false;


 int searchArea = 50; // TODO experiment with LARGE serach area to uncover bugs....
 int halfSearchArea = searchArea * 0.5;

 int searchRows = searchArea;
 int serachCols = searchArea;

 x -= halfSearchArea;
 y -= halfSearchArea;


 if( x < 0 ){
 serachCols -= -x;
 x = 0;
 }

 if( y < 0 ){
 searchRows -= -y;
 y = 0;
 }

 if( x + serachCols > newSurface->w ){
 serachCols -= (x + serachCols) - newSurface->w;
 }

 if( y + searchRows > newSurface->h ){
 searchRows -= (y + searchRows) - newSurface->h;
 }

 serachCols = x+serachCols;
 searchRows = y+searchRows;

 int bestRow = 0;
 int bestCol = 0;

 SDL_Log("ABOUT TO BEGIN LOOKING.... x:%i y:%i", x, y);


 for( int row = x; row < serachCols; x++ ){
 for( int col = y; col < searchRows; x++ ){

 SDL_Log("LOOKING");
 colorFromSurface(newSurface, row, col, &tmpColor);

 if( seekClr->r == tmpColor.r && seekClr->r == tmpColor.r && seekClr->r == tmpColor.r ){
 SDL_Log(" ----------- A BETTER MATCH WAS FOUND!!! ------------ ");
 *outx = row;
 *outy = col;
 return true;
 }


 }
 }


 return false;// we didn't move anything...

 */
