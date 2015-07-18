#ifndef ARGS_H
#define ARGS_H
typedef struct Arg Arg;

struct Arg {
	char *flag;
	char *val;
};

Arg *init_arglist(int argc, const char *argv[]);
void free_arglist();
Arg *get_args();
size_t get_n_args();
const char **args_flag(const char *flag);
const char *first_arg_flag(const char *flag);
#endif