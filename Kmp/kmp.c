#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool end_char_set(char c) {
    return(
        c == '\n' 
    // ||  c == ' ' 
    // ||  c == EOF
    );

}

char * input_string(unsigned len){
    char *buf = (char *)malloc(len+1);
    
    char c;
    unsigned int cnt = 0;
    while (cnt < len){
        if (end_char_set(c = getchar())) break;
        buf[cnt] = c;
        cnt ++;
    }
    buf[cnt] = 0;
    return buf;
}

int* build_fail_array(char *match){
    int len = strlen(match), now=0;
    int *fail = (int *)malloc((len + 1) * sizeof(int));
    fail[0] = -1;//Fail Function a.k.a Longest Prefix subfix array
    

    // printf("%d ", fail[0]);
    //now point two lefts
    for (int i = 1; i < len; ++i) {
        while(now && match[now]!=match[i]) now=fail[now-1]; //easy to see
        if(match[now]==match[i]) fail[i]= ++now; 
        // printf("%d ", fail[i]);
    }
    // printf("\n");
    return fail;
}

void KMP(char *content, char *match){
    int *fail = build_fail_array(match);
    int len = strlen(content),tar = strlen(match), now=0;
    char * res;

    for(int i=0 ; i<len; ++i){
        while (now && content[i] != match[now]) now = fail[now-1];
        if (content[i] == match[now])now ++;
        if (now == tar) {
            // printf("\n");
            printf("Find at%d\n", i+1-now);
            now = fail[now-1]; //this time matches at i - now
        }
        // printf("%d ", now);
    }
    free(fail);
    fail = 0;

}

int main() {
    char *content = input_string(100);
    char *match = input_string(20);

    // build_fail_array(match);
    KMP(content,match);
    printf("\n");



    free(content);
    free(match);
    content = 0;
    match = 0;

}


