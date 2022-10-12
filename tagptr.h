

#ifndef TAGPTR_H
#define TAGPTR_H



#define PTAG_PTR_OK 0x0
#define PTAG_CHECK_PTR(p) ( *( ( (char *)p ) - 1 ) == PTAG_PTR_OK )




void *ptag_calloc(size_t nmemb, size_t msize);
void *ptag_malloc(size_t msize);
int ptag_free(void *ptr);
void *ptag_realloc(void *ptr, size_t msize);


#endif /* TAGPTR_H */

