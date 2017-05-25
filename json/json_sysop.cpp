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
#include "json_sysop.h"
#include "thread_object.h"

using namespace std;
using namespace rapidjson;


const char  *json_error_msg(const char *ordertype);


json_sysop::json_sysop()
{
	json_op_name = "sysop";
	//cout <<"json_sysop"<<endl;
	LOG_INFO<<"json_sysop";
}

json_sysop::~json_sysop()
{
	
}

 int   json_sysop::json_encode_sysop(string &return_string)
{
	StringBuffer s;		
	Writer<StringBuffer> writer(s);
	writer.StartObject();  
	writer.Key("ordertype");   
	writer.String(json_op_name.c_str()); 
	writer.Key("op_result");   
	writer.String("ok"); 
	writer.EndObject();
	return_string= s.GetString();
	LOG_INFO<<return_string;	
	return  0;
	
}





int    json_sysop::json_parse(const char *   json_string,shared_ptr<thread_object>  ptr_thread_object ,  string &return_string)
{	

	  Document d;
	  d.Parse(json_string);	


	 if ( d.HasMember("op_name") ) {

	 	 string   op_name = d["op_name"].GetString();

		cout <<op_name<<endl;
		if (op_name  == "reboot"){
			LOG_INFO<<"reboot";	

		}else if (op_name == "shutdown"){
		
		LOG_INFO<<"shutdown";
		}
				
		
	 }

	json_encode_sysop(return_string);

	return  0;
	


}



void json_sysop_init(map <string,shared_ptr<json_op> >  &json_op_map)
{	

	shared_ptr<json_op> ptrjson_sysop (new json_sysop());
	
	RegisterJsonOp(ptrjson_sysop,json_op_map);
}

