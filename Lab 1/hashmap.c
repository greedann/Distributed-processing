#include <stdio.h>
#include <stdlib.h>
#include "hashmap.h"

struct HashMap *createHashMap(int (*hashfunc)(key_t key), int (*comparefunc)(key_t key1, key_t key2))
{
    struct HashMap *map = malloc(sizeof(struct HashMap));
    map->hashfunc = hashfunc;
    map->comparefunc = comparefunc;
    map->size = 0;
    map->firstelement = NULL;
    map->lastelement = NULL;
    return map;
}

data_t get(struct HashMap *map, key_t key)
{
    hash_t hash = map->hashfunc(key);
    struct HashNode *node = map->firstelement;
    while (node != NULL)
    {
        if (node->hash == hash)
        {
            struct HashElement *element = node->element;
            while (element != NULL)
            {
                if (map->comparefunc(element->key, key) == 0)
                {
                    return element->value;
                }
                element = element->next;
            }
        }
        node = node->next;
    }
    return 0;
}

void put(struct HashMap *map, key_t key, data_t value)
{
    hash_t hash = map->hashfunc(key);
    struct HashNode *node = map->firstelement;
    while (node != NULL)
    {
        int compare_result = map->comparefunc(node->hash, hash);
        if (compare_result == 0) // hash already exists
        {
            struct HashElement *element = node->element;
            while (element != NULL)
            {
                if (map->comparefunc(element->key, key) == 0)
                {
                    element->value = value;
                    return;
                }
                element = element->next;
            }
            struct HashElement *newelement = malloc(sizeof(struct HashElement));
            newelement->key = key;
            newelement->value = value;
            newelement->next = node->element;
            node->element = newelement;
            return;
        }
        else if (compare_result > 0)
        {
            // insert new node before current node
            struct HashNode *newnode = malloc(sizeof(struct HashNode));
            newnode->hash = hash;
            newnode->element = malloc(sizeof(struct HashElement));
            newnode->element->key = key;
            newnode->element->value = value;
            newnode->element->next = NULL;

            newnode->next = node;
            newnode->prev = node->prev;
            if (node->prev != NULL)
            {
                node->prev->next = newnode;
            }
            else
            {
                map->firstelement = newnode;
            }
            node->prev = newnode;

            map->size++;
            return;
        }

        node = node->next;
    }
    struct HashNode *newnode = malloc(sizeof(struct HashNode)); // insert new node at the end
    newnode->hash = hash;
    newnode->element = malloc(sizeof(struct HashElement));
    newnode->element->key = key;
    newnode->element->value = value;
    newnode->element->next = NULL;

    newnode->next = NULL;
    newnode->prev = map->lastelement;

    if (map->lastelement != NULL)
    {
        map->lastelement->next = newnode;
    }
    else
    {
        map->firstelement = newnode;
    }

    map->lastelement = newnode;

    map->size++;
}

void erace(struct HashMap *map, key_t key)
{
    hash_t hash = map->hashfunc(key);
    struct HashNode *node = map->firstelement;
    while (node != NULL)
    {
        if (node->hash == hash)
        {
            struct HashElement *element = node->element;
            struct HashElement *prev = NULL;
            while (element != NULL)
            {
                if (map->comparefunc(element->key, key) == 0)
                {
                    if (prev == NULL)
                    {
                        node->element = element->next;
                        if (node->element == NULL)
                        {
                            if (node->prev != NULL)
                            {
                                node->prev->next = node->next;
                            }
                            else
                            {
                                map->firstelement = node->next;
                            }
                            if (node->next != NULL)
                            {
                                node->next->prev = node->prev;
                            }
                            else
                            {
                                map->lastelement = node->prev;
                            }
                            free(node);
                        }
                    }
                    else
                    {
                        prev->next = element->next;
                    }
                    free(element);
                    map->size--;
                    return;
                }
                prev = element;
                element = element->next;
            }
        }
        node = node->next;
    }
}

void deleteHashMap(struct HashMap *map)
{
    struct HashNode *node = map->firstelement;
    while (node != NULL)
    {
        struct HashElement *element = node->element;
        while (element != NULL)
        {
            struct HashElement *next = element->next;
            free(element);
            element = next;
        }
        struct HashNode *next = node->next;
        free(node);
        node = next;
    }
    free(map);
}

void printHashMap(struct HashMap *map)
{
    struct HashNode *node = map->firstelement;
    while (node != NULL)
    {
        struct HashElement *element = node->element;
        while (element != NULL)
        {
            printf("key: %d, value: %d\n", (int)element->key, (int)element->value);
            element = element->next;
        }
        node = node->next;
    }
}