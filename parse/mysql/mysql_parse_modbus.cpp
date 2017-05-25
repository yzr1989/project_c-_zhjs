#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <iostream>
#include <iomanip>
#include <mysql++.h>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "config.h"

#include "mysql_parse_modbus.h"
#include "mysql_hardtype.h"
#include "struct_hardtype.h"
#include "struct_hdparamlist.h"
#include "struct_comharddevice.h"
#include "get_config_value.h"
#include "mysql_hdparamlist.h"
#include "thread_object.h"


using namespace std ;
using namespace mysqlpp ;

extern list<shared_ptr<comharddevice> > g_comharddevice_list;
int   get_comharddevice_object_from_mysql(Connection  conn,list<shared_ptr<comharddevice> > &comharddevice_list);



int get_comharddevice_list( list<shared_ptr<comharddevice> >  &comharddevice_list)
{	
	 string  mysql_user_name;
	 string mysql_passwd;
	 string mysql_database;	
	 string mysql_hostname;	

	   LOG_INFO << "get_comharddevice_list";	
	
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

		//cout <<"mysql_init"<<endl;
		//LOG_INFO<<"mysql_init";

		get_comharddevice_object_from_mysql(conn,comharddevice_list);
		//Showcomharddevice(comharddevice_list);
	
	}else {
		//cerr << "DB connection failed: " << conn.error() << endl;
		  LOG_ERROR<< "DB connection failed: " << conn.error() ;
		return -EINVAL;
	}	

	return 0;	
		
}



