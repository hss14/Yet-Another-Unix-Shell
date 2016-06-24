#include "yaush.h"

int main(int argc, char** argv)
{
	char** arg = NULL;
	int ntokens = 0;
	struct list_head *head;
	struct list_head *plist, *n;
	struct sigaction act;
	struct sigaction act_z;
	char *line_read = (char *)NULL;

	// init list head for jobs_list
	jobs_list = (struct list_head*) malloc(sizeof( struct list_head ) );
	INIT_LIST_HEAD(jobs_list);

	// init list head for pid_list
	pid_list = (struct list_head*) malloc(sizeof( struct list_head ) );
	INIT_LIST_HEAD(pid_list);

	// signal - Ctrl-C
	sigfillset( &(act.sa_mask) );
	act.sa_flags = SA_SIGINFO;
	act.sa_handler = handle_signals_ctrl_c;
	if ( sigaction(SIGINT, &act, NULL) < 0 ) 
	{
		printf("failed to register interrupts with kernel\n");
	}

	// signal - Ctrl-Z
	sigfillset( &(act_z.sa_mask) );
	act_z.sa_flags = SA_SIGINFO;
	act_z.sa_handler = handle_signals_ctrl_z;
	if ( sigaction(SIGTSTP, &act_z, NULL) < 0) 
	{
		printf("failed to register interrupts with kernel\n");
	}

	// sigsetjmp
	if ( sigsetjmp( ctrlc_buf, 1 ) != 0 )
		;
	if ( sigsetjmp( ctrlz_buf, 1 ) != 0 )
	{
		log_debug("ctrlz sigsetjmp\n");
		struct list_head *plist, *n;
		// move the list in pid_list to jobs_list
		list_for_each_safe( plist, n, pid_list)
		{
			struct node_process *node = list_entry(plist, struct node_process, list);
			log_debug("pid:%d pstatus:%d\n", node->pid, node->pstatus);
			struct node_process *newnode = (struct node_process*) malloc(sizeof(struct node_process) );
			newnode->pstatus = Stopped;
			newnode->pid = node->pid;
			list_add(&newnode->list, jobs_list);
			list_del(&node->list);
		}
		log_debug("jobs_list empty:%d\n", list_empty(jobs_list));
	}

	strcpy( process_status_str[0], "Running");
	strcpy( process_status_str[1], "Stopped");
	strcpy( process_status_str[2], "Done");

	// loop
	while(1)
	{
		arg = NULL;
		ntokens = 0;

		rl_gets( &line_read );
		
		// print the jobs that were done
		int i = 0;
		int ret;
		list_for_each_safe(plist, n, jobs_list)
		{
			struct node_process *node = list_entry(plist, struct node_process, list);
			ret = waitpid(node->pid, NULL, WNOHANG);
			if (ret == -1)
				node->pstatus = Done;
			if (node->pstatus == Done)
			{
				printf("[%d]\tpid:%d\t%s\n", i, node->pid, process_status_str[node->pstatus]);
				list_del(&node->list); // delete this node
			}
			i++;
		}

		// handle the input 
		if (line_read && *line_read)
		{
			arg = lexer(line_read, &ntokens);
			head = parser(arg, ntokens);
			exec_multicmd(head);
			free_list(head);
		}
	}
 
	return 0;
}
