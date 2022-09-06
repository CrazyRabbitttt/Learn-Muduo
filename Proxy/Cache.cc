#include "Cache.h"

#include <stdlib.h>

void initializeCache(cahce_t* cache) {
    // 进行双向链表的初始化操作
    cache->head = (obj_t*)malloc(sizeof(*(cache->head)));
    cache->head->flag = '@';
    cache->head->prev = NULL;
    cache->head->next = NULL;


    cache->tail = (obj_t*)malloc(sizeof(*(cache->tail)));
    cache->tail->flag = '@';
    cache->tail->prev = NULL;
    cache->tail->next = NULL;


    cache->head->next = cache->tail;
    cache->tail->prev = cache->head;

    cache->nitems = 0;

}