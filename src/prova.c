#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "roaring.c"


void main() {
    //rrd_get_error();
    roaring_bitmap_t *bitmap = roaring_bitmap_create();
    for(i=0;i<10;i++){
        roaring_bitmap_add(bitmap, i);
    }
    roaring_free(res[i]->ports);
}