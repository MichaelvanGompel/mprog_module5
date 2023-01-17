/***********************************************************************************
 * dictionary.c
 *
 * Michael van Gompel - mprog2
 *
 * implements a dictionary's functionality using these supplementing functions:
 * - check() - compares users' input with dictionary
 * - load() - loads dictionary into hashtable
 *
 * also: unload() clears hashtable and size() calculates the dictionary size
***************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

#include "dictionary.h"

// define the number of buckets
#define NUM_BUCKETS 25000

// defining structure for linked list
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// global variables
node *hashtable[NUM_BUCKETS];
int word_count = 0;
int hash_index;

/**
 * check()
 * returns true if word is in dictionary else false
*/
bool check(const char *word)
{
    // copy word input for the hash function (input for hash needs to be lower case)
    int wlength = strlen(word);
    char *copy_word = malloc(wlength + 1);

    // copying the characters in lower case
    for (int i = 0; i < wlength + 1; i++)
    {
        copy_word[i] = tolower(word[i]);
    }

    // calculate the hash_index with the lowercase word
    hash_index = calc_hash(copy_word);
    free(copy_word);
    // create cursor node for spelling check
    node *cursor = hashtable[hash_index];

    // loop through linked list until last node (next == NULL)
    while (cursor != NULL)
    {
        // bitwise comparison with strcasecmp will result in 0 if equeal
        if (strcasecmp(cursor -> word, word) == 0)
        {
            return true;
        }
        cursor = cursor -> next;
    }

    return false;
}

/**
 * load()
 * loads dictionary into memory, returning true if successful else false
*/
bool load(const char *dictionary)
{
    // open dictionary
    FILE *file = fopen(dictionary, "r");
    // check if opening was succesful
    if (file == NULL)
    {
        return false;
    }

    // create string to temporarily save word from dictionary
    char new_word[LENGTH + 1];

    // scan the dictionary to create hash table from library word list
    while (fscanf(file, "%s", new_word) != EOF)
    {
        // allocate memory from the heap for new word
        node *new_node = malloc(sizeof(node));
        // check if allocating memory was successful
        if (new_node == NULL)
        {
            unload();
            return false;
        }

        // copy dictionary word into the node
        strcpy(new_node -> word, new_word);
        // calculate the hash for indexing in the hashtable
        hash_index = calc_hash(new_word);

        // insert the new node into the hash table
        new_node -> next = hashtable[hash_index];
        hashtable[hash_index] = new_node;
        word_count++;
    }

    fclose(file);
    return true;
}

/**
 * size()
 * returns number of words in dictionary if loaded else 0 if not yet loaded
*/
unsigned int size(void)
{
    // word count by default is 0
    return word_count;
}

/**
 *  unload()
 * unloads dictionary from memory, returning true if successful else false
*/
bool unload(void)
{
    // iteration in the hashtable
    for (int i = 0; i < NUM_BUCKETS; i++)
    {
        // appoint a cursor for clearing memory
        node *cursor = hashtable[i];

        while (cursor != NULL)
        {
            // create temporary node to hold the cursor's place before moving
            node *temp = cursor;
            cursor = cursor -> next;
            free(temp);
        }
    }

    return true;
}

/**
 * calc_hash()
 * source: Mohammed Taha
 * https://gist.github.com/MohamedTaha98/ccdf734f13299efb73ff0b12f7ce429f
 * djb2 hash function to store dictionary
*/
unsigned long calc_hash(char *str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c;
    }

    return hash % NUM_BUCKETS;
}