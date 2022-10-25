
#ifndef ERRDEFS_H
#define ERRDEFS_H


/*
 * возможные ошибки, во время работы со структурой track_ptr_t и памятью
 */
#define ETRACK_NO_ERROR                 0
#define ETRACK_MEM_WAS_CHANGED          13
#define ETRACK_WENT_LOWER_LIMIT         14
#define ETRACK_WENT_UPPER_LIMIT         15
#define ETRACK_MEM_NOT_FOUND            16
#define ETRACK_NULL_PTR_PASSED          17
#define ETRACK_INIT_REQUIRED            18
#define ETRACK_ALLOC                    19
#define ETRACK_INIT_TWICE               20

#define ETRACK_MEMCPY                   21
#define ETRACK_MEMCPY_MEM_OVERLAP       22
#define ERTACK_MEMCPY_DEST_UP_LIM       23
#define ERTACK_MEMCPY_SRC_UP_LIM        24


#endif /* ERRDEFS_H */
