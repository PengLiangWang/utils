#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "utils.h"

static char error[256];

void set_errmsg(char *errfmt, ...)
{
	memset(error, 0, sizeof(error));

	va_list	 arg;
	va_start(arg, errfmt);
	vsnprintf(error, 255, errfmt, arg);
	va_end(arg);
}

char *get_errmsg()
{
	return error;
}

int hashCode(const char *key, int len)
{
	int		hcode;
	return hcode;
}

HashTable *HashTable_New(int size)
{
	HashTable *htable = malloc(sizeof(HashTable));
	htable->size = size;
	htable->bucket = calloc(size, sizeof(HashNode));	
	return htable;
}

void HashTable_Destroy(HashTable **htable)
{
	free((*htable)->bucket);
	free(*htable);
	*htable = NULL;
}

HashNode *HashNode_New(char *key, char *data)
{
	HashNode	*node = malloc(sizeof(HashNode));
	strncpy(node->key, key, 64);
	node->data = malloc(256);
	strncpy(node->data, data, 255);
	node->next = NULL;
	return node;
}

int HSetValue(HashTable *htable, char *key, char *data)
{
	if(strlen(key) > 64)
		return -1;
	int index = hashCode(key, strlen(key)) % htable->size;	
	
	HashNode *hnode = HashNode_New(key, data);

	if(htable->bucket[index] == NULL)
	{
		htable->bucket[index] = hnode;	
	}
	else
	{
		hnode->next = htable->bucket[index];
		htable->bucket[index] = hnode;
	}

	return 0;
}

int HGetValue(HashTable *htable, char *key, char *value)
{
	int index = hashCode(key, strlen(key)) % htable->size;

	HashNode *hnode = htable->bucket[index];

	if(hnode == NULL)
		return -1;
	else
	{
		while(hnode != NULL){
			if(strcmp(hnode->key, key) == 0)
			{
				strcpy(value, hnode->data);
				return 0;
			}
			hnode = hnode->next;
		}
	}

	return -1;
}


