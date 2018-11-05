#include "custom_stdio.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

FILE *
fopen(char *fileName, char *flags)
{
	FILE *file;
	int fd;

//if flag is not correct
	int isNotCorrectFlag = strcmp(flags, "r") * strcmp(flags, "r+") * strcmp(flags, "w") * \
				strcmp(flags, "w+") * strcmp(flags, "a") * strcmp(flags, "a+");
	//printf("%d", isNotCorrectFlag);
	if (isNotCorrectFlag) {
		return NULL;
	}
	for(file = _openFD; file < _openFD + MAX_FILES; file++) {
		if ((file->access_flag & (_READ | _WRITE)) == 0) {  //NEED TO CHECK IT! WHY?
			break;
			// we have found free position and our file have access to read or write
    	}
	}
	if (file >= _openFD + MAX_FILES) { // we haven't got any free position
		return NULL;
	}

/* file is OK, so we can open it with correct permissions and mode */
	if (strcmp(flags, "r") == 0) {
		if ((fd = open(fileName, O_RDONLY)) != -1) {
			file->access_flag = _READ;
		}
	} else if (strcmp(flags, "w") == 0) {
		if ((fd = open(fileName, O_WRONLY | O_TRUNC)) != -1) {
			file->access_flag = _WRITE;
		}
	} else if (strcmp(flags, "r+") == 0) {
		if ((fd = open(fileName, O_RDWR)) != -1) {
			file->access_flag = _WRITE | _READ;
		}
	} else if (strcmp(flags, "w+") == 0) {
		if ((fd = open(fileName, O_RDWR | O_CREAT, PERMISSIONS)) != -1) {
			file->access_flag = _WRITE | _READ;
		}
	} else if (strcmp(flags, "a") == 0) {
		if ((fd = open(fileName, O_WRONLY | O_CREAT | O_APPEND, PERMISSIONS)) != -1) {
			file->access_flag = _WRITE;
		}
	} else if (strcmp(flags, "a+") == 0) {
		if ((fd = open(fileName, O_RDWR | O_CREAT | O_APPEND, PERMISSIONS)) != -1) {
			file->access_flag = _WRITE | _READ;
		}
	}
	if (fd == -1) {
		errno = 1;
		return NULL;
	}
	file->fd = fd;
	file->count = 0;
	return file;
}


int
fgetc(char *fileName) {
	
}
