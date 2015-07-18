#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "args.h"
#include "dyn_array.h"

#define MAXMEMSIZE 100000
#define MAXPROGSIZE 100000
#define DEFMEMSIZE 30000
#define MAXITERATIONS 40000

size_t memsize = DEFMEMSIZE;

// Handles arguments, allocates mem, prog.
void init(int argc, const char* argv[], char **mem, char **prog) {
	/* Flags:
		-i arg		:: provide BF program directly as an argument
		[-f] arg	:: grab program from a file
		-mi arg		:: initialise mem contents to arg
		-mf arg		:: initialise mem contents from the file located at arg
		-s num		:: specify memory to be num bytes long -- if not provided, default to DEFMEMSIZE bytes
	*/

	bool prog_supplied = false;

	init_arglist(argc, argv);
	
	// Handle memory size, if specified
	const char *arg = first_arg_flag("-s");
	
	if (arg) {
		memsize = (size_t)strtol(arg, NULL, 0);
		if (memsize > MAXMEMSIZE) {
			fprintf(stderr, "Specified mem size exceeds max.\n");
			exit(1);
		}
		else if (memsize < 1) {
			fprintf(stderr, "C'mon, at least gimme one memory cell to work with.");
		}
	}

	// Load in BF program
	arg = first_arg_flag("");

	if (!arg) {
		arg = first_arg_flag("-f");
	}
	
	if (arg) {
		prog_supplied = true;

		FILE *pfile = fopen(arg, "r");

		if (!pfile) {
			fprintf(stderr, "Error opening file %s.\n", arg);
			exit(1);
		}

		fseek(pfile, 0, SEEK_END);
		size_t progsize = ftell(pfile) + 1;
		fseek(pfile, 0, SEEK_SET);

		if (progsize >= MAXPROGSIZE) {
			fprintf(stderr, "Program too long.\n");
			exit(1);
		}
		

		if (!(*prog = (char *)malloc(progsize + 1))) {
			fprintf(stderr, "Prog malloc failed.\n");
			exit(1);
		}
		
		char *pos = *prog;
		while (!feof(pfile)) {
			int c = fgetc(pfile);

			switch (c) {
			case '+':
			case '-':
			case '>':
			case '<':
			case '[':
			case ']':
			case '.':
			case ',':
				*pos = (char)c;
				++pos;
			}
		}

		*pos = 0;

		fclose(pfile);
	}
	else {
		arg = first_arg_flag("-i");

		if (arg) {
			prog_supplied = true;

			if (strlen(arg) >= MAXPROGSIZE) {
				fprintf(stderr, "Given program exceeds maximum size.\n");
				exit(1);
			}

			if (!(*prog = (char *)malloc(strlen(arg) + 1))) {
				fprintf(stderr, "Prog malloc failed.\n");
				exit(1);
			}
			else {
				char *pos = *prog;
				const char *c = arg;

				while (*c) {
					switch (*c) {
					case '+':
					case '-':
					case '>':
					case '<':
					case '[':
					case ']':
					case '.':
					case ',':
						*pos = *c;
						++pos;
					}
					++c;
				}
				*pos = 0;
			}
		}
	}

	arg = first_arg_flag("-mf");

	if (arg) {
		FILE *pfile = fopen(arg, "r");

		if (!pfile) {
			fprintf(stderr, "Error opening file %s.\n", arg);
			exit(1);
		}

		fseek(pfile, 0, SEEK_END);
		if (ftell(pfile) >= memsize) {
			fprintf(stderr, "Mem contents too large.\n");
			exit(1);
		}
		fseek(pfile, 0, SEEK_SET);

		if (!(*mem = (char *)calloc(memsize + 1, 1))) {
			fprintf(stderr, "Mem malloc failed.\n");
			exit(1);
		}
		
		char *pos = *mem;
		while (!feof(pfile)) {
			int c = fgetc(pfile);

			if (c != EOF) {
				*pos = (char)c;
				++pos;
			}
		}
		fclose(pfile);
	}
	else {
		arg = first_arg_flag("-mi");

		if (arg) {
			if (strlen(arg) >= memsize) {
				fprintf(stderr, "Specified contents exceed mem size.\n");
				exit(1);
			}
			else if (!(*mem = (char *)calloc(memsize + 1, 1))) {
				fprintf(stderr, "Mem malloc failed.\n");
				exit(1);
			}
			else {
				strcpy_s(*mem, strlen(arg) + 1, arg);
			}
		}
		else if (!(*mem = (char *)calloc(memsize, 1))) {
			fprintf(stderr, "Mem malloc failed.\n");
			exit(1);
		}
	}

	if (!prog_supplied) {
		DynArray *arr = new_dyn_array(1);

		if (!arr) {
			fprintf(stderr, "Prog malloc failed.\n");
			exit(1);
		}

		while (!feof(stdin)) {
			int c = getchar();
	
			if (c != EOF) {
				char ch_c = (char)c;
				push(&ch_c, arr);
			}
		}

		if (arr->size >= MAXPROGSIZE) {
			fprintf(stderr, "Given program exceeds maximum size.\n");
			exit(1);
		}
		if (!(*prog = (char *)malloc(arr->size + 1))) {
			fprintf(stderr, "Prog malloc failed.\n");
			exit(1);
		}
		else {
			char *pos = *prog;
			const char *c = (const char *)arr->array;

			for (int _ = 0; _ < arr->size; ++_) {
				switch (*c) {
				case '+':
				case '-':
				case '>':
				case '<':
				case '[':
				case ']':
				case '.':
				case ',':
					*pos = *c;
					++pos;
				}
				++c;
			}

			*pos = 0;
		}
	}

}

void run_program(char *mem, char *prog) {
	char *p = mem;
	size_t proglen = strlen(prog);
	DynArray *p_stack = new_dyn_array(sizeof(size_t));

	for (size_t i = 0; i < proglen; ++i) {

		switch (prog[i]) {
		case '>':
			++p;
			if (p == mem + memsize) {
				p = mem;
			}
			break;
		case '<':
			--p;
			if (p == mem - 1) {
				p = mem + memsize - 1;
			}
			break;
		case '+':
			++(*p);
			break;
		case '-':
			--(*p);
			break;
		case '.':
			//putchar(*p);
			printf("%d ", *p);
			break;
		case ',':
			*p = (char)getchar();
			break;
		case '[':
			if (!*p) {
				//WRONG
				while (prog[i] != ']' && i < proglen) ++i;
			}
			else {
				push(&i, p_stack);
			}
			break;
		case ']':
			if (!*p) {
				void *t = pop(p_stack);

				if (!t) {
					fprintf(stderr, "Mismatched ].");
					exit(1);
				}

				i = *(size_t *)t;
			}	
			break;
		}
	}
}

int main(int argc, const char* argv[]) {
	char *mem, *prog;
	init(argc, (const char **)argv, &mem, &prog);
	
	printf("Mem contents: %s \nProg Contents: %s \nMem Size: %d\n", mem, prog, memsize);

	run_program(mem, prog);

	free(mem);
	free(prog);

	return 0;
}

