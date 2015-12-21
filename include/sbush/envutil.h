#ifndef _SB_ENVUTIL_H
#define _SB_ENVUTIL_H

int parse_envp(char* envp[]);


int getusrnm(char* buffer, int size);
char* get_path();

char** get_envp();
int get_formatted_prompt(char* buffer, char* cwd, char* status_str);
int process_argv(char** argv);
char* process_val(char* val);

int var_set(char* varname, char* varval);

#endif
