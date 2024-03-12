#include <stdio.h>
#include <stdlib.h>

struct HashElement
{
    void *key;
    void *value;
    struct HashElement *next;
};

struct HashNode
{
    void *hash;
    struct HashElement *element;
    struct HashNode *next;
    struct HashNode *prev;
};

struct HashMap
{
    void *(*hashfunc)(void *key);
    int (*comparefunc)(void *key1, void *key2);
    struct HashNode *firstelement;
    struct HashNode *lastelement;
    int size;
};

struct HashMap *createHashMap(void *(*hashfunc)(void *key), int (*comparefunc)(void *key1, void *key2));

void *get(struct HashMap *map, void *key);

void put(struct HashMap *map, void *key, void *value);

void erace(struct HashMap *map, void *key);

void deleteHashMap(struct HashMap *map);

void printHashMap(struct HashMap *map);
