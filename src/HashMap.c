// Author: Emmanuel Odeke <odeke@ualberta.ca>

#include <stdlib.h>

#include "RTrie.h"
#include "HashMap.h"

inline HashMap *allocHashMap(const UInt base) {
    return (HashMap *)malloc(sizeof(HashMap));
}

inline HashMap *newHashMap(const UInt base) {
    HashMap *hm = allocHashMap(base);
    hm->map = NULL;
    hm->base = base;

    return hm;
}

HashMap *destroyHashMap(HashMap *hm) {
    if (hm != NULL) {
        hm->map = destroyRTrie(hm->map, hm->base);
        free(hm);
        hm = NULL;
    }

    return hm;
}

HashMap *put(HashMap *hm, const ULInt hash, void *data, const UInt allocStyle) {
    if (hm != NULL) {
        if (hm->map == NULL)
            hm->map = newRTrie(hm->base);

        DataSav dataSav = {.isHeapd=allocStyle, .data=data, .prevFreer=free};
        hm->map = (RTrie *)putRTrie(hm->map, &dataSav, hash, hm->base);
    }

    return hm;
}

void *get(HashMap *hm, const ULInt hash) {
    void *retr = NULL;
    if (hm != NULL && hm->map != NULL)
        retr = getRTrie(hm->map, NULL, hash, hm->base);

    return retr;
}

HashMap *pop(HashMap *hm, const ULInt hash, void (*prevFreer)(void *), const void **popSave) {
    if (hm != NULL && hm->map != NULL && popSave != NULL) {
        DataSav dSav = {.prevFreer=prevFreer};
        *popSave = popRTrie(hm->map, &dSav, hash, hm->base); 
    }

    return hm;
}

int main() {
    HashMap *hm = newHashMap(10);
    hm = put(hm, 9, (void *)hm, 0);
    void *retr;
    hm = pop(hm, 9, free, (const void **)&retr);
    printf("retr: %p\n", retr);
    destroyHashMap(hm);
    return 0;
}
