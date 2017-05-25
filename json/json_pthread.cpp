#include <stdio.h> 
#include <string.h>
#include <stdint.h>
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
#include <stdbool.h>
#include <json/json.h>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "config.h"
#include "utils.h"
#include "parse_json.h"
#include "json_op.h"
#include "serial_op.h"
#include "json_pthread.h"
#include "struct_roominfo.h"
#include "struct_hdparamlist.h"
#include "thread_object.h"
#include "pthread_op.h"
#include "mysql_parse_modbus.h"
#include "struct_comharddevice.h"


using namespace std;
using namespace boost;
using namespace rapidjson;


bool 	thread_air_control_run_flag  ;
bool		thread_running_flag = false ;

extern  bool  thread_server_udp_select_flag ;


int    thread_air_control_start(shared_ptr<thread_object>   thr_air_control_obj);


extern shared_ptr<thread_object>   thr_air_control_obj;

int   thread_air_control(shared_ptr<thread_object>  ptr_thread_object);

void create_thread_object(void);

extern list<shared_ptr<comharddevice> > g_comharddevice_list;



int    thread_start()
{	
	thread_running_flag = true;	
	freecomharddevice(g_comharddevice_list);
	get_comharddevice_list(g_comharddevice_list);
	//create_thread(g_comharddevice_list);
	get_thread_list(g_comharddevice_list);
	show_thread_object();
	create_thread_object();
	thread_air_control_run_flag = true;	
	thr_air_control_obj->thr  = shared_ptr<thread> ( new thread(&thread_air_control,ref(thr_air_control_obj)));
	
	return 0;
}


int thread_stop()
{
	//thread_exit(g_comharddevice_list);
	exit_thread_object();
       thread_air_control_run_flag = false;
	//freecomharddevice(comharddevice_list);	
	thread_running_flag =false ;
	return 0;
}


int thread_shutdown()
{
	//thread_exit(g_comharddevice_list);
	exit_thread_object();
       thread_air_control_run_flag = false;
	freecomharddevice(g_comharddevice_list);	
	thread_running_flag =false ;
	thread_server_udp_select_flag = false;
	return 0;
}

int  json_thread::json_encode_thread(const char *op_result,string & return_json_string)
{

	StringBuffer s;	
	Writer<StringBuffer> writer(s);
	writer.StartObject();  
	writer.Key("ordertype");   
	writer.String(json_op_name.c_str()); 
	writer.Key("op_result");   
	writer.String(op_result); 
	writer.EndObject();
	return_json_string= s.GetString();
	LOG_INFO<<return_json_string;

	return 0;
}







int   json_thread::json_parse(const char *   json_string,shared_ptr<thread_object>  ptr_thread_object ,  string &return_string)
{

	  Document d;
	  d.Parse(json_string);	

	string op_name ;

	 if ( d.HasMember("op_name") ) {

	 	  op_name = d["op_name"].GetString();

		  cout <<op_name<<endl;

	if (op_name == "start"){
		// start  
		if (thread_running_flag){
			 json_encode_thread("error",return_string);
			 
		}else {
			thread_start();
			json_encode_thread("ok",return_string);
		}
		

	}else if (op_name == "stop"){
		//stop
		thread_stop();
		 json_encode_thread("ok",return_string);
	}else if (op_name =="restart" ){
		// restart
		thread_stop();
		sleep(1);
		if (!thread_running_flag){
			thread_start();
			 json_encode_thread("ok",return_string);
		}else {
			 json_encode_thread("error",return_string);
		}
		
	}else if (op_name =="status"){
		if (thread_running_flag){
			 json_encode_thread("ok",return_string);
		}else {
			json_encode_thread("error",return_string);
		}

	}else if (op_name =="shutdown"){
		thread_shutdown();
		json_encode_thread("ok",return_string);

	}else if (op_name =="test"){		
		thread_running_flag = true;
		freecomharddevice(g_comharddevice_list);
		get_comharddevice_list(g_comharddevice_list);
		get_thread_list(g_comharddevice_list);
		show_thread_object();
		create_thread_object();	
		json_encode_thread("ok",return_string);
	}else if (op_name =="air_control"){		
		
		if (!thread_air_control_run_flag){

			freecomharddevice(g_comharddevice_list);
			get_comharddevice_list(g_comharddevice_list);	
			thr_air_control_obj->thr  = shared_ptr<thread> ( new thread(&thread_air_control,ref(thr_air_control_obj)));
		
			 json_encode_thread("ok",return_string);
		}else {
			json_encode_thread("error",return_string);
		}		
		
	}else {
		// unkown op
		 json_encode_thread("error",return_string);
	}

		
		
	 }else {

			json_encode_thread("error",return_string);
		

	 }

	return 0;


}




json_thread::json_thread()
{
	json_op_name = "thread";	
	LOG_INFO<<"json_thread";
}

json_thread::~json_thread()
{
	
}



void json_thread_init(map <string,shared_ptr<json_op> >  &json_op_map)
{
	shared_ptr<json_op> ptrjson_thread (new json_thread());
	RegisterJsonOp(ptrjson_thread,json_op_map);
}
