#include <string.h>
#include <stdlib.h>
#define _CRT_SECURE_NO_WARNINGS //windows nonsense

class FilePath
{
public:
	char *name;
	char *extension;
	char *path;
	
	FilePath(const char *path)
	{
		char *fullPath = (char*) malloc(sizeof(char) * (strlen(path) + 1) );
		strncpy(fullPath, path, strlen(path)+1);
		
		this->extension = NULL;
		this->name = NULL;
		this->path = NULL;
		
		char *extensionStart = getExtensionPos(fullPath);
		char *nameStart = getNamePos(fullPath);
		
		this->extension = getPart(extensionStart);
		extensionStart[-1] = '\0';
		
		this->name = getPart(nameStart);
		nameStart[0] = '\0';
		
		this->path = getPart(fullPath);
		
		free(fullPath);
	}
	
	~FilePath()
	{
		free(name);
		free(extension);
		free(path);
	}
	
private:
	
	char* getPart(char *start)
	{
		int partLength = strlen(start);
		char *temp = (char*) malloc(sizeof(char) * (partLength + 1) );
		strncpy(temp, start, partLength+1);
		return temp;
	}

	char* getNamePos(char *file)
	{
		char *slashPosition = strrchr(file, '/');
		if(slashPosition == NULL)
			slashPosition = strrchr(file, '\\');
		if(slashPosition == NULL)
			return file;
		
		slashPosition++;
		return slashPosition;
	}
	
	char* getExtensionPos(char *file)
	{
		char *dotPosition = strrchr(file, '.');
		if(dotPosition == NULL)
			return NULL;
		
		dotPosition++;
		return dotPosition;
	}
	
	static const int maxExtensionLength = 10;
	static const int maxPathLength = 500;
	static const int maxName = 256;
};