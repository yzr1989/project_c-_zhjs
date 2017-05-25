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
#include "thread_object.h"
#include "thread_server_udp.h"

using namespace std;
using namespace boost;
using namespace rapidjson;



  int json_error_msg(const char *ordertype, string &return_string)
{

	StringBuffer s;	
	const   char *msg = "this string is not json format or this ordertype is not  Implementation";
	Writer<StringBuffer> writer(s);
	writer.StartObject();  
	writer.Key("ordertype");   
	writer.String(ordertype); 
	writer.Key("info");   
	writer.String(msg); 
	writer.EndObject();
	return_string= s.GetString();
	LOG_INFO<<return_string;

	return 0;
}


  int json_error_msg(const char *ordertype,  const char *msg,  string &return_string)
{

	StringBuffer s;		
	Writer<StringBuffer> writer(s);
	writer.StartObject();  
	writer.Key("ordertype");   
	writer.String(ordertype); 
	writer.Key("info");   
	writer.String(msg); 
	writer.EndObject();
	return_string= s.GetString();
	LOG_INFO<<return_string;

	return 0;
}


 int    parse_json( const  char *  json_string,shared_ptr<thread_object>  ptr_thread_object , string &return_string )
{
	  Document d;
	  d.Parse(json_string);
	  string  ordertype;

	if (d.IsObject()){
	
	  
	 if (d.HasMember("ordertype")){
	 
	   ordertype = d["ordertype"].GetString();
	  cout << "ordertype:"<<ordertype<<endl;
		 shared_ptr<thread_server_udp_object>ptr_thread_server_udp_object   =    dynamic_pointer_cast<thread_server_udp_object>(ptr_thread_object);

	shared_ptr<json_op>  PTjson_op=GetJsonOp(ordertype,ptr_thread_server_udp_object->json_op_map);
	if (PTjson_op == NULL){
		
		LOG_ERROR<<"GetJsonOp error";
		json_error_msg(ordertype.c_str(),"GetJsonOp error",return_string);
		return  -1;
		
	}

		  int  ret = PTjson_op->json_parse(json_string,ptr_thread_object,return_string);
		LOG_INFO <<return_string  ;
	      return     ret ;	 
	

	 }else {

		LOG_ERROR  << "json_error";	
		json_error_msg(ordertype.c_str(),"hasn't  ordertype type",return_string);
		return  -1;
		

	 }

		}else {

		LOG_ERROR  << "json_error";	
		ordertype = "unkown";
		json_error_msg(ordertype.c_str(),"isn't  json Object",return_string);
		return  -1;
		

	 }
	 
	
}






int parse_json_init(map <string,shared_ptr<json_op> >  &json_op_map)
{	
	LOG_INFO<<"parse_json_init";
	json_heartbeat_init(json_op_map);
	json_setorder_init(json_op_map);
	json_thread_init(json_op_map);
	json_sysinfo_init(json_op_map);
	json_netinfo_init(json_op_map);
	json_serial_number_init(json_op_map);	
	json_netset_init(json_op_map);
	json_sysop_init(json_op_map);
	json_cmd_init(json_op_map);
	json_battery_init(json_op_map);
	json_sysset_init(json_op_map);
	return 0;
}

