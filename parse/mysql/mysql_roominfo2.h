#ifndef   _MYSQL_ROOMINFO_2_H_
#define _MYSQL_ROOMINFO_2_H_

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
#include <boost/shared_ptr.hpp>
//#include "thread_object.h"

using namespace std ;
using namespace mysqlpp ;
using namespace boost;

class thread_object;

int  get_roominfo_from_mysql(Connection  conn,shared_ptr<thread_object>  ptr_thread_object);
int get_serial_info(Connection  conn,shared_ptr<roominfo> PTroominfo,shared_ptr<thread_object>  ptr_thread_object);
int  op_roominfo_object(Connection  conn,shared_ptr<roominfo> PTroominfo,shared_ptr<thread_object>  ptr_thread_object);
int  op_roominfo(Connection  conn,shared_ptr<thread_object>  ptr_thread_object);

int get_timetable_detail(Connection  conn,shared_ptr<roominfo> PTroominfo);

int op_hddevice_list(Connection  conn ,shared_ptr<roominfo>  PTroominfo,
				shared_ptr<thread_object>  ptr_thread_object);


int get_jnkz_nobody_info(Connection  conn,shared_ptr<roominfo> PTroominfo);

int get_jnkz_modelid_isused(Connection  conn,shared_ptr<roominfo> PTroominfo);

int op_hddevice_list_jnkz(Connection  conn ,shared_ptr<roominfo>  PTroominfo,
				shared_ptr<thread_object>  ptr_thread_object);


int handle_jnkz_op(Connection  conn ,shared_ptr<roominfo>  PTroominfo,
				shared_ptr<thread_object>  ptr_thread_object);


int  op_roominfo2(Connection  conn,shared_ptr<thread_object>  ptr_thread_object);


#endif   /*_MYSQL_ROOMINFO_2_H_*/