#ifndef _UTILS_H
#define _UTILS_H

typedef struct hash_node_s
{
	char	key[65];
	char	*data;
	struct hash_node_s *next;
} HashNode;

typedef struct hash_table_s
{
	int			 size;
	HashNode	**bucket;
} HashTable;

void set_errmsg(char *errfmt, ...);
char* get_errmsg();

#define GetError	get_errmsg
#define SetError	set_errmsg

#endif
