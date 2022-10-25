
#ifndef ERRMSGS_H
#define ERRMSGS_H


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
static const char ETRACK_NO_ERROR_MSG[]             = "there are no errors";
static const char ETRACK_MEMCPY_MEM_OVERLAP_MSG[]   = "memory by pointers when copying n bytes is superimposed on each other";
static const char ERTACK_MEMCPY_DEST_UP_LIM_MSG[]   = "size n goes beyond the memory limit by the dest pointer";
static const char ERTACK_MEMCPY_SRC_UP_LIM_MSG[]    = "size n goes beyond the memory limit by the src pointer";
static const char ETRACK_MEMCPY_MSG[]               = "error in the memcpy function";

static const char ETRACK_UNKNOW_ERROR_MSG[]         = "unknown error";


#endif /* ERRMSGS_H */
