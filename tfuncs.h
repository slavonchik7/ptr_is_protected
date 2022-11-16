

#ifndef TFUNCS_H
#define TFUNCS_H

#include "tcore.h"
#include "crc32.h"


dht_list_t *main_track_list;


void __track_links_list_full_free(
                struct_core_track_ptr_t *pscore);

/*
 * функция очищает данные по указателю ptr
 *      в случае, если ptr указывает на память в каком-либо блоке
 *      он будет удалён из списка ссылок на этот блок памяти
 *      и только после этого произойдёт очистка
 * @ptr: указатель на данные в узле списка (struct_core_track_ptr_t *)
 */
void __track_main_list_node_data_free(
                void *ptr);

/*
 * функция исключает структуру по указателю 
 */
static inline void __track_links_list_node_data_remove(
                struct_core_track_ptr_t *ptr) __attribute__((always_inline));


void __track_check_free_mem_block(
                struct_core_track_ptr_t *psc_blck);

void __track_check_free_links_list(
                struct_core_track_ptr_t *psc_links); 

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
static inline int __track_memory_changed(
                struct_core_track_ptr_t *req_check) __attribute__((always_inline));


int __track_ptr_move(
                struct_core_track_ptr_t *pscore, long int n);


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
track_ptr_t *__track_create_memory_control(
                void *pmem, size_t msize, int flags);


/*
 * функция выделяет память под узел для хранения структуры pscore
 *      и добавляет этот узел в главный список
 * @pscore: указатель на структуру struct_core_track_ptr_t
 *      которую требуется поместить в список
 * вернёт:
 *      указатель на инициализированную сруктуру npl_node_t
 *      NULL в случае ошибки (код ошибки смотерть в errtrack)
 */
npl_node_t *__track_push_core_to_main_list(
                struct_core_track_ptr_t *pscore);


/*
 * функция выделяет память под struct_core_track_ptr_t
 *      и выполняет начальную инициализацию структуры в соответствии 
 *      с переданными функции аргумента
 * в случае, если вызывавший процесс создаёт пустую управляющую структуру
 *      функция всё равно выполнет инициализацию, как при инициализации с выделением памяти
 *      если в это время в pmem не будет передан NULL, 
 *      могут произойти ошибки (поведение не определено)
 * вернёт:
 *      указатель на инициализированную сруктуру struct_core_track_ptr_t
 *      NULL в случае ошибки (код ошибки смотерть в errtrack)
 */
struct_core_track_ptr_t *__track_struct_core_first_init(
                void *pmem, size_t msize, int flags);


/*
 * функция выделяет память под track_ptr_t
 *      и выполняет начальную инициализацию структуры в соответствии 
 *      с переданным функции аргументом
 * в случае, если в pscore будет передан NULL,
 *      функция отработает в обычном режиме (не рекомендуется,
 *      так как в дальнейшем при попытки обращения к этому полю структуры track_ptr_t,
 *      вероятны ошибки сегментации)
 * вернёт:
 *      указатель на инициализированную сруктуру track_ptr_t
 *      NULL в случае ошибки (код ошибки смотерть в errtrack)
 */
track_ptr_t *__track_ptr_first_init(
                struct_core_track_ptr_t *pscore);


/*
 * функция выделяет память под track_ptr_t
 *      и выполняет начальную инициализацию структуры в соответствии 
 *      с переданными функции аргументами
 * в случае, если в l будет передан NULL, 
 *      список будет инициализирован начальными значениями и сохранён
 * @l: указатель на список, который будет сохранён в соответствующем поле структуры lmblock_t
 * @maddr: адрес, который был получен из функции семейства alloc во время выделения памяти
 * вернёт:
 *      указатель на инициализированную сруктуру track_ptr_t
 *      NULL в случае ошибки (код ошибки смотерть в errtrack)
 */
lmblock_t *__track_links_mem_block_first_init(
                dht_list_t *l, addr_t maddr);

int __track_check_overlay_cur_addr(
                const track_ptr_t *tp0, const track_ptr_t *tp2, size_t offset);

int __track_check_overlay_border_addr(
                const track_ptr_t *tp0, const track_ptr_t *tp2);

/*
 * функция переносит пересчитанные,
 *      в соответствии с pscore_new->mem_cur_addr,
 *      значения полей pscore_new в pscore_set
 * функция не проверяет значения передаваемых аргументов
 *      в случае, если хотя бы один из них NULL ошибка сегментации гарантирована
 */
void __track_migrate_cur_ptr(
                struct_core_track_ptr_t *pscore_set, 
                struct_core_track_ptr_t *pscore_new);


/*
 * функция просматривает список ссылок структуры psc_stored_list на общий блок памяти
 *      если psc_discuss там уже присутствует, то функция ничего не делает
 *      если psc_discuss там нет, то добавляем указатель на него
 *      если указатель на список структуры psc_stored_list равен NULL, то список инициализируется
 *          и psc_discuss добавляется в этот список
 * функция не проверяет значение аргумента psc_stored_list
 *      в случае, если он NULL ошибка сегментации гарантирована
 */
int __track_review_mem_links_list(
                struct_core_track_ptr_t *psc_stored_list, 
                struct_core_track_ptr_t *psc_discuss);




void __track_links_list_node_data_remove(
                struct_core_track_ptr_t *ptr) {
    ptr->links_shared_mem_block = NULL;
}

void __track_calc_memory_checksum(
                struct_core_track_ptr_t *req,
                unsigned int *res) {

    *res = Crc32((unsigned char *)req->mem_start_addr, req->msize);
}

int __track_memory_changed(
                struct_core_track_ptr_t *req_check) {

    unsigned int calc_crc;

    __track_calc_memory_checksum(req_check, &calc_crc);

    return (req_check->checksum != calc_crc);
}

#endif /* TFUNCS_H */


