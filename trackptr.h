

#ifndef TRACKPTR_H
#define TRACKPTR_H

#include <stdint.h>
#include "maddr.h"


/*
 * возможные значения флага, с которым может инициализироваться структура track_ptr_t
 */
#define TRACK_FLAGS_NOT_SET         ( 0x0 )
#define TRACK_FLAG_ADDR_PROTECT     ( 0x2 )
#define TRACK_FLAG_ADDR_CHECK_SUM   ( 0x4 )



#define TRACK_CHECK_IS_FLAG(fset, f) ( ( fset & f ) != 0 )

#define TRACK_SET_TYPE(ptrack, type) (ptrack->iter_step = sizeof((type)))

#ifdef NO_TRACKPTR
#define TRACK_PTR(ptrack)       ( ptrack->__tptr )
#else
#define TRACK_PTR(ptrack)       ( (void *)( *( (addr_t *)( (char *)ptrack->__tptr + 24 ) ) ) )
#endif
#define TRACK_INC(ptrack)       ( track_move_ptr(ptrack,  ptrack->iter_step) )
#define TRACK_DEC(ptrack)       ( track_move_ptr(ptrack, -(ptrack->iter_step)) )
#define TRACK_ADD(ptrack, n)    ( track_move_ptr(ptrack,  n) )

/* временное определение */
#define TEST_PTR_ACCESS


#ifndef NO_TRACKPTR
extern int errtrack;
#endif

#ifdef NO_TRACKPTR
typedef struct {
    void *__tptr;
    unsigned int iter_step;
} track_ptr_t;
#else
/* структура, с через которую будет работать пользователь */
typedef struct {


    /*
     * идентификатор структуры указателя, через который
     * пользователь всегда сможет найти саму структуру
     * в общем списке, и получить информацию о ней
     */
    ptr_id_t __ptrid;


    /*
     * указатель на данные, хранящиеся в узле списка, в котором хранятся все
     * когда-либо выделенные структуры (и выделенная память)
     */
    void *__tptr;

#ifdef TEST_PTR_ACCESS
    void *test_ptr;
#endif // TEST_PTR_ACCESS

    /*
     * число байт, которым задётся шаг инкремента и декремента
     * для текущей выделенной памяти
     */
    unsigned int iter_step;

} track_ptr_t;
#endif /* NO_TRACKPTR */



/*
 * функция возвращает последнюю произошедшую ошибку errtrack
 *      и сбрасывает значение этой переменной
 */
#ifdef NO_TRACKPTR
inline __attribute__((always_inline)) int track_error(void) {
    return 0;
}
#else
extern int track_error(void);
#endif


/*
 * функция возвращает сообщение соответствующее ошибке errnum,
 * @errnum: значение полученое из функции track_last_error
 */
#ifdef NO_TRACKPTR
inline __attribute__((always_inline)) const char *track_str_error(int errnum) {
    return "";
}
#else
extern const char *track_str_error(int errnum);
#endif


/*
 * функция инициализирует структуры и переменные,
 *      необходимые для работы библиотеки
 * вернёт:
 *       0 в случае успеха
 *      -1 в случае какой-либо ошибки (код ошибки смотерть в errtrack)
 */
#ifdef NO_TRACKPTR
inline __attribute__((always_inline)) int track_init(void) {
    return 0;
}
#else
extern int track_init(void);
#endif


/*
 * функция очищает структуры и переменные (освобождает память),
 *      которые были необходимы во время работы библиотеки
 * вернёт:
 *       0 в случае успеха
 *      -1 в случае какой-либо ошибки (код ошибки смотерть в errtrack)
 */
#ifdef NO_TRACKPTR
inline __attribute__((always_inline)) int track_destroy(void) {
    return 0;
}
#else
extern int track_destroy(void);
#endif


/*
 * выделения памяти размером msize байт
 *      и инициализация структуры track_ptr_t
 * @msize: количество байт для выделения
 * @flags: битовые поля отвечающие за
 *      варианты работы с выделенной памятью
 * вернёт:
 *       track_ptr_t * указатель на пользовательскую структуру в случае успеха
 *       NULL в случае какой-либо ошибки (код ошибки смотерть в errtrack)
 */
extern track_ptr_t *track_malloc(size_t msize, int flags);



extern track_ptr_t *track_calloc(size_t nmemb, size_t msize, int flags);


/*
 * функция очищает выделенную по запросу пользователя память
 *      а также очищает память выделенную под управляющие структуры
 *      в том числе под переданную ptrack
 * @ptrack: указатель на пользовательскую структуру данных,
 *      через которую осуществляется управление выделенной памятью
 * вернёт:
 *       0 в случае успеха
 *      -1 в случае какой-либо ошибки (код ошибки смотерть в errtrack)
 */
extern int track_free(track_ptr_t *ptrack);

/*
 * функция пересчитывает контрольную сумму текущей выделенной памяти
 * @ptrack: указатель на пользовательскую структуру данных,
 *      через которую осуществляется управление выделенной памятью
 * вернёт:
 *       0 в случае успеха
 *      -1 в случае какой-либо ошибки (код ошибки смотерть в errtrack)
 */
#ifdef NO_TRACKPTR
inline __attribute__((always_inline)) int track_overwrite_checksum(track_ptr_t *ptrack) {
    return 0;
}
#else
extern int track_overwrite_checksum(track_ptr_t *ptrack);
#endif

/*
 * функция проверяет память на все возможные ошибки
 *      в соответствии с флагами, с которыми она была выделена
 * @ptrack: указатель на пользовательскую структуру данных,
 *      через которую осуществляется управление выделенной памятью
 * вернёт:
 *       1 если в какой-то из проверок произошла ошибка
 *       0 если все проверки прошли бех ошибок
 *      -1 в случае ошибки в функции (код ошибки смотерть в errtrack)
 */
#ifdef NO_TRACKPTR
inline __attribute__((always_inline)) int track_check_mem(track_ptr_t *ptrack) {
    return 0;
}
#else
extern int track_check_mem(track_ptr_t *ptrack);
#endif


/*
 *  функция смещает указатель на выделенную память на nmove байт
 *      nmove может быть как положительным (перемещает вперёд указатель),
 *      так и отрицательным (сдвигаем назад)
 *      все действие производятся относительно текущего адреса,
 *      на который указывает указатель
 *  @ptrack: указатель на пользовательскую структуру данных,
 *     через которую осуществляется управление выделенной памятью
 * вернёт:
 *       0 в случае успеха
 *      -1 в случае какой-либо ошибки (код ошибки смотерть в errtrack)
 */
#ifdef NO_TRACKPTR
inline __attribute__((always_inline)) int track_move_ptr(track_ptr_t *ptrack, long int nmove) {
    ptrack->__tptr = (void *)((char *)ptrack->__tptr + nmove);
    return 0;
}
#else
extern int track_move_ptr(track_ptr_t *ptrack, long int nmove);
#endif

extern void *get_list();
extern int get_offset();


/*
 * функция работает как memset, только проверяет, перекрывается память или нет,
 *      если перекрывается, вернёт ошибку
 * @dest: указатуль на структуру track_ptr_t, содержащую память,
 *      в которую трбуется скопировать
 * @src: указатуль на структуру track_ptr_t, содержащую память,
 *      из которой трбуется скопировать
 * @n: число байт, которые требуется скопировать
 * вернёт:
 *       0 в случае успеха
 *      -1 в случае какой-либо ошибки (код ошибки смотерть в errtrack)
 */
extern int track_memcpy(track_ptr_t *dest, const track_ptr_t *src, size_t n);


/*
 * функция перемещает указатель структы tset на структуру tnew,
 *      старый указатель tset перемещается в told
 *      если told равен NULL, то память по старому указателю tset очищается
 *      и контроль над ней прикращается
 * @tset: указатель на структуру track_ptr_t, содержащую указатель,
 *      который в последствии будет указывать на tnew (текущий адрес, а не начальный)
 * @tnew: указатель на структуру track_ptr_t
 * @told: указатель на структуру track_ptr_t,
 *      в которую будет сохранена старая память tset
 * вернёт:
 *       0 в случае успеха
 *      -1 в случае какой-либо ошибки (код ошибки смотерть в errtrack)
 */
extern int track_set_ptr(track_ptr_t *tset, track_ptr_t *tnew, track_ptr_t *told);


extern track_ptr_t *track_make_empty(void);

#endif /* TRACKPTR_H */


