

#ifndef ERRCHECK_H
#define ERRCHECK_H


#define __TRACK_CHECK_RET_INIT_REQ(ret) \
    do { \
        if ( !main_track_list ) { \
            errtrack = ETRACK_INIT_REQUIRED; \
            return ret; \
        } \
    } while ( 0 )


#define __TRACK_CHECK_RET_NULL_PTR(p, ret) \
    do { \
        if ( !p ) { \
            errtrack = ETRACK_NULL_PTR_PASSED; \
            return ret; \
        } \
    } while ( 0 )


#define __TRACK_CHECK_RET_MEM_CHANGED(pscore, ret) \
    do { \
        if ( __track_memory_changed(pscore) ) { \
            errtrack = ETRACK_MEM_WAS_CHANGED; \
            return ret; \
        } \
    } while ( 0 )


#define __TRACK_CHECK_WENT_UPPER_LIM(pscore, ret) \
    do { \
        if ( pscore->mem_cur_addr > pscore->mem_end_addr ) { \
            errtrack = ETRACK_MEM_WAS_CHANGED; \
            return ret; \
        } \
    } while ( 0 )


#define __TRACK_CHECK_WENT_LOWER_LIM(pscore, ret) \
    do { \
        if ( pscore->mem_cur_addr < pscore->mem_start_addr ) { \
            errtrack = ETRACK_MEM_WAS_CHANGED; \
            return ret; \
        } \
    } while ( 0 )


#endif /* ERRCHECK_H */
