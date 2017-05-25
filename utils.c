
#include <stdio.h> 
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
#include <stdlib.h>
#include <unistd.h>
#include <execinfo.h>
#include "utils.h"



int bcd_code_h2int(uint32_t bcd_code)
{
	return (((bcd_code&0xf000)>>12)*10)+((bcd_code&0x0f00)>>8);
}

int bcd_code_l2int(uint32_t bcd_code)
{
	return (((bcd_code&0x00f0)>>4)*10)+(bcd_code&0x0f);
}

int bcd_code_2int(uint32_t bcd_code)
{
	
	return (((bcd_code&0xf000)>>12)*1000)+((bcd_code&0x0f00)>>8)*100+
					(((bcd_code&0x00f0)>>4)*10)+(bcd_code&0x0f);
}







void DebugBacktrace(int signal_no)
{


#define SIZE  1000
	void *array[SIZE];
	int size ,i;
	char **strings;

	fprintf(stderr,"\nSegmenstion fault\n");
	size = backtrace(array,SIZE);
	fprintf(stderr,"Backtrace (%d deep):\n",size);
	strings = backtrace_symbols(array,size);
	for ( i =0;i<size;i++){
		fprintf(stderr,"%d:%s\n",i,strings[i]);
	}
	free(strings);
	exit(0);
}

void  print_help(char *argv[])
{
	printf("%s: help  use -d run as a daemon\n",argv[0]);
	exit(0);
}




char *print_binary_16(uint16_t bin_value,char   *bin_string , int len)
{
	

	//memset(bin_string,0,strlen(bin_string));

	memset(bin_string,0,len);
	
	int i =0;
	for (i = 0;i<16;i++){
		if (bin_value & (1<<(15-i))){
			strcat(bin_string,"1");
		}else {
			strcat(bin_string,"0");
		}
	}

	return bin_string;
}

