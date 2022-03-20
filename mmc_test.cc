#include<stdio.h>
#include<iostream>
#include "mmc.hpp"


int basic_test2() {
    Allocator2 *alloc = new Allocator2;
    void *ptr = alloc->malloc(19);
    printf("get addr : %p\n", ptr);
    delete alloc;
    return 0;

}

// 表示確認するためだけのtest.(絶対ERRにはならない)
int display_test() {
    Allocator2 *alloc = new Allocator2;
    alloc->display_mem_layout();
    alloc->malloc(10);
    alloc->display_mem_layout();
    return 0;
}

int free_test2() {
    Allocator2 *alloc = new Allocator2;
    alloc->display_mem_layout();
    void *ptr = alloc->malloc(10);
    if (ptr == 0) {
        return 1;
    }
    alloc->display_mem_layout();
    alloc->display_free_list();
    alloc->free(ptr);
    alloc->display_mem_layout();
    alloc->display_free_list();


    // freeした後にアクセスしたら、再度使えるか
    // 現在の仕様ではこれはptr == ptr2になるべき
    void *ptr2 = alloc->malloc(10);
    if (ptr != ptr2) {
        printf("different addr aquiered, %p and %p\n", ptr, ptr2);
        return 1;
    }
    return 0;
}

int display_free_list_test() {
    Allocator2 *alloc = new Allocator2;
    void* ptr = alloc->malloc(10);
    alloc->free(ptr);
    if(alloc->display_free_list() != 0) {
        return -1;
    }
    alloc->display_mem_layout();
    return 0;
}

int free_list_left_join_test() {
    Allocator2 *alloc = new Allocator2;
    void* ptr1 = alloc->malloc(10);
    void* ptr2 = alloc->malloc(10);
    void* ptr3 = alloc->malloc(10);
    alloc->display_mem_layout();

    alloc->free(ptr1);
    alloc->display_mem_layout();
    alloc->free(ptr2);
    alloc->display_mem_layout();
    return 0;
}

int free_list_both_size_join_test() {
    Allocator2 *alloc = new Allocator2;
    void* ptr1 = alloc->malloc(10);
    void* ptr2 = alloc->malloc(10);
    void* ptr3 = alloc->malloc(10);
    alloc->display_mem_layout();

    alloc->free(ptr1);
    alloc->free(ptr3);
    alloc->display_mem_layout();
    alloc->free(ptr2);
    alloc->display_mem_layout();
    return 0;
}

int wrap_test(std::string name, int f()) {
    printf("- Test %s Start ...\n", name.c_str());
    if(f() != 0) {
        printf("Test %s Fail\n", name.c_str());
        return 1;
    }
    printf("... OK\n");
    return 0;
}

int main() {
    if(wrap_test("basic_test2",basic_test2) != 0) {
        goto ERR;
    }
    if(wrap_test("display_test",display_test) != 0) {
        goto ERR;
    }
    if(wrap_test("free_test2",free_test2) != 0) {
        goto ERR;
    }
    if(wrap_test("display_free_list_test",display_free_list_test) != 0) {
        goto ERR;
    }
    if(wrap_test("free_list_left_join_test",free_list_left_join_test) != 0) {
        goto ERR;
    }
    if(wrap_test("free_list_both_size_join_test",free_list_both_size_join_test) != 0) {
        goto ERR;
    }


    std::cout << "=== All Test Passed ===" << std::endl;
    return 0;
ERR:
    std::cout << "=== Err ===" << std::endl;
    return 1;
}
