#include <stdio.h>
#include <stdlib.h>
#include "hashmap.h"

hash_t hashfunc(key_t key)
{
    return (hash_t)key;
}

int comparefunc(key_t key1, key_t key2)
{
    return (int)key1 - (int)key2;
}

int main(int argc, char **argv)
{
    struct HashMap *map = createHashMap(hashfunc, comparefunc);
    put(map, (key_t)1, (data_t)1);
    put(map, (key_t)2, (data_t)2);
    put(map, (key_t)3, (data_t)3);
    put(map, (key_t)4, (data_t)4);
    printHashMap(map);

    erace(map, (key_t)2);
    printHashMap(map);

    deleteHashMap(map);
    return 0;
}
