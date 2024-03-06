#include <stdio.h>
#include <stdlib.h>

typedef int hash_t;
typedef int data_t;
typedef int key_t;

struct HashElement
{
    key_t key;
    data_t value;
    struct HashElement *next;
};

struct HashNode
{
    hash_t hash;
    struct HashElement *element;
    struct HashNode *next;
    struct HashNode *prev;
};

struct HashMap
{
    int (*hashfunc)(key_t key);
    int (*comparefunc)(key_t key1, key_t key2);
    struct HashNode *firstelement;
    struct HashNode *lastelement;
    int size;
};

struct HashMap *createHashMap(hash_t (*hashfunc)(key_t key), int (*comparefunc)(key_t key1, key_t key2));

data_t get(struct HashMap *map, key_t key);

void put(struct HashMap *map, key_t key, data_t value);

void erace(struct HashMap *map, key_t key);

void deleteHashMap(struct HashMap *map);

void printHashMap(struct HashMap *map);
