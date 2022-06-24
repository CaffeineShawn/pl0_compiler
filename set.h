#ifndef SET_H
#define SET_H

typedef struct snode
{
	int elem;
	struct snode* next;
} snode, *symset;

symset phi, declbegsys, statbegsys, facbegsys, relset;

symset createSet(int elem, .../* SYM_NULL */);
void destroySet(symset s);
symset appendSet(symset s1, symset s2);
int checkIfInSet(int elem, symset s);
void printSet(symset s, char *symtypeDesc[]);

#endif
// EOF set.h
