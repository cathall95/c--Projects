#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "utils.h"

#define true  1
#define false 0

#define pid_t unsigned int

int main(int argc, char** argv) {
	int fd[2];
	pid_t child_pid;
	
	int flip = false;
	char *filename, *copyfilename;

	if (argc == 1) {

		filename      = "file.txt";
		copyfilename = "copy.txt";

	} else if (argc == 2) {

		if (strcmp(argv[1], "-f") == 0) {
			flip          = true;
			filename      = "file.txt";
			copyfilename = "copy.txt";
		} else {
			printf("Usage: copyFile [-f] [in-file out-file]\n");
			exit(-1);
		}

	} else if (argc == 3) {

		filename      = argv[1];
		copyfilename = argv[2];

	} else if (argc == 4) {

		if (strcmp(argv[1], "-f") == 0) {
			flip          = true;
			filename      = argv[2];
			copyfilename = argv[3];
		} else {
			printf("Usage: copyFile [-f] [in-file out-file]\n");
			exit(-1);
		}

	} else {
		printf("Usage: copyFile [-f] [in-file out-file]\n");
		exit(-1);
	}

	pipe(fd);

	if ((child_pid = fork()) == -1) {
		perror("fork failed");
		exit(1);
	}
	if (child_pid == 0) {
		char buf;
		FILE *to;
		
		close(fd[1]);
		to = fopen(copyfilename, "w");
		if (to == NULL) {
			perror(copyfilename);
			exit(1);
		}

		read(fd[0], &buf, 1);
		while (buf != EOF) {
			if ((int)buf != -1) {
				if (flip == 1) {
					putc(encrypt_char(buf), to);
				} else {
					putc(buf, to);
				}
			}
			read(fd[0], &buf, 1);
		}
		close(fd[0]);
		fclose(to);
		exit(0);
	} else { 
		char out;
		FILE *from;
		close(fd[0]);
		from = fopen(filename, "r");
		if (from == NULL) {
			perror(filename);
			exit(1);
		}
		
		do {
			out = getc(from);
			write(fd[1], &out, 1);
		} while (out != EOF);

		close(fd[1]);
		fclose(from);
		wait(NULL);
	}

	exit(0);
}
