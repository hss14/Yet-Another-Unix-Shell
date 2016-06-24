#include "yaush.h"

/* @line_read -- the string to be parsed
 * @_ntokens -- (address) number of tokens, some tokens could be NULL
 * @return: arg -- the tokens, arg[i] respents the ith token
 */
char** lexer(char* line_read, int* _ntokens)
{
	char delim = ' ';
	int nspace = 0;
	int i;
	int ntokens;
	char **arg;
	char *pstr;
	int offset = 0;
	int pos = 0;
	int count = 0;

	// count the number of 'space'
	for (i = 0; i < strlen(line_read); i++)
	{
		if (line_read[i] == delim)
		{
			if (i > 0 && line_read[i-1] == '\\')
				continue;	
			nspace++;
		}
	}

	// tokens = nspace + 1; the additional one is set as NULL to label the end
	ntokens = nspace + 2;
	arg = (char**)malloc(ntokens*sizeof(char*));

	for (i = 0; i < ntokens; i++)
	{
		arg[i] = (char*)malloc( STRLEN * sizeof(char) );
		strcpy(arg[i], "\0");
	}

	// seperate the string using 'space' to get tokens
	while (offset < strlen(line_read))
	{
		if (offset == 0 && pos == 0)
			pstr = strchr(line_read + offset, delim);
		else
			pstr = strchr(line_read + pos + 1, delim);

		if (pstr == NULL)
			pos = strlen(line_read);
		else
			pos = strlen(line_read) - strlen(pstr);

		// "\ " means a whitespace in the filename
		if (pos > 0 && line_read[pos-1] == '\\')
		{	
			//offset = pos+1;
			continue;
		}

		if (pos > offset)
		{
			strncpy(arg[count], line_read + offset, pos - offset);
			arg[count][pos-offset] = '\0';
			// elimate the "\"
			char* ptmp = strchr(arg[count], '\\');
			if (ptmp != NULL)
			{
				int postmp = strlen(arg[count]) - strlen(ptmp);
				int i;
				for(i = postmp; i < strlen(arg[count]); i++)
					arg[count][i] = arg[count][i+1]; 	
			}
		}
		else    // this is a 'space', so skip this string
		{
			offset = pos+1;
			continue;
		}
		offset = pos+1;
		count++;
	}
 
	for (i = count; i < ntokens; i++)
	{
		free(arg[i]);
		arg[i] = NULL;
	}
	*_ntokens = ntokens;
 
	return arg;
}
