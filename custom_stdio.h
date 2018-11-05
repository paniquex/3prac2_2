#ifndef PRAC2_2_CUSTOM_STDIO_H
#define PRAC2_2_CUSTOM_STDIO_H
#include <errno.h>
#endif //PRAC2_2_CUSTOM_STDIO_H

#define EOF (-1)
#define NULL 0
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define MAX_FILES 10

enum _flags {
	_READ = 01,
	_WRITE = 02,
	_EOF = 010,
	_ERR = 020
};
typedef struct _FILE {
	int access_flag; //
	int fd; 		 // file descriptor
	int count;		//number of symbols left
	char symbol; 	//symbol, which was read
	char *next;		//ptr to next symbol
} FILE;

FILE _openFD[MAX_FILES]; // array of file descriptors
#define stdin (&_openFD[0])
#define stdout (&_openFD[1])
#define stderr (&_openFD[2])

#define PERMISSIONS 0600 // permissions for creator only (RW)
FILE *
fopen(char *fileName, char *flags);

int
fgetc(FILE *file);