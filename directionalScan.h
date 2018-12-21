//
//

#ifndef ColorPick_iOS_SDL_directionalScan_h
#define ColorPick_iOS_SDL_directionalScan_h


static int vectorIsComponent(SDL_Point componentVec, SDL_Point searchVec){
    // is search vector reducable to initial (component) vector?  what scale?  otherwise 0 ?
    // we need to compare: direction and length, one way or another

    //SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "vectorIsComponent BEGIN (%i %i) (%i %i)", componentVec.x, componentVec.y, searchVec.x, searchVec.y);

    if( componentVec.x == 0 && componentVec.y == 0 ) return 0; // lets avoid division by zero anyway !  this just avoids a little extra calculation, for what happens to be our first vector - arguably we should not check this all the time...
    if( (searchVec.x < 0 && componentVec.x > 0) || (searchVec.x > 0 && componentVec.x < 0)  ) return 0; // vector is going a different direction... cannot be equiv
    if( (searchVec.y < 0 && componentVec.y > 0) || (searchVec.y > 0 && componentVec.y < 0)  ) return 0; // vector is going a different direction... cannot be equiv
    if( componentVec.x == 0 && searchVec.x != 0 ) return 0;
    if( componentVec.y == 0 && searchVec.y != 0 ) return 0;

    float xScaled = 0.0;
    float yScaled = 0.0;

    if( componentVec.x != 0 ){
        xScaled = (float)searchVec.x / (float)componentVec.x;
    }
    if( componentVec.y != 0 ){
        yScaled = (float)searchVec.y / (float)componentVec.y;
    }

    int xScaledInt = int(xScaled); // SDL_floorf is bad with negative numbers
    int yScaledInt = int(yScaled);

    float xDiff = xScaled - xScaledInt;
    float yDiff = yScaled - yScaledInt;

    if( SDL_fabs(xDiff) < 0.000001 && SDL_fabs(yDiff) < 0.000001 ){
        // they better not both be zero if we reach here... !

        // the vector is a tidy scaler of the other one???

        // if either component is non zero though, teh scale should be identical....
        if( xScaledInt == 0 ){
            //SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "vectorIsComponent y component return");
            return yScaledInt;
        }else if( yScaledInt == 0 ){
            //SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "vectorIsComponent x component return");
            return xScaledInt;
        }else{
            if( yScaledInt == xScaledInt ){
                //SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "vectorIsComponent linear return");
                return yScaledInt;
            }
        }
    }

    //SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "vectorIsComponent DEFAULT RETURN");
    return 0;
}

static float getDistanceForVec3s(glm::vec3 a, glm::vec3 b){
    return glm::distance(a,b);
}

// probably SDL_Point, glm::vec3
template <typename dimensonalType, typename destType>
struct DirectionalScan
{

    typedef int (*aDimensionalTypeIsDimensionOfdimensionalTypeComparitor)(dimensonalType a, dimensonalType b); // the way we use this, its more of an "is new vector reducable to some quantity of initial vector, if so what integer scale"

    typedef float (*aDistanceFunction)(destType dest, destType current); // why float ??? maybe okay.... glm::distance
    typedef destType (*aGetItem2dFn)(int x, int y); // not really needed.... ?
    typedef destType (*aGetItemFn)(dimensonalType point);


    typedef struct DirectionalElement
    {
        DirectionalElement(dimensonalType* iPoint, int iOrigIndex, float iDist){
            coordinate = *iPoint;
            distanceFromCenter = iDist;
            origIndex = iOrigIndex;
            vectorIndex = 0;
            vectorInterval = 0;
        }

        int origIndex;
        int vectorIndex;
        int vectorInterval; // of the vector how many units are we
        dimensonalType coordinate; // its probably SDL_Point
        float distanceFromCenter;

    } DirectionalElement;

    static int cmpDirectionalElementsDistance(const void *A, const void *B){
        const DirectionalElement *a = (const DirectionalElement *) A;
        const DirectionalElement *b = (const DirectionalElement *) B;
        float diff = a->distanceFromCenter - b->distanceFromCenter;
        if( diff > 0.0 ){
            return (int)SDL_ceilf(diff);
        }else if( diff < 0.0 ){
            return (int)SDL_floorf(diff);
        }else{
            return 0;
        }
    }

    typedef struct DirectionalVector
    {
        DirectionalVector(dimensonalType pVector){
            // a given angle integer (degrees) may have a list of vectors.... whihc we could invalidate
            // for a given distance, there is a total number of vectors, and they may be sorted by angle
            // then there are all the vectors sorted by angle (except 0,0), which should maybe be distance 0 (its not) vectorInterval is 1
            // interval is not distance since thats per vector....
            // instead maybe we could count the loops of distanceBasedCoordinateSpaceIndex and get a feeling for when we step out in distance
            // then we have distance index....
            // and then for a given distance if 2 tangental vectors are invalid perhaps all vectors between those can also be invalidated....
            vector = pVector; // assignment of struct... is a copy
            float d_angle = SDL_atan2(vector.x, vector.y);
            angle = ((int)glm::degrees(d_angle) + 360) % 360;
            //SDL_Log("VECTOR: %i,%i and atan2: %f %i", vector.x, vector.y, d_angle, angle);
        }
        dimensonalType vector; // its probably SDL_Point
        int angle; // degrees??
    } DirectionalVector;

    typedef struct VectorValidity
    {
        VectorValidity(int iVectorIndex){
            valid = true;
            vectorIndex = iVectorIndex;
            vectorInterval = 0;
            //distIndexOfBestMatch = 0; // this really needed???
            bestMatch = 100000.0;
            bestMatchAt = 0;
        }
        bool valid;
        int vectorIndex;
        int vectorInterval; // walk / interval counter (how many of this vector have we stepped)
        //int distIndexOfBestMatch;
        float bestMatch;
        int bestMatchAt; // interval where we found the best match.... typically 1 by default for the first match, zero if never checked
    } VectorValidity;


    aDimensionalTypeIsDimensionOfdimensionalTypeComparitor dimensionalTypeIsDimensionOfdimensionalType;

    DirectionalElement* coordinateSpace;
    int* distanceBasedCoordinateSpaceIndex;
    VectorValidity* default_search_area;


    dimensonalType* coordinateIndex;
    bool* bitmap;
    float* distances;

    DirectionalVector* possible_vectors;
    int _possible_vector_total;

    //dimensonalType* coordinateSpaceMap;

    int _maxSize;
    int _startIndex;
    int _endIndex;
    int _rowLen;
    int _quad_offset;

    int _total_size;

    int _vector_search_cutoff = 2; // for any given vector, if we take this many steps and teh results are not improving, stop going in that direction

    float lastBestDistance = 0;

    //DirectionalScan(){}
    DirectionalScan(int pmaxSize, aDimensionalTypeIsDimensionOfdimensionalTypeComparitor pDimensionalTypeIsDimensionOfdimensionalType){

        dimensionalTypeIsDimensionOfdimensionalType = pDimensionalTypeIsDimensionOfdimensionalType;
//        SDL_Log("const char ize %i", sizeof(const char*));
//        SDL_Log("const char ize %i", sizeof(char*));
//        SDL_Log("const char ize %i", sizeof(const char));
//        SDL_Log("const char ize %i", sizeof(char));
//        SDL_Log("const char ize %i", sizeof(int));
//        SDL_Log("const char ize %i", sizeof(Uint8));
//        SDL_Log("const char ize %i", sizeof(Uint16));
//        SDL_Log("const char ize %i", sizeof(float *));
//        SDL_Log("const char ize %i", sizeof(bool *));
//        SDL_Log("const char ize %i", sizeof(bool));
//        SDL_Log("pint ize %i", sizeof(dimensonalType));
//SDL_Log("DirectionalElement ize %i", sizeof(DirectionalElement));
//        SDL_Log("bool ize %i", sizeof(bool));
//        SDL_Log("float ize %i", sizeof(float));

        _vector_search_cutoff = 3;

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
        bitmap = (bool*)SDL_malloc( sizeof(bool)*_total_size );
        distances = (float*)SDL_malloc( sizeof(float)*_total_size ); // we really do not need this anymore?!

        for( int y=_startIndex; y<_endIndex; y++ ){
            for( int x=_startIndex; x<_endIndex; x++ ){


                int myIndex = indexGet2d(x,y);

                dimensonalType* myItem = new dimensonalType();
                // assignment fn? ?
                myItem->x = x;
                myItem->y = y;

                float distance = SDL_sqrtf((x*x) + (y*y));

                coordinateSpace[myIndex] = DirectionalElement(myItem, myIndex, distance);
                //origCoordinateSpace[myIndex] = coordinateSpace[myIndex]; // this is a strange one... so is this assigning the reference (in which case we double allocated) or ....  it makes sense if we just use memcpy after entire coordinateSpace is created though... but then it won't be the same reference.... maybe we really want to store a pointer here instead though... and only alloc the sizeof the pointer... we should really jsut iterate after sorting and use the origIndex property though....

                coordinateIndex[myIndex] = *myItem; // not sure this is really used?
                bitmap[myIndex] = false; // do we really need bitmap??/ maybe we can just use nullptr...
                distances[myIndex] = distance;

                //SDL_Log("Distance %2i is: %f" , myIndex, distances[myIndex]);
//                dimensonalType* result = indexGet2d(x, y);
            }
        }

        // this allocates a lot more memory than is strictly needed.... and leads us to point to two distinct DirectionalElement - realistically we only need the int of the index and we can lookup and sort based on that!  then we can know its the same directional element (not that we'll modify it anyway)
        DirectionalElement* distanceBasedCoordinateSpace = (DirectionalElement*)SDL_malloc( sizeof(DirectionalElement)*_total_size );
        SDL_memcpy(distanceBasedCoordinateSpace, coordinateSpace, sizeof(DirectionalElement)*_total_size);
        SDL_qsort(distanceBasedCoordinateSpace, _total_size, sizeof(DirectionalElement), cmpDirectionalElementsDistance);
        // so lets use a temp item and clean this up...

        distanceBasedCoordinateSpaceIndex = (int*)(SDL_malloc(sizeof(int)*_total_size));
        for( int t=0; t<_total_size; t++ ){
            distanceBasedCoordinateSpaceIndex[t] = distanceBasedCoordinateSpace[t].origIndex;
        }
        SDL_free(distanceBasedCoordinateSpace);


     //   SDL_qsort(<#void *base#>, <#size_t nmemb#>, <#size_t size#>, <#int (*compare)(const void *, const void *)#>)

        
        // next we need to count how many vectors we need to reach every pixel in the space....
        // so we can even allocate our array of vectors properly.... we sort of do the same thing 2x
        // we'lll just over allocate the first time...
        DirectionalVector* temp_vectors = (DirectionalVector*)SDL_malloc( sizeof(DirectionalVector)*_total_size );

        int vectorCounter = 0;
        int interval = 0; // it should never remain zero....
        DirectionalElement *de;
        bool found;
        int v;

        // we loop in distance order to create the lower vectors first, then we can see if the upper ones are a scaled lower vector.
        for( int i=0; i<_total_size; i++ ){
            de = getItemAtDistanceIndex(i);

            found = false;
            for( v=0; v<vectorCounter; v++ ){
                interval = dimensionalTypeIsDimensionOfdimensionalType(temp_vectors[v].vector, de->coordinate);
                if( interval > 0 ){
                    found=true;
                    break;
                }
            }

            if( !found ){
                v = vectorCounter;
                temp_vectors[vectorCounter++] = DirectionalVector(de->coordinate);
                interval = 1;
            }

            de->vectorIndex = v;
            de->vectorInterval = interval;
        }


        // now lets cleanup the size....
        _possible_vector_total = vectorCounter;
        possible_vectors = (DirectionalVector*)SDL_malloc( sizeof(DirectionalVector)*_possible_vector_total );
        SDL_memcpy(possible_vectors, temp_vectors, sizeof(DirectionalVector)*_possible_vector_total);
        SDL_free(temp_vectors);



        // we have a list of vectors
        // each DirectionalElement has a vector's index
        // each DirectionalElement has an interval of that vector


        // next up - lets create the default "vector validity" mapping...
        // this can at minimium consist of bool
        // cana also have last best rating???? and what interval that occured at??
        //  - bool valid
        //  - int walkCounter // interval counter
        //  - int bestMatchAt // what interval we had our best match (probably 1, unless it is improving)
        // then I suppose if the current interval is like 5 away from our best match we went way too far ?  the best match should remain close to the interval otherwise we are going the wrong way!

        default_search_area = (VectorValidity*)SDL_malloc( sizeof(VectorValidity)*_possible_vector_total );
        for( v=0; v<_possible_vector_total; v++ ){
            default_search_area[v] = VectorValidity(v);
        }

        // we probably need some other default thing to map where the best matches were.... unless thats external....
        //unless the output of whatever function gets called is actually a single DirectionalElement.coordinate (of the best match?)
        // possibly also the distance of the match would be helpful...

        // so what we search for here will be the same at each coordinate.... arguably this could vary per coordinate!  but it may be unclear how much it might vary due to scale/rotational effects..
    }


    dimensonalType getOffsetForBestMatch(destType dest, aDistanceFunction getDistance, aGetItemFn getItem){
        // see also searchSurfaceForColor....
        /*
         probable args:

         typedef float (*aDistanceFunction)(destType dest, destType current); // why float ??? maybe okay.... glm::distance
         typedef destType (*aGetItem2dFn)(int x, int y); // not really needed.... ?
         typedef destType (*aGetItemFn)(dimensonalType point); // todo - somehow this will need to know how to offset the point, right?

         // current distance??? or is this just the first vector 0,0 ....
         // distance function needs 2 args, so we will probalby need destType seekClr (as vec3 "dest")

         */

        SDL_Point result = {0,0}; // hopefully we find a good result below....

        VectorValidity* search_area = (VectorValidity*)SDL_malloc( sizeof(VectorValidity)*_possible_vector_total );
        SDL_memcpy(search_area, default_search_area, sizeof(VectorValidity)*_possible_vector_total);

        destType current;
        float dist;
        float bestDist = 100000.0;

        DirectionalElement* bestDe = getItemAtDistanceIndex(0);
        DirectionalElement* de;
        VectorValidity* vv;
        dimensonalType* vector;
        int vecIndex;
        for( int i=0; i<_total_size; i++ ){
            de = getItemAtDistanceIndex(i);
            vecIndex = de->vectorIndex; // see if vecIndex is needed
            vv = &search_area[vecIndex];

            if( vv->valid ){
                //vector = getVector(de); // not sure we really need this var

                // we need a way to track if all our vectors are exhausted other then reaching end of loop ?
                // and other then win conditon of exact match ?

                current = getItem(de->coordinate); //ssee somehow this will need to know how to offset the point, right? ?
                dist = getDistance(dest, current);

                if( dist == 0.0){

                    // we found it
                    result = de->coordinate;
                    lastBestDistance = 0; // exact match!
                    return result;
                }else{

                    if( dist < bestDist ){
                        bestDist = dist;
                        bestDe = de;
                    }

                    vv->vectorInterval = de->vectorInterval; // we are moving through our search space...

                    if( dist < vv->bestMatch ){

                        vv->bestMatch = dist;
                        //vv->distIndexOfBestMatch = i; // maybe we should just store de instead?  or better yet not need this
                        vv->bestMatchAt = vv->vectorInterval;

                        // todo it seems superficially at least we should track the best vector?? for the closest match (bestDe)?
                        // otherwise we are left to scan the valid vectors at the end of the loop...

                    }else{
                        // not the best matchf for this vector.... we should see about invalidating it...
                        if( vv->vectorInterval - vv->bestMatchAt > _vector_search_cutoff ){
                            // we checked "5" and it either stayed the same or got worse... just cease checking this vector
                            vv->valid = false;

                            // potentially we should invalidate surrounding vectors too.... or all between 2 known bad vectors.... ?
                            // we may also prioritize hot vectors or ranges of vectors?
                        }
                    }
                }
            }else{
                //SDL_Log("skipping invalid vector %i %i ", getVector(de)->x, getVector(de)->y);
            }
        }

        lastBestDistance = bestDist;
        result = bestDe->coordinate;
        return result;
    }

    int indexGet2d(int x, int y){
        return ((_rowLen * y) + x) + _quad_offset;
        //return coordinateIndex[/*the index*/];
    }

    float getDistance(int index){
        return coordinateSpace[index].distanceFromCenter;
    }

    dimensonalType* getVector(DirectionalElement* de){
        return &possible_vectors[de->vectorIndex].vector;
    }

    dimensonalType* getVector(int index){
        return &possible_vectors[coordinateSpace[index].vectorIndex].vector;
    }

    dimensonalType* getVectorAngle(DirectionalElement* de){
        return &possible_vectors[de->vectorIndex].angle;
    }

    dimensonalType* getVectorAngle(int index){
        return &possible_vectors[coordinateSpace[index].vectorIndex].angle;
    }

    DirectionalVector* getVectorObj(DirectionalElement* de){
        return &possible_vectors[de->vectorIndex];
    }

    DirectionalVector* getVectorObj(int index){
        return &possible_vectors[coordinateSpace[index].vectorIndex];
    }

    int getVectorInterval(int index){
        return coordinateSpace[index].vectorInterval;
    }

    DirectionalElement* getItemAtDistanceIndex(int index){
        return &coordinateSpace[distanceBasedCoordinateSpaceIndex[index]];
    }

    float getItemDistanceAtDistanceIndex(int index){
        return getItemAtDistanceIndex(index)->distanceFromCenter;
    }

};




#endif
