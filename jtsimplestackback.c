#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <execinfo.h>
#include <assert.h>
#include <signal.h>

#include "jtsimplestackback.h"

static void jt_show_signal_type(int value)
{
	switch(value)
	{
		case 2:
			printf("\n SIGINT : 点击了Ctrl+C 关闭\n");
		break;
		case 3:
			printf("\n SIGQUIT \n");
		break;
		case 8:
			printf("\n SIGFPE : 浮点异常\n");
		break;
		case 9:
			printf("\n SIGKILL \n");
		break;
		case 11:
			printf("\n SIGSEGV : 段错误\n");
		break;
		case 13:
			printf("\n SIGPIPE : 管道异常\n");
		break;
		case 15:
			printf("\n SIGTERM : 被另一个进程关闭\n");
		break;
		default:
			printf("\n Unknow type %d \n", value);
	}
	
}


static void jt_handle_exception(int value)
{
	printf("\n#########handle_exception  catch signal %d ,"
		" show signal name use $kill -l  #########\n", value);
	void *stack_p[10];
	char **stack_info;
	int size = 0;
	char strKillTheSamePortLnvrCmd[255];

	jt_show_signal_type(value);
	
	size = backtrace(stack_p, sizeof(stack_p));
	stack_info = backtrace_symbols(stack_p, size);
	
	printf("%d stack frames.\n", size);

	for(int i = 0; i < size; i++)
	{
		printf ("%s\n", stack_info[i]);
	}

	//在守护里再输出
	fprintf(stderr, "\n#########handle_exception  catch ERROR %d ,"
		" show use $kill -l  #########\n",value);
	fprintf(stderr, "%d 行 stack frames.\n", size);
	
	for(int i = 0; i < size; i++)
	{
		fprintf(stderr,"%s\n", stack_info[i]);
	}

	if(stack_info)
	{
		free(stack_info);
		stack_info = NULL;
	}

	//ShowMemInfo();
	fflush(stdout);

	if(value == SIGINT || value == SIGQUIT
		|| value == SIGFPE || value == SIGKILL
		|| value == SIGSEGV || value == SIGTERM)
	{	
		//printf("kill -9 $(lsof -i:%d | sed -n '2p' | awk '{print $2}') \n", gLoginPort);
		//sprintf(strKillTheSamePortLnvrCmd, "kill -9 $(lsof -i:%d | sed -n '2p' | awk '{print $2}')", gLoginPort);
		//system(strKillTheSamePortLnvrCmd);
		assert(false);
	}

}

void needshowstackbackwhencrack(void)
{
	signal(SIGQUIT, jt_handle_exception);
	signal(SIGKILL, jt_handle_exception);
	signal(SIGFPE,	jt_handle_exception);
	signal(SIGSEGV, jt_handle_exception);
	signal(SIGTERM, jt_handle_exception);
	signal(SIGBUS,	jt_handle_exception);
	signal(SIGPIPE, SIG_IGN);
}
