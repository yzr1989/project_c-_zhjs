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
//#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>


#include "config.h"
#include "utils.h"

#include "protocol_manager.h"
#include "struct_hardtype.h"
#include "handle_op.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
//#include "struct_roominfo.h"

using namespace std;
using namespace boost;



void  protocol_manager::handle_harddevice(  shared_ptr<harddevice> PTharddevice) 
{

	LOG_INFO<<"hdid="<<PTharddevice->hdid;	
	LOG_INFO<<"max_addr_register="<<PTharddevice->gparamlist->max_addr_register;	
	LOG_INFO<<"min_addr_register="<<PTharddevice->gparamlist->min_addr_register;	
	LOG_INFO<<"addr_register_num="<<PTharddevice->gparamlist->addr_register_num;
	LOG_INFO<<"hdaddr="<<PTharddevice->hdaddr;
	PTharddevice->time_now = time(NULL);	
}





int     protocol_manager::sql_insert_handle(  shared_ptr<harddevice>  PTharddevice, shared_ptr<protocol_manager>  PTprotocol_manager , Connection  conn ) 
{
	int i =0;
	string  sql_cmd ;
	int nbhdparamlist =0;
	nbhdparamlist =PTharddevice->gparamlist->nbhdparamlist;		

	sql_cmd = "update harddevice set ";

	for (i=0;i< nbhdparamlist ;i++){
		sql_cmd =sql_cmd+PTharddevice->gparamlist->hdparamlists[i]->colum;
		sql_cmd = sql_cmd + "='";
		PTprotocol_manager->handle_hd_register_length((PTharddevice->gparamlist->hdparamlists[i]->hd_register_length),PTharddevice,sql_cmd,i);
 		sql_cmd = sql_cmd + "',";
	}	
	sql_cmd = sql_cmd + "tbmodytime=now()  where  hdid="+PTharddevice->hdid+ ";";
	LOG_INFO<<"cmd="<<sql_cmd;	
	mysqlpp::Query query = conn.query(sql_cmd);	
	if (query.execute()){
	}else {
		LOG_INFO<<"error: " << query.error();
		return -1;
	}	
	return  0;
}






 int    protocol_manager::update_hd_connect ( shared_ptr<harddevice> PTharddevice, Connection  conn)
 {
	
	char bin_string[16];
	char cmd[100];	
	LOG_INFO<<"PTharddevice->hd_connect="<<PTharddevice->hd_connect;
	LOG_INFO<<	"print_binary_16="<<print_binary_16(PTharddevice->hd_connect,bin_string,sizeof(bin_string));
	sprintf(cmd,"update harddevice set hd_connect='%s'  where hdid= %s;",print_binary_16(PTharddevice->hd_connect,bin_string,sizeof(bin_string)),(PTharddevice->hdid).c_str());
	string sql_cmd  = string(cmd);	
	LOG_INFO<<"cmd="<<sql_cmd;	
	mysqlpp::Query query = conn.query(sql_cmd);
	if (query.execute()){
	}else {
		LOG_ERROR<<"error: " << query.error();
		return -1;
	}
	return 0;
 }	

 int    protocol_manager::update_hd_connect_times ( shared_ptr<harddevice> PTharddevice, Connection  conn)
{
	int fine_percent =0;
	char cmd[100];
	LOG_INFO<<"PTharddevice->connect_times="<<PTharddevice->connect_times;
	LOG_INFO<<"PTharddevice->connect_fine_times="<<PTharddevice->connect_fine_times;	
	if (PTharddevice->connect_times ==0){
		//·ÀÖ¹³ýÊýÎªÁã
		PTharddevice->connect_times = 1;
	}
	fine_percent = PTharddevice->connect_fine_times*100/PTharddevice->connect_times;
	sprintf(cmd,"update harddevice set fine_percent='%d'  where hdid= %s;",fine_percent,(PTharddevice->hdid).c_str());
	string sql_cmd = string (cmd);
	LOG_INFO<<"cmd= "<<sql_cmd;
	mysqlpp::Query query = conn.query(sql_cmd);
	if (query.execute()){
	}else {
		LOG_ERROR<<"error: " << query.error();
		return -1;
	}
	return 0;
}
 

  int    protocol_manager::update_lastconnecttime ( shared_ptr<harddevice> PTharddevice, Connection  conn)
 {

	LOG_INFO<<"hdid ="+PTharddevice->hdid;
	string sql_cmd = "update harddevice set hd_lastconnecttime=now()  where hdid="+ PTharddevice->hdid+";";
	LOG_INFO<<"sql_cmd="<<sql_cmd;
	mysqlpp::Query query = conn.query(sql_cmd);
	if (query.execute()){
	}else {
		LOG_ERROR<<"error: " << query.error();
		return -1;
	}
	return 0;

 }	





