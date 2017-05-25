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
#include <iostream>
#include <boost/lexical_cast.hpp>  

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
#include "json_prog_name.h"
#include "thread_object.h"
#include "get_config_value.h"


using namespace std;
using namespace rapidjson;


char  *json_error_msg(const char *ordertype);



json_prog_name::json_prog_name()
{
	json_op_name = "prog_name";
	
	LOG_INFO<<"json_prog_name";
}

json_prog_name::~json_prog_name()
{
	
}

int   read_prog_info(   string   &release_info );


int  json_prog_name::json_encode_prog_name( string &return_json_string)
{	


	string file_name;
	
	string release_info;	
	
	int ret =   read_prog_info(release_info );

	if (ret == 0){

	string json_string;
	StringBuffer s;
	Writer<StringBuffer> writer(s);
	writer.StartObject();  
	writer.Key("ordertype");   
	writer.String(json_op_name.c_str()); 
	writer.Key("release_info");   
	writer.String(release_info.c_str()); 	
	writer.EndObject();
	return_json_string= string(s.GetString());
	LOG_INFO<<return_json_string;	

	}else {

	string json_string;
	StringBuffer s;
	Writer<StringBuffer> writer(s);
	writer.StartObject();  
	writer.Key("ordertype");   
	writer.String(json_op_name.c_str()); 
	writer.Key("prog_name");   
	writer.String("not exist"); 
	writer.Key("release_info");   
	writer.String("not exist"); 	
	writer.EndObject();
	return_json_string= string(s.GetString());
	LOG_INFO<<return_json_string;	

	}
	

	
	return 0 ;
	
}


 





int   json_prog_name::json_parse(const char *   json_string, string &return_string)
{
	 
	  Document d;
	  d.Parse(json_string);
	   string  ordertype = d["ordertype"].GetString();

	 cout <<	json_op_name <<endl;	

	 json_encode_prog_name(return_string);
	cout <<return_string<<endl;

	return  0;		
}




void json_prog_name_init(shared_ptr<thread_object>  ptr_thread_object)
{	

	shared_ptr<json_op> ptrjson_prog_name (new json_prog_name());
	
	RegisterJsonOp(ptrjson_prog_name,ptr_thread_object);
}

