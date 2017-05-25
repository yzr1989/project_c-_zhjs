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
#include "thread_server_udp.h"
#include "mysql_parse_modbus.h"
#include "struct_hardtype.h"
#include "protocol_manager.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
#include "struct_roominfo.h"
#include "mysql_hdparamlist.h"
#include "json_op.h"


using namespace std ;
using namespace mysqlpp ;

 bool  thread_server_udp_select_flag;
extern  shared_ptr<thread_object>   thr_air_control_obj;
extern list<shared_ptr<comharddevice> > g_comharddevice_list;
void asyncOutput(const char* msg, int len);


int  thread_server_udp(shared_ptr<thread_object>  ptr_thread_object)
{

	string mysql_user_name;
	string  mysql_passwd;
	string mysql_database;	
	string mysql_hostname;

	muduo::Logger::setOutput(asyncOutput);

	 shared_ptr<thread_server_udp_object>ptr_thread_server_udp_object   =    dynamic_pointer_cast<thread_server_udp_object>(ptr_thread_object);

	LOG_INFO<<"thread_server_udp_select";
	struct timeval timeout;
	fd_set read_set;
	int ret = -1;
	string json_string;
	const  int port=61000;  	
	setvbuf(stdout, NULL, _IONBF, 0); 
	fflush(stdout);

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
		return -EINVAL;
	}

	thread_server_udp_select_flag = true;
	parse_json_init(ptr_thread_server_udp_object->json_op_map);
	protocol_init(ptr_thread_object->protocol_manager_list);	
	hardtype_op_init(ptr_thread_object->hardtype_list);		
	get_ghdparamlist_object_from_mysql(conn,ptr_thread_object->ghdparamlist_list);
	get_hdparamlist_object_from_mysql(conn,ptr_thread_object->ghdparamlist_list);
	get_comharddevice_list(ptr_thread_object->comharddevice_list);		
	ptr_thread_object->parse_comharddevice(ptr_thread_object->comharddevice_list,ptr_thread_object->harddevice_list);
	
		
   	ptr_thread_server_udp_object->parse_comharddevice(ptr_thread_server_udp_object->comharddevice_list,ptr_thread_server_udp_object->harddevice_list);

	struct sockaddr_in addrto;
	bzero(&addrto, sizeof(struct sockaddr_in));
	addrto.sin_family = AF_INET;
	addrto.sin_addr.s_addr = htonl(INADDR_ANY);
	addrto.sin_port = htons(port);
	

	struct sockaddr_in from;
	bzero(&from, sizeof(struct sockaddr_in));
	from.sin_family = AF_INET;
	from.sin_addr.s_addr = htonl(INADDR_ANY);
	from.sin_port = htons(port);
	
	int sock = -1;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {   	
		LOG_ERROR<<"socket error";
		exit(-1);
	}   


	if(::bind(sock,(struct sockaddr *)&(addrto), sizeof(struct sockaddr_in)) == -1){	
		LOG_ERROR << "bind error";
		exit(-1);
	}

	int len = sizeof(struct sockaddr_in);
	char smsg[1024*8] ;	
 
	while(thread_server_udp_select_flag){

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		FD_ZERO(&read_set);
		FD_SET(sock, &read_set);

		ret = select(sock+1,&read_set ,NULL,NULL,&timeout );
				if (ret <0){

			LOG_ERROR<<"select error";
			return -1;

		}else if (ret ==0){
			continue;
	
		}else {

		if (FD_ISSET(sock, &read_set)) {

		memset(smsg,0,sizeof(smsg));
		

		int ret=recvfrom(sock, smsg, sizeof(smsg), 0, (struct sockaddr*)&from,(socklen_t*)&len);
		if(ret<=0){			
			
			LOG_ERROR<<"socket error";
		}else{		
			
		}

		LOG_INFO<<"recvfrom:"<<smsg;		
		 parse_json(smsg,ptr_thread_server_udp_object,json_string);
		LOG_INFO<<"parse_json:"<<json_string;		
	        ret =   sendto(sock,json_string.c_str(),strlen(json_string.c_str())+1,0,(struct sockaddr*)&from,len);
		if(ret<=0){			
			LOG_ERROR<<"socket error";
			
		}else{		
			LOG_INFO<<"ret="<<ret;
		}
		
			
			
		}
	
		}
	}
	free_json_op(ptr_thread_server_udp_object->json_op_map);
	freeharddevice(ptr_thread_server_udp_object->harddevice_list);
	free_protocol(ptr_thread_server_udp_object->protocol_manager_list);
	free_harddevice_of_comharddevice(ptr_thread_server_udp_object->comharddevice_list);
	
	freecomharddevice(ptr_thread_server_udp_object->comharddevice_list);
	printf("thread_broadcast quit\n");

	if (thr_air_control_obj){	
		if (thr_air_control_obj->thr ){			
			thr_air_control_obj->thr ->join();
		}

	}
	exit_thread_object();
	
	freecomharddevice(g_comharddevice_list);
	return  0;
}


thread_server_udp_object::thread_server_udp_object()
{
	
}

thread_server_udp_object::~thread_server_udp_object()
{
	
}
