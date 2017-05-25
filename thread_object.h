#ifndef _THREAD_OBJECT_H_
#define _THREAD_OBJECT_H_


#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <mysql++.h>

using namespace boost;
using namespace std;
using namespace mysqlpp ;

class  json_op;
class  harddevice;
class  roominfo;
class  comharddevice;
class  protocol_manager;
class  hardtype;
class  ghdparamlist;

class  thread_object {
public  :	
	thread_object();
	virtual ~thread_object();
	string thread_name ;
	shared_ptr<thread>  thr;
	mutex mutex_obj;
	pid_t pthread_id ;
	bool  pthread_run_flag ;	
	
	list<shared_ptr<harddevice> > harddevice_list;
	list<shared_ptr<roominfo> > roominfo_list;
	list<shared_ptr<comharddevice> > comharddevice_list;
	list<shared_ptr<protocol_manager> >protocol_manager_list;
	list<shared_ptr<hardtype> > hardtype_list;
	list<shared_ptr<ghdparamlist> > ghdparamlist_list;
	
	shared_ptr<comharddevice>  comharddevicePtr;
	virtual  int   get_harddevice(Connection  conn, shared_ptr<comharddevice>   PTcomharddevice,				
				list<shared_ptr<comharddevice> > &comharddevice_list,list<shared_ptr<harddevice> > &harddevice_list);

	virtual  int  parse_comharddevice(list<shared_ptr<comharddevice> > &comharddevice_list ,
			list<shared_ptr<harddevice> > &harddevice_list);
	
	
};

 shared_ptr<thread_object>  get_thread_object(string  thread_name);
 void show_thread_object(void);
int register_thread_object(shared_ptr<thread_object> ptr_thread_object);
 void create_thread_object(void);
 void  wait_thread();
void exit_thread_object(void);

#endif /*_THREAD_OBJECT_H_*/
