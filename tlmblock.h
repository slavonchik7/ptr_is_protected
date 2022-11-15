
#ifndef TLMBLOCK_H
#define TLMBLOCK_H

#include "tracklist.h"
#include "trackptr.h"



/* ДОДЕЛАТЬ ПРОТЕСТИРОВАТЬ */
typedef struct {

    /* 
     * начальный адрес блока памяти полученный из функция семейства *alloc 
     * требуется, для вызова free, во время очистки блока памяти
     * */
    addr_t blckaddr;

    /* указатель на список ссылок на текущий блок памяти */
    dht_list_t *links;

} lmblock_t;


static inline lmblock_t *__lmblock_init() __attribute__((always_inline));


static inline lmblock_t *__lmblock_init() {

    lmblock_t *blck = (lmblock_t *)malloc(sizeof(lmblock_t));
    if ( blck ) {
        blck->links = NULL;
        blck->blckaddr = 0;
    }

    return blck;
}

#endif /* TLMBLOCK_H */