#include "yaush.h"

void free_string(char** arg, int ntokens)
{
	int i; 

	for (i = 0; i < ntokens; i++)
	{
		if (arg[i] != NULL);
		{
			free(arg[i]);
			arg[i] = NULL;
		}
	}
	free(arg);
	arg = NULL;
	return;
}


void free_list(struct list_head *head)
{
	struct list_head *plist, *n;	

	list_for_each_safe(plist, n, head)
	{
		struct node_cmd *node = list_entry(plist, struct node_cmd, list);
		free_string(node->arg,node->ntokens);
		list_del(&node->list);
		node = NULL;
	}
	head = NULL;
	return;
}
