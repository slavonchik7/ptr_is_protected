


#include <malloc.h>
#include <stdlib.h>
#include <string.h>


#include "tagptr.h"






void *ptag_calloc(size_t nmemb, size_t msize) {

    char *nptr;
    size_t salloc = nmemb * msize + 1;

    if ( ( nptr = (char *)malloc(salloc) ) ) {

        if ( memset(nptr, 0, salloc) )
            *nptr = PTAG_PTR_OK;
        else
            return NULL;

    } else
        return NULL;

    return ++nptr;

}


void *ptag_malloc(size_t msize) {

    char *nptr;

    if ( ( nptr = (char *)malloc(msize + 1) ) )
        *nptr = PTAG_PTR_OK;
    else
        return NULL;

    return ++nptr;
}



int ptag_free(void *ptr) {

    if ( !ptr )
        return -1;

    if ( !PTAG_CHECK_PTR(ptr) )
        return -1;

    ptr = (void *)( ( (char *)ptr ) - 1 );
    free(ptr);

    return 0;
}



void *ptag_realloc(void *ptr, size_t msize) {

    if ( !ptr )
        return NULL;

    if ( !PTAG_CHECK_PTR(ptr) )
        return NULL;

    char *nptr;
    ptr = (void *)( ( (char *)ptr ) - 1 );

    if ( ( nptr = (char *)realloc(ptr, msize + 1) ) )
        *nptr = PTAG_PTR_OK;
    else
        return NULL;

    return ++nptr;
}
