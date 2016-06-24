/* reference: mannual page of GNU readline */

#include "yaush.h"


/* read a string, return a pointer to it. returns NULL on EOF. 
 * gives the user the default behaviour of TAB completion: completion on file names 
 * */
void rl_gets( char ** line_read )
{
	/*if the buffer has already been alocated, return the memory to the free pool */
	if( *line_read != NULL )
	{
		free( *line_read );
		*line_read = (char *)NULL;
	}

	/* get a line from the user */
	*line_read = readline("\n=w=: ");

	/* if the line has any text in it, save it on the history */
	if( *line_read && **line_read )
		add_history( *line_read );

	return;
}
