#include <cstddef>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "set.h"

symset appendSet(symset s1, symset s2)
{
	symset s;
	snode* p;
	
	s = p = (snode*) malloc(sizeof(snode));
	s->elem = 0;
	p->elem = 0;
	while (s1 && s2)
	{
		p->next = (snode*) malloc(sizeof(snode));
		p = p->next;
		if (s1->elem < s2->elem)
		{
			p->elem = s1->elem;
			s1 = s1->next;
		}
		else
		{
			p->elem = s2->elem;
			s2 = s2->next;
		}
	}

	while (s1)
	{
		p->next = (snode*) malloc(sizeof(snode));
		p = p->next;
		p->elem = s1->elem;
		s1 = s1->next;
		
	}

	while (s2)
	{
		p->next = (snode*) malloc(sizeof(snode));
		p = p->next;
		p->elem = s2->elem;
		s2 = s2->next;
	}

	p->next = NULL;

	return s;
} // appendSet

void insertIntoSet(symset s, int elem)
{
	snode* p = s;
	snode* q;

	while (p->next && p->next->elem < elem)
	{
		p = p->next;
	}
	
	q = (snode*) malloc(sizeof(snode));
	q->elem = elem;
	q->next = p->next;
	p->next = q;
} // insertIntoSet

symset createSet(int elem, .../* SYM_NULL */)
{
	va_list list;
	symset s;

	s = (snode*) malloc(sizeof(snode));
	s->elem = 0;
	s->next = NULL;

	va_start(list, elem);
	while (elem)
	{
        insertIntoSet(s, elem);
		elem = va_arg(list, int);
	}
	va_end(list);
	return s;
} // createSet

void destroySet(symset s)
{
	snode* p;

	while (s)
	{
		p = s;
		s = s->next;
		free(p);
	}
} // destroySet

int checkIfInSet(int elem, symset s)
{
	s = s->next;
	while (s && s->elem < elem)
		s = s->next;

	if (s && s->elem == elem)
		return 1;
	else
		return 0;
} // checkIfInSet

void printSet(symset s, const char *symtypeDesc[]) {
    while (s->next) {
		int idx = s->elem;
		if (s->elem <= 41) {
        	printf("%s->", symtypeDesc[s->elem]); 
		}

        s=s->next;
    }
    printf("END \r\n");
}

// EOF set.c
