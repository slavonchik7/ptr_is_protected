


#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "trackptr.h"

volatile static uint32_t test_out;


void print_test() {
    test_out = 10;
    printf("test_out: %d\n", test_out);
}


/*
 *
 */
static const char ETRACK_MEM_WAS_CHANGED_MSG[]      = "the memory was changed without the knowledge";
static const char ETRACK_WENT_LOWER_LIMIT_MSG[]     = "went beyond the lower limit of the available allocated address space";
static const char ETRACK_WENT_UPPER_LIMIT_MSG[]     = "went beyond the upper limit of the available allocated address space";
static const char ETRACK_UNKNOW_ERROR_MSG[]         = "unknown error";

int track_last_error(void);
const char *track_str_error(int errnum);




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
    unsigned long int check_sum;


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
    char flags;



} struct_core_track_ptr_t;


/*
 * функция высчитывает контрольную сумму требуемой структуры памяти req
 * и возвращает высчитанное значение
 */
static unsigned long int __track_calc_mem_check_sum(struct_core_track_ptr_t *req);


/*
 * проверяет, была ли изменена память после предыдущего её сохранения
 * вернёт 1, если память была изменена
 * вернёт 0, если память не была изменена
 * вернёт -2  случае ошибки в самой функции
 */
static int __track_memory_changed(struct_core_track_ptr_t *req_check);






int track_last_error(void) {

}

const char *track_str_error(int errnum) {

    switch ( errnum ) {

        case ETRACK_MEM_WAS_CHANGED:
            return ETRACK_MEM_WAS_CHANGED_MSG;

        case ETRACK_WENT_LOWER_LIMIT:
            return ETRACK_WENT_LOWER_LIMIT_MSG;

        case ETRACK_WENT_UPPER_LIMIT:
            return ETRACK_WENT_UPPER_LIMIT_MSG;

        default:
            return ETRACK_UNKNOW_ERROR_MSG;
    }

    return "";
}













