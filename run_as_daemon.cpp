#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <signal.h>

#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>// open
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>  


#define MAXFILE 65535


void SignHandler(int iSignNo); 



void run_as_daemon()
{
	pid_t pid;

	signal(SIGINT,  SIG_IGN);
	signal(SIGHUP,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTERM, SIG_IGN);


	// [1] fork child process and exit father process
	pid = fork();
	if(pid < 0)
	{
		perror("fork error!");
		exit(1);
	}
	else if(pid > 0)
	{
		exit(0);
	}

	// [2] create a new session
	setsid();

	// [3] set current path
	char szPath[1024];
	if(getcwd(szPath, sizeof(szPath)) == NULL)
	{
		perror("getcwd");
		exit(1);
	}
	else
	{
		chdir(szPath);
		printf("set current path succ [%s]\n", szPath);
	}

	// [4] umask 0
	umask(0);

	// [5] close useless fd
	int i;
	//for (i = 0; i < MAXFILE; ++i)
	for (i = 3; i < MAXFILE; ++i)
	{
		close(i);
	}

	// [6] set termianl signal
	signal(SIGTERM, SignHandler);




}
