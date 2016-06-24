#include "yaush.h"

/* @arg -- arguments list
 * @ntokens -- the ntokens of the list
 * @return: the head of the list, every node of the list represents one command
 */
struct list_head* parser(char** arg, int ntokens)
{
	int i;
	int pos = 0;
	int prevpos = 0;
	struct list_head *head = (struct list_head*)malloc(sizeof(struct list_head));
	int count = 0;
	INIT_LIST_HEAD(head);

	while (pos < ntokens)
	{
		// find the position of pipes - '|' 
		if (arg[pos] == NULL || strcmp(arg[pos], "|") == 0 || strcmp(arg[pos], "&") == 0)
		{
			// if this is the last token
			struct node_cmd *node = (struct node_cmd*)malloc(sizeof(struct node_cmd));
			node->background = 0;

			// output redirection
			strcpy(node->out, "next");
			if (arg[pos] == NULL)
				strcpy(node->out, "stdout");
			else if (pos+1 < ntokens &&  strcmp(arg[pos], "&") == 0 && arg[pos+1] == NULL)
				strcpy(node->out, "stdout");

			// input redirection
			if (prevpos == 0)
				strcpy(node->in , "stdin");
			else
				strcpy(node->in, "prev");
 
			char **cmdarg = (char**)malloc((pos-prevpos+1)*sizeof(char*));
			node->arg = cmdarg;
			for (i = 0; i < pos - prevpos + 1; i++)
			{
				cmdarg[i] = (char*)malloc(STRLEN*sizeof(char));
				strcpy(cmdarg[i], "\0");
			}
			node->ntokens = pos - prevpos + 1;
 
			// prevpos ~ pos : one command
			i = prevpos;
			count = 0;
			while (i < pos)
			{
				//log_debug("%s\n", arg[i]);
				// output redirection
				if (strcmp(arg[i], ">") == 0)
				{
					if (i+1 < pos)
						strcpy(node->out, arg[++i]);
				}
				// input redirection
				else if (strcmp(arg[i], "<") == 0)
				{
					if (i+1 < pos)
						strcpy(node->in, arg[++i]);
				}
				else
				{
					strcpy(cmdarg[count], arg[i]);
					log_debug("cmdarg[%d] = %s\n", count, cmdarg[count]);
					count++;
				}
				i++;	
			}
 
			// set the remaining tokens to NULL
			for (i = count; i < node->ntokens; i++)
			{
				free(cmdarg[i]);
				cmdarg[i] = NULL;
			}
			prevpos = pos+1;

			// are these commands running in background or not
			// only if the '&' appear in the end of the command, it takes effect.
			if (pos+1 < ntokens && arg[pos] != NULL && strcmp(arg[pos], "&") == 0 )
				node->background = 1;

			// add this node to the list
			list_add(&node->list, head);
			if (arg[pos] == NULL)
				break;
			if (pos+1 < ntokens && arg[pos] != NULL && strcmp(arg[pos], "&") == 0 && arg[pos+1] == NULL)
				break;
		}
		pos++;
	}
	free_string(arg, ntokens);
	return head;
}
