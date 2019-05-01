/* This program copies all bytes from a file starting from n-th index to the beginning of the file */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <string.h>

int open_file(const char * name);
int close_file(int descriptor);
int get_file_size(int descriptor);
int copy_bytes(int descriptor, int file_size, int byte_index);

/* opens a file for reading and writing */
int open_file(const char * file_name) {
	int descriptor;
	
	descriptor = open(file_name, O_RDWR);
	
	if (descriptor < 0) {
		perror("Failed to open the file");
		abort();
	}
	
	printf("Opening the file\n");
	
	return descriptor;
}

/* closes a file with the given descriptor */
int close_file(int descriptor) {
	int return_value;
	
	return_value = close(descriptor);
	
	if (return_value != 0) {
		perror("Failed to close the file");
		exit(255);
	}
	
	printf("Closing the file\n");
	
	return return_value;
}

/* gets file size in bytes */
int get_file_size(int descriptor) {
	int size;
	
	size = lseek(descriptor, 0, SEEK_END);
	
	if (size < 0) {
		perror("Could not get the file size");
		exit(255);
	}
	
	lseek(descriptor, 0, SEEK_SET);
	
	return size;
}

/* copies all bytes from byte_index to the beginning of the file */
int copy_bytes(int descriptor, int file_size, int byte_index) {
	int bytes_copied;
	int offset;
	int8_t * buffer;
	size_t byte_count;
	int bytes_to_write;
	
	offset = lseek(descriptor, byte_index, SEEK_SET);
	
	if (offset == -1) {
		perror("Error seeking the file");
		exit(255);
	}
	
	buffer = (int8_t *) malloc(file_size - byte_index);
	
	byte_count = read(descriptor, buffer, file_size - byte_index);
	
	if (byte_count < 0) {
		perror("Error reading the file");
		exit(255);
	}
	
	offset = lseek(descriptor, 0, SEEK_SET);
	
	if (offset == -1) {
		perror("Error seeking the file");
		exit(255);
	}
	
	if (byte_count < byte_index) {
		bytes_to_write = byte_count;
	} else {
		bytes_to_write = byte_index - 1;
	}
	
	bytes_copied = write(descriptor, buffer, bytes_to_write);
	
	free(buffer);
	
	return bytes_copied;
}

/* entry point of the program */
int main(int arg_count, char ** args) {
	int descriptor;
	int n;
	int file_size;
	int bytes_copied;
	
	if (arg_count != 3) {
		printf("Invalid number of command line arguments!\n");
		printf("Usage: <executable> <file_name> <number>\n");
		exit(255);
	}
	
	n = atoi(args[2]);
	descriptor = open_file(args[1]);
	file_size = get_file_size(descriptor);
	
	if (n >= file_size) {
		printf("Byte index too big");
		exit(255);
	} else if(n == 0) {
		printf("Byte index was specified as 0, no work needs to be done");
		exit(0);
	}
	
	bytes_copied = copy_bytes(descriptor, file_size, n);
	
	printf("Bytes copied: %d\n", bytes_copied);
	
	close_file(descriptor);
	
	return 0;
}
