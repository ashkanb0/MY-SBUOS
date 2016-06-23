#include <stdio.h>
#include <stdlib.h>
#include <sbush/util.h>


int _log_list(char** argv){
	// ONLY FOR DEBUGGING
	printf("LOGGING LIST\n");

	int i = 0;
	do{
		printf(">%s<,\n", argv[i]);
		i++;
	}while(argv[i]);
	printf("LOGGING LIST DONE: %d ELEMS\n", i);
	return 0;
}


int exit_yet = 0;
int exit_status = 0;
// char active_path [PATH_LEN];
char prompt [BUFFERSIZE];
char buffer [BUFFERSIZE];
char cwd [DIR_NAME_LEN];
char joined_filename [DIR_NAME_LEN];
int last_command_status = 0;
char status_str [15];
char user_name_str [30];

int process_command(char* command){
	int newlind = strsearch(command, '\n');
	// printf("command is: >%s<\n", command);
	//Checking to see if it's a complete command
	if (newlind<0)return -1;
	command[newlind]=0;

	return 0;
}

int do_command(char* command){
	if (strprefix("cd ", command)){
		char* dir = command+ 3;
		if(chdir(dir)){
			printf("Directory '%s' not found!\n", dir);
			return -1;
		}
		return 0;
	}
	// printf("HEREEEEE 1\n");
	if (strcomp("exit", command)==0){
		exit_yet = 1;
		exit_status = 0;
		return 0;
	}

	char bg = 0;
	int l = strlen(command);
	if(command[l-1]=='&'){
		bg = 1;
		command[l-1]=0;
	}

	// printf("HEREEEEE 2\n");
	char ** argv = strsplit(command, ' ');

	// log_argv(argv);
	process_argv(argv);
	// log_argv(argv);

	int pos = strsearch(argv[0], '=');
	if (pos!=-1){
		char* var_and_val [2];
		str_split_line(argv[0], pos, var_and_val);
		var_set(var_and_val[0], var_and_val[1]);
		free(var_and_val[0]);
		free( var_and_val[1]);
		return 0;
	}

	// printf("HEREEEEE 3\n");
	char ** envp = get_envp();
	int pid = fork();
	printf("PID: '%d''%x'\n", pid, pid);
	printf("cmmnd =>0x%x<\n", (uint64_t)command);
	// printf("command = >%s<\n", command);
	// printf("stack  >0x%x<\n", *((uint64_t*)command-48));
	// printf("stack  >0x%x<\n", *((uint64_t*)command-32));
	// printf("stack  >0x%x<\n", *((uint64_t*)command-16));
	// printf("stack ->0x%x<\n", *((uint64_t*)command));
	// printf("stack  >0x%x<\n", *((uint64_t*)command+16));
	// printf("stack  >0x%x<\n", *((uint64_t*)command+32));
	// printf("stack  >0x%x<\n", *((uint64_t*)command+48));
	// printf("stack  >0x%x<\n", *((uint64_t*)command+48+16));
	// printf("stack  >0x%x<\n", *((uint64_t*)command+48+32));
	// printf("stack  >0x%x<\n", *((uint64_t*)command+48+48));
	// log_argv(argv);
	uint64_t rsp = 0;
	__asm__ volatile("movq %%rsp, %0":"=r"(rsp):);
	printf("shell: rsp : >%x<\n", );
	if(pid){
			if(bg==0){
				// printf("WAITING!\n" );
				waitpid(pid, &last_command_status, 0);
				// printf("here after wait: %x\n", last_command_status );
				return last_command_status;
			}
			return 0;
	}else{
		printf("ELSE?\n");
		if (command[0] == '.' || command[0] == '/' ){
			execve(argv[0], argv, envp);
			str_free_splitted_list(argv);
			printf("No command or binary found: '%s'\n", argv[0]);
			exit(1);
		}

		printf("ELSE2\n");
		// do we have to do this?
		// strcat(".:",get_path(), active_path, PATH_LEN);
		char** path_list = strsplit(get_path(), ':');
		
		// _log_list(path_list);

		printf("preparing to execute: >%s<\n", argv[0]);
		for (int i=0; path_list[i]; i++)
		{
			char* path_elem = path_list[i];
			strcat(path_elem, "/", joined_filename, DIR_NAME_LEN);
			strcat(joined_filename, argv[0], joined_filename, DIR_NAME_LEN);
			char* temp = argv[0];

			argv[0] = joined_filename;
			printf("going to execute: >%s<\n", argv[0]);
			execve(argv[0], argv, envp);
			argv[0] = temp;
			// TODO: uncomment after fixed get_envp;
			// str_free_splitted_list(envp);
		}
		printf("No command or binary found: '%s'\n", argv[0]);
		str_free_splitted_list(argv);
		str_free_splitted_list(path_list);
		exit(1);
	}
	return 0;
}


void make_prompt(){
	getcwd(cwd, DIR_NAME_LEN);
	itoa(status_str, last_command_status);
	
	getusrnm(user_name_str, 30);

	get_formatted_prompt(prompt, cwd, status_str, user_name_str);
	

}


int main(int argc, char* argv[], char* envp[]) {

	// printf("PARSING STUFF FORMATTED\n");
	parse_envp(envp);
	// printf("PARSED IT\n");

	while(!exit_yet){
		make_prompt();
		readline(buffer, prompt);
		if(process_command(buffer)){
			printf("^D\n");
			return 0;
		}
		last_command_status = do_command(buffer);
	}
	return exit_status;
}

