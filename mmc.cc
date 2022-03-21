#include<stdio.h>
#include<iostream>
#include<string.h>
#include "mmc.hpp"

namespace mmc {
// return 0 if fail.
void *Allocator::malloc(u64 available_size) {
    // 1: 確保するメモリの準備(貸し出すメモリの先頭にsize情報を埋める)
    // 2: free listの更新

    // どこを貸し出すか -> free listの先頭.
    FreeNode *cur_node = this->free_node_list;
    for(;;) {
        if(cur_node == nullptr) {
            printf("fail to malloc...\n");
            return 0;
        }
        if(cur_node->node_size_available() >= available_size) {
            // freenodeの書き換え
            // 余った領域で新しく作られるFreeNodeのaddr
            void *next_free_addr = (void*)(available_size + FreeNodeSize + (u64)cur_node);

            cur_node->toUsed();
            u32 cur_node_size = cur_node->node_size();
            cur_node->change_size(available_size+FreeNodeSize);

            // 確保するメモリよりFreeNodeのサイズの方が大きかったら、余った領域で新しくFreeNodeを作成
            // FreeNode *next_node = new(next_free_addr) FreeNode(next_free_addr, cur_node->node_size()-FreeNodeSize-size);
            // TODO: cur_node->node_size_available() == available_size だった場合のハンドルができていない.
            FreeNode *next_node = new(next_free_addr) FreeNode(next_free_addr, cur_node_size-available_size-FreeNodeSize);
            FreeNode *before = cur_node->prev;
            if (before == nullptr) {
                // 先頭のNodeが置き換えられた時は、Allocatorのfreenodeを書き換える.
                this->free_node_list = next_node;
            } else {
                before->next = next_node;
            }
            next_node->next = cur_node->next;
            next_node->prev = before;
            return cur_node->addr;
        }
        cur_node = cur_node->next;
        continue;
    }

    return 0;
}

// ptr: mallocでuserに渡したaddrが来る
int Allocator::free(void* ptr) {
    u64 u_ptr = reinterpret_cast<uintptr_t>(ptr);
    FreeNode *free_node = (FreeNode*)(u_ptr - FreeNodeSize);
    // headerの書き換え
    free_node->toFree();

    // free listへの追加
    if(this->append_node_to_free_list(free_node) != 0) {
        return -1;
    }
    return 0;
}

int Allocator::append_node_to_free_list(FreeNode *node) {
    FreeNode *current = this->free_node_list;
    if (current->prev != nullptr) {
        // currentが先頭を指してるはず.
        printf("this->free_node_list is not point to list's head, something wrong...\n");
        return -1;
    }

    void *target_addr = node->addr;
    // free_listをaddr順にsortする
    // TODO: linked listの先頭から1つずつ比較しつつ探してるためやや遅い.
    for(;;) {
        void *cmp_addr = current->addr;
        if (target_addr < cmp_addr) {
            // currentの前に挿入
            FreeNode *cur_prev = current->prev;
            if (cur_prev == nullptr) {
                // currentがfree_listの先頭だったら、
                // this->free_node_list も書き換える.
                this->free_node_list = current->prev = node;
                node->next = current;
                break;
            }
            current->prev = node;
            cur_prev->next = node;
            node->next = current;
            node->prev = cur_prev;
            break;
        }
        current = current->next;
    }

    // 解放した結果、メモリ上で連続してるfree listのnodeが生成されたら、
    // それらを連結する.
    void *my_addr = node;
    FreeNode *prev = node->prev;
    void *next = node->next;

    uint32_t prev_u = reinterpret_cast<uintptr_t>(prev);
    uint32_t next_u = reinterpret_cast<uintptr_t>(next);
    uint32_t my_addr_u = reinterpret_cast<uintptr_t>(my_addr);
    if (prev != nullptr && next != nullptr) {
        if (prev->node_size()+prev_u == my_addr_u
            && my_addr_u+node->node_size() == next_u
        ) {
            FreeNode *prev_node = (FreeNode*)prev;
            FreeNode *next_next = node->next;
            prev_node->next = next_next;
            next_next->prev = prev_node;
            prev_node->change_size(node->node_size()+prev_node->node_size()+next_next->node_size());
        }
    } else if (next != nullptr) {
        if(my_addr_u+node->node_size() == next_u) {
            // nodeとnode->nextが連続していたら
            FreeNode *next_node = node->next;
            node->next = next_node->next;
            node->change_size(node->node_size()+next_node->node_size());
        }
    } else if (prev != nullptr) {
        if (prev->node_size()+prev_u == my_addr_u) {
            // node->prevとnodeが連続していたら
            FreeNode *prev_node = (FreeNode*)prev;
            FreeNode *next_next = node->next;
            prev_node->next = next_next;
            next_next->prev = prev_node;
            prev_node->change_size(node->node_size()+prev_node->node_size());
        }
    }
    return 0;
}

void Allocator::display_mem_layout() const {
    using namespace std;
    cout << "== mem usage     ==" << endl;
    char* initial = (char*)this->initial_ptr;
    // // 10晋表示
    for(int i = 0; i < this->size; ){
        char* cur_pos = initial + i;
        if(memcmp(cur_pos, NodeFree, 4) == 0) {
            FreeNode *free_node = (FreeNode*)cur_pos;
            printf("|addr %04d-%04d ... free(header: %04d-%04d; data: %04d-%04d) |\n",i , i+free_node->node_size()-1, i,i-1+FreeNodeSize, i+FreeNodeSize, i+free_node->node_size()-1);
            i += free_node->node_size();
            continue;
        } else if(memcmp(cur_pos, NodeUsed, 4) == 0) {
            FreeNode *free_node = (FreeNode*)cur_pos;
            printf("|addr %04d-%04d ... used(header: %04d-%04d; data: %04d-%04d) |\n",i , i+free_node->node_size()-1,i,i+FreeNodeSize-1,i+FreeNodeSize, i+free_node->node_size()-1);
            i += free_node->node_size();
        } else {
            printf("something wrong...\n");
            for(;;);
        }
    }
    // HEX表示
    // printf("All Space: 0x%08x-0x%08x (size is %d)\n", initial, initial + this->size -1, this->size);
    // for(int i = 0; i < this->size; ){
    //     char* cur_pos = initial + i;
    //     if(memcmp(cur_pos, NodeFree, 4) == 0) {
    //         FreeNode *free_node = (FreeNode*)cur_pos;
    //         printf("|addr 0x%08x-0x%08x ... free(header: 0x%08x-0x%08x; data: 0x%08x-0x%08x) |\n",initial+i , initial+i+FreeNodeSize +free_node->node_size()-1, initial+i,initial+i+FreeNodeSize-1, initial+i+FreeNodeSize, initial+i+free_node->node_size()+FreeNodeSize-1);
    //         i += free_node->node_size() + FreeNodeSize;
    //         continue;
    //     } else {
    //         FreeNode *free_node = (FreeNode*)cur_pos;
    //         printf("|addr 0x%08x-0x%08x ... used(header: 0x%08x-0x%08x; data: 0x%08x-0x%08x) |\n",initial+i , initial+i+FreeNodeSize +free_node->node_size()-1, initial+i,initial+i+FreeNodeSize-1, initial+i+FreeNodeSize, initial+i+free_node->node_size()+FreeNodeSize-1);
    //         i += free_node->node_size() + FreeNodeSize;
    //     }
    // }
    cout << endl <<  "== mem usage end ==" << endl;
    return;
}

int Allocator::display_free_list() const {
    printf("## free list ##\n");
    FreeNode *cur = this->free_node_list;
    if (cur == nullptr) {
        printf("this allocator dont have free_node..\n");
        return -1;
    }
    if (cur->prev != nullptr) {
        printf("this->free_node_list is not head...\n");
        return -1;
    }
    for(;;) {
        printf("| FreeNode(addr: %lu-%lu, size: %u |", (u64)(cur->addr)-FreeNodeSize, (u64)(cur->addr)+cur->node_size()-1, cur->node_size());
        if (cur->next == nullptr) {
            break;
        }
        cur = cur->next;
        printf(" -> ");
    }
    printf("\n## free list end##\n");
    printf("\n");
    return 0;
}

}   // namespace mmc