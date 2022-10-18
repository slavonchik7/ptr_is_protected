

#ifndef TRACKPTR_H
#define TRACKPTR_H

#include <stdint.h>


#if __WORDSIZE == 64
    typedef unsigned long int addr64_t;
    typedef addr64_t addr_t;
#else
    typedef unsigned long int addr32_t;
    typedef addr32_t addr_t;
#endif /* __WORDSIZE */

typedef addr_t ptr_id_t;




/*



    ДОБАВИТЬ СИСТЕМУ ОЧИСТКИ ВЫДЕЛЕННОЙ ПРИ ПОМОЩИ ЭТОГО МЕТОДА ПАМЯТИ,
        В СЛУЧАЕ, ЕСЛИ ПРОГРАММА БУДЕТ ЗАВЕРШИНА ПО СИГНАЛУ SIGINT ИЛИ SIGTERM
        СДЕЛАТЬ ДЛЯ ЭТОЙ ОПЦИИ ОТДЕЛЬНЫЙ ФЛАГ ???????





*/


/* коды ошибок и другая конфеденциальная информация указателя будет хранться в статическом списке внутри trackptr.c */

/* в каждой доступной пользователю структуре будет id выделенной пользователю структуры,
 * который будет являться адресом полной скрытой структуры в статическом списке скрытых структур */



#define for_each_protect_ptr(type, p)


#define PTK_INCR_GET(p)
#define PTK_GET_INCR(p)
#define PTK_INCR(p)
#define PTK_GET(p)

#define PTK_CHECK(p)
#define PTK_CHECK_HNDL(p, h)



/*
 * возможные значения флага, с которым может инициализироваться структура track_ptr_t
 */
#define TRACK_FLAGS_NOT_SET         (0b00000000)
#define TRACK_FLAG_ADDR_PROTECT     (0b00000010)
#define TRACK_FLAG_ADDR_CHECK_SUM   (0b00000100)



/*
 * возможные ошибки, во время работы со структурой track_ptr_t и памятью
 */
#define ETRACK_MEM_WAS_CHANGED          13
#define ETRACK_WENT_LOWER_LIMIT         14
#define ETRACK_WENT_UPPER_LIMIT         15
#define ETRACK_MEM_NOT_FOUND            16
#define ETRACK_NULL_PTR_PASSED          17
#define ETRACK_INIT_REQUIRED            18
#define ETRACK_ALLOC                    19
#define ETRACK_INIT_TWICE               20

#define TRACK_CHECK_IS_FLAG(fset, f) ( ( fset & f ) != 0 )







extern int errtrack;



/* структура, с через которую будет работать пользователь */
typedef struct {


    /*
     * идентификатор структуры указателя, через который
     * пользователь всегда сможет найти саму структуру
     * в общем списке, и получить информацию о ней
     */
    ptr_id_t ptrid;


    /*
     * указатель на данные, хранящиеся в узле списка, в котором хранятся все
     * когда-либо выделенные структуры (и выделенная память)
     */
    void *__tptr;


    /*
     * число байт, которым задётся шаг инкремента и декремента
     * для текущей выделенной памяти
     */
    int iter_step;

} track_ptr_t;




/*
 * функция возвращает последнюю произошедшую ошибку errtrack
 * и сбрасывает значение этой переменной
 */
extern int track_last_error(void);

/*
 * функция возвращает сообщение соответствующее ошибке errnum,
 * @errnum: значение полученое из функции track_last_error
 */
extern const char *track_str_error(int errnum);


/*
 * функция инициализирует структуры и переменные,
 * необходимые для работы библиотеки
 */
extern int track_init(void);


/*
 * функция очищает структуры и переменные (освобождает память),
 * которые были необходимы во время работы библиотеки
 */
extern int track_destroy(void);


/*
 * выделения памяти размером msize байт
 * и инициализация структуры track_ptr_t
 * @msize: количество байт для выделения
 * @flags: битовые поля отвечающие за
 *      варианты работы с выделенной памятью
 */
extern track_ptr_t *track_malloc(size_t msize, int flags);


/*
 * функция очищает выделенную по запросу пользователя память
 * а также очищает память выделенную под управляющие структуры
 * в том числе под переданную ptrack
 * @ptrack: указатель на пользовательскую структуру данных,
 *      через которую осуществляется управление выделенной памятью
 */
extern int track_free(track_ptr_t *ptrack);

/*
 * функция пересчитывает контрольную сумму текущей выделенной памяти
 * @ptrack: указатель на пользовательскую структуру данных,
 *      через которую осуществляется управление выделенной памятью
 */
extern int track_overwrite_checksum(track_ptr_t *ptrack);

/*
 * функция проверяет память на все возможные ошибки
 * в соответствии с флагами, с которыми она была выделена
 * @ptrack: указатель на пользовательскую структуру данных,
 *      через которую осуществляется управление выделенной памятью
 * вернёт:
 *       1 если в какой-то из проверок произошла ошибка
 *       0 если все проверки прошли бех ошибок
 *      -1 в случае ошибки в функции
 */
extern int track_check_mem(track_ptr_t *ptrack);


#endif /* TRACKPTR_H */
