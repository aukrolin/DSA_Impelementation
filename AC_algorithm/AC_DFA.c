#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define CHARSET_SIZE 95 // 假设使用95个字符 (例如 ASCII 从 0x20 到 0x7E)

typedef struct _trie_node
{
    bool isword;
    int label;  // 可以用来标识模式的长度
    struct _trie_node **ptr, *fail;  // array of pointers
} Trie_node;

Trie_node* new_trie_node(int label){
    Trie_node* node = (Trie_node*)malloc(sizeof(Trie_node));
    node->isword = false;
    node->ptr = malloc(sizeof(Trie_node *) * CHARSET_SIZE);
    memset(node->ptr, 0, sizeof(Trie_node *) * CHARSET_SIZE);
    node->fail = NULL;
    node->label = label;
    return node;
}

struct trie
{
    Trie_node *root;
    char *charset;
};

void init_trie(struct trie* _trie){
    _trie->root = new_trie_node(0);
}

void insert(struct trie* _trie, const char *str){
    Trie_node* N = _trie->root;
    if (str == NULL) {
        return; // 如果字符串为空，直接返回
    }

    while (*str){
        char c = *str;
        int index = c - 0x20; // 计算字符在字符集中的索引

        if (N->ptr[index]) {
            N = N->ptr[index];
        } else {
            N = N->ptr[index] = new_trie_node(N->label+1);
        }
        str++;
    }
    N->isword = true;
}

void free_trie(Trie_node* node) {
    if (node == NULL) return;

    for (int i = 0; i < CHARSET_SIZE; i++) {
        if (node->ptr[i]) {
            free_trie(node->ptr[i]);
        }
    }

    free(node->ptr);
    free(node);
}

void free_trie_struct(struct trie* _trie) {
    free_trie(_trie->root);
    free(_trie);
}

#define MAX_BFS 0x10000
void build_fail_link(struct trie* _trie) {
    Trie_node* N = _trie->root;
    Trie_node** Queue = malloc(sizeof(Trie_node*) * MAX_BFS);
    
    N->fail = N;
    int qptr = 0;
    for (int i = 0; i < CHARSET_SIZE; i++) {
        if (N->ptr[i]) {
            Queue[qptr++] = N->ptr[i];
            N->ptr[i]->fail = N;  // 根节点的子节点的 fail 链指向根节点
        }
    }

    while (qptr != 0) {
        N = Queue[--qptr];
        for (int i = 0; i < CHARSET_SIZE; i++) {
            if (N->ptr[i]) {
                Trie_node* now = N->fail;
                while (now != _trie->root && !now->ptr[i]) now = now->fail;
                N->ptr[i]->fail = now->ptr[i] ? now->ptr[i] : _trie->root;
                Queue[qptr++] = N->ptr[i];
            }
        }
    }
}

void query(struct trie* _trie, const char *str) {
    Trie_node* N = _trie->root;
    const char* S = str;

    // 遍历文本中的每个字符
    while (*str) {
        char c = *str;
        int index = c - 0x20;

        // 沿着 fail 链回退，直到找到匹配的字符或者回到根节点
        while (N != _trie->root && !N->ptr[index]) {
            N = N->fail;
        }

        // 找到匹配的子节点
        if (N->ptr[index]) {
            N = N->ptr[index];
            // printf("Find: Pattern starts at position %p, length \n", N);
        }

        Trie_node* now = N;
        while (now != _trie->root) {
            
            if (now->isword) {
                // break;
                printf("Matched word: %ld to %ld\n", str - S- now->label + 1, str - S ); 
            }
            now = now->fail;  // 如果不是单词结尾，继续检查 fail 链
        }
        // if (now->isword) {
            // printf("Matched word: %ld to %ld\n", str - S- now->label + 1, str - S ); 
        // }

        str++;
    }
}

void print_fail_links_recursive(Trie_node* node, int depth) {
    if (node == NULL) {
        return;
    }

    // 打印当前节点的信息
    if (node->fail == node) {
        printf("Node at depth %d (%p): Fail -> (Self)\n", depth, node);
    } else {
        printf("Node at depth %d (%p): Fail -> %p, %s\n", depth, node,(void*)node->fail, (node->isword?"isword":""));
    }

    // 遍历所有子节点
    for (int i = 0; i < CHARSET_SIZE; i++) {
        if (node->ptr[i]) {
            print_fail_links_recursive(node->ptr[i], depth + 1);
        }
    }
}

// 打印字典树中所有节点的 fail 链
void print_fail_links(struct trie* _trie) {
    printf("Printing fail links:\n");
    print_fail_links_recursive(_trie->root, 0);
}

int main() {
    struct trie t;
    init_trie(&t);
    
    // insert(&t, "i");
    insert(&t, "he");
    insert(&t, "she");
    insert(&t, "his");
    insert(&t, "hers");
    printf("A\n");

    build_fail_link(&t);
    printf("A\n");

    print_fail_links(&t);
    query(&t, "ushersheishis");
    // 清理内存
    // free_trie_struct(&t);
    return 0;
}
//           1
// 0123456789012
// ushersheishis
//  123
//   23
//   2345
//      567
//       67
//         8
//           012
//            1 