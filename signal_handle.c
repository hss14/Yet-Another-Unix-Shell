#include "yaush.h"

void handle_signals_ctrl_c(int signo)
{
	if (signo == SIGINT)
	{
		log_debug("You pressed Ctrl+C. pid:%d\n", getpid());
		printf("\n");
		siglongjmp(ctrlc_buf, 1);
	}
}

void handle_signals_ctrl_z(int signo)
{
	if (signo == SIGTSTP)
	{
		log_debug("You pressed Ctrl+Z. pid:%d\n", getpid());
		printf("\n");
		siglongjmp(ctrlz_buf, 1);
	}
}
