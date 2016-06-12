#include <sbush/util.h>
#include <stdio.h>
#include <stdlib.h>

char** env_vars;

char path [PATH_LEN];
char usrnm [USERNAME_LEN];
char ps1 [MAX_STR_LEN];

typedef struct var_store_linked_list_structure
{
	char* varname;
	char* varval;

	struct var_store_linked_list_structure* next;
}var_list;

var_list* head;
var_list* tail;

int log_argv(char** argv){
	// ONLY FOR DEBUGGING
	printf("LOGGING ARGV\n");

	int i = -1;
	do{
		i++;
		printf(">%s<,\n", argv[i]);
	}while(argv[i]);
	printf("LOGGING ARGV DONE: %d ELEMS\n", i);
	return 0;
}


void print_envp(){
	// FOR DEBUGGING ONLY!!!!
	for (int i=0; env_vars[i]; i++){
		printf("%d:>>%s<<\n",i, env_vars[i]);
	}
}


void print_extracted_vals(){
	
	// FOR DEBUGGING ONLY!!!!
	for(var_list* current = head; current; current = current-> next){
		printf(">%s< = >%s<\n", current->varname, current->varval);
	}
}

int var_init(){
	char* x = malloc(15);
	// printf("HERE 0 >>>%x<<<\n", x);
	head = (var_list*)malloc(sizeof(var_list));
	// printf("HERE 1 >>>%x<<<\n", head);
	x[0] = 'c';
	tail = head;
	// printf("HERE 1.1 \n", head);
	head -> varname = NULL;
	head -> varval  = NULL;
	// printf("HERE 1.2 \n", head);
	head -> next  = NULL;
	// printf("HERE 2\n");
	return 0;
}

int var_add(char* varname, char* varval){
	var_list* var = (var_list*)malloc(sizeof(var_list));
	var -> varname = varname;
	var -> varval = varval;
	var -> next = NULL;
	tail -> next = var;
	tail = var;
	return 0;
}

int var_count(){
	int count=0;
	for(var_list* current = head->next; current; current = current-> next){
		count+=1;
	}
	return count;
}

var_list* var_find(char* varname){
	for(var_list* current = head->next; current; current = current-> next){
		// printf("what? %s %s %p, %p\n",varname, current->varname , current, current->next);
		if(strcomp(current-> varname, varname)==0) return current;
		// printf("what? %p, %p\n", current, current->next);
	}
	return NULL;
}

int var_set(char* varname, char* varval){
	// printf(">%s< is going to be set to >%s<\n", varname, varval);
	varval = process_val(varval);

	var_list* target = var_find(varname);
	if (target != NULL){
		// printf(">%s< found\n", varname);
		free(target->varval);
		int l = strlen(varval)+1;
		target->varval = (char*)malloc(l);
		strcpy(varval, target->varval);
		// print_extracted_vals();
		// printf(">%s< changed to >%s<\n", varname, target->varval);
		return 0;
	}
	// printf(">%s< not found, going to add as>%s<\n", varname, varval);
	int lr = strlen(varname)+1;
	int ll = strlen(varval)+1;
	char* my_varname = (char*)malloc(lr);
	strcpy(varname, my_varname);
	char* my_varval = (char*)malloc(ll);
	strcpy(varval, my_varval);

	var_add(my_varname, my_varval);
	// printf("added! supposedly\n");
	// print_extracted_vals();
	return 0;
}
int parse_envp(char* envp[]){
	env_vars = envp;

	// printf("PSSED STAGE 1\n");
	var_init();
	// printf("PSSED STAGE 1.1\n");

	for (int i=0; envp[i]; i++){
		int assign_index = strsearch(envp[i], '=');
		
		if(assign_index== -1){
			// printf("ERROR: line >%s< has no '='!!");
		}
		else{
			char* var [2];
			str_split_line(envp[i], assign_index, var);
			// printf("here, here, \n");
			var_add(var[0], var[1]);
			// printf("here, there, \n");
		}
	}

	// printf("PSSED STAGE 2\n");
	var_list* ps1 = var_find("PS1");
	if(ps1== NULL){
		char* varname = (char*)malloc(4);
		char* varval = (char*)malloc(20);
		strcpy("PS1", varname);
		strcpy("\\u@sbush:\\d-(\\S)->", varval);
		var_add(varname, varval);
	}

	var_list* path = var_find("PATH");
	if(path== NULL){
		char* varname = (char*)malloc(6);
		char* varval = (char*)malloc(20);
		strcpy("PATH", varname);
		strcpy("/bin", varval);
		var_add(varname, varval);
	}

	var_list* user = var_find("USER");
	if(user== NULL){
		char* varname = (char*)malloc(6);
		char* varval = (char*)malloc(20);
		strcpy("USER", varname);
		strcpy("abanitalebid", varval);
		var_add(varname, varval);
	}

	// printf("PSSED STAGE 3!!!!!! HORAAAAAAAAAY!\n");
	// print_envp();
	return 0;
	
}

int getusrnm(char* buffer, int size){
	// int l = strlen(usrnm);
	// if (l>size){
	// 	return -1;
	// }

	l = strcpy(var_find("USER")->varval, buffer);
	return -1;

}

char* get_path(){
	return var_find("PATH")->varval;
}


char** get_envp(){
	//TODO : do this!
	// It should return the new envp to be run in new
	// process, ex: updated username, updated path, etc ...
	return env_vars;
}

int get_formatted_prompt(char* buffer, char* cwd, char* status_str){
	/////////////////////////////////////////////////////
	//
	//	supports:
	//	\u - username
	//	\d - current working dir
	//	\S - status
	//
	/////////////////////////////////////////////////////

	int buffer_index = 0;
	int format_index = 0;

	char* ps1 = var_find("PS1")->varval;

	for(;format_index< strlen(ps1); format_index++){
		if (ps1[format_index]=='\\'){
			int formatting = 0;
			char* format_with = NULL;
			if(ps1[format_index+1]=='u'){
				formatting = 1;
				format_with = usrnm;
			}
			if(ps1[format_index+1]=='d'){
				formatting = 1;
				format_with = cwd;
			}
			if(ps1[format_index+1]=='S'){
				formatting = 1;
				format_with = status_str;
			}
			if (formatting){
				format_index += 1;
				for(int i=0; i<strlen(format_with); i++){
					buffer[buffer_index] = format_with[i];
					buffer_index++;
				}
				continue;
			}

		}
		buffer[buffer_index] = ps1[format_index];
		buffer_index++;
	}
	buffer[buffer_index] = 0;
	return 0;
}

char* var_get_val(char* varname){
	var_list* var = var_find(varname);
	char* replaced_val;
	if (var){
		char* val = var->varval;
		replaced_val = (char*)malloc(strlen(val)+1);
		strcpy(val, replaced_val);
	}
	else{
		replaced_val = (char*)malloc(1);
		replaced_val[0] = 0;
	}
	return replaced_val;
}

int process_argv(char** argv){

	
	//taking care of "'s.
	int i1=-1;
	int i2=-2;
	for(int i=0; argv[i]; i++){
		int count=strcount(argv[i],'"');
		if(count%2 == 1){
			if(i1==-1){
				i1=i;
				continue;
			}
			else{
				i2=i;
				//merge(i1,i2)
				int merge_count = 0;
				int merge_len = 0;
				for (int j = i1; j <= i2; ++j){
					strremove(argv[j],'"');
					merge_count++;
					merge_len+=strlen(argv[j]);
				}
				char* buffer = (char*)malloc(merge_len+1);
				int l=0;
				for (int j = i1; j <= i2; ++j){
					if(j>i1){
						l+=strcpy(" ", buffer+l);
					}
					l+=strcpy(argv[j], buffer+l);
				}
				argv[i1]= buffer;
				for (int j = i1+1;j<=i2; ++j){
					argv[j][0] = 0;
				}
				i1=i2=-1;
			}
		}
		else{
			strremove(argv[i],'"');
		}
	}


	for(int i=0; argv[i]; i++){
		if (argv[i][0] == '\0'){
			//remove empty things
			free(argv[i]);
			for(int j=i; argv[j]; j++){
				argv[j]=argv[j+1];
			}
			i--;
		}

		if (argv[i][0] == '$'){
			char* replaced_val = var_get_val((argv[i])+1);
			free(argv[i]);
			argv[i] = replaced_val;
		}
	}

	return 0;
}


char* process_val(char* val){
	// TODO : take care of $ within!!!!!
	// REMEMBER: do NOT free val
	return val;
}

