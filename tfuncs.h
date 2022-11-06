

#ifndef TFUNCS_H
#define TFUNCS_H

#include <stdlib.h>

#include "tcore.h"
#include "crc32.h"
#include "errcheck.h"
#include "errmsgs.h"
#include "errdefs.h"


static dht_list_t *main_track_list;


static void __track_links_list_node_data_free(struct_core_track_ptr_t *pscore);
static void __track_main_list_node_data_free(void *ptr);
static inline void __track_links_list_node_data_remove(void *ptr) __attribute__((always_inline));

static void __track_check_free_mem_block(struct_core_track_ptr_t *psc_blck);

static void __track_check_free_links_list(struct_core_track_ptr_t *psc_links); 

/*
 * функция высчитывает контрольную сумму требуемой структуры памяти req
 * и возвращает высчитанное значение
 */
static inline void __track_calc_memory_checksum (
                struct_core_track_ptr_t *req,
                unsigned int *res) __attribute__((always_inline));



/*
 * проверяет, была ли изменена память после предыдущего её сохранения
 * вернёт 0, если память была изменена
 * вернёт -1, если память не была изменена
 */
static inline int __track_memory_changed(struct_core_track_ptr_t *req_check) __attribute__((always_inline));


static int __track_ptr_move(struct_core_track_ptr_t *pscore, long int n);


/*
 * функция выделяет память и иницализирует структуры
 *      управления выделенной памятью по указателю pmem
 * @pmem: указатель на выделенную память
 * @msize: размер выделенной памяти
 * @flags: флаги, заданные для этой памяти
 * вернёт:
 *      указатель на инициализированную сруктуру track_ptr_t
 *      NULL в случае ошибки (код ошибки смотерть в errtrack)
 */
static track_ptr_t *__track_create_memory_control(void *pmem, size_t msize, int flags);


/*
 * функция выделяет память под узел для хранения структуры pscore
 *      и добавляет этот узел в главный список
 * @pscore: указатель на структуру struct_core_track_ptr_t
 *      которую требуется поместить в список
 * вернёт:
 *      указатель на инициализированную сруктуру npl_node_t
 *      NULL в случае ошибки (код ошибки смотерть в errtrack)
 */
static npl_node_t *__track_push_core_to_main_list(struct_core_track_ptr_t *pscore);

static struct_core_track_ptr_t *__track_struct_core_first_init(void *pmem, size_t msize, int flags);

static track_ptr_t *__track_ptr_first_init(struct_core_track_ptr_t *pscore);

static lmblock_t *__track_links_mem_block_first_init(dht_list_t *l, addr_t maddr);

static int __track_check_overlay_cur_addr(const track_ptr_t *tp0, const track_ptr_t *tp2, size_t offset);
static int __track_check_overlay_border_addr(const track_ptr_t *tp0, const track_ptr_t *tp2);

/*
 * функция очищает старую память, которую контролирует tset
 *      tset станет контролировать память по адресу tnew mem_cur_addr
 */
void __track_migrate_cur_ptr(struct_core_track_ptr_t *pscore_set,
                                struct_core_track_ptr_t *pscore_new);


/*
 * функция просматривает список ссылок структуры psc_stored_list на общий блок памяти
 *      если psc_discuss там уже присутствует, то функция ничего не делает
 *      если psc_discuss там нет, то добавляем указатель на него
 *      если указатель на список структуры psc_stored_list равен NULL, то список инициализируется
 *          и psc_discuss добавляется в этот список
 */

static int __track_review_mem_links_list(
            struct_core_track_ptr_t *psc_stored_list, 
            struct_core_track_ptr_t *psc_discuss);





/*
 * функция очищает данные по указателю ptr
 * @ptr: указатель на данные в узле списка
 */
static void __track_main_list_node_data_free(void *ptr) {

    struct_core_track_ptr_t *pscore = (struct_core_track_ptr_t *)ptr;

    __track_check_free_mem_block(pscore);

    /* очистка памяти, выделенной под структуру пользователя track_ptr_t */
    free( (void *)(pscore->user_track_data) );

    /* очистка основной управляющей структуры struct_core_track_ptr_t */
    free( (void *)(pscore) );
}

static inline void __track_links_list_node_data_remove(void *ptr) {
    ((struct_core_track_ptr_t *)ptr)->links_shared_mem_block = NULL;
}

static void __track_links_list_node_data_free(struct_core_track_ptr_t *pscore) {
    printf("data free OK---\n");
    dht_list_t *rml = pscore->links_shared_mem_block->links;
    
    /* очистка списка ссылок на блок памяти текущей удаляемой структуры */
    npl_node_t *pos;
    dht_list_while_each_start(rml, pos)
        if ( pos->data == pscore ) {
            __track_links_list_node_data_remove(pscore);
            dht_list_remove_node(rml, pos);
            free(pos);
            break;
        }
    dht_list_while_each_end()
}

static int __track_memory_changed(struct_core_track_ptr_t *req_check) {

    unsigned int calc_crc;

    __track_calc_memory_checksum(req_check, &calc_crc);

    return (req_check->checksum != calc_crc);
}



static inline void __track_calc_memory_checksum(
                struct_core_track_ptr_t *req,
                unsigned int *res) {

    *res = Crc32((unsigned char *)req->mem_start_addr, req->msize);
}



static npl_node_t *__track_push_core_to_main_list(struct_core_track_ptr_t *pscore) {

    npl_node_t *n;

    /*
     * добавляю новые пользовательские данные
     * в главный список
     */

    if ( (n = npl_node_alloc((void *)pscore)) == NULL )
        return NULL;

    /*
     * сохраняю указатель на узел, как идентификатор,
     * по которому всегда смогу найти данные пользователя
     */
    pscore->user_track_data->__ptrid = (ptr_id_t)n;


    if ( dht_list_add_node_tail(main_track_list, n) < 0 ) {
        free(n);
        return NULL;
    }

    return n;
}



static track_ptr_t *
__track_create_memory_control(void *pmem, size_t msize, int flags) {


    struct_core_track_ptr_t *score;
    track_ptr_t *usrtrack;
    npl_node_t *n;


    if ( (score = __track_struct_core_first_init(pmem, msize, flags)) == NULL )
        return NULL;


    if ( pmem ) {
        if ( (score->links_shared_mem_block = 
                __track_links_mem_block_first_init(NULL, (addr_t)pmem)) == NULL ) {
            free(score);
            return NULL;
        }
    }
    
    //printf("create mem OK addr: %p\n", (void *)score->links_shared_mem_block->blckaddr);
    /* если память не выделялась, то список просто инициализируется,
     *      поэтому передаём NULL вторым параметром 
     * иначе инициализируем список и добавляем в него score */
    if ( !TRACK_CHECK_IS_FLAG(flags, _TRACK_FLAG_EMPTY) ) {
        /* добавляем в список ссылок на текущий блок памяти
        * только что выделенную управляющую структуру */
        if ( __track_review_mem_links_list(score, score) < 0 ) {
            free(score->links_shared_mem_block);
            free(score);
            return NULL;
        }
    }

    if ( (usrtrack = __track_ptr_first_init(score)) == NULL ) {
        free(score->links_shared_mem_block);
        free(score);
        return NULL;
    }

    /*
     * расчёт контрольной суммы, если такой флаг задан
     */
    if ( TRACK_CHECK_IS_FLAG(flags, TRACK_FLAG_ADDR_CHECK_SUM) )
        track_overwrite_checksum(usrtrack);

    if ( (n = __track_push_core_to_main_list(score)) == NULL ) {
        free(score->links_shared_mem_block);
        free(usrtrack);
        free(score);
        return NULL;
    }

    return usrtrack;
}



static struct_core_track_ptr_t *
__track_struct_core_first_init(void *pmem, size_t msize, int flags) {

    struct_core_track_ptr_t *score =
            (struct_core_track_ptr_t *)malloc(sizeof(struct_core_track_ptr_t));

    if ( !score )
        return NULL;

     score->flags = flags;

#if 0
    /* если память не выделялась, то инициализировать полностью нет смысла */
    if ( TRACK_CHECK_IS_FLAG(flags, _TRACK_FLAG_EMPTY) )
        return score;

    score->msize = msize;
#endif 

    /*
     * расчитываю границы выделенной памяти
     * score->mem_end_addr указывает на последний байт выделенной памяти
     * а не на байт, следующий срузу после выделенной памяти
     */
    score->mem_start_addr = (addr_t)pmem;
    score->mem_end_addr = (addr_t)( (char *)pmem + (msize - 1) );

    score->mem_cur_addr = score->mem_start_addr;
    score->ptr_cur = pmem;
    score->save_ptr = pmem;

    return score;
}



static track_ptr_t *__track_ptr_first_init(struct_core_track_ptr_t *pscore) {

    /*
     * заполнение пользовательской структуры
     * и сохранение указателя на неё
     */
    track_ptr_t *usrtrack =
            (track_ptr_t *)malloc(sizeof(track_ptr_t));

    if ( !usrtrack )
        return NULL;


    usrtrack->iter_step = 1;
    usrtrack->__tptr = (void *)pscore;
#ifdef TEST_PTR_ACCESS
    usrtrack->test_ptr = (void *)pscore->mem_start_addr;
#endif // TEST_PTR_ACCESS
    pscore->user_track_data = usrtrack;

    return usrtrack;
}


static lmblock_t *__track_links_mem_block_first_init(dht_list_t *l, addr_t maddr) {

    lmblock_t *blck = (lmblock_t *)malloc(sizeof(lmblock_t));

    if ( blck ) {
        if ( !l ) {
            DHT_LIST_INIT(l);
        }

        blck->blckaddr = maddr;
        blck->links = l;
    }


    return blck;
}


static int __track_ptr_move(struct_core_track_ptr_t *pscore, long int n) {

    addr_t new_addr = pscore->mem_cur_addr + n;

    if ( new_addr > pscore->mem_end_addr ) {
        errtrack = ETRACK_WENT_UPPER_LIMIT;
        return -2;
    } else if ( new_addr < pscore->mem_start_addr ) {
        errtrack = ETRACK_WENT_LOWER_LIMIT;
        return -2;
    }

    pscore->mem_cur_addr = new_addr;
    pscore->ptr_cur = (void *)(new_addr);

    return 0;
}



/*  */
/* ПРОТЕСТИРОВАТЬ */
/*  */
static int __track_check_overlay_cur_addr(const track_ptr_t *tp1, const track_ptr_t *tp2, size_t offset) {

    struct_core_track_ptr_t *t1_score = (struct_core_track_ptr_t *)tp1->__tptr;
    struct_core_track_ptr_t *t2_score = (struct_core_track_ptr_t *)tp2->__tptr;


    /*
     * проверка, находится ли адрес, который содержит src
     *  между начальным и конечным адресом, который содержит dest
     */
    if (
        ( (t1_score->mem_cur_addr >= t2_score->mem_cur_addr) &&
          (t1_score->mem_cur_addr <= (t2_score->mem_cur_addr + offset)) )
            ||
        ( ((t1_score->mem_cur_addr + offset) >= t2_score->mem_cur_addr) &&
          ((t1_score->mem_cur_addr + offset) <= (t2_score->mem_cur_addr + offset)) )
       )
    {
        return 1;
    } else
        return 0;

}


/*  */
/* ПРОТЕСТИРОВАТЬ */
/*  */
static int __track_check_overlay_border_addr(const track_ptr_t *tp1, const track_ptr_t *tp2) {

    struct_core_track_ptr_t *t1_score = (struct_core_track_ptr_t *)tp1->__tptr;
    struct_core_track_ptr_t *t2_score = (struct_core_track_ptr_t *)tp2->__tptr;


    if (
        ( (t1_score->mem_start_addr >= t2_score->mem_start_addr) &&
          (t1_score->mem_start_addr <= t2_score->mem_end_addr) )
            ||
        ( (t1_score->mem_end_addr >= t2_score->mem_start_addr) &&
          (t1_score->mem_end_addr <= t2_score->mem_end_addr) )
       )
    {
        return 1;
    } else
        return 0;

}

/*  */
/* ПРОТЕСТИРОВАТЬ */
/*  */
void __track_migrate_cur_ptr(struct_core_track_ptr_t *pscore_set,
                                struct_core_track_ptr_t *pscore_new) {

    addr_t ncaddr = pscore_new->mem_cur_addr;
    addr_t neaddr = pscore_new->mem_end_addr;

    /* вношу информацию о новой памяти */
    pscore_set->mem_start_addr = pscore_set->mem_cur_addr = ncaddr;
    pscore_set->mem_end_addr = neaddr;
    pscore_set->msize = (size_t)(neaddr - ncaddr);
    pscore_set->flags = pscore_new->flags;

    pscore_set->ptr_cur = pscore_set->save_ptr = (void *)ncaddr;

}


/*  */
/* ПРОТЕСТИРОВАТЬ */
/*  */
static int __track_review_mem_links_list(
            struct_core_track_ptr_t *psc_stored_list, 
            struct_core_track_ptr_t *psc_discuss) {
    
    npl_node_t *pos;
    dht_list_t *llist = psc_stored_list->links_shared_mem_block->links;
    
    if ( psc_discuss == NULL )
        /* в случае, если требуется лишь инициализировать список
         * и сохранить указатель */
        return 0;

    /* 
     * проверка, вдруг psc_discuss уже указывает на текущий блок памяти
     *      скорее всего функция была вызвана, после перемещения имеющегося указателя
     */
    dht_list_while_each_start(llist, pos)
        if ( pos->data == (void *)psc_discuss )
            break;
    dht_list_while_each_end()

    if ( !pos ) {
        /* если psc_discuss ранее не контролировала память из текущего блока */

        npl_node_t *n = npl_node_alloc((void *)psc_discuss);
        if (!n)
            return -1;

        if ( dht_list_add_node_head(llist, n) < 0 )
            return -1;

        /* сохраняю указатель список ссылок на блок выделенной памяти */
        psc_discuss->links_shared_mem_block = psc_stored_list->links_shared_mem_block;
    }

    return 0;
}

static void __track_check_free_mem_block(struct_core_track_ptr_t *psc_blck) {

    lmblock_t *blck = psc_blck->links_shared_mem_block;
    if ( blck ) {

        /* если структура ссылается на память в каком-либо выделенном блоке */
        __track_check_free_links_list(psc_blck);

        if ( blck->links == NULL ) {
            printf("NULL\n----");
            free( (void *)(blck) );
        }
    }

}

static void __track_check_free_links_list(struct_core_track_ptr_t *psc_links) {

    lmblock_t *blck = psc_links->links_shared_mem_block;
    dht_list_t *rml = psc_links->links_shared_mem_block->links;

    __track_links_list_node_data_free(psc_links);

    if ( rml->cnt == 0 ) {
        printf("WILL BE removed\n");
        /* если никто больше не ссылается на текущий блок памяти
            * то память очищается */
        DHT_LIST_DESTROY(rml);
            printf("free OK addr: %p\n", (void *)blck->blckaddr);
        free( (void *)(blck->blckaddr) );
        blck->links = NULL;
    }
}


#endif /* TFUNCS_H */


