#include "yaush.h"

/* @arg -- arguments of the command
   @ntokens -- number of the tokens in arg ( actually length(arg) == ntokens+1, because the last token is always set to NULL )
 */
void exec_singlecmd(char** arg, int ntokens)
{
	// fork to execute the command
	if (fork() == 0)
	{	
		int ret = execvp(arg[0], arg);
		// print the error message
		if (ret < 0)
			perror("exec_singlecmd");
		free_string(arg, ntokens);
		exit(0);
	}
	// wait until the child process return
	wait(NULL);
}


// @head -- the head of the list, every node in the list represent a command
void exec_multicmd(struct list_head *head)
{
	int forkstatus = 1;
	int i;
	int pnum = 0;
	int background = 0;
	struct list_head *plist;

	// count the number of the command
	list_for_each(plist, head)
	{
		struct node_cmd *node = list_entry(plist, struct node_cmd, list);	
		if (pnum == 0 && node->background == 1)
		{
			background = 1;
		}
		pnum++;
	}
	
	// pipe descriptor
	int **fd = NULL;
	if (pnum > 1)
	{
		fd = (int**)malloc((pnum-1)*sizeof(int*));
		for (i = 0; i < pnum-1; i++)
			fd[i] = (int*)malloc(2*sizeof(int)); 
	}

	int idx = 0;
	int cust = 0;
	int ret;
	// loop for every command
	list_for_each(plist, head)
	{
		struct node_cmd *node = list_entry(plist, struct node_cmd, list);	

		for (i = 0 ; i < node->ntokens; i++)
		{
			log_debug("%p %s %s %s %d\n", node, node->arg[i], node->in, node->out, node->background);
		}

		cust = execute_cust_cmd(node->arg[0], node->arg);

		if (cust == 0)
			continue;
		else if (cust > 0)
		{
			
			if (strcmp(node->arg[0],"fg") == 0)              // fg
			{
				// add it to the pid_list
				struct node_process *node = (struct node_process*)malloc(sizeof(struct node_process));
				node->pid = cust;
				list_add(&node->list, pid_list);

				// and remove from the jobs_list;
				struct list_head *plist, *n;
				list_for_each_safe(plist, n, jobs_list)
				{
					struct node_process *node = list_entry(plist, struct node_process, list);
					if (node->pid == cust)
					{
						list_del(&node->list);
						break;
					}
				}
				break;
			}
			
			else                              // bg
			{
				struct list_head *plist, *n;
				list_for_each_safe(plist, n, jobs_list)
				{
					struct node_process *node = list_entry(plist, struct node_process, list);
					if (node->pid == cust)
					{
						node->pstatus = Running;
						kill(node->pid, SIGCONT);
						break;
					}
				}
				background = 1;
				break;
			}
		}


		// fork to execute the command
		forkstatus = fork();
		if (forkstatus == 0)
		{
			// input
			if (strcmp(node->in, "prev") == 0)
			{
				close(STDIN_FILENO);
				dup2(fd[idx][0], STDIN_FILENO);
			}
			else if(strcmp(node->in, "stdin") != 0)
			{
				int fp = open(node->in, O_RDONLY);
				dup2(fp, STDIN_FILENO);
			}

			// output
			if (strcmp(node->out, "next") == 0)
			{
				close(STDOUT_FILENO);
				dup2(fd[idx-1][1], STDOUT_FILENO);
			}
			else if(strcmp(node->out, "stdout") != 0)
			{
				int fp = open(node->out, O_WRONLY | O_CREAT | O_TRUNC, 0600);
				dup2(fp, STDOUT_FILENO);
			}


			for (i = 0; i < pnum-1; i++)
			{
				close(fd[i][1]);
				close(fd[i][0]);
			}
			ret = execvp(node->arg[0], node->arg);

			// print the error message
			if (ret < 0)
			{
				//printf("error:%s\n", strerror(errno));
				fprintf(stderr, "%s:%s\n", node->arg[0], strerror(errno));
				exit(-1);
			}
		}		
		else if (background == 0)   // in parent process: put every pid of non-background process into the list
		{
			struct node_process *node = (struct node_process*)malloc(sizeof(struct node_process));
			node->pid = forkstatus;
			node->pstatus = Running;
			log_debug("pid %d inqueue\n", forkstatus);
			list_add(&node->list, pid_list);
		}	
		else        // in parent process: add background process into the jobs list
		{
			struct node_process *node = (struct node_process*)malloc(sizeof(struct node_process));
			node->pid = forkstatus;
			node->pstatus = Running;
			list_add(&node->list, jobs_list);
		}
		idx++;
	}//end of listeach

	// wait until the child process return
	if (forkstatus > 0 && cust != 0)
	{
		// close the pipe in the parent process
		for (i = 0; i < pnum-1; i++)
		{
			close(fd[i][0]);
			close(fd[i][1]);
		}

		// wait until all the process return
		if (background == 0)
		{
			while (!list_empty(pid_list))     // wait the process in the pid_list
			{
				int ret = wait(NULL);
				struct list_head *plist, *n;
				
				list_for_each_safe(plist, n, pid_list)  // search the pid:ret
				{
					struct node_process *node = list_entry(plist, struct node_process, list);
					if (node->pid == ret)
					{
						log_debug("wait %d return %d %d\n", i, ret, node->pid);
						// delete this node
						list_del(&node->list);
						break;
					}
				}
				
				list_for_each(plist, jobs_list)   // update the jobs_list
				{
					struct node_process *node = list_entry(plist, struct node_process, list);
					if (node->pid == ret)
					{
						node->pstatus = Done;
						break;
					}
				}
			} //end of while
		}

	}
}

