


#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "crc32.h"
#include "tracklist.h"
#include "trackptr.h"
#include "errcheck.h"




/*
 *
 */
static const char ETRACK_MEM_WAS_CHANGED_MSG[]      = "the memory was changed without the knowledge";
static const char ETRACK_WENT_LOWER_LIMIT_MSG[]     = "went beyond the lower limit of the available allocated address space";
static const char ETRACK_WENT_UPPER_LIMIT_MSG[]     = "went beyond the upper limit of the available allocated address space";
static const char ETRACK_MEM_NOT_FOUND_MSG[]        = "the passed structure does not manage any allocated memory";
static const char ETRACK_NULL_PTR_PASSED_MSG[]      = "a null pointer was passed to the function";
static const char ETRACK_INIT_REQUIRED_MSG[]        = "initialization is required, first you should call track_init()";
static const char ETRACK_ALLOC_MSG[]                = "could not allocate the requested memory or memory for the control structure";
static const char ETRACK_INIT_TWICE_MSG[]           = "track_init() has already been called before";

static const char ETRACK_UNKNOW_ERROR_MSG[]         = "unknown error";

/*
 * основной список, в котором будут храниться
 * все выделенные и недоступные пользователю
 * структуры типа struct_core_track_ptr_t
 */
static dht_list_t *main_track_list = NULL;


/*
 * переменная, для хранения кода последней произошедшей ошибки
 */
 int errtrack = 0;



int track_last_error(void);
const char *track_str_error(int errnum);



int track_init(void);

int track_destroy(void);

track_ptr_t *track_malloc(size_t msize, int flags);

int track_free(track_ptr_t *ptrack);

int track_overwrite_checksum(track_ptr_t *ptrack);

int track_check_mem(track_ptr_t *ptrack);



/* структура, которая будет статической  будет скрыта */
typedef struct {

    addr_t mem_start_addr;          /* адрес с которого выделена память */
    addr_t mem_end_addr;            /* адрес на котором заканчивается участок памяти */


    addr_t mem_cur_addr;            /* адрес на который в данный момент указывает ptr_cur */


    void *ptr_cur;                  /* указатель, который содержит адрес mem_cur_addr */


    /*
     * указатель будет хранить последнее корректное,
     * проверенное значение поля ptr_cur на случай, если произойдет ошибка
     * и пользователю не удастся самому вернуть, к корректному адресу
     */
    void *save_ptr;

    size_t msize;

    /*
     * указатель на структуру, доступную пользователю
     * которая соответствует текущей структуре struct_core_track_ptr_t
     * требуется, для изменения поля last_error этой структуры
     * и, возможно, не только
     */
    track_ptr_t *user_track_data;


    /*
     * контрольная сумма, расчитаная по выделенной памяти
     * используется, если установлен флаг TRACK_ADDR_PROTECT
     * или TRACK_ADDR_CHECK_SUM
     */
    unsigned int checksum;


    /*
     * поле flags отвечает за текущий статус выделеной памяти ()
     *
     * если flags содержит TRACK_ADDR_PROTECT, то в случае,
     *  если память будет изменена в каком либо участке программы
     *  любая функция библиотеки завершится с ошибкой
     *
     * если flags содержит TRACK_ADDR_CHECK_SUM,
     *  то контрольная сумма check_sum будет пересчитываться при каждом вызове фукнции,
     *  которая предусматривает изменение данных в этой области памяти
     */
    int flags;



} struct_core_track_ptr_t;




/*
 * функция высчитывает контрольную сумму требуемой структуры памяти req
 * и возвращает высчитанное значение
 */
static inline int __track_calc_mem_checksum(
                struct_core_track_ptr_t *req,
                unsigned int *res);



/*
 * проверяет, была ли изменена память после предыдущего её сохранения
 * вернёт 1, если память была изменена
 * вернёт 0, если память не была изменена
 */
static int __track_memory_changed(struct_core_track_ptr_t *req_check);












/*
 * функция очистки памяти для вызова dht_list_func_full_free()
 * очищает память выделенную под структуру struct_core_track_ptr_t
 * @ptr: указатель на структуру struct_core_track_ptr_t
 */
static void __track_struct_core_free(void *ptr) {

    /*
        НЕ ИСПОЛЬЗУЕТСЯ
    */

    struct_core_track_ptr_t *pscore = (struct_core_track_ptr_t *)ptr;

    free(pscore);
}

/*
 * функция очищает данные по указателю ptr
 * @ptr: указатель на узел списка
 */
static void __track_main_list_node_data_free(void *ptr) {

    struct_core_track_ptr_t *pscore = (struct_core_track_ptr_t *)ptr;

    /* очистка выделяемой для пользователя памяти */
    free( (void *)(pscore->mem_start_addr) );

    /* очистка памяти, выделенной под структуру пользователя track_ptr_t */
    free( (void *)(pscore->user_track_data) );

    /* очистка основной управляющей структуры struct_core_track_ptr_t */
    free( (void *)(pscore) );
}



static inline int __track_memory_changed(struct_core_track_ptr_t *req_check) {

    unsigned int calc_crc;

    __track_calc_mem_checksum(req_check, &calc_crc);

    return (req_check->checksum != calc_crc);
}



static inline int __track_calc_mem_checksum(
                struct_core_track_ptr_t *req,
                unsigned int *res) {

    *res = Crc32((unsigned char *)req->mem_start_addr, req->msize);

    return 0;
}




int track_last_error(void) {

    int lerr = errtrack;

    errtrack = 0;

    return lerr;
}


const char *track_str_error(int errnum) {

    switch ( errnum ) {

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

        default:
            return ETRACK_UNKNOW_ERROR_MSG;
    }

    return "";
}


int track_approve_flags(track_ptr_t *ptrack) {



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



    /*
     * если главный список не был инициализирован
     * то есть пользователь не вызвал функцию track_init
     */
    __TRACK_CHECK_RET_INIT_REQ(NULL);


    struct_core_track_ptr_t *score =
            (struct_core_track_ptr_t *)malloc(sizeof(struct_core_track_ptr_t));

    if ( !score )
        goto ret_error_alloc;

    score->flags = flags;
    score->msize = msize;


    /* ПРОТЕСТИРОВАТЬ !!! */
    if ( TRACK_CHECK_IS_FLAG(flags, TRACK_FLAG_ADDR_CHECK_SUM) ) {
        unsigned long int chsum;

        if ( __track_calc_mem_checksum(score, &chsum) < 0 ) {
            free(score);
            return NULL;
        }

        score->checksum = chsum;
    }

    void *memory_ptr = malloc(msize);
    if ( !memory_ptr ) {
        free(score);
        goto ret_error_alloc;
    }

    /*
     * расчитываю границы выделенной памяти
     * score->mem_end_addr указывает на последний байт выделенной памяти
     * а не на байт, следующий срузу после выделенной памяти
     */
    score->mem_start_addr = (addr_t)memory_ptr;
    score->mem_end_addr = (addr_t)( (char *)memory_ptr + (msize - 1) );

    score->mem_cur_addr = score->mem_start_addr;
    score->ptr_cur = memory_ptr;
    score->save_ptr = memory_ptr;

    /*
     * заполнение пользовательской структуры
     * и сохранение указателя на неё
     */
    track_ptr_t *usrtrack =
            (track_ptr_t *)malloc(sizeof(track_ptr_t));

    if ( !usrtrack ) {
        free(memory_ptr);
        free(score);
        goto ret_error_alloc;
    }

    usrtrack->iter_step = 1;
    usrtrack->__tptr = (void *)score;

    score->user_track_data = usrtrack;


    /*
     * добавляю новые пользовательские данные
     * в главный список
     */
    npl_node_t *n = npl_node_alloc((void *)score);
    if ( !n ) {
        free(usrtrack);
        free(memory_ptr);
        free(score);
        goto ret_error_alloc;
    }

    /*
     * сохраняю указатель на ноду, как идентификатор,
     * по которому всегда смогу найти данные пользователя
     */
    usrtrack->ptrid = (ptr_id_t)n;


    if ( dht_list_add_node_tail(main_track_list, n) < 0 ) {
        free(n);
        free(usrtrack);
        free(memory_ptr);
        free(score);
        goto ret_error_alloc;
    }


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

    npl_node_t *n = dht_list_remove_node(main_track_list, (npl_node_t *)(ptrack->ptrid));

    if ( !n ) {
        /* память под эту структуру не выделялась */
        errtrack = ETRACK_MEM_NOT_FOUND;
        return -1;
    }

    __track_main_list_node_data_free(n->data);

    free(n);


    return 0;
}


int track_overwrite_checksum(track_ptr_t *ptrack) {

    __TRACK_CHECK_RET_INIT_REQ(-1);
    __TRACK_CHECK_RET_NULL_PTR(ptrack, -1);

    __track_calc_mem_checksum(
            (struct_core_track_ptr_t *)ptrack->__tptr,
            &(((struct_core_track_ptr_t *)ptrack->__tptr)->checksum) );

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




