#ifndef __H_YAUSH__
#define __H_YAUSH__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <setjmp.h>

#include "list.h"

#define STRLEN	255
#define MAX_CUST_CMD 32
#define CUST_CMD_NAME_LEN 32

struct node_cmd 
{
	char** arg;		// the arguments list of the command
	int ntokens;
	char out[255];		// the output : "stdout" "next" "filename"
	char in[255];		// the input :  "stdin"  "prev" "filename"
	struct list_head list;
	int background;		// run in background (1) or not (0)
};

enum process_status
{
	Running = 0,
	Stopped,
	Done
};


struct node_process
{
	int pid;
	enum process_status pstatus;
	struct list_head list;
};

struct list_head* jobs_list;	// all the background process
struct list_head* pid_list;	// all the foreground process

sigjmp_buf ctrlc_buf;
sigjmp_buf ctrlz_buf;

char process_status_str[3][10];

/**************************************** dealwith input ***************************************/
void rl_gets( char ** line_read );   
char** lexer(char* line_read, int* _ntokens);
struct list_head* parser(char** arg, int ntokens);


/************************************* execute command *******************************************/
void exec_singlecmd(char** arg, int ntokens);
void exec_multicmd(struct list_head *head);


/************************************* build-in command *******************************************/
int execute_cust_cmd(char* cmd, char** arg);
int yaush_cd(char* cmd, char** arg);
int yaush_exit(char* cmd, char** arg);
int yaush_jobs(char* cmd, char** arg);
int yaush_fg(char* cmd, char** arg);
int yaush_bg(char* cmd, char** arg);


/************************************* signal handle *******************************************/
void handle_signals_ctrl_c(int signo);
void handle_signals_ctrl_z(int signo);


/*************************************** clear up **********************************************/
void free_string(char** arg, int ntokens);
void free_list(struct list_head *head);


/*************************************** log for debug ******************************************/
#ifdef HSSDEBUG
#define log_debug(fmt, ...) \
    fprintf(stderr, "DEBUG %s:%s:%d: " fmt "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#else
#define log_debug(fmt, ...)
#endif


#endif   //__H_YAUSH__
