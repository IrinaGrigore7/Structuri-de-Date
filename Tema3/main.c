//GRIGORE IRINA-DOMNICA
//315CB


#include <stdio.h>
#include <stdlib.h>
#include "StringUtils.h"
#include "Trie.h"
#include "FSCommands.h"

/*in functie de comanda primita, se apeleaza functiile corespunzatoare*/
char* executeCommand(Trie* root, const char* cmd, const char* currentDir)
{
	char* current = (char*)malloc(strlen(currentDir));
	strcpy(current, currentDir);
	int count = 0;
	char** splited = split(cmd, &count, " ");
	if (count == 0)
	{
		return current;
	}
	if (strcmp(splited[0], "cd") == 0)
	{
		if (count == 1)
		{
			fprintf(stderr, "%s: missing operand\n", splited[0]);
			return current;
		}
		if (count > 2)
		{
			fprintf(stderr, "%s: too many arguments\n", splited[0]);
			return current;
		}
		current = cd(root, splited[count - 1], currentDir);
		return current;
	}
	if (strcmp(splited[0], "ls") == 0)
	{
		int f = 0;
		if(count == 1)
		{
			ls(root, NULL, currentDir, f);
			return current;
		}
		if (count > 3)
		{
			fprintf(stderr, "%s: too many arguments\n", splited[0]);
			return current;
		}
		ls(root, splited[count - 1], currentDir, f);
		return current;
    }
	if (strcmp(splited[0], "mkdir") == 0)
	{
		if (count == 1)
		{
			fprintf(stderr, "%s: missing operand\n", splited[0]);
			return current;
		}
		int i;
		for (i = 1; i < count; ++i)
		{
			mkdir(root, splited[i], currentDir);
		}
		return current;
	}
	if (strcmp(splited[0], "rmdir") == 0)
	{
		if (count == 1)
		{
			fprintf(stderr, "%s: missing operand\n", splited[0]);
			return current;
		}
		int i;
		for (i = 1; i < count; ++i)
		{
			rmdir(root, splited[i], currentDir);
		}
		return current;
	}
	if (strcmp(splited[0], "rm") == 0)
	{
		if (count == 1)
		{
			fprintf(stderr, "%s: missing operand\n", splited[0]);
			return current;
		}
		int i;
		for (i = 1; i < count; ++i)
		{
			rm(root, splited[i], currentDir);
		}
		return current;
	}
	if (strcmp(splited[0], "touch") == 0)
	{
		if (count == 1)
		{
			fprintf(stderr, "%s: missing operand\n", splited[0]);
			return current;
		}
		int i;
		for (i = 1; i < count; ++i)
		{
			touch(root, splited[i], currentDir);
		}
		return current;
	}
	if (strcmp(splited[0], "mv") == 0)
	{
		if (count < 3)
		{
			if(count > 1)
			{
				fprintf(stderr, "%s %s: missing operand\n", splited[0], splited[1]);
			}
			else
			{
				fprintf(stderr, "%s: missing operand\n", splited[0]);
			}
			return current;
		}
		int i;
		for (i = 1; i < count-1; ++i)
		{
			mv(root, splited[count-1], splited[i], currentDir);
		}
		return current;
	}
	if (strcmp(splited[0], "cp") == 0)
	{
		if (count < 3)
		{
			if(count > 1)
			{
				fprintf(stderr, "%s %s: missing operand\n", splited[0],splited[1]);
			}
			else
			{
				fprintf(stderr, "%s: missing operand\n", splited[0]);
			}
			return current;
		}
		int i;
		for (i = 1; i < count-1; ++i)
		{
			cp(root, splited[count-1], splited[i], currentDir);
		}
		return current;
	}
	if (strcmp(splited[0], "pwd") == 0)
	{
		pwd(currentDir);
		return current;
	}
	fprintf(stderr, "%s: command not found\n", splited[0]);
	return current;
}

int main()
{
	size_t len = 0;
	int cmdCount = 0;
	char* buffer = NULL;
	getline(&buffer, &len, stdin);
	cmdCount = atoi(buffer);
	Trie root = createNode();
	char* currentDir = cd(&root, "/", NULL);
	int i = 0;
	for(i = 0; i < cmdCount; ++i)
	{
	    free(buffer);
	    buffer = NULL;
	    len = 0;
	    len = getline(&buffer, &len, stdin);
	    if(buffer[len-1] == '\n')
	    {
	    	buffer[len-1] = '\0';
	    }
	    currentDir = executeCommand(&root, buffer, currentDir);
	}
	trieCleanup(&root);
	return 0;
}