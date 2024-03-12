#include <stdio.h>
#include <stdlib.h>
#include "hashmap.h"

void * hashfunc(void *  key)
{
    return (void * )key;
}

int comparefunc(void *  key1, void *  key2)
{
    return (int)key1 - (int)key2;
}

int main(int argc, char **argv)
{
    int *arr = malloc(sizeof(int)*5);
    for (int i = 0; i < 5; i++)
    {
        arr[i] = i+1;
    }
    struct HashMap *map = createHashMap(hashfunc, comparefunc);

    put(map, (void *)1, (void *)2);
    put(map, (void *)2, (void *)3);
    put(map, (void *)3, (void *)4);
    put(map, (void *)4, (void *)5);
    put(map, (void *)5, (void *)6);

    printHashMap(map);

    deleteHashMap(map);
    return 0;
}
