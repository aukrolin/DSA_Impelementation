#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define CHARSET_SIZE 95 // 假设使用95个字符 (例如 ASCII 从 0x20 到 0x7E)


typedef struct _trie_node
{
    bool isword;
    struct _trie_node **ptr;//array of pointer
}Trie_node;

Trie_node* new_trie_node(){
    Trie_node* node = (Trie_node*)malloc(sizeof(Trie_node));
    node->isword =false;
    node->ptr = malloc(sizeof(Trie_node *) * CHARSET_SIZE);
    memset(node->ptr,0,sizeof(Trie_node *) * CHARSET_SIZE);
    return node;
}


struct trie
{
    Trie_node *root;
    char *charset;
};

void init_trie(struct trie* _trie){
    _trie->root = new_trie_node();
}


void insert(struct trie* _trie, const char *str){
    Trie_node* N=_trie->root;
    if (str == NULL) {
        return; // 如果字符串为空，直接返回
    }

    while (*str){
        char c = *str;
        int index = c - 0x20; // 计算字符在数组中的索引

        if(N->ptr[c - 0x20]) N = N->ptr[c - 0x20];
        else {
            N = N->ptr[c - 0x20] = new_trie_node();
        }
        str++;
    }
    N->isword = 1;
}
void free_trie(Trie_node* node) {
    if (node == NULL) return;

    for (int i = 0; i < CHARSET_SIZE; i++) {
        if (node->ptr[i]) {
            free_trie(node->ptr[i]); // 递归释放子节点
        }
    }

    free(node->ptr); // 释放指向子节点的数组
    free(node);      // 释放当前节点
}

inline void free_trie_struct(struct trie* _trie) {
    free_trie(_trie->root);
    free(_trie); 
}


bool find(struct trie* _trie, const char * str){
    Trie_node* N=_trie->root;
    if (str == NULL) {
        return false; // 如果字符串为空，直接返回
    }

    while (*str){
        char c = *str;
        int index = c - 0x20; // 计算字符在数组中的索引

        if(N->ptr[index]) N = N->ptr[index];
        else {
            return false;
        }
        str++;
    }
    return N->isword;
}



int main() {
    struct trie t;
    init_trie(&t);
    insert(&t, "abc");
    insert(&t, "bcd");
    insert(&t, "bca");
    printf("%s", (find(&t,"abc")? "true\n" : "false\n"));
    printf("%s", (find(&t,"bcde")? "true\n" : "false\n"));
    printf("%s", (find(&t,"bcd")? "true\n" : "false\n"));
    printf("%s", (find(&t,"bcc")? "true\n" : "false\n"));




}