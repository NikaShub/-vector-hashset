#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
    if (initialAllocation == 0) {
        initialAllocation = 4;
    }
    v->allocsize = initialAllocation;
    v->logsize = 0;
    v->VectorFreeFunction = freeFn;
    v->elemSize = elemSize;
    v->elems = malloc(initialAllocation * elemSize);
    assert(v->elems != NULL);
    assert(elemSize > 0);
    assert(initialAllocation >= 0);
}

void VectorDispose(vector *v)
{
    if (v->VectorFreeFunction) {
        for (int i = 0; i < v->logsize; i++) {
            void* elemAdd = (char*)v->elems + i * v->elemSize; 
            v->VectorFreeFunction(elemAdd);
        }
    }
    free(v->elems);
}

int VectorLength(const vector *v)
{ 
    return v->logsize;
}

void *VectorNth(const vector *v, int position)
{   
    assert(position >= 0);
    assert(position < v->logsize);
    return (char*)v->elems + position * v->elemSize;
 }

void VectorReplace(vector *v, const void *elemAddr, int position)
{
    assert(position >= 0);
    assert(position < v->logsize);
    void* addr = (char*)v->elems + position * v->elemSize;
    memcpy(addr, elemAddr, v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
    assert(position >= 0);
    assert(position <= v->logsize);
    if (v->logsize == v->allocsize) {
        v->elems = realloc(v->elems, 2 * v->allocsize * v->elemSize);
        v->allocsize *= 2;
    }
    void* addr = (char*)v->elems + position * v->elemSize;
    if (position < v->logsize) {
        memmove((char*)addr + v->elemSize, addr, (v->logsize - position) * v->elemSize);
    }
    memcpy(addr, elemAddr, v->elemSize);
    v->logsize++;
}

void VectorAppend(vector *v, const void *elemAddr)
{
    if (v->logsize == v->allocsize) {
        v->elems = realloc(v->elems, 2 * v->allocsize * v->elemSize);
        v->allocsize *= 2;
    }
    void* addr = (char*)v->elems + v->logsize * v->elemSize;
    memcpy(addr, elemAddr, v->elemSize);
    v->logsize++;
}

void VectorDelete(vector *v, int position)
{
    assert(position >= 0);
    assert(position < v->logsize);
    void* addr = (char*)v->elems + position * v->elemSize;
     if (v->VectorFreeFunction) {
        v->VectorFreeFunction(addr);
    }
    memmove(addr, (char*)addr + v->elemSize, (v->logsize - position - 1) * v->elemSize);
    v->logsize--;
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
    assert(compare != NULL);
    qsort(v->elems, v->logsize, v->elemSize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
    assert(mapFn != NULL);
    for (int i = 0; i < v->logsize; i++) {
        void* addr = (char*)v->elems + i * v->elemSize;
        mapFn(addr, auxData);
    }
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{ 
    assert(searchFn != NULL);
    assert(startIndex >= 0);
    assert(startIndex <= v->logsize);
    if (!isSorted) {
        for (int i = startIndex; i < v->logsize; i++) {
            void* add = (char*)v->elems + i * v->elemSize;
            if (!searchFn(add, key)) {
                int ans = (char*)add - (char*)v->elems;
                ans /= v->elemSize;
                return ans;
            }
        }
        return kNotFound;
    }
    void* addr = bsearch(key, (char*)v->elems + startIndex * v->elemSize, v->logsize - startIndex, v->elemSize, searchFn);
    if (addr == NULL) return kNotFound;
    int ans = (char*)addr - (char*)v->elems;
    ans /= v->elemSize;
    return ans;
} 
