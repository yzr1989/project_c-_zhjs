#ifndef   _MYSQL_ROOMINFO_PCOUNT_H_
#define  _MYSQL_ROOMINFO_PCOUNT_H_


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


using namespace std ;
using namespace mysqlpp ;
using namespace boost;

class thread_object;

int handle_pcount_op(Connection  conn ,shared_ptr<roominfo>  PTroominfo,
				shared_ptr<thread_object>  ptr_thread_object);



#endif   /*_MYSQL_ROOMINFO_PCOUNT_H_*/