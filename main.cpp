#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <pthread.h> 
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>  
#include <execinfo.h>
#include <stdbool.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>  
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>
#include <stdio.h>
#include <sys/resource.h>
#include "config.h"
#include "utils.h"
#include "protocol_manager.h"
#include "get_config_value.h"
#include "parse_json.h"
#include "handle_op.h"
#include "parse_bit_field.h"
#include "struct_comharddevice.h"
#include "struct_harddevice.h"
#include "struct_hardtype.h"
#include "struct_hdparamlist.h"
#include "pthread_op.h"
#include "mysql_parse_modbus.h"
#include "get_info_from_files.h"
#include "thread_object.h"
#include "run_as_daemon.h"
#include "thread_server_udp.h"
#include "thread_air_control.h"
#include "mysql_parse_modbus.h"
#include "struct_comharddevice.h"

using namespace std;
using namespace boost;

int kRollSize = 500*1000*1000;
muduo::AsyncLogging* g_asyncLog = NULL;
extern bool 	thread_air_control_run_flag  ;
extern bool	thread_running_flag ;
extern  bool  thread_server_udp_select_flag;
extern list<shared_ptr<comharddevice> > g_comharddevice_list;
 shared_ptr<thread_object>   thr_server_udp_obj;
 shared_ptr<thread_object>   thr_air_control_obj;
int    thread_air_control_start(shared_ptr<thread_object>   thr_air_control_obj);
int  thread_server_udp_select(shared_ptr<thread_object>  ptr_thread_object);
int  thread_server_udp(shared_ptr<thread_object>   thr_server_udp_obj);
void SignHandler(int iSignNo)
{	
	
	exit_thread_object();
	thread_air_control_run_flag = false ;
	thread_running_flag = false;	
	thread_server_udp_select_flag = false;
}


void asyncOutput(const char* msg, int len)
{
   g_asyncLog->append(msg, len);
   write(STDOUT_FILENO,msg,len);  
}


int main( int argc ,char **argv)
{ 
	signal(SIGSEGV,DebugBacktrace); 
	signal(SIGABRT,DebugBacktrace);
	run_as_daemon();	
	printf("pid = %d\n", getpid());
	char name[256];
	strncpy(name, argv[0], 256);
	muduo::AsyncLogging log(::basename(name), kRollSize);
	log.start();
	g_asyncLog = &log;

	string debug_level ;
	GetProfileString(CONFIG_FILE_NAME, PROG_NAME,"debug_level", debug_level);
	cout <<"debug_level:"<<debug_level<<endl;
	if (debug_level.empty()){
		muduo::Logger::setLogLevel(muduo::Logger::TRACE);
	}

	if (debug_level == "TRACE"){

		muduo::Logger::setLogLevel(muduo::Logger::TRACE);
	}else if (debug_level == "DEBUG"){

		muduo::Logger::setLogLevel(muduo::Logger::DEBUG);
	}else if (debug_level == "INFO"){

		muduo::Logger::setLogLevel(muduo::Logger::INFO);
	}else if (debug_level == "ERROR"){

		muduo::Logger::setLogLevel(muduo::Logger::ERROR);
	}else {

		muduo::Logger::setLogLevel(muduo::Logger::INFO);
	}


	muduo::Logger::setOutput(asyncOutput);
 	LOG_INFO << "Thread start";	
	get_comharddevice_list(g_comharddevice_list);
	thread_air_control_run_flag = false ;
	thread_server_udp_select_flag = false;
	thread_running_flag = false;
	thr_air_control_obj = shared_ptr<thread_air_control_object>(new thread_air_control_object());	
	thr_server_udp_obj = shared_ptr<thread_server_udp_object>(new thread_server_udp_object());	
	thr_server_udp_obj->thr  = shared_ptr<thread> ( new thread(&thread_server_udp,ref(thr_server_udp_obj)));
	thr_server_udp_obj->thr ->join();
	if (thr_air_control_obj->thr ){			
		thr_air_control_obj->thr ->join();
	}	
	wait_thread();	
	freecomharddevice(g_comharddevice_list);
	return 0;

}
