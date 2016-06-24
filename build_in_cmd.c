#include "yaush.h"
 
char cmd_list[MAX_CUST_CMD][CUST_CMD_NAME_LEN] = {"cd", "exit", "jobs", "fg", "bg" };
int (*f[MAX_CUST_CMD])(char*, char**) = { yaush_cd, yaush_exit,  yaush_jobs, yaush_fg, yaush_bg };

/* @cmd -- the command name, useless in this function, just to uniform the arguments of all custom functions
 * @arg -- the argumnets of the command, note that arg[0] is the command name
 * @return: 0 if successful
 */
int yaush_cd(char* cmd, char** arg)
{
	int ret;

	if (arg[1] == NULL)       // if no parameters, change the current dir to home
	{
		char buf[255];
		sprintf(buf, "/home/%s", getenv("USER"));
		ret = chdir(buf);
	}
	else
		ret = chdir(arg[1]);

	if (ret < 0)
		perror("cd");
	return 0;
}


int yaush_exit(char* cmd, char** arg)
{
	exit(0);
	return 0;
}

//print the jobs list
int yaush_jobs(char* cmd, char** arg)
{
	struct list_head *plist;
	int i = 0;
	strcpy( process_status_str[0], "Running");
	strcpy( process_status_str[1], "Stopped");
	strcpy( process_status_str[2], "Done");

	list_for_each(plist, jobs_list)
	{
		struct node_process *node = list_entry(plist, struct node_process, list);
		printf("[%d]\tpid:%d\t%s\n", i, node->pid, process_status_str[node->pstatus]);
		i++;
	}
	return 0;
}


// @return: the pid of the sprcific process
int yaush_fg(char* cmd, char** arg)
{
	int idx = 0;
	int pid = 0;

	if (arg[1] != NULL)
	{
		idx = atoi(arg[1]);	
	}
	if (idx <  -1)
		pid = -1;	

	struct list_head *plist;
	int i = 0;
	list_for_each(plist, jobs_list)
	{
		struct node_process *node = list_entry(plist, struct node_process, list);
		pid = node->pid;
 		if ( i >= idx)
			break;
		i++;
	}
	return pid;
}


int yaush_bg(char* cmd, char** arg)
{
	int idx = 0;
	int pid = 0;

	if (arg[1] != NULL)
	{
		idx = atoi(arg[1]);	
	}
	if (idx <  -1)
		pid = -1;	

	struct list_head *plist;
	int i = 0;
	list_for_each(plist, jobs_list)
	{
		struct node_process *node = list_entry(plist, struct node_process, list);
		pid = node->pid;

		if ( i >= idx)
		{
			if (node->pstatus != Stopped)
				pid = 0;
			break;
		}
		i++;
	}
	return pid;
}


/* execute_cust_cmd(): execute an custom command 
 * @cmd -- the command name
 * @arg -- the argumnets of the command, noted that arg[0] is the command name
 * @return: 0 is successful, or -1 if not found, or > 0 represents the pid
 */
int execute_cust_cmd(char* cmd, char** arg)
{
	int i;
	int flag = -1;

	for (i = 0; i < MAX_CUST_CMD; i++)
	{
		if (strlen(cmd_list[i]) > 0 && strcmp(cmd, cmd_list[i]) == 0)
		{
			flag = (*f[i])(cmd, arg);
			break;
		}
	}
	return flag;
}
