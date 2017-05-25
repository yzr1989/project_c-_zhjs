#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <boost/shared_ptr.hpp>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "thread_object.h"

using namespace std;
using namespace boost;

list<shared_ptr<thread_object> > thread_object_list;

thread_object::thread_object()
{
	
}


thread_object::~thread_object()
{

}

int    mysql_create_pthread(shared_ptr<thread_object>  ptr_thread_object);


int register_thread_object(shared_ptr<thread_object> ptr_thread_object)
{
	thread_object_list.push_back(ptr_thread_object);
	return 0;
}




void  wait_thread()
{
		for (list<shared_ptr<thread_object> >::iterator  pr = thread_object_list.begin(); pr != thread_object_list.end();pr++){
		

		(*pr)->thr->join();
	}
}


void show_thread_object(void)
{
	
	for (list<shared_ptr<thread_object> >::iterator  pr = thread_object_list.begin(); pr != thread_object_list.end();pr++){
		
	
		//cout << "thread_name " <<(*pr)->thread_name <<endl;
		LOG_INFO<< "thread_name " <<(*pr)->thread_name ;
	}
	
}


void create_thread_object(void)
{
	
	for (list<shared_ptr<thread_object> >::iterator  pr = thread_object_list.begin(); pr != thread_object_list.end();pr++){
		

		(*pr)->thr = shared_ptr<thread> ( new thread(&mysql_create_pthread,ref(*pr)));
		
	}
	
}

void exit_thread_object(void)
{
	for (list<shared_ptr<thread_object> >::iterator  pr = thread_object_list.begin(); pr != thread_object_list.end();pr++){		

		//(*pr)->thr->join();
		if ((*pr)){
			(*pr)->pthread_run_flag = false;
		}
	}
}



