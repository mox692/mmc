// THESE MACROs SHOULD NOT BE CHANGED.
#define IS_DEBUG "true"

#include <cstdlib>
#include <stdint.h>
#include <iostream>
#include <stdio.h>


#ifdef IS_DEBUG
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...) 
#endif

#ifndef INITIAL_MEM_SIZE
#define INITIAL_MEM_SIZE 1000
#else
#error "INITIAL_MEM_SIZE is already defined."
#endif

namespace mmc {

typedef uint64_t u64;
typedef uint32_t u32;

constexpr u32 FreeNodeSize = 40;
const char NodeUsed[4] = {0x10, 0x31, 0x19, 0x23};
const char NodeFree[4] = {0x49, 0x12, 0x09, 0x78};

// TODO: fix class size and field ordering in memory.
class FreeNode {
public:
    char magic[4];
    FreeNode *next;
    FreeNode *prev;
    FreeNode(void *addr, u64 size): size(size), next(nullptr), prev(nullptr) {
        this->addr = (void*)((u64)addr + FreeNodeSize);
        this->toFree();
    };
    ~FreeNode() {};

    // (headerも含めた)free node全体のサイズ
    u32 node_size() const {
        return this->size;
    };
    // (headerは含まない)実際にmalloc呼び出し元に提供できる分のサイズ
    u32 node_size_available() const {
        return this->size-FreeNodeSize;
    };

    // total_size: headerも含めたsize
    void toUsed() {
        // this->size = total_size;
        for(int i = 0; i < 4; i++) {
            this->magic[i] = NodeUsed[i];
        }
    }
    void toFree() {
        for(int i = 0; i < 4; i++) {
            this->magic[i] = NodeFree[i];
        }
    }
    // 実際にappに貸し出すaddr(コンストラクタ参照)
    // magicを書き換える & sizeを変更する
    void *addr;
    void change_size(u32 size) {
        this->size = size;
    }
private:
    // (headerも含めた)free node全体のサイズ
    u32  size;
};

// freenodeだけ、確保したメモリにおくようにする
// それ以外は予めなんらかの方法でメモリが確保されているものとする.
class Allocator {
public:
    Allocator() {
        this->initial_ptr = std::malloc(INITIAL_MEM_SIZE);
        this->size = INITIAL_MEM_SIZE;
        // 領域全てをfreelistに.
        this->free_node_list = new(initial_ptr) FreeNode(this->initial_ptr, this->size);
    };
    ~Allocator() {
        // initial_ptrを開放
        std::free(this->initial_ptr);
    };
    void *malloc(u64);
    int  free(void*);
    int  append_node_to_free_list(FreeNode*);
    void display_mem_layout() const;
    int  display_free_list() const;

    FreeNode *free_node_list;
private:
    // はじめに確保されるメモリ領域の先頭を指すpointer
    void *initial_ptr;
    // 管理領域のsize
    u64  size;
};
}   // namespace mmc