#ifndef _SB_ENVUTIL_H
#define _SB_ENVUTIL_H

typedef struct var_store_linked_list_structure
{
	char* varname;
	char* varval;

	struct var_store_linked_list_structure* next;
}var_list;



int parse_envp(char* envp[]);


int getusrnm(char* buffer, int size);
char* get_path();

char** get_envp();
int process_argv(char** argv);
char* process_val(char* val);

int var_set(char* varname, char* varval);
var_list* var_find(char* varname);

int log_argv(char** argv);

#endif
