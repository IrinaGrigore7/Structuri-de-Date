//GRIGORE IRINA-DOMNICA
//315CB


#ifndef STRINGUTILS_H
#define STRINGUTILS_H
#include <stdlib.h>
#include <string.h>

/*ajuta la concatenarea cuvintelor*/
char* concat(char** splited, int count, int startIndex)
{
	char* result;
	int i;
	if (startIndex >= count)
	{
		return NULL;
	}
	if (splited != NULL && splited[startIndex] != NULL) 
	{
		result = (char*)malloc(strlen(splited[startIndex]) * sizeof(char));
		strcpy(result, splited[startIndex]);
	}
	else
	{
		result = (char*)malloc(sizeof(char*));
		strcpy(result, "/");
	}
	if (count - 1 > startIndex)
	{
		strcat(result, "/");
	}
	for (i = startIndex + 1; i < count - 1; ++i)
	{
		strcat(result, splited[i]);
		strcat(result, "/");
	}
	if (count - 1 > startIndex)
	{
		strcat(result, splited[count - 1]);
	}
	return result;
}

/*desparte un string dupa un anumit separator*/
char** split(const char* string, int* count, const char* separator)
{

	int cnt = 0;
	char* aux = (char*)malloc(strlen(string) * sizeof(char));
	strcpy(aux, string);
	char* ptr = strtok(aux, separator);
	while (ptr != NULL)
	{
		cnt++;
		ptr = strtok(NULL, separator);
	}
	char** result;
	result = (char**)malloc(cnt * sizeof(char*));
	int currentIndex = 0;
	strcpy(aux, string);
	ptr = strtok(aux, separator);
	while (ptr != NULL)
	{
		result[currentIndex] = (char*)malloc(strlen(ptr) * sizeof(char));
		strcpy(result[currentIndex], ptr);
		ptr = strtok(NULL, separator);
		currentIndex++;
	}
	*count = cnt;
	return result;
}

/*obtine numele fisierului, in cazul in care este data 
	toata calea in care trebuie adaugat*/
char* getFilename(const char* path)
{
	char* result;
	int count = 0;
	char** splited = split(path, &count, "/");
	if(count == 0)
		return NULL;
	if(splited[count-1] != NULL)
	{
		result = (char*)malloc(strlen(splited[count-1]));
		strcpy(result, splited[count-1]);
		return result;
	}
	return NULL;
}

/*verificare daca primul caracter este '/'*/
int startsInRoot(const char* dir)
{
	if(dir != NULL && strlen(dir)>0)
	if (dir[0] == '/')
		return 1;
	return 0;
}

/*verificare daca se porneste din calea curenta*/
int startsInCurrentDir(const char* dir)
{
	if (dir != NULL && strlen(dir) > 0)
	if (dir[0] == '.' && dir[1] == '/')
		return 1;
	return 0;
}

char* removeDot(const char* dir)
{
	int count = 0;
	char** splited = split(dir, &count, "/");
	return concat(splited, count, 1);
}

/*formeaza calea*/
char* createAbsolutePath(const char* currentDir, const char* arg)
{
	char* result = (char*)malloc(1 * sizeof(char));
	if (arg == NULL || strlen(arg) == 0)
	{
		result = (char*)malloc(sizeof(char)*strlen(currentDir));
		strcpy(result, currentDir);
		return result;
	}
	if (startsInRoot(arg) == 1)
	{
		result = (char*)malloc(sizeof(char)*strlen(arg));
		strcpy(result, arg);
		return result;
	}
	int currentCount = 0;
	if (startsInRoot(currentDir) == 1)
	{
		currentDir++;
	}
	char** splitCurrent = split(currentDir, &currentCount, "/");
	int argCount = 0;
	char* tempArg;
	if (startsInCurrentDir(arg) == 1)
	{
		tempArg = removeDot(arg);
	}
	else
	{
		tempArg = (char*)malloc(sizeof(char)*strlen(arg));
		strcpy(tempArg, arg);
	}
	char** splitArg = split(tempArg, &argCount, "/");
	int argStart = 0;
	int i;
	for (i = 0; i < argCount; ++i)
	{
		if (strcmp(splitArg[i], "..") == 0)
		{
			argStart++;
		}
	}
	int tempCount = currentCount - argStart;
	if (tempCount >= 0)
	{	
		char* fullPath;
		if (currentCount > 0) 
		{
			fullPath = concat(splitCurrent, tempCount, 0);
		}
		else
		{
			fullPath = NULL;
		}
		char* addedPath = concat(splitArg, argCount, argStart);
		strcpy(result, "/");
		if (fullPath != NULL) {
			strcat(result, fullPath);
			if (addedPath != NULL)
				strcat(result, "/");
		}
		if (addedPath != NULL)
		{
			strcat(result, addedPath);
		}
		return result;
	}
	else
	{
        return NULL;
		result = (char*)malloc(sizeof(char)*2);
		strcpy(result, "/");
		return result;
	}
	return NULL;
}






#endif // !STRINGUTILS_H

