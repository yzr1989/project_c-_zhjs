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
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <fcntl.h>
#include <sys/file.h>

#include <mysql/mysql.h>
#include <mysql++.h>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>
#include "config.h"
#include "utils.h"
#include "protocol_manager.h"
#include "get_config_value.h"
#include "handle_op.h"
#include "thread_object.h"
#include "mysql_pthread.h"
#include "mysql_hdparamlist.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
#include "struct_roominfo.h"

using namespace std;
using namespace boost;
using namespace mysqlpp ;
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
extern list<shared_ptr<comharddevice> > g_comharddevice_list;
void asyncOutput(const char* msg, int len);
int  mysql_pthread_op(shared_ptr<thread_object>  ptr_thread_object ,int i)
{


	int rc =0;
	mutex::scoped_lock lock(ptr_thread_object->comharddevicePtr->mutex_obj);
	shared_ptr<protocol_manager>  PTprotocol_manager = Getprotocol(ptr_thread_object->comharddevicePtr->hdharddevices[i]->Ptprotocol_manager->protocol_name,ptr_thread_object->protocol_manager_list);
	if (!PTprotocol_manager){	
		LOG_ERROR << "Getprotocol error"<<i;
	{
		struct timespec ts;		
		ts.tv_nsec = 0;   
		ts.tv_sec = 1;
		nanosleep(&ts,NULL);	
	}

		
		return -1;
	}

	LOG_INFO<<"protocol_name="<<PTprotocol_manager->protocol_name;	
	rc =PTprotocol_manager->protocol_init(ptr_thread_object->comharddevicePtr->hdharddevices[i]);
	if (rc  == -1){	
		LOG_ERROR<<"protocol_init error";		
		PTprotocol_manager->protocol_close(ptr_thread_object->comharddevicePtr->hdharddevices[i]);			
		{
			struct timespec ts;		
			ts.tv_nsec = 0;   
			ts.tv_sec = 1;
			nanosleep(&ts,NULL);	
		}			
		return -1;
		
	}

	PTprotocol_manager->protocol_set_slave(ptr_thread_object->comharddevicePtr->hdharddevices[i]);
	rc = PTprotocol_manager->protocol_read_registers(ptr_thread_object->comharddevicePtr->hdharddevices[i]);
	PTprotocol_manager->protocol_close(ptr_thread_object->comharddevicePtr->hdharddevices[i]);
	{
		struct timespec ts;		
		ts.tv_nsec = 0;   
		ts.tv_sec = 1;
		nanosleep(&ts,NULL);	
	}
	return rc;
}

int    mysql_create_pthread(shared_ptr<thread_object>  ptr_thread_object)
{  
	int rc=0;	
	muduo::Logger::setOutput(asyncOutput);	
	string mysql_user_name;
	string  mysql_passwd;
	string mysql_database;	
	string mysql_hostname;	
	int i=0;	
	string sql_cmd;
	int  ret;	
	shared_ptr<protocol_manager>  PTprotocol_manager;
	LOG_INFO<<"thread_modbus_readinto_sqlite:com_name="<<ptr_thread_object->comharddevicePtr->com_name;
	LOG_INFO<<"nbharddevice="<<ptr_thread_object->comharddevicePtr->nbharddevice;
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
	Connection conn(false);
    	bool con_ret = conn.connect(mysql_database.c_str(), mysql_hostname.c_str(),mysql_user_name.c_str(), mysql_passwd.c_str());
	if (con_ret == false){	
		LOG_ERROR<<"DB connection failed: " << conn.error();
		ptr_thread_object->pthread_run_flag = false ;				
	}
	

	protocol_init(ptr_thread_object->protocol_manager_list);
	ret = hardtype_op_init(ptr_thread_object->hardtype_list);
	if (ret == -1){
		LOG_ERROR<<"com_name:"<<ptr_thread_object->comharddevicePtr->com_name<<" hardtype_op_init error" ;
		free_protocol(ptr_thread_object->protocol_manager_list);		
		free_hardtype_op(ptr_thread_object->hardtype_list);		
		return ret;	
	}
	ret = get_ghdparamlist_object_from_mysql(conn,ptr_thread_object->ghdparamlist_list);
	if (ret == -1){
		LOG_ERROR<<"com_name:"<<ptr_thread_object->comharddevicePtr->com_name<<" get_ghdparamlist_object_from_mysql error";
		free_protocol(ptr_thread_object->protocol_manager_list);		
		free_ghdparamlist(ptr_thread_object->ghdparamlist_list);
		free_hardtype_op(ptr_thread_object->hardtype_list);	
		return ret;	
	}


	ret = get_hdparamlist_object_from_mysql(conn,ptr_thread_object->ghdparamlist_list);
	if (ret == -1){
		LOG_ERROR<<"com_name:"<<ptr_thread_object->comharddevicePtr->com_name<<" get_hdparamlist_object_from_mysql error";
		free_protocol(ptr_thread_object->protocol_manager_list);		
		free_ghdparamlist(ptr_thread_object->ghdparamlist_list);
		free_hardtype_op(ptr_thread_object->hardtype_list);	
		return ret;	
	}


	ret =  ptr_thread_object->get_harddevice(conn,ptr_thread_object->comharddevicePtr,g_comharddevice_list,ptr_thread_object->harddevice_list);
	if (ret == -1){
		LOG_ERROR<<"com_name:"<<ptr_thread_object->comharddevicePtr->com_name<<" get_harddevice error";
		free_protocol(ptr_thread_object->protocol_manager_list);	
		free_ghdparamlist(ptr_thread_object->ghdparamlist_list);
		free_hardtype_op(ptr_thread_object->hardtype_list);	
		free_protocol(ptr_thread_object->protocol_manager_list);
		freeharddevice_of_hdharddevices_vect(ptr_thread_object->comharddevicePtr);	
		return ret;	
	}
	

	{

	}
	
	if (ptr_thread_object->comharddevicePtr->nbharddevice == 0){
		ptr_thread_object->pthread_run_flag  == false;
	}
	ptr_thread_object->pthread_run_flag = true;
	while(ptr_thread_object->pthread_run_flag){		
		
		/*find all device on current com*/
	    for (i = 0;i < ptr_thread_object->comharddevicePtr->nbharddevice;i++){
			
	      if (ptr_thread_object->pthread_run_flag  == false)
		  	break;		
		
		if (time(NULL) >= ptr_thread_object->comharddevicePtr->hdharddevices[i]->time_now +
			ptr_thread_object->comharddevicePtr->hdharddevices[i]->hddelay_time  ){
			PTprotocol_manager = Getprotocol(ptr_thread_object->comharddevicePtr->hdharddevices[i]->Ptprotocol_manager->protocol_name, ptr_thread_object->protocol_manager_list);
			if (!PTprotocol_manager){
				LOG_ERROR<<"com_name:"<<ptr_thread_object->comharddevicePtr->com_name<<" Getprotocol error";
				continue;;
			}
			
			rc = mysql_pthread_op(ptr_thread_object ,i);

			++(ptr_thread_object->comharddevicePtr->hdharddevices[i]->connect_times);
			
				/*  insert operation  */ 
			if (rc != -1){
				++(ptr_thread_object->comharddevicePtr->hdharddevices[i]->connect_fine_times);							
				PTprotocol_manager->handle_harddevice(ptr_thread_object->comharddevicePtr->hdharddevices[i]);	
				rc =PTprotocol_manager->sql_insert_handle(ptr_thread_object->comharddevicePtr->hdharddevices[i],PTprotocol_manager,conn);	
				if (rc == -1){
					LOG_ERROR<<"com_name:"<<ptr_thread_object->comharddevicePtr->com_name<<" sql_insert_handle  error" ;
					continue;
				}	
				
				rc =PTprotocol_manager->update_lastconnecttime(ptr_thread_object->comharddevicePtr->hdharddevices[i],conn);
					if (rc == -1){
					LOG_ERROR<<"com_name:"<<ptr_thread_object->comharddevicePtr->com_name<<" update_lastconnecttime  error";
					continue;
				}			
				ptr_thread_object->comharddevicePtr->hdharddevices[i]->hd_connect =  (ptr_thread_object->comharddevicePtr->hdharddevices[i]->hd_connect<<1)|0;	

				//do some hdtype  op 

				LOG_INFO<<"hdtype="<<ptr_thread_object->comharddevicePtr->hdharddevices[i]->hdtype;
				shared_ptr<hardtype>  hardtypePtr = Gethardtype(ptr_thread_object->comharddevicePtr->hdharddevices[i]->hdtype,ptr_thread_object->hardtype_list);
				if (hardtypePtr){						
						hardtypePtr->hardtype_op(ptr_thread_object->comharddevicePtr->hdharddevices[i],conn);
				}				
					
			}else if (rc == -1){
					LOG_INFO<<"serial  read error";
					ptr_thread_object->comharddevicePtr->hdharddevices[i]->hd_connect =  (ptr_thread_object->comharddevicePtr->hdharddevices[i]->hd_connect<<1)|1;
				}	
				rc = PTprotocol_manager->update_hd_connect(ptr_thread_object->comharddevicePtr->hdharddevices[i],conn);				
				if (rc == -1){
					LOG_ERROR<<"com_name:"<<ptr_thread_object->comharddevicePtr->com_name<<" update_hd_connect error" ;
					continue;
				}
				rc =  PTprotocol_manager->update_hd_connect_times(ptr_thread_object->comharddevicePtr->hdharddevices[i],conn);
				if (rc == -1){
					LOG_ERROR<<"com_name:"<<ptr_thread_object->comharddevicePtr->com_name<<" update_hd_connect_times error" ;	
					continue;
				}
				
	    		}
	    	}
	}

	free_ghdparamlist(ptr_thread_object->ghdparamlist_list);
	free_hardtype_op(ptr_thread_object->hardtype_list);	
	free_protocol(ptr_thread_object->protocol_manager_list);
	freeharddevice_of_hdharddevices_vect(ptr_thread_object->comharddevicePtr);
	LOG_INFO<<"com_name:"<<ptr_thread_object->comharddevicePtr->com_name<<" thread exit !!!";
	return 0;
	
}  



mysql_pthread_object::mysql_pthread_object(shared_ptr<comharddevice>  comharddevicePtr)
{
	thread_name = comharddevicePtr->com_name;
	this->comharddevicePtr = comharddevicePtr;
}
