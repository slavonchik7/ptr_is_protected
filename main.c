

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>


#include "tagptr.h"
#include "trackptr.h"



void test_malloc_time();

void test_clock_malloc_destroy();

void test_check_sum();

void print_track_error(track_ptr_t *ptrack);

void print_struct_offset();

void test_track_memcpy();

void test_migrate();

void test_data_struct_offset();

typedef struct {

    int t;
    void *b;

} test_offset_t;


int main() {

    test_migrate();

    return 0;
}















void test_malloc_time() {

    clock_t before;
    double after;
    int tcount = 100;
    double all_time = 0;

    for (int i = 0; i < tcount; i++) {
        before = clock();
        track_malloc(100000, TRACK_FLAG_ADDR_CHECK_SUM);
        after = (clock() - before);
//        printf("track_malloc proc time: %3.5f\n", after);

        all_time += after;
    }

//    printf("track_malloc proc middle time: %3.5f\n", all_time / tcount);


}



void  test_clock_malloc_destroy() {


    clock_t before;
    double after;
    int tcount = 100;
    double all_time = 0;

    for (int i = 0; i < tcount; i++) {

        before = clock();
        if ( track_init() < 0 ) {
            perror("error init\n");
            return;
        }
        after = (clock() - before);
        test_malloc_time();

    //    track_ptr_t *test_track = track_malloc(100, TRACK_FLAG_ADDR_CHECK_SUM);



        if ( track_destroy() < 0 ) {
            perror("error destroy\n");
            return;
        }
                all_time += after;
    }

    printf("track_free proc middle time: %3.5f\n", all_time / tcount * 100);


}


void test_check_sum() {

    track_init();
    track_ptr_t *crc_track = track_malloc(100, 0);

    print_track_error(crc_track);
    if ( errtrack != 0 )
        exit(1);
    #ifdef TEST_PTR_ACCESS
    unsigned char *tptr = (unsigned char *)crc_track->test_ptr;

    memset(tptr, 0, 100);

    tptr[0] = 10;
    tptr[1] = 255;
    tptr[2] = 88;
    #endif // TEST_PTR_ACCESS

    track_overwrite_checksum(crc_track);


    printf("offset: %d\n", get_offset());
    printf("byte: %d\n", *tptr);
    printf("byte: %d\n", *(unsigned char *)TRACK_PTR(crc_track));
    TRACK_INC(crc_track);
    printf("byte: %d\n", *(unsigned char *)TRACK_PTR(crc_track));
    TRACK_INC(crc_track);
    printf("byte: %d\n", *(unsigned char *)TRACK_PTR(crc_track));
    TRACK_DEC(crc_track);
    printf("byte: %d\n", *(unsigned char *)TRACK_PTR(crc_track));

    print_track_error(crc_track);

    track_destroy();


}


void print_track_error(track_ptr_t *ptrack) {

    printf("last errtrack: %d, msg: %s\n", errtrack, track_str_error(errtrack));

}


void print_struct_offset() {

//    int pa = 9;
//    test_offset_t strct = {pa, &pa};
//
//    printf("strct a: %d\n", strct.a);
//    printf("addr pa: %p\n", &pa);
//    printf("strct addr: %p\n", &strct);
//    printf("strct addr a: %p\n", &strct.a);
//    printf("strct addr b: %p\n", &strct.b);
//    printf("strct addr b val: %d\n", *(int *)(strct.b));
//    printf("strct offset addr b: %p\n", ((char *)&strct + 8));
//    printf("strct offset addr b value: %d\n", *(int *)(*( (addr_t *)((char *)&strct + 8) )) );
////    printf("strct offset addr b value: %d\n", *(int *) ((char *)&strct + 8) );
//    printf("strct offset b: %d\n", (int)offsetof(test_offset_t, b));

}


void test_track_memcpy() {



}

void test_migrate() {

    printf("lmain: %d\n", (get_list() == NULL));

    track_init();

    track_ptr_t * trtest = track_malloc(100, TRACK_FLAGS_NOT_SET);
    sprintf(TRACK_PTR(trtest), "hello world");
    printf("res str: %s\n", (char *)TRACK_PTR(trtest));
    track_move_ptr(trtest, 6);


    track_ptr_t *oldtest = track_malloc(100, TRACK_FLAGS_NOT_SET);
    sprintf(TRACK_PTR(trtest), "test old");

    printf("ALL OK\n");
    track_ptr_t *settest = track_make_empty();
    #if 1
    if ( track_set_ptr(settest, trtest, oldtest) < 0) {
        printf("error\n");
        exit(-1);
    }
    
    
    printf("set str: %s\n", (char *)TRACK_PTR(settest));
    #endif

    printf("start destroy\n");
    track_destroy();

}

void test_data_struct_offset() {
    track_ptr_t * trtest = track_malloc(100, TRACK_FLAGS_NOT_SET);

    printf("lib offsetof: %d\n", (int)__track_struct_ptr_offset);
    //printf("lib offsetof: %d\n", (int)(((track_ptr_t *)0)));

}