#include <stdlib.h>
#include <stdio.h>

#include <syscall.h>

__thread int errno=0;


void exit(int status){
	syscall_1(SYS_exit, status);
}



//SO UGLY!!! :))) HOW DOES IT WORK?????!!!!
int do_nothing(void* p){
	return 0;
}


void *sbrk(int inc){ 
	uint64_t break_point = syscall_1(SYS_brk, 0);
	// printf("sbrk: break_point: %x\n", break_point);
	// do_nothing((void *)break_point);
	if(break_point == -1)
		return (void *)-1;
 	uint64_t end_point = syscall_1(SYS_brk, break_point + inc);
	// printf("sbrk: end_point: %x\n", end_point);
	do_nothing((void *)end_point);
 	// if( (void *) syscall_1(SYS_brk, break_point + inc) == NULL)
 	if( (void *) end_point == NULL)
		return  (void *) -1;
  	return (void *)break_point;
}

void *malloc(size_t size){
	void * p = sbrk (size);
	if ( p == (void *) -1)
		return NULL;
	return p;
}

void free(void *ptr){

}
// int brk(void *end_data_segment);

// // processes
pid_t fork(void){
	uint64_t res = syscall_0(SYS_fork);
	// printf("return from fork: %d\n", res);
	return res;
}
pid_t getpid(void){
	return syscall_0(SYS_getpid);
}
pid_t getppid(void){
	return syscall_0(SYS_getppid);
}
int execve(const char *filename, char *const argv[], char *const envp[]){
	printf("stdlib: execve filename: >%s<\n");
	int status =  (int)syscall_3(SYS_execve, (uint64_t)filename, (uint64_t)argv, (uint64_t)envp);
	errno = -status;
	return -1;

}
pid_t waitpid(pid_t pid, int *status, int options){
	return (pid_t)syscall_3(SYS_wait4, (uint64_t)(pid),(uint64_t)(status), (uint64_t)(options));
}

// unsigned int sleep(unsigned int seconds);

// // signals
// typedef void (*sighandler_t)(int);
// sighandler_t signal(int signum, sighandler_t handler);


int kill(pid_t pid, int sig){
	return (int)syscall_2(SYS_kill, (uint64_t)(pid), (uint64_t)(sig));
	
}
unsigned int alarm(unsigned int seconds){
	return (int)syscall_1(SYS_alarm, (uint64_t)(seconds));
}

// // paths
char *getcwd(char *buf, size_t size){
	return (char *)syscall_2(SYS_getcwd, (uint64_t)(buf), (uint64_t)(size));
}
int chdir(const char *path){
	return syscall_1(SYS_chdir, (uint64_t)(path));
}

// files
int open(const char *pathname, int flags){
	return syscall_2(SYS_open, (uint64_t)pathname, flags);
}
ssize_t read(int fd, void *buf, size_t count){
	return syscall_3(SYS_read, fd, (uint64_t)(buf), count);
}
ssize_t write(int fd, const void *buf, size_t count){
	return syscall_3(SYS_write, fd, (uint64_t)(buf), count);
}
off_t lseek(int fildes, off_t offset, int whence){
	return syscall_3(SYS_lseek, fildes, offset, whence);
}
int close(int fd){
	return syscall_1(SYS_close, fd);
}
int dup(int oldfd){
	return syscall_1(SYS_dup, oldfd);
}
int dup2(int oldfd, int newfd){
	return syscall_2(SYS_dup2, oldfd, newfd);
}

// // directories


// DEBUGGING ONLY!!
void printdirent(struct dirent* d){
	printf("struct dirent\n {\n d_ino = %x\n d_off = %x\n d_reclen = %x\n d_type = %x\n d_name = %s\n }; \n",
	 d->d_ino, d->d_off, d->d_reclen, 0, d->d_name);
}

void *opendir(const char *name){
	struct dirent* dents = (struct dirent*)malloc(MAX_DIR_SIZE*sizeof(struct dirent));
	int dd = open(name, O_DIRECTORY);
	int status = syscall_3(SYS_getdents, dd, (uint64_t)dents, MAX_DIR_SIZE);
	if (status<0){
		// printf(">>%d<<\n", status);
		errno = -status;
		return NULL;
	}

	// long holder = (long)dents;
	// for (int i = 0; i < status; ++i)
	// {

	// 	printdirent((void*)(holder+i));
	// }

	DIR* res = (DIR*)malloc(sizeof(DIR));
	res -> fd = dd;
	res -> dents = dents;
	res -> offset = 0;
	return res;
}

struct dirent *readdir(void *dir){
	DIR* d = (DIR*) dir;
	uint64_t offset = d ->offset;
	struct dirent * res = (struct dirent *)( ((uint64_t)(d-> dents)) + offset);
	if (res->d_ino==0){
		return NULL;
	}
	// printf(">%d<\n", offset);
	// printdirent(res);
	// offset += res -> d_off;
	offset += res -> d_reclen;
	// offset += 1;
	// printf(">>%d<\n", offset);
	d -> offset = offset;
	return res;
}
int closedir(void *dir){
	DIR* d = (DIR*) dir;
	free(d->dents);
	close(d->fd);
	return 0;
}


