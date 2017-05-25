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
#include "json_battery.h"
#include "thread_object.h"

using namespace std;
using namespace rapidjson;


char  *json_error_msg(const char *ordertype);



json_battery::json_battery()
{
	json_op_name = "battery";

	LOG_INFO<<"json_battery";
}

json_battery::~json_battery()
{
	
}

int    json_battery:: json_encode_battery(string & return_json_string)
{

	
	
	string json_string;
	StringBuffer s;
	Writer<StringBuffer> writer(s);
	writer.StartObject();  
	writer.Key("ordertype");   
	writer.String(json_op_name.c_str()); 
	writer.Key("result");   
	writer.String("ok"); 
	writer.EndObject();
	return_json_string= string(s.GetString());
	LOG_INFO<<return_json_string;
	
	return 0;
	
}


 





int   json_battery::json_parse(const char *   json_string,shared_ptr<thread_object>  ptr_thread_object , string &return_string)
{
	
	  Document d;
	  d.Parse(json_string);
	   string  ordertype = d["ordertype"].GetString();

	 cout <<	json_op_name <<endl;	

	 json_encode_battery(return_string);
	cout <<return_string<<endl;

	return 0;	
}


void json_battery_init(map <string,shared_ptr<json_op> >  &json_op_map)
{	

	shared_ptr<json_op> ptrjson_battery (new json_battery());
	
	RegisterJsonOp(ptrjson_battery,json_op_map);
}

