#include <stdio.h>
#include <mmc/mmc.hpp>

int main() {
    printf("hey\n");
    Allocator2 *alloc = new Allocator2;
    alloc->display_free_list();
}