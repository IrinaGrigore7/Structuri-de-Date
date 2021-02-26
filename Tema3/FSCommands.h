//GRIGORE IRINA-DOMNICA
//315CB


#ifndef FSCOMMANDS_H
#define FSCOMMANDS_H
#include <stdio.h>
#include <stdlib.h>
#include "StringUtils.h"
#include "Trie.h"

/*verifica daca se poate adauga in cale*/
int canCreate(Trie root, const char* dir, const char* currentDir)
{
	if (startsInRoot(dir))
	{
		return canCreate(root, dir + 1, "/"); //+1 elimina primul caracter din sir (/)
	}
	else
	{
		int count = 0;
		char** splited = split(dir, &count, "/");
		int i = 0;
		char* checkPath = createAbsolutePath(currentDir, splited[0]);
		for (i = 1; i < count; ++i)
		{
			if (trieSearch(root, checkPath, 1) == 1)
			{
				checkPath = createAbsolutePath(checkPath, splited[i]);
			}
			else
			{
				return 0;
			}
		}
		return 1;
	}
}

/*creaza director*/
int mkdir(Trie* root, const char* dir, const char* currentDir)
{
	if (canCreate(*root, dir, currentDir) == 1) 
	{
		char* pathToInsert = createAbsolutePath(currentDir, dir);
		if (trieSearch(*root, pathToInsert, 1) == 0)
		{
			trieInsert(root, pathToInsert, 1);
			return 1;
		}
		else
		{
			fprintf(stderr, "%s: already exists\n", dir);
			return 0;
		}
	}
	else
	{
        fprintf(stderr, "%s: No such file or directory\n", dir);
	}
	return 0;
}

/*creaza fisier*/
int touch(Trie* root, const char* dir, const char* currentDir)
{
	if (canCreate(*root, dir, currentDir)==1) 
	{
		char* pathToInsert = createAbsolutePath(currentDir, dir);
		if (trieSearch(*root, pathToInsert, 2) == 0)
		{
			trieInsert(root, pathToInsert, 2);
			return 1;
		}
		else
		{
			fprintf(stderr, "%s: already exists\n", dir);
			return 0;
		}
	}
	else
	{
        fprintf(stderr, "%s: No such file or directory\n", dir);
	}
	return 0;
}

/*schimba directorul*/
char* cd(Trie* root, const char* dir, const char* currentDir)
{
	char* result;
	if (strcmp(dir, "/") == 0)
	{
		result = (char*)malloc(2 * sizeof(char));
		strcpy(result, "/");
	}
	else
	{
		char* absolute = createAbsolutePath(currentDir, dir);
		if (absolute != NULL && strcmp(absolute, "/") == 0)
		{
			result = (char*)malloc(2 * sizeof(char));
			strcpy(result, "/");
			return result;
		}
		if (trieSearch(*root, absolute, 1) == 1)
		{
			result = absolute;
		}
		else
		{
			Trie node = getNode(*root, absolute, 2);
			if (node != NULL)
				fprintf(stderr, "%s: Not a directory\n", dir);
			else
				fprintf(stderr, "%s: No such file or directory\n", dir);
			result = (char*)malloc(strlen(currentDir));
			strcpy(result, currentDir);
		}
	}
	return result;
}

/*afiseaza calea directorului curent*/
void pwd(const char* currentDir)
{
	printf("%s\n", currentDir);
}

/*afiseaza directoarele si fisierele din directorul curent*/
int ls(Trie* root, const char* dir, const char* currentDir, int f)
{
	Trie node;
    char* absolute = createAbsolutePath(currentDir, dir);
	int result = 1;
	if (absolute != NULL)
		result = strcmp(absolute, "/");
	if (result == 0)
	{
		node = *root;
	}
	else
	{
        node = getNode(*root, absolute, 1);
	}
	if (node)
	{
		char str[255];
		displayDir(node, str, 0, 0, f);
		printf("\n");
		return 1;
	}
	return 0;
}

/*sterge director*/
int rmdir(Trie* root, const char* dir, const char* currentDir)
{
	char* absolute = createAbsolutePath(currentDir, dir);
	if(trieSearch(*root,absolute,1)==1)
	{
		Trie node=getNode(*root, absolute, 1);
		if(node)
		{
			int count;
			countChilds(node, &count);
			if(count == 0)
			{
				trieRemove(*root, absolute, 0, 1);
				return 1;
			}
			else
			{
				fprintf(stderr,"%s: Directory not empty\n", dir);
			}
		}
		else
		{
			node=getNode(*root, absolute, 2);
			if(node)
			{
				fprintf(stderr,"%s: Not a directory\n", dir);
			}
			else
			{
				fprintf(stderr,"%s: No such file or directory\n", dir);
			}	
		}	
	}
	else
	{
		fprintf(stderr,"%s: No such file or directory\n", dir);
	}
	return 0;
}

int rmHelper(Trie* root, const char* dir, const char* currentDir)
{
	char* absolute = createAbsolutePath(currentDir, dir);
	if(trieSearch(*root,absolute, 2) == 1)
	{
		trieRemove(*root, absolute, 0, 2);
		return 1;
	}
	if(trieSearch(*root, absolute, 1) == 1)
	{
		Trie node = getNode(*root, absolute, 1);
		if(node)
		{
			int count;
			countChilds(node, &count);
			if(count == 0)
			{
				rmdir(root, dir, currentDir);
				return 1;
			}
		}
		return 1;
	}
	return 0;
}
int rm(Trie* root, const char* dir, const char* currentDir)
{
	char* absolute = createAbsolutePath(currentDir, dir);
	if(trieSearch(*root, absolute, 2) == 1)
	{
		trieRemove(*root, absolute, 0, 2);
		return 1;
	}
	if(trieSearch(*root, absolute, 1) == 1)
	{
		Trie node = getNode(*root, absolute, 1);
		if(node)
		{
			int count = 0;
			countChilds(node, &count);
			if(count == 0)
			{
				rmdir(root, dir, currentDir);
				return 1;
			}
			char** childs=(char**)malloc(count * sizeof(char*));
			char str[255];
			int current = 0;
			getChildDirs(node, str, 0, childs, &current);
			int i;
			for(i = count - 1; i >= 0; --i)
			{
				char* toRemove = createAbsolutePath(absolute, childs[i]);
				rmHelper(root, toRemove, currentDir);
			}
			rmdir(root, dir, currentDir);
		}
		return 1;
	}
	fprintf(stderr,"%s: No such file or directory\n", dir);
	return 0;
}

/*muta un fisier in alt director*/
int mv(Trie* root, const char* dir, const char* file, const char* currentDir)
{
	char* absoluteFile = createAbsolutePath(currentDir, file);
	char* absoluteDir = createAbsolutePath(currentDir, dir);
	if(trieSearch(*root, absoluteFile, 2) == 1)
	{
		if(trieSearch(*root, absoluteDir, 1)==1)
		{
			char* filename = getFilename(absoluteFile);
			if(filename)
			{
				char* newFile = createAbsolutePath(absoluteDir, filename);
				trieRemove(*root, absoluteFile, 0, 2);
				return touch(root, newFile, currentDir);
			}
		}
		else
		{
			if(trieSearch(*root, absoluteDir, 2) == 1)
			{
				fprintf(stderr,"%s: Not a directory\n", dir);
			}
			else
			{
				fprintf(stderr,"%s: No such file or directory\n", dir);
			}	
		}
	}
	else
	{
		fprintf(stderr,"%s: No such file or directory\n", file);
	}
	return 0;
}

/*copiaza un fisier in alt director*/
int cp(Trie* root, const char* dir, const char* file, const char* currentDir)
{
	char* absoluteFile = createAbsolutePath(currentDir,file);
	char* absoluteDir = createAbsolutePath(currentDir,dir);
	if(trieSearch(*root, absoluteFile, 2) == 1)
	{
		if(trieSearch(*root, absoluteDir, 1) == 1)
		{
			char* filename = getFilename(absoluteFile);
			if(filename)
			{
				char* newFile = createAbsolutePath(absoluteDir, filename);
				return touch(root, newFile, currentDir);
			}
		}
		else
		{
			fprintf(stderr,"%s: No such file or directory\n", dir);
		}
	}
	else
	{
		fprintf(stderr,"%s: No such file or directory\n", file);
	}
	return 0;
}
#endif