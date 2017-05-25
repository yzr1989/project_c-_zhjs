
#include <unistd.h>
#include <stdbool.h>
//#include <iostream>
#include <string>
#include <vector>
#include <deque> 
#include <list>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>


#include "struct_comharddevice.h"
#include "protocol_manager.h"
#include "thread_object.h"
#include "mysql_pthread.h"

using namespace std;
using namespace boost;



int    mysql_create_pthread(shared_ptr<thread_object>  ptr_thread_object);


int  get_thread_list(list<shared_ptr<comharddevice> > &comharddevice_list)
{
	//cout<<"create_thread"<<endl;
	LOG_INFO<<"create_thread";
	
	//cout <<"comharddevice_list size"<< comharddevice_list.size()<<endl;
	LOG_INFO<<"comharddevice_list size"<< comharddevice_list.size();

	for (list<shared_ptr<comharddevice> >::iterator pr = comharddevice_list.begin(); pr != comharddevice_list.end();pr++){
		
		//cout<<(*pr)->com_name<<" "<<(*pr)->device_name<<endl;
		LOG_INFO<<(*pr)->com_name<<" "<<(*pr)->device_name;
		
		shared_ptr<thread_object>   mysql_pthread_obj ( new mysql_pthread_object(*pr));
		register_thread_object(mysql_pthread_obj);	
		
		
		//(*pr)->pthread_run_flag = true;
		
		//(*pr)->thr = shared_ptr<thread>  (new thread(mysql_create_pthread, ref(*pr)) );		
		//usleep(200*1000);	
		struct timespec ts;		
		ts.tv_nsec = 200000000;   
		ts.tv_sec = 0;
		nanosleep(&ts,NULL);			
	}
	
	return 0 ;
}





