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
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>
#include <pthread.h>
//#include <iostream>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <mysql++.h>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>


#include "config.h"
#include "parse_json.h"
#include "get_config_value.h"
#include "thread_object.h"
#include "struct_comharddevice.h"
#include "thread_air_control.h"
#include "thread_server_udp.h"
#include "struct_hardtype.h"

using namespace boost;
using namespace std ;
using namespace mysqlpp ;


int  thread_object::parse_comharddevice(list<shared_ptr<comharddevice> > &comharddevice_list ,
			list<shared_ptr<harddevice> > &harddevice_list)
{

	LOG_INFO<<"thread_object";

	 string  mysql_user_name;
	 string mysql_passwd;
	 string mysql_database;
	string mysql_hostname;		 
	
	GetProfileString(CONFIG_FILE_NAME,PROG_NAME,"mysql_user_name", mysql_user_name);
	GetProfileString(CONFIG_FILE_NAME, PROG_NAME,"mysql_passwd", mysql_passwd);
	GetProfileString(CONFIG_FILE_NAME, PROG_NAME,"mysql_database_name", mysql_database);
	GetProfileString(CONFIG_FILE_NAME, PROG_NAME,"mysql_hostname", mysql_hostname);

	//todo need to add error fixup

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
	
	Connection conn(false);
	if (conn.connect(mysql_database.c_str(), mysql_hostname.c_str(),mysql_user_name.c_str(), mysql_passwd.c_str())) {

	for (list<shared_ptr<comharddevice> >::iterator pr = comharddevice_list.begin(); pr != comharddevice_list.end();pr++){
		LOG_INFO<<(*pr)->com_name<<" "<<(*pr)->device_name;		
		get_harddevice(conn,(*pr),comharddevice_list,harddevice_list);
		struct timespec ts;		
		ts.tv_nsec = 200000000;   
		ts.tv_sec = 0;
		nanosleep(&ts,NULL);			
	}	

	
	}else {
		//cerr << "DB connection failed: " << conn.error() << endl;
		LOG_ERROR<<"DB connection failed: " << conn.error();
		return -EINVAL;
	}
	
	return 0 ;
}



int  thread_air_control_object::parse_comharddevice(list<shared_ptr<comharddevice> > &comharddevice_list ,
			list<shared_ptr<harddevice> > &harddevice_list)
{
	//cout<<"thread_air_control_object"<<endl;
	LOG_INFO<<"thread_air_control_object";
	 string  mysql_user_name;
	 string mysql_passwd;
	 string mysql_database;	
	string mysql_hostname;		 
	
	GetProfileString(CONFIG_FILE_NAME, PROG_NAME,"mysql_user_name", mysql_user_name);
	GetProfileString(CONFIG_FILE_NAME, PROG_NAME,"mysql_passwd", mysql_passwd);
	GetProfileString(CONFIG_FILE_NAME, PROG_NAME,"mysql_database_name", mysql_database);
	GetProfileString(CONFIG_FILE_NAME, PROG_NAME,"mysql_hostname", mysql_hostname);

	//todo need to add error fixup

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
	
	Connection conn(false);
	if (conn.connect(mysql_database.c_str(), mysql_hostname.c_str(),mysql_user_name.c_str(), mysql_passwd.c_str())) {

	for (list<shared_ptr<comharddevice> >::iterator pr = comharddevice_list.begin(); pr != comharddevice_list.end();pr++){
		LOG_INFO<<(*pr)->com_name<<" "<<(*pr)->device_name;
		thread_air_control_object::get_harddevice(conn,(*pr),comharddevice_list,harddevice_list);
		struct timespec ts;		
		ts.tv_nsec = 200000000;   
		ts.tv_sec = 0;
		nanosleep(&ts,NULL);			
	}	

	
	}else {
		//cerr << "DB connection failed: " << conn.error() << endl;
		LOG_ERROR<<"DB connection failed: " << conn.error();
		return -EINVAL;
	}
	
	return 0 ;
}

