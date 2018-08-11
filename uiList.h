//
//

#ifndef ColorPick_iOS_SDL_uiList_h
#define ColorPick_iOS_SDL_uiList_h



// min
template <typename genType, typename indexType>// index is optional...
struct uiList
{
    typedef int (*aIndexUpdateFn)(genType* item);

    uiList(){
        clear();
    }
    uiList(int pmaxSize){
        clear();
        setSize(pmaxSize);
    }

    void setSize(int pmaxSize){
        maxSize=pmaxSize;
        listItself = (genType*)SDL_malloc( maxSize );
    }

    void clear(){
        _nextIndex=0;
        _previousIndex=0;
        _largestIndex=0;
        _indexed=false;
    }

    void index(int size, aIndexUpdateFn pindexOffsetGen){
        // index is used to guarante unique entries and locate entries
        indexOffsetGen=pindexOffsetGen;
        indexItself = (indexType*)SDL_malloc( size );
        for( int x=0; x<size; x++ ){
            indexItself[x] = maxSize; //-1; // largest Uint..
        }
        // todo: we could loop and update the index.... but it is empty right now
        _indexed=true;

    }


    genType* listItself;
    indexType* indexItself;
    aIndexUpdateFn indexOffsetGen;
    int maxSize;
    int _nextIndex;
    int _previousIndex;
    int _largestIndex;
    int _tmp_index_offset;

    bool _indexed;

    int locate(genType item){
        if( _indexed ){
            _tmp_index_offset = indexOffsetGen(&item);

            int position = indexItself[_tmp_index_offset];

            if( position > -1 && position < maxSize ){
                return position;
            }

        }else{
            // search whole array?
            SDL_Log("Sorry locate in un-indexed list not currently supported");
        }
        return -1; // missing
    }

    int add(genType item){

        _previousIndex = _nextIndex; // now curent

        if( _indexed ){
            _tmp_index_offset = indexOffsetGen(&item);

            genType* existing = &listItself[_nextIndex];

            int _offset_in_exi_index = indexOffsetGen(existing);
            if( indexItself[_offset_in_exi_index] == _nextIndex){
                indexItself[_offset_in_exi_index] = maxSize; // unset // we are about to overwrite this entry in list... lets reset it from index
            }

            // really this shold only happen after we assing = item; below.... so this is not exactly thread safe but we do not want to test the boolean again...
            //indexItself[_tmp_index_offset] = _previousIndex;
        }

        if( _nextIndex > _largestIndex ) _largestIndex = _nextIndex;

        listItself[_nextIndex++] = item;

        if( _indexed ){
            indexItself[_tmp_index_offset] = _previousIndex;
        }

        if( _nextIndex >= maxSize ){
            _nextIndex = 0; // LOOP!!!
        }

        return _previousIndex;
    }

    genType* get(int index){
        if( index < 0 || index > _largestIndex || index >= maxSize-1 ) return nullptr;
        return &listItself[index];
    }

    genType* getLast(){
        return get(_previousIndex);
    }

    void remove(int index){
        if( index < 0 || index > _largestIndex || index >= maxSize-1 ) return;

        if( _indexed ){     // also the location for ALL the moving entries will need to be now updated........
            genType* item = &listItself[index];
            _tmp_index_offset = indexOffsetGen(item);
            indexItself[_tmp_index_offset] = maxSize; // unset

            while( index < _nextIndex - 1 ){
                listItself[index] = listItself[index+1];

                item = &listItself[index]; // double access?
                _tmp_index_offset = indexOffsetGen(item);
                indexItself[_tmp_index_offset] = index;

                index++;
            }

        }else{
            // instead of loop maybe we can just copy memory?
            while( index < _nextIndex - 1 ){
                listItself[index] = listItself[index+1];
                index++;
            }
        }

        _nextIndex--;
        _largestIndex--;

        _previousIndex--; // unless equal to zero??
        if( _previousIndex < 0 ){
            _previousIndex = 0;
        }
    }

    int total(){
        return _nextIndex;
    }
    int largestIndex(){
        return _largestIndex;
    }
    int previousIndex(){
        return _previousIndex;
    }
    //Uint8 palleteColorsIndex[COLOR_INDEX_MAX]; // we do not search the array
    //Uint8* palleteColorsIndex = (Uint8*)SDL_malloc( COLOR_INDEX_MAX ); // totally equivilent to above

//
//    GLM_STATIC_ASSERT(
//                      detail::type<genType>::is_float ||
//                      detail::type<genType>::is_int ||
//                      detail::type<genType>::is_uint, "'min' only accept numbers");
//
//struct uiList
//{
//    uiList(){
//
//    }
//
/*
 
 
 palette->colors =
 (SDL_Color *) SDL_malloc(ncolors * sizeof(*palette->colors));
 if (!palette->colors) {
 SDL_free(palette);
 return NULL;
 }
 palette->ncolors = ncolors;
 palette->version = 1;
 palette->refcount = 1;

 SDL_memset(palette->colors, 0xFF, ncolors * sizeof(*palette->colors));
 
 SDL_memcpy(palette->colors + firstcolor, colors,
 ncolors * sizeof(*colors));
 
 
 SDL_memcmp
 (src->colors, dst->colors,
 src->ncolors * sizeof(SDL_Color)) == 0)) {
 

 */
};



#endif
