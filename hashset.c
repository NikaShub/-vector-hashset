#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
	assert(elemSize > 0);
    assert(numBuckets > 0);
    assert(hashfn != NULL);
    assert(comparefn != NULL);
	h->elemSize = elemSize;
	h->numBuckets = numBuckets;
	h->size = 0;
	h->HashSetCompareFunction = comparefn;
	h->HashSetHashFunction = hashfn;
	h->HashSetFreeFunction = freefn;
	h->buckets = malloc(numBuckets * sizeof(vector));
	assert(h->buckets != NULL);
	for (int i = 0; i < numBuckets; i++) {
		VectorNew(&h->buckets[i], elemSize, freefn, 4);
	}
}


void HashSetDispose(hashset *h)
{
	for (int i = 0; i < h->numBuckets; i++) {
		VectorDispose(&h->buckets[i]);
	}
	free(h->buckets);
}

int HashSetCount(const hashset *h)
{ 
	return h->size;
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
	assert(mapfn != NULL);
	for (int i = 0; i < h->numBuckets; i++) {
		VectorMap(&h->buckets[i], mapfn, auxData);
	}
}

void HashSetEnter(hashset *h, const void *elemAddr)
{
	int hash = h->HashSetHashFunction(elemAddr, h->numBuckets);
	assert(elemAddr != NULL);
	assert(hash >= 0);
	assert(hash< h->numBuckets);

	int pos = VectorSearch(&h->buckets[hash], elemAddr, h->HashSetCompareFunction, 0, false);
	if (pos == -1) {
		VectorAppend(&h->buckets[hash], elemAddr);
		h->size++;
	} else {
		VectorReplace(&h->buckets[hash], elemAddr, pos);
	}
}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{ 
	int hash = h->HashSetHashFunction(elemAddr, h->numBuckets);
	assert(elemAddr != NULL);
	assert(hash >= 0);
	assert(hash< h->numBuckets);

	int pos = VectorSearch(&h->buckets[hash], elemAddr, h->HashSetCompareFunction, 0, false);
	if (pos == -1) return NULL;
	void* addr = VectorNth(&h->buckets[hash], pos);
	return addr;
}
