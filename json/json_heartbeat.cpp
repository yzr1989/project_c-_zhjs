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
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

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
#include "json_heartbeat.h"
#include "thread_object.h"

using namespace std;
using namespace rapidjson;
using namespace boost::posix_time;
using namespace boost::gregorian;

extern bool thread_running_flag  ;

const char  *json_error_msg(const char *ordertype);

json_heartbeat::json_heartbeat()
{
	json_op_name = "heartbeat";
	LOG_INFO<<"json_heartbeat";
}


json_heartbeat::~json_heartbeat()
{
	
	LOG_INFO<<"~json_heartbeat";
	
}

 int    json_heartbeat::json_encode_heartbeat(string & return_json_string)
{
	ostringstream os;
	time_facet *tfacet = new time_facet("%Y-%m-%d %H:%M:%S");
	os.imbue(locale(os.getloc(),tfacet));
	ptime p_now = second_clock::local_time();	

	os <<p_now;
	string string_ret  = os.str() ;	
	
	string json_string;
	StringBuffer s;
	Writer<StringBuffer> writer(s);
	writer.StartObject();  
	writer.Key("ordertype");   
	writer.String(json_op_name.c_str()); 
	writer.Key("time");   
	writer.String(string_ret.c_str()); 
	writer.EndObject();
	return_json_string= string(s.GetString());
	LOG_INFO<<return_json_string;

	return 0;
	
}


int    thread_start();

int     json_heartbeat::json_parse(const char *   json_string, shared_ptr<thread_object>  ptr_thread_object , string &return_string)
{

      
     	LOG_INFO<<"json_parse_heartbeat";

	
	  Document d;
	  d.Parse(json_string);
	   string  ordertype = d["ordertype"].GetString();


	if (!thread_running_flag){			

		thread_start();
		
	}


	 cout <<	json_op_name <<endl;

	 if ( d.HasMember("time") ) {

	 time_string = d["time"].GetString();

		ptime p1 =time_from_string(time_string);
		cout << p1 <<endl;
		tm  tm1 = to_tm(p1);
		time_t t = mktime(&tm1);
		stime(&t);
	
	 }

	 json_encode_heartbeat(return_string);
	cout <<return_string<<endl;

	return  0 ; 
	
     
}


void json_heartbeat_init(map <string,shared_ptr<json_op> >  &json_op_map)
{	

	shared_ptr<json_op> ptrjson_heartbeat (new json_heartbeat());
	
	RegisterJsonOp(ptrjson_heartbeat,json_op_map);
}

