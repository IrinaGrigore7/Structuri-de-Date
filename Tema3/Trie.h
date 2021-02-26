//GRIGORE IRINA-DOMNICA
//315CB


#ifndef TRIE_H
#define TRIE_H

#include <stdio.h>
#include <stdlib.h>	
#include <string.h>

typedef struct Nod
{
	int nodeType; //0-Nod 1-Director 2-Fisier
	struct Nod *copii[38];
}TNod, *Trie;

/*functie de alocare a unui nod in trie*/
Trie createNode()
{
	Trie nod = (Trie)malloc(sizeof(TNod));
	if (!nod)
		return NULL;
	int i;
	nod->nodeType = 0;
	for (i = 0; i < 38; i++)
		nod->copii[i] = NULL;
	return nod;
}

/*intoarece indicele fiecarui caracter
0-25: litere de la a la z; 26-35: cifre; 36: '.'; 37: '/'*/
int getIndex(char chr)
{
	if ('a' <= chr && chr <= 'z')
		return chr - 'a';
	if ('0' <= chr && chr <= '9')
		return 26 + chr - '0';
	if (chr == '.')
		return 36;
	if (chr == '/')
		return 37;
	return -1;
}

/*transforma indicele in caracter*/
char getChar(int index)
{
	if (0 <= index && index <= 25)
		return index + 'a';
	if (26 <= index && index <= 35)
		return index - 26 + '0';
	if (index == 36)
		return '.';
	if (index == 37)
		return '/';
	return -1;
}

/*insereaza un string in arbore*/
void trieInsert(Trie *head, char* str, int type)
{
	Trie aux = *head;
	while (*str)
	{
		if (aux->copii[getIndex(*str)] == NULL)
			aux->copii[getIndex(*str)] = createNode();
		aux = aux->copii[getIndex(*str)];
		str++;
	}
	aux->nodeType = type;
}

/*cauta un string in arbore*/
int trieSearch(Trie head, char* str, int type)
{
	if (head == NULL)
	{
		return 0;
	}
    if (str == NULL)
    {
        return 0;
    }
	Trie aux = head;
	while (*str)
	{
		aux = aux->copii[getIndex(*str)];
		if (aux == NULL)
		{
			return 0;
		}
		str++;
	}
	if (aux->nodeType == type)
		return 1;
	else
		return 0;
}

/*functie de cautare a unui string in trie*/
Trie getNode(Trie head, char* str, int type)
{
	if (head == NULL)
	{
		return NULL;
	}
    if (str == NULL)
    {
        return NULL;
    }
	Trie aux = head;
	while (*str)
	{
		aux = aux->copii[getIndex(*str)];
		if (aux == NULL)
		{
			return NULL;
		}
		str++;
	}
	if (aux->nodeType == type)
		return aux;
	else
		return NULL;
}

/*verifica daca arborele este null*/
int trieEmpty(Trie head)
{
	if(head==NULL)
		return 1;
	int i;
	for (i = 0; i < 38; i++)
		if (head->copii[i])
			return 0;
	return 1;
}

/*parcurge arborele in mod recursiv pana gasete string-ul in arbore;
  apoi il sterge*/
Trie trieRemove(Trie head, char* str, int depth, int type)
{
	if (!head)
	{
		return NULL;
	}
	if (depth == strlen(str))
	{
		if (head->nodeType != 0)
			head->nodeType = 0;
		if (trieEmpty(head) == 1)
		{
			free(head);
			head = NULL;
		}
		return head;
	}
	int index = getIndex(str[depth]);
	head->copii[index] = trieRemove(head->copii[index], str, depth + 1,type);
	if (trieEmpty(head) && head->nodeType == type)
	{
		free(head);
		head = NULL;
	}
	return head;
}

/*elibereaza toti copii dintr-un nod*/
void trieCleanupNode(Trie head)
{
    if(!head)
        return;
    int i;
    for (i = 0; i < 37; i++)
        trieCleanupNode(head->copii[i]);
    free(head);
}

/*elibereaza arborele*/
void trieCleanup(Trie *head)
{
    if(!(*head))
        return;
    trieCleanupNode(*head);
    *head = NULL;
}

/*pentru director returneaza '/', iar pentru fisier '*'*/
char getSpecialChar(int type)
{
	switch (type)
	{
	case 1:
		return '/';
		break;
	case 2: 
		return '*';
	default:
		break;
	}
	return '0';
}

/*functie de afisare*/
void trieDisplay(Trie root, char str[], int level, int type, int f)
{
	if (root == NULL)
		return;
	if (root->nodeType != 0)
	{
		if (f != 0)
		{
			str[level] = getSpecialChar(root->nodeType);
			level++;
		}
		str[level] = '\0';
		printf("%s ",str+1);
	}
	int i;
	for(i = 0; i < 37; ++i)
	{
		if (root->copii[i] != NULL)
		{
			str[level] = getChar(i);
			trieDisplay(root->copii[i], str, level + 1, type, f);
		}
	}
}

void displayDir(Trie root, char str[], int level, int type, int f)
{
	int i;
	for (i = 0; i < 38; ++i)
	{
		if (root->copii[i] != NULL)
		{
			str[level] = getChar(i);
			trieDisplay(root->copii[i], str, level+1, type, f);
		}
	}
}

/*numara toti copii dintr-un nod*/
void trieCount(Trie root, int* count)
{
	if (root == NULL)
		return;
	if (root->nodeType != 0)
	{
		(*count)=(*count) + 1;
		
	}
	int i;
	for(i = 0; i < 38; ++i)
	{
		if (root->copii[i] != NULL)
		{
			trieCount(root->copii[i], count);
		}
	}
}

void countChilds(Trie root, int* count)
{
	(*count)=0;
	if (root->copii[37] != NULL)
	{	
		trieCount(root->copii[37], count);
	}
}

void trieGetChild(Trie root, char str[], int level, char** childs, int* current)
{
	if (root == NULL)
		return;
	if (root->nodeType != 0)
	{
		childs[*current]=(char*)malloc(strlen(str) * sizeof(char));
		strcpy(childs[*current],str+1);
		(*current)=(*current)+1;	
	}
	int i;
	for(i = 0; i < 38; ++i)
	{
		if (root->copii[i] != NULL)
		{
			str[level] = getChar(i);
			trieGetChild(root->copii[i],str, level + 1, childs, current);
		}
	}
}

void getChildDirs(Trie root, char str[], int level, char** childs, int* current)
{
		if (root->copii[37] != NULL)
		{
			str[level] = getChar(37);
			trieGetChild(root->copii[37],str, level + 1, childs, current);
		}
}

#endif // !TRIE_H


