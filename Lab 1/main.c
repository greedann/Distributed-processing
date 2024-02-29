#include <stdio.h>
#include <stdlib.h>
#include "hashmap.h"

int hashfunc(void *key)
{
    return (int)key;
}

int comparefunc(void *key1, void *key2)
{
    return (int)key1 - (int)key2;
}

int main(int argc, char **argv)
{
    struct HashMap *map = createHashMap(hashfunc, comparefunc);
    put(map, (void *)1, (void *)2);
    put(map, (void *)2, (void *)3);
    put(map, (void *)3, (void *)4);
    put(map, (void *)4, (void *)5);
    put(map, (void *)5, (void *)6);
    put(map, (void *)10, (void *)11);
    put(map, (void *)6, (void *)7);
    put(map, (void *)7, (void *)8);
    put(map, (void *)8, (void *)9);
    put(map, (void *)9, (void *)10);

    printHashMap(map);
    // printf("%d\n", (int)get(map, (void *)1));
    // printf("%d\n", (int)get(map, (void *)2));
    // printf("%d\n", (int)get(map, (void *)3));
    erace(map, (void *)2);
    printHashMap(map);
    // printf("%d\n", (int)get(map, (void *)1));
    // printf("%d\n", (int)get(map, (void *)2));
    // printf("%d\n", (int)get(map, (void *)3));
    deleteHashMap(map);
    return 0;
}
