//
//
// TODO rename uxList
#ifndef ColorPick_iOS_SDL_uiList_h
#define ColorPick_iOS_SDL_uiList_h

template <typename parentType, typename genType>
struct uiListIterator
{
    parentType* listInstance;
    int nextIndex;
    int lastIndex;

    uiListIterator(parentType* plistInstance){
        listInstance=plistInstance;
        nextIndex = 0;
        lastIndex = listInstance->_previousIndex;
    }

    genType* next(){
        return listInstance->get(nextIndex++);
    }

    genType* nextLast(){
        return listInstance->get(lastIndex--);
    }
};

template <typename parentType, typename genType>
struct uiListLoopingIterator
{
    parentType* listInstance;
    int nextIndex;

    uiListLoopingIterator(parentType* plistInstance){
        listInstance=plistInstance;
        nextIndex = 0;
    }

    genType* next(){
        genType* next = listInstance->get(nextIndex++);
        if( nextIndex > listInstance->largestIndex() ){
            nextIndex = 0;
        }
        return next;
    }
};


// min
template <typename genType, typename indexType>// index is optional...
struct uiList
{
    typedef int (*aIndexUpdateFn)(genType* item);
    typedef int (*aIteratorFn)(genType* item);

    typedef int (*aComparitorFn)(genType* item, genType* itemB);
    typedef int (*aGenericComparitorFn)(const void *A, const void *B);


    uiList(){
        _indexed=false;
        clear();
    }
    uiList(int pmaxSize){
        _indexed=false;
        clear();
        setSize(pmaxSize);
    }

    void setSize(int pmaxSize){
        maxSize=pmaxSize;
        listItself = (genType*)SDL_malloc( sizeof(genType) * maxSize );
    }

    void clear(){
        _nextIndex=0;
        _previousIndex=-1;
        _largestIndex=-1;
        _out_of_space = false;
        clearIndex();
    }

    void clearIndex(){
        if(!_indexed) return;
        for( int x=0; x<_indexSize; x++ ){
            indexItself[x] = maxSize; //-1; // largest Uint..
        }
    }

    // helpful tip - don't return zero for more than a single value
    void index(int size, aIndexUpdateFn pindexOffsetGen){
        // index is used to guarante unique entries (though not enforced by add) and locate entries
        _indexSize=size;
        indexOffsetGen=pindexOffsetGen;

            // sizeof(indexType) = Uint8 = 1byte = max ~255 --- should be greater than maxSize otherwise the index won't work right...
        // each value in the index must map back to a value in maxSize
        //SDL_Log("Indexing teh list now %i %i %i", sizeof(indexType), _indexSize, maxSize );

        indexItself = (indexType*)SDL_malloc( sizeof(indexType) * _indexSize );
        _indexed=true;
        clearIndex();
        // todo: we could loop and update the index.... but it is empty right now

    }

    uiList<genType, indexType>* clone(){
        // DOES NOT CLONE THE INDEX
        if( _indexed ){
            SDL_Log("Sorry the index is NOT cloned - you have to index your clone if you want an indexed clone");
        }
        uiList<genType, indexType>* newList = new uiList<genType, indexType>(maxSize);

        SDL_memcpy(newList->listItself, listItself, sizeof(genType)*total()-1);

        newList->indexOffsetGen = indexOffsetGen;
        newList->_nextIndex = _nextIndex-1;
        newList->_previousIndex = _previousIndex-1;
        newList->_largestIndex = _largestIndex-1;
        newList->_out_of_space = _out_of_space;

        return newList;
    }

    void sort(aComparitorFn p_ComparitorFn){
        sort((aGenericComparitorFn)p_ComparitorFn);
    }

    void sort(aGenericComparitorFn p_ComparitorFn){
        if( _indexed ){
            SDL_Log("Sorry sort in indexed list not currently supported");
            return;
        }
        SDL_qsort(listItself, total(), sizeof(genType), p_ComparitorFn);
        //TODO: does not fix the index....
    }

    genType* listItself;
    indexType* indexItself;
    aIndexUpdateFn indexOffsetGen;
    int maxSize;
    int _nextIndex;
    int _previousIndex;
    int _largestIndex;
    bool _out_of_space;
    int _tmp_index_offset;

    bool _indexed;
    int _indexSize;

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

    uiListIterator<uiList, genType>* iterate(){
        return new uiListIterator<uiList, genType>(this);
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
        }

        if( _nextIndex > _largestIndex ) _largestIndex = _nextIndex;

        listItself[_nextIndex++] = item;

        if( _indexed ){
            indexItself[_tmp_index_offset] = _previousIndex;
        }

        if( _nextIndex >= maxSize ){
            //_nextIndex = 0; // LOOP!!!
            _nextIndex = maxSize-1; // stuck
            _out_of_space= true;
        }else{
            _out_of_space= false;
        }

        return _previousIndex;
    }

    genType* get(int index){
        if( index < 0 || index > _largestIndex || index >= maxSize ) return nullptr;
        return &listItself[index];
    }

    genType* getLast(){
        return get(_previousIndex);
    }

    void remove(int index){
        if( index < 0 || index > _largestIndex || index >= maxSize ) return;

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
        return _largestIndex+1;//_nextIndex; // if we looped... then this will be wrong
    }
    int largestIndex(){
        return _largestIndex;
    }
    int previousIndex(){
        return _previousIndex;
    }

    void addAll(genType* newItems, int totalBytes){
        int eachElementSize = elementSize();
        int memOffset = 0;
        int offset = 0;

        while( memOffset < totalBytes ){
            add(newItems[offset++]);
            memOffset+=eachElementSize;
        }
    }

    int elementSize(){
        return sizeof(genType);
    }

    int memorySize(){
        return total() * sizeof(genType);
    }

    int maxMemorySize(){
        return maxSize * sizeof(genType);
    }

    //Uint8 palleteColorsIndex[COLOR_INDEX_MAX]; // we do not search the array
    //Uint8* palleteColorsIndex = (Uint8*)SDL_malloc( sizeof(Uint8) * COLOR_INDEX_MAX ); // totally equivilent to above

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
