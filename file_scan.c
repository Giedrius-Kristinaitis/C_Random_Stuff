/* compiled with gcc compiler with options -Wall -Werror -pedantic */

/* author: Giedrius Kristinaitis */

/* this program scans the current directory and it's sub-directories */
/* and for every found file checks if it is a dynamic library, */
/* an elf file, or a normal file and for normal and elf files */
/* prints out the first 10 bytes */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <ftw.h>

int process_file(const char * path, const struct stat * info, int flag, struct FTW *ftwbuff);
int process_dir(const char * name);
int is_lib(const char * file);
int is_elf(int descriptor, size_t size);
char read_byte(int descriptor, int index, size_t file_size);
int open_file(const char * name);
int close_file(int descriptor);
void print_bytes(int descriptor, int count, size_t size);

int open_file(const char * name) {
	int descriptor;
	
	descriptor = open(name, O_RDONLY);
	
	if (descriptor < 0) {
		perror("Error opening file");
		exit(255);
	}
	
	return descriptor;
}

int close_file(int descriptor) {
	int returned;
	
	returned = close(descriptor);
	
	if (returned != 0) {
		perror("Error closing file");
		exit(255);
	}
	
	return returned;
}

char read_byte(int descriptor, int index, size_t file_size) {
	int offset;
	char read_byte;
	int bytes_read;
	
	if (index >= file_size) {
		return -1;
	}
	
	offset = lseek(descriptor, index, SEEK_SET);
	
	if (offset < 0) {
		perror("Error seeking file");
		exit(255);
	}
	
	bytes_read = read(descriptor, &read_byte, 1);
	
	if (bytes_read != 1) {
		printf("Error reading file\n");
		exit(255);
	}
	
	return read_byte;
}

void print_bytes(int descriptor, int count, size_t size) {
	int i;
	char byte;
	
	for (i = 0; i < count; i++) {
		if (i >= size) {
			printf("- failas mazesnis nei 10 baitu, spausdinimas nutraukiamas");
			break;
		}
		
		byte = read_byte(descriptor, i, size);
		
		printf("%d ", byte);
	}
}

int is_lib(const char * file) {
	void * dl;
	
	dl = dlopen(file, RTLD_LAZY | RTLD_LOCAL);
	
	if (dl == NULL) {
		return 0;
	}
	
	dlclose(dl);
	
	return 1;
}

int is_elf(int descriptor, size_t size) {
	char byte1, byte2, byte3, byte4;
	
	byte1 = read_byte(descriptor, 0, size);
	byte2 = read_byte(descriptor, 1, size);
	byte3 = read_byte(descriptor, 2, size);
	byte4 = read_byte(descriptor, 3, size);
	
	if (byte1 == 0x7F && byte2 == 0x45 && byte3 == 0x4c && byte4 == 0x46) {
		return 1;
	}
	
	return 0;
}

int process_file(const char * path, const struct stat * info, int flag, struct FTW *ftwbuff) {
	if (flag != FTW_F) {
		return 0;
	}
	
	printf("%s : ", path);
	
	if (is_lib(path) == 1) {
		/* the file is a lib */
		printf("dinamine biblioteka\n");
	} else {
		/* the file is not a lib */
		int descriptor = open_file(path);
		
		if (is_elf(descriptor, info->st_size) == 1) {
			/* the file is an elf file */
			printf("(ELF failas) ");
		}
		
		print_bytes(descriptor, 10, info->st_size);
		
		printf("\n");
		
		close_file(descriptor);
	}
	
	return 0;
}

int process_dir(const char * name) {
	int returned;
	
	returned = nftw(name, process_file, 20, 0);
	
	if (returned == -1) {
		perror("Error");
		exit(255);
	}
	
	return returned;
}

/* entry point of the program */
int main(int arg_count, char ** args) {
	if (arg_count != 2) {
		printf("Blogas argumentu skaicius. Reikia nurodyti katalogo varda\n");
		exit(255);
	}
	
	printf("(C) 2019 G. Kristinaitis\n");
	
	process_dir(args[1]);
	
	return 0;
}