//
//

#ifndef ColorPick_iOS_SDL_directionalScan_h
#define ColorPick_iOS_SDL_directionalScan_h

// probably SDL_Point, glm::vec3
template <typename dimensonalType, typename destType>
struct DirectionalScan
{

    typedef float (*aDistanceFunction)(destType* item);
    typedef destType (*aGetItem2dFn)(int x, int y);


    typedef struct DirectionalElement
    {
        DirectionalElement(dimensonalType* iPoint, float iDist){
            coordinate = *iPoint;
            distanceFromCenter = iDist;
        }

        dimensonalType coordinate; // its probably SDL_Point
        float distanceFromCenter;

    } DirectionalElement;



    DirectionalElement* coordinateSpace;

    dimensonalType* coordinateIndex;
    bool* bitmap;
    float* distances;

    //dimensonalType* coordinateSpaceMap;



    int _maxSize;
    int _startIndex;
    int _endIndex;
    int _rowLen;
    int _quad_offset;

    int _total_size;

    DirectionalScan(){}
    DirectionalScan(int pmaxSize){

//        SDL_Log("const char ize %i", sizeof(const char*));
//        SDL_Log("const char ize %i", sizeof(char*));
//        SDL_Log("const char ize %i", sizeof(const char));
//        SDL_Log("const char ize %i", sizeof(char));
//        SDL_Log("const char ize %i", sizeof(int));
//        SDL_Log("const char ize %i", sizeof(Uint8));
//        SDL_Log("const char ize %i", sizeof(Uint16));
//        SDL_Log("const char ize %i", sizeof(float *));
//        SDL_Log("const char ize %i", sizeof(bool *));
//        SDL_Log("pint ize %i", sizeof(dimensonalType));
//SDL_Log("DirectionalElement ize %i", sizeof(DirectionalElement));
//        SDL_Log("bool ize %i", sizeof(bool));
//        SDL_Log("float ize %i", sizeof(float));

        _maxSize = pmaxSize;
        _startIndex = -_maxSize;
        _endIndex = _maxSize+1;
        _rowLen = ((_maxSize * 2) + 1);

        _quad_offset = (_rowLen * _maxSize) + _maxSize;

        // max size is used in one direction and extrapolated to 4
        // pmaxSize in each direction including centroid
        _total_size = _rowLen * _rowLen; // 2d

        // we are always centered on 0,0 so...



        // we really only use the index during generation.....

        coordinateSpace = (DirectionalElement*)SDL_malloc( sizeof(DirectionalElement)*_total_size );

        coordinateIndex = (dimensonalType*)SDL_malloc( sizeof(dimensonalType)*_total_size );
        bitmap = (bool*)SDL_malloc( sizeof(dimensonalType)*_total_size );
        distances = (float*)SDL_malloc( sizeof(dimensonalType)*_total_size );

        for( int y=_startIndex; y<_endIndex; y++ ){
            for( int x=_startIndex; x<_endIndex; x++ ){



                int myIndex = indexGet2d(x,y);

                dimensonalType* myItem = new dimensonalType();
                // assignment fn? ?
                myItem->x = x;
                myItem->y = y;

                float distance = SDL_sqrtf((x*x) + (y*y));

                coordinateSpace[myIndex] = DirectionalElement(myItem, distance);

                coordinateIndex[myIndex] = *myItem;
                bitmap[myIndex] = false; // do we really need bitmap??/ maybe we can just use nullptr...
                distances[myIndex] = SDL_sqrtf((x*x) + (y*y));

                //SDL_Log("Distance %2i is: %f" , myIndex, distances[myIndex]);

//                dimensonalType* result = indexGet2d(x, y);

            }
        }

     //   SDL_qsort(<#void *base#>, <#size_t nmemb#>, <#size_t size#>, <#int (*compare)(const void *, const void *)#>)

        // next we need to count how many vectors we need to reach every pixel in the space....
        // so we can even allocate our array of vectors properly.... we sort of do the same thing 2x
        int vectorCount;
        // we can use bitmap to help

        for( int x=_startIndex; x<_endIndex; x++ ){
            for( int y=_startIndex; y<_endIndex; y++ ){

                int myIndex = indexGet2d(x,y);



            }
        }

    }

    int indexGet2d(int x, int y){
        return ((_rowLen * y) + x) + _quad_offset;
        //return coordinateIndex[/*the index*/];
    }


    float getDistance(int index){
        return coordinateSpace[index].distanceFromCenter;
    }

};




#endif
