#include <stdio.h>
#include <mmc/mmc.hpp>
using namespace mmc;

int main() {
    Allocator *alloc = new Allocator;
    alloc->display_mem_layout();

    void *ptr1 = alloc->malloc(10);
    alloc->display_mem_layout();

    void *ptr2 = alloc->malloc(10);
    alloc->display_mem_layout();

    alloc->free(ptr1);
    alloc->display_mem_layout();

    alloc->free(ptr2);
    alloc->display_mem_layout();

    return 0;
}