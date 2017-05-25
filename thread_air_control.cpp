#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <string.h>  
#include <stdbool.h>
#include <netdb.h>  
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/file.h>
#include <mysql/mysql.h>
#include <mysql++.h>
#include <boost/asio.hpp>
#include <boost/date_time.hpp>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "config.h"
#include "struct_roominfo.h"
#include "mysql_roominfo.h"
#include "mysql_roominfo_ggzm.h"
#include "get_config_value.h"
#include "thread_object.h"
#include "thread_air_control.h"
#include "mysql_hdparamlist.h"
#include "mysql_parse_modbus.h"
#include "mysql_hdparamlist.h"
#include "mysql_hdparamlist.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
#include "struct_roominfo.h"
#include "parse_json.h"
#include "struct_comharddevice.h"
#include "protocol_manager.h"

using namespace std;
using  namespace boost;
using  namespace boost::asio;
extern bool 	thread_air_control_run_flag;
extern list<shared_ptr<comharddevice> > g_comharddevice_list;
void asyncOutput(const char* msg, int len);
int   thread_air_control(shared_ptr<thread_object>  ptr_thread_object)
{

	muduo::Logger::setOutput(asyncOutput);
	LOG_INFO<< "thread_air_control";	
	string mysql_user_name;
	string  mysql_passwd;
	string mysql_database;	
	string mysql_hostname;
	int thread_air_control_loop_time=30;
	int ret = GetProfileInteger(CONFIG_FILE_NAME, PROG_NAME,"loop_time", thread_air_control_loop_time);
	if (ret   == -1){
		thread_air_control_loop_time = 30;
	}

	if (thread_air_control_loop_time <30){
		thread_air_control_loop_time  =30;
	}
	GetProfileString(CONFIG_FILE_NAME, PROG_NAME,"mysql_user_name", mysql_user_name);
	GetProfileString(CONFIG_FILE_NAME, PROG_NAME,"mysql_passwd", mysql_passwd);
	GetProfileString(CONFIG_FILE_NAME, PROG_NAME,"mysql_database_name", mysql_database);
	GetProfileString(CONFIG_FILE_NAME, PROG_NAME,"mysql_hostname", mysql_hostname);
	
	if (mysql_user_name.empty()){
		mysql_user_name ="root";
	}
	if (mysql_passwd.empty()){
		mysql_passwd ="123456";
	}
	if (mysql_database.empty()){
		mysql_database =PROG_NAME;
	}
	if (mysql_hostname.empty()){
		mysql_hostname ="localhost";
	}	
      thread_air_control_run_flag = true;	
	Connection conn(false);	
    	bool con_ret = conn.connect(mysql_database.c_str(), mysql_hostname.c_str(),mysql_user_name.c_str(), mysql_passwd.c_str());
	if (con_ret == false){
		LOG_ERROR<<"DB connection failed: " << conn.error();
		return -EINVAL;
	}	
	LOG_INFO<<"mysql_init";		
	protocol_init(ptr_thread_object->protocol_manager_list);	
	hardtype_op_init(ptr_thread_object->hardtype_list);		
	get_ghdparamlist_object_from_mysql(conn,ptr_thread_object->ghdparamlist_list);
	get_hdparamlist_object_from_mysql(conn,ptr_thread_object->ghdparamlist_list);
	get_comharddevice_list(ptr_thread_object->comharddevice_list);		
	ptr_thread_object->parse_comharddevice(ptr_thread_object->comharddevice_list,ptr_thread_object->harddevice_list);
	while(thread_air_control_run_flag){
		/*   智慧教室   */	
		get_roominfo_from_mysql(conn,ptr_thread_object);	
		LOG_INFO<<"thread_air_control";
		op_roominfo(conn,ptr_thread_object);
		op_roominfo_ggzm(conn,ptr_thread_object);
		freeroominfo(ptr_thread_object->roominfo_list);
		/*节能控制*/	
		{
			struct timespec ts;		
			ts.tv_nsec = 0;   
			ts.tv_sec = thread_air_control_loop_time;
			nanosleep(&ts,NULL);				
		}			
		
	}

	LOG_INFO<<"thread_air_control pid ="<<pthread_self();
	free_harddevice_of_comharddevice(ptr_thread_object->comharddevice_list);
	free_hardtype_op(ptr_thread_object->hardtype_list);
	freeharddevice(ptr_thread_object->harddevice_list);
	freecomharddevice(ptr_thread_object->comharddevice_list);
	return 0;
}



thread_air_control_object::thread_air_control_object()
{
	
}

thread_air_control_object::~thread_air_control_object()
{
	
}