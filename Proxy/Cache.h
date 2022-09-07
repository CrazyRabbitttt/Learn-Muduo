#ifndef PROXY_CAHCE_H
#define PROXY_CAHCE_H

#define MAX_CACHE_SIZE  1049000
#define MAX_OBJECT_SIZE 102400


// 双向链表中存储的属性等
typedef struct _obj_t { 
    char flag;                      // judge if tear or front
    // char uri[100];
    const char* uri;
    char respHeader[1024];
    char respBody[MAX_OBJECT_SIZE];
    int respHeaderLen;
    int respBodyLen;
    struct _obj_t* prev;
    struct _obj_t* next;
}obj_t;


typedef struct cache_t{
    obj_t* head;
    obj_t* tail;
    int nitems;
}cahce_t;


void initializeCache(cahce_t* );

#endif

