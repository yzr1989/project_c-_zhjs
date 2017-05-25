
#include <stdio.h> 
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <ctype.h>
#include <locale.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/vfs.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/utsname.h>
#include <iostream>
#include  <cstdlib>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "parse_json.h"
#include "json_op.h"
#include "json_sysinfo.h"
#include "json_sysinfo_op.h"
#include "json_netinfo.h"
#include "thread_object.h"

using namespace std;
using namespace rapidjson;

 int    get_net_info(struct net_info *PT_net_info[]  ,int *num);

    	
int   json_sysinfo::json_encode(string &return_json_string)
{
	struct utsname  uname_buffer;
	
	double av[3];
	int num;
	struct net_info *PT_net_info[10];
	StringBuffer s;

	Writer<StringBuffer> writer(s);
	writer.StartObject();  
	writer.Key("ordertype");   
	writer.String(json_op_name.c_str()); 	
	
	int ret =0;

    get_net_info(PT_net_info,&num);

     ret = uname(&uname_buffer);
     if (ret <0){
	   printf("uname error\n");
	   return NULL;
	}
	 meminfo();
  	loadavg(&av[0], &av[1], &av[2]);
	struct statfs diskInfo;  
	statfs("/", &diskInfo);  
	unsigned long long totalBlocks = diskInfo.f_bsize;  
	unsigned long long totalSize = totalBlocks * diskInfo.f_blocks;  
	size_t mbTotalsize = totalSize>>20;  
	unsigned long long freeDisk = diskInfo.f_bfree*totalBlocks;  
	size_t mbFreedisk = freeDisk>>20; 
	size_t mbUsedisk  = mbTotalsize - mbFreedisk;
	
	writer.Key("loadavg");   
	writer.Double(av[0]); 

	writer.Key("memused");   
	writer.Int(kb_main_used*100/kb_main_total); 

	writer.Key("diskused");   
	writer.Int(mbUsedisk*100/mbTotalsize); 

	for (int i=0;i<num;i++){
		if (strcmp(  PT_net_info[i]->net_name ,"lo")){			

		writer.Key("net_name");   
		writer.String(PT_net_info[i]->net_name);

		writer.Key("rx_bytes");   
		writer.String(PT_net_info[i]->rx_bytes); 

		writer.Key("tx_bytes");   
		writer.String(PT_net_info[i]->tx_bytes); 

		break;
		}
	
	}
	string release_data_time =string( __DATE__)+" "+ string(__TIME__);	
	writer.Key("release_data_time");   
	writer.String(release_data_time.c_str()); 
	writer.EndObject();
	return_json_string= s.GetString();	
	return 0;
	
}






int  json_sysinfo::json_parse(const char *   json_string, shared_ptr<thread_object>  ptr_thread_object , string &return_string)
{	
	

	json_encode(return_string);

	return 0 ;
}


json_sysinfo::json_sysinfo()
{
	json_op_name = "sysinfo";	
	LOG_INFO<<"json_sysinfo";
}

json_sysinfo::~json_sysinfo()
{

}


void json_sysinfo_init(map <string,shared_ptr<json_op> >  &json_op_map)
{	
	shared_ptr<json_op> ptrjson_sysinfo (new json_sysinfo());
	RegisterJsonOp(ptrjson_sysinfo,json_op_map);
}


