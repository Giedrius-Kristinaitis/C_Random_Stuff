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
void info(const char * message);
void * map_file(int descriptor, int size);
int unmap_file(void * mapped_file, int size);
int get_file_size(int descriptor);
void print_byte(void * mapped_file, int byte_index);

/* prints a message to the console, appends new line at the end */
void info(const char * message) {
	printf("%s\n", message);
}

/* opens a file for reading and writing (append flag) */
int open_file(const char * file_name) {
	int descriptor;
	
	descriptor = open(file_name, O_RDWR | O_APPEND);
	
	if (descriptor < 0) {
		perror("Failed to open the file");
		abort();
	}
	
	info("Opening the file");
	
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
	
	info("Closing the file");
	
	return return_value;
}

/* maps the given file into virtual memory */
void * map_file(int descriptor, int size) {
	void * ptr = NULL;
	
	ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
	
	if (ptr == MAP_FAILED) {
		perror("File mapping failed");
		exit(255);
	}
	
	info("Mapping the file");
	
	return ptr;
}

/* unmaps a file from virtual memory */
int unmap_file(void * mapped_file, int size) {
	int return_value;
	
	return_value = munmap(mapped_file, size);
	
	if (return_value != 0) {
		perror("Failed to unmap the file");
		exit(255);
	}
	
	info("Unmapping the file");
	
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

/* prints the byte with the byte_index from the mapped file */
void print_byte(void * mapped_file, int byte_index) {
	printf("%d\n", *(((int8_t *)mapped_file) + byte_index));
}

/* entry point of the program */
int main(int arg_count, char ** args) {
	int n;
	int descriptor;
	int file_size;
	void * mapped_file;
	
	if (arg_count != 3) {
		info("Invalid number of command line arguments!");
		info("Usage: <executable> <file_name> <number>");
		exit(255);
	}
	
    n = atoi(args[2]);
	
	descriptor = open_file(args[1]);
	file_size = get_file_size(descriptor);
	
	if (n >= file_size) {
		info("The specified byte index is too big!");
		exit(255);
	}
	
	mapped_file = map_file(descriptor, file_size);
	
	print_byte(mapped_file, n);
	unmap_file(mapped_file, file_size);
	close_file(descriptor);
	
	return 0;
}