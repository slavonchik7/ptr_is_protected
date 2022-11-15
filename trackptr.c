


#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>


#include "tcore.h"
#include "tfuncs.h"
#include "errdefs.h"
#include "errmsgs.h"
#include "errcheck.h"
#include "crc32.h"

/*
 * основной список, в котором будут храниться
 * все выделенные и недоступные пользователю
 * структуры типа struct_core_track_ptr_t
 */
extern dht_list_t *main_track_list;


/*
 * переменная, для хранения кода последней произошедшей ошибки
 */
 int errtrack = 0;



int track_error(void);
const char *track_str_error(int errnum);



int track_init(void);

int track_destroy(void);

track_ptr_t *track_malloc(size_t msize, int flags);

int track_free(track_ptr_t *ptrack);

int track_overwrite_checksum(track_ptr_t *ptrack);

int track_check_mem(track_ptr_t *ptrack);

int track_move_ptr(track_ptr_t *ptrack, long int nmove);

int track_memcpy(track_ptr_t *dest, const track_ptr_t *src, size_t n);

int track_set_ptr(track_ptr_t *tset, track_ptr_t *tnew, track_ptr_t *told);

track_ptr_t *track_make_empty();



void *get_list() {
    return (void *)main_track_list;
    
}

int get_offset() {
    return offsetof(struct_core_track_ptr_t, ptr_cur);
}





int track_last_error(void) {

    int lerr = errtrack;

    errtrack = 0;

    return lerr;
}


const char *track_str_error(int errnum) {

    switch ( errnum ) {

        case ETRACK_NO_ERROR:
            return ETRACK_NO_ERROR_MSG;

        case ETRACK_MEM_WAS_CHANGED:
            return ETRACK_MEM_WAS_CHANGED_MSG;

        case ETRACK_WENT_LOWER_LIMIT:
            return ETRACK_WENT_LOWER_LIMIT_MSG;

        case ETRACK_WENT_UPPER_LIMIT:
            return ETRACK_WENT_UPPER_LIMIT_MSG;

        case ETRACK_MEM_NOT_FOUND:
            return ETRACK_MEM_NOT_FOUND_MSG;

        case ETRACK_NULL_PTR_PASSED:
            return ETRACK_NULL_PTR_PASSED_MSG;

        case ETRACK_INIT_REQUIRED:
            return ETRACK_INIT_REQUIRED_MSG;

        case ETRACK_ALLOC:
            return ETRACK_ALLOC_MSG;

        case ETRACK_INIT_TWICE:
            return ETRACK_INIT_TWICE_MSG;

        case ETRACK_MEMCPY_MEM_OVERLAP:
            return ETRACK_MEMCPY_MEM_OVERLAP_MSG;

        case ERTACK_MEMCPY_DEST_UP_LIM:
            return ERTACK_MEMCPY_DEST_UP_LIM_MSG;

        case ERTACK_MEMCPY_SRC_UP_LIM:
            return ERTACK_MEMCPY_SRC_UP_LIM_MSG;

        case ETRACK_MEMCPY:
            return ETRACK_MEMCPY_MSG;

        default:
            return ETRACK_UNKNOW_ERROR_MSG;
    }

    return "";
}



int track_init(void) {

    /*
     * если список уже иннициализирован
     * то есть пользователь ранее уже вызывал эту функцию
     */
    if ( main_track_list ) {
        errtrack = ETRACK_INIT_TWICE;
        return -1;
    }

    DHT_LIST_INIT(main_track_list);

    if ( !main_track_list )
        return -1;

    return 0;
}

int track_destroy(void) {

    __TRACK_CHECK_RET_INIT_REQ(-1);

    /* очистка списка */
    dht_list_func_full_free(main_track_list,
            __track_main_list_node_data_free);

    DHT_LIST_DESTROY(main_track_list);

    return 0;
}





track_ptr_t *track_malloc(size_t msize, int flags) {

    void *memory_ptr;
    track_ptr_t *usrtrack;

    /*
     * если главный список не был инициализирован
     * то есть пользователь не вызвал функцию track_init
     */
    __TRACK_CHECK_RET_INIT_REQ(NULL);


    /*
     * выделение запрошенной памяти
     */

    if ( (memory_ptr = malloc(msize)) == NULL )
        goto ret_error_alloc;

    /*
     * переходим к добавлению выделенной памяти в общий список
     * и инициализации управляющих ею структур
     */
    if ( ( usrtrack = __track_create_memory_control(memory_ptr, msize, flags)) == NULL) {
        free(memory_ptr);
        goto ret_error_alloc;
    }

    printf("malloc OK addr: %p\n", memory_ptr);
    
    return usrtrack;

    /*
     * переход будет осуществлён в случае, если не удалось выделить
     * хотя бы один требуемый для корректной работы объём памяти
     */
ret_error_alloc:
    errtrack = ETRACK_ALLOC;
    return NULL;
}






int track_free(track_ptr_t *ptrack) {

    __TRACK_CHECK_RET_INIT_REQ(-1);
    __TRACK_CHECK_RET_NULL_PTR(ptrack, -1);

    npl_node_t *n = dht_list_remove_node(main_track_list, (npl_node_t *)(ptrack->__ptrid));

    if ( !n ) {
        /* память под эту структуру не выделялась */
        errtrack = ETRACK_MEM_NOT_FOUND;
        return -1;
    }

    __track_main_list_node_data_free(n->data);

        printf("ALL OK\n");
    free(n);


    return 0;
}


int track_overwrite_checksum(track_ptr_t *ptrack) {

    __TRACK_CHECK_RET_INIT_REQ(-1);
    __TRACK_CHECK_RET_NULL_PTR(ptrack, -1);

// #define TEST_CHECKSUM
#ifdef TEST_CHECKSUM
    uint32_t tcrc;
    struct_core_track_ptr_t *tscore = (struct_core_track_ptr_t *)ptrack->__tptr;

    __track_calc_memory_checksum(
            tscore,
            &tcrc );
    printf("ptrack old check: %u\n", tscore->checksum);
    printf("ptrack new check: %u\n", tcrc);

    tscore->checksum = tcrc;
#else

    __track_calc_memory_checksum(
            (struct_core_track_ptr_t *)ptrack->__tptr,
            &(((struct_core_track_ptr_t *)ptrack->__tptr)->checksum) );
#endif // TEST_CHECKSUM


    return 0;
}


int track_check_mem(track_ptr_t *ptrack) {

    __TRACK_CHECK_RET_INIT_REQ(-1);
    __TRACK_CHECK_RET_NULL_PTR(ptrack, -1);

    struct_core_track_ptr_t * pscore = (struct_core_track_ptr_t *)ptrack->__tptr;

    __TRACK_CHECK_RET_MEM_CHANGED(pscore, 1);
    __TRACK_CHECK_WENT_LOWER_LIM(pscore, 1);
    __TRACK_CHECK_WENT_UPPER_LIM(pscore, 1);

    return 0;
}



int track_move_ptr(track_ptr_t *ptrack, long int nmove) {
    __TRACK_CHECK_RET_INIT_REQ(-1);
    __TRACK_CHECK_RET_NULL_PTR(ptrack, -1);


    struct_core_track_ptr_t * pscore = (struct_core_track_ptr_t *)ptrack->__tptr;
    int flags = pscore->flags;

    if ( TRACK_CHECK_IS_FLAG(flags, TRACK_FLAG_ADDR_CHECK_SUM) )
        __TRACK_CHECK_RET_MEM_CHANGED(pscore, -1);

    return __track_ptr_move(pscore, nmove);
}



/*  */
/* ПРОТЕСТИРОВАТЬ */
/*  */
int track_memcpy(track_ptr_t *dest, const track_ptr_t *src, size_t n) {

    __TRACK_CHECK_RET_INIT_REQ(-1);
    __TRACK_CHECK_RET_NULL_PTR(dest, -1);
    __TRACK_CHECK_RET_NULL_PTR(src, -1);

    struct_core_track_ptr_t *dest_score = (struct_core_track_ptr_t *)dest->__tptr;
    struct_core_track_ptr_t *src_score = (struct_core_track_ptr_t *)src->__tptr;

    /* проверка выхода за границы памяти dest */
    if ( (dest_score->mem_cur_addr + n) > dest_score->mem_end_addr ) {
        errtrack = ERTACK_MEMCPY_DEST_UP_LIM;
        return -1;
    }

    /* проверка выхода за границы памяти src */
    if ( (src_score->mem_cur_addr + n) > src_score->mem_end_addr ) {
        errtrack = ERTACK_MEMCPY_SRC_UP_LIM;
        return -1;
    }

    /* проверка, перекрываются ли участки памяти */
    if ( __track_check_overlay_cur_addr(dest, src, n) ) {
        errtrack = ETRACK_MEMCPY_MEM_OVERLAP;
        return -1;
    }

    if ( memcpy((void *)dest_score->ptr_cur, (void *)src_score->ptr_cur, n) == NULL ) {
        errtrack = ETRACK_MEMCPY;
        return -1;
    }

    return 0;
}




int track_set_ptr(track_ptr_t *tset, track_ptr_t *tnew, track_ptr_t *told) {

    __TRACK_CHECK_RET_INIT_REQ(-1);
    __TRACK_CHECK_RET_NULL_PTR(tset, -1);
    __TRACK_CHECK_RET_NULL_PTR(tnew, -1);

    struct_core_track_ptr_t *pscore_set = (struct_core_track_ptr_t *)tset->__tptr;
    struct_core_track_ptr_t *pscore_new = (struct_core_track_ptr_t *)tnew->__tptr;

    /* если tnew не контролирует никакую память, выходим */
    __TRACK_CHECK_RET_IS_EMPTY(pscore_new, -1);


    if ( told != NULL ) {
        struct_core_track_ptr_t *pscore_old = (struct_core_track_ptr_t *)told->__tptr;
        addr_t osaddr = pscore_old->mem_start_addr;

        __track_check_free_mem_block(pscore_old);

        /* 
         * установливаю начальный адрес участка памяти, чтобы вся память, 
         * контролируемая структурой tset, контролировалась структурой told 
         */
        pscore_old->mem_cur_addr = osaddr;
        /* теперь told будет контролировать память, которую контролирует tset */
        __track_migrate_cur_ptr(pscore_old, pscore_set);
        
        if ( pscore_set->links_shared_mem_block != NULL ) {
            track_overwrite_checksum(pscore_old->user_track_data);
            __track_review_mem_links_list(pscore_set, pscore_old);
        }
    } else
        __track_check_free_mem_block(pscore_set);


    /* теперь tset будет контролировать память, которую контролирует tnew */
    __track_migrate_cur_ptr(pscore_set, pscore_new);

    /* пересчитываю  crc32 начиная с mem_cur_addr структуры tnew */
    track_overwrite_checksum(pscore_set->user_track_data);

    __track_review_mem_links_list(pscore_new, pscore_set);


       // printf("ALL set OK\n");

    return 0;
}


track_ptr_t *track_make_empty() {
    track_ptr_t *usrtrack;

    /*
     * если главный список не был инициализирован
     * то есть пользователь не вызвал функцию track_init
     */
    __TRACK_CHECK_RET_INIT_REQ(NULL);

    /*
     * переходим к добавлению выделенной памяти в общий список
     * и инициализации управляющих ею структур
     */
    if ( ( usrtrack = __track_create_memory_control(NULL, 0, _TRACK_FLAG_EMPTY)) == NULL)
        goto ret_error_alloc;


    return usrtrack;

ret_error_alloc:
    errtrack = ETRACK_ALLOC;
    return NULL;
}


