#include "custom_stdio.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

FILE _openFD[MAX_FILES] = {
		{ _READ,  0, 0, 'a' },
		{ _WRITE, 1, 0, 'a' },
		{ _WRITE, 2, 0, 'a' }
}; // array of file descriptors


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
		errno = EINVAL; //invalid argument
		return NULL;
	}
	for(file = _openFD; file < _openFD + MAX_FILES; file++) {
		if ((file->access_flag & (_READ | _WRITE)) == 0) {  //NEED TO CHECK IT! WHY?
			break;
			// we have found free position and our file have access to read or write
    	}
	}
	if (file >= _openFD + MAX_FILES) { // we haven't got any free position
		errno = EMFILE;
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
			lseek(fd, 0, SEEK_END);
		}
	} else if (strcmp(flags, "a+") == 0) {
		if ((fd = open(fileName, O_RDWR | O_CREAT | O_APPEND, PERMISSIONS)) != -1) {
			file->access_flag = _WRITE | _READ;
			lseek(fd, 0, SEEK_END);
		}
	}
	if (fd == -1) {
		file->access_flag = 0;
		return NULL;
	}
	file->fd = fd;
	file->count = 0;
	return file;
}


int
fgetc(FILE *file) {
	if ((file->access_flag & (_READ | _EOF | _ERR)) != _READ) {
		/* if file has opened WITHOUT READ permissions */
		return EOF;
	}
	const int read_size = 1; //amount of bytes to read
	ssize_t read_was_correct = 1;
	read_was_correct = read(file->fd, &(file->symbol), read_size);
	if (read_was_correct == 0) {
		/* we can't read anymore */
		return EOF;
	} else if (read_was_correct == -1) {
		/* error, not EOF */
		return EOF;
	}
	file->count++;
	return (unsigned int) file->symbol;

}


int
fputc(int symbol, FILE *file) {
	if ((file->access_flag & (_EOF | _ERR | _WRITE)) != _WRITE) {
		/*if file has opened WITHOUT WRITE permissions */
		return EOF;
	}
	const int write_size = 1; //amount of bytes to write
	ssize_t write_was_correct = 1;
	write_was_correct = write(file->fd, &(symbol), write_size);
	if (write_was_correct == 0) {
		/* we can't write anymore */
		return EOF;
	} else if (write_was_correct == -1) {
		/* error */
		file->count = 0;
		return EOF;
	}
	file->count++;
	return (unsigned int) symbol;
}

/* returns 1 if file is null
 * returns 2 if our pointer < start pointer
 * returns 0 if success*/
int
fseek(FILE *file, long offset, int origin) {
	if (file == NULL) {
		return 1;
 	} else {
		long check_offset;
		if (file->access_flag != 0) { // if file have at least one permission
			//check
			long diff = lseek(file->fd, 0, SEEK_END) - check_offset; //difference between end of file and cur_position
			//check
			if (diff == 0) {  // if it is end of file
				file->access_flag |= _EOF;
			} else {
				file->access_flag &= ~(_EOF); //it is not of file
			}
			diff = lseek(file->fd, 0, SEEK_SET) - check_offset; //differ between start of file and cur position
			//check
			if (diff > 0) { //cur position is "left-hand" for start position = error
				return 2;
			}
		}
	}
	return 0;
}


int
fclose(FILE *file) {
	if (file != NULL) {
		if (file->access_flag == 0) {
			return EOF;
		}
		file->access_flag = 0; //free position of this file in _openFD
		return close(file->fd);
	}
	return EOF;

}