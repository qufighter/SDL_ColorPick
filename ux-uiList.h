//
//
// TODO rename uxList
#ifndef ColorPick_iOS_SDL_uiList_h
#define ColorPick_iOS_SDL_uiList_h

#ifndef FREE_FOR_NEW
#define FREE_FOR_NEW SDL_free
#ifdef __WINDOWS__
#define FREE_FOR_NEW free // on windows, using the new operator to consturct means we really CANNOT use SDL_Free, and its tough to detect... sprinked around the project this way!
#endif
#endif

// TODO: move thse struct into the ohter struct and either build a static accessor or.... whatever....
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

static int getRandomInt(int min, int max){ // sort of duplicate!  see randomInt(
    return min + rand() % (max - min + 1);
}

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
        // note: if _indexed==true already, we must free/clear the index first...
        if( _indexed==true ){
            SDL_Log("WARNING you are probably calling index 2+ times in ERROR; did you mean reindex...");
            SDL_free(indexItself);
        }
        // index is used to guarante unique entries (though not enforced by add) and locate entries
        _indexSize=size;
        indexOffsetGen=pindexOffsetGen;

            // sizeof(indexType) = Uint8 = 1byte = max ~255 --- should be greater than maxSize otherwise the index won't work right...
        // each value in the index must map back to a value in maxSize
        //SDL_Log("Indexing teh list now %i %i %i", sizeof(indexType), _indexSize, maxSize );

        indexItself = (indexType*)SDL_malloc( sizeof(indexType) * _indexSize );
        _indexed=true;
        reindex();
    }

    void reindex(){
        if(!_indexed){
            return;
            SDL_Log("Sorry reindex requires the list has previously been indexed...");
        }
        clearIndex();
        // for properly initialized index (prior to list use), the following will never run (list size 0)
        for( int x=0; x<=_largestIndex; x++ ){
            _tmp_index_offset = indexOffsetGen(&listItself[x]);
            if( indexItself[_tmp_index_offset] < maxSize ){
                SDL_Log("Warning, you already have duplicates in your set, the index is going to point to the last entry...");
            }
            indexItself[_tmp_index_offset] = x;
        }
    }

    uiList<genType, indexType>* clone(){
        // DOES NOT CLONE THE INDEX
        if( _indexed ){
            SDL_Log("Warning, Sorry the index is NOT cloned - you have to index your clone if you want an indexed clone");
        }
        uiList<genType, indexType>* newList = new uiList<genType, indexType>(maxSize);

        SDL_memcpy(newList->listItself, listItself, sizeof(genType)*total());

        newList->indexOffsetGen = indexOffsetGen;
        newList->_nextIndex = _nextIndex;
        newList->_previousIndex = _previousIndex;
        newList->_largestIndex = _largestIndex;
        newList->_out_of_space = _out_of_space;

        return newList;
    }

    void sort(aComparitorFn p_ComparitorFn){
        sort((aGenericComparitorFn)p_ComparitorFn);
    }

    void sort(aGenericComparitorFn p_ComparitorFn){
        // NOTE: make sure your p_ComparitorFn for SDL_qsort is NON-RANDOM (i.e deterministic) esp on windows...
        SDL_qsort(listItself, total(), sizeof(genType), p_ComparitorFn);

        if( _indexed ){ reindex(); };
    }

    void randomize_order(){
        randomize_order(false);
    }

    void randomize_order(bool force){
        randomize_order(force, _largestIndex);
    }

    void randomize_order(bool force, int forceFalloff){
        /// force: may return in same order when false
        /// forceFalloff: since force maximizes each position being different (
        ///          up to the final one, it's less random (approx 0% chance any item ends up in the SAME position))
        ///          so if you specify a number lower than total()-2 you can provide more slots further up the list
        ///          MIGHT have the same value as before, thus INCREASING randomness...
        if( total()-1 >= SDL_MAX_UINT32 ){ // probably breaks before this point unless int is 64bit...
            SDL_Log("Sorry with Uint32 usage randomize_order will be broken by such a large list with %i items", total());
            return;
        }

        if( total() == 1 ){ force = false; } // cannot force random when size 1

        Uint8* takenSlots = (Uint8*)SDL_malloc( sizeof(Uint8) * total() ); // only needs bit, Uint8 overkill
        Uint32* newOrder =   (Uint32*)SDL_malloc( sizeof(Uint32) * total() ); // NOTE: Uint32 here enforces a maximum list size... not necessary!
        genType* newListItself = (genType*)SDL_malloc( sizeof(genType) * maxSize );
        int reordering_completion = 0;

        SDL_memset(takenSlots, 0, sizeof(Uint8) * total());

        while(reordering_completion < total()){
            int temp = getRandomInt(0, _largestIndex);
            if( takenSlots[temp] == 0 && (!force || temp != reordering_completion)){
                takenSlots[temp] = 1;
                newOrder[reordering_completion] = temp;
                reordering_completion++;
                if( reordering_completion >= _largestIndex || reordering_completion >= forceFalloff ){
                    force = false; // once we force one+ position, we MUST stop prior to the final one.. or we may infinite looop.. (eg sort 0,1,2 then 1,0,<only 2 remains>)
                }
            }
        }

        reordering_completion = 0;
        while(reordering_completion < total()){
            newListItself[reordering_completion] = listItself[newOrder[reordering_completion]];
            reordering_completion++;
        }
        // copy results to source...
        SDL_memcpy(listItself, newListItself, sizeof(genType)*total());
        SDL_free(newListItself);
        SDL_free(takenSlots);
        SDL_free(newOrder);

        if( _indexed ){ reindex(); };
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
            return locateIndex(indexOffsetGen(&item));
        }else{
            // search whole array?
            //SDL_Log("Sorry locate in un-indexed list not currently supported");

            // TODO this needs testing with NON reference types for genType still... since those are indexed?
            // see bool operator==(const Ux::ColorList i)
            // SO we tested this but NOT with the above type!! however with SDL_Color we COULD NOT compare references in the opperator we had to write....
            // still some to test here with more actual types..... and to know when ref compare works good vs not....
            for( int x=0; x<=_largestIndex; x++ ){
                // SDL_Log("spla %i %i", item, listItself[x]  );
                if( item == listItself[x] ){
                    return x;
                }
            }
        }
        return -1; // missing
    }

    int locateIndex(int provided_index){
        if( _indexed ){
            int position = indexItself[provided_index];
            if( position > -1 && position < maxSize ){
                return position;
            }
        }else{
            // search whole array?
            SDL_Log("Sorry locateIndex in un-indexed list not currently supported");
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

            if( _offset_in_exi_index >= 0 && _offset_in_exi_index < _indexSize && indexItself[_offset_in_exi_index] == _nextIndex){
                indexItself[_offset_in_exi_index] = maxSize; // unset // we are about to overwrite this entry in list... lets reset it from index
            }
        }

        if( _nextIndex > _largestIndex ) _largestIndex = _nextIndex;

		//SDL_memcpy(&listItself[_nextIndex++], &item, sizeof(genType)); // equivilent to following...
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

        if( _out_of_space ){
            _nextIndex++;
            _out_of_space=false;
        }

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
        return _largestIndex+1;//_nextIndex; // if we looped... then _nextIndex will be wrong
    }
    int largestIndex(){
        return _largestIndex;
    }
    int previousIndex(){
        return _previousIndex;
    }
    int validateIndex(int listIndex){
        if( listIndex > _largestIndex ){
            return _largestIndex;
        }
        if( listIndex < 0 ){
            return 0;
        }
        return listIndex;
    }
    int validateIndexLooping(int listIndex){
        if( listIndex > _largestIndex ){
            return 0;
        }
        if( listIndex < 0 ){
            return _largestIndex;
        }
        return listIndex;
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

    void free_list(){
        SDL_free(listItself);
        if( _indexed ){
            SDL_free(indexItself);
        }
		FREE_FOR_NEW(this); // shold not be needed???
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
