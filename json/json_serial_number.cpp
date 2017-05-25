
#include <stdio.h> 
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <glib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>


#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


#include "parse_json.h"
#include "json_op.h"
#include "config.h"
#include "json_serial_number.h"
#include "thread_object.h"
#include "get_config_value.h"


using namespace rapidjson;
using namespace std;



int    json_serial_number::json_encode_get_serial_number(string & return_json_string)
{
	StringBuffer s;

	string  serial_number_string;

	GetProfileString(CONFIG_SYS_FILE_NAME, "system_config","serial_number", serial_number_string);

	
	
	Writer<StringBuffer> writer(s);
	writer.StartObject();  
	writer.Key("ordertype");   
	writer.String(json_op_name.c_str()); 
	writer.Key("serial_number");   
	writer.String(serial_number_string.c_str()); 
	writer.EndObject();
	return_json_string= s.GetString();
	LOG_INFO<<return_json_string;

	return 0;
}






int    json_serial_number::json_parse(const char *   json_string, shared_ptr<thread_object>  ptr_thread_object , string &return_string)
{	

	  Document d;
	  d.Parse(json_string);	


	 if ( d.HasMember("serial_number") ) {

	 	 string   serial_number_string = d["serial_number"].GetString();

		cout <<serial_number_string<<endl;

		json_encode_write_serial_number(serial_number_string.c_str());		
		
	 }

	json_encode_get_serial_number(return_string);

	return  0;
	


}


json_serial_number::json_serial_number()
{
	json_op_name = "serial_number";

	LOG_INFO<<"json_serial_number";
}


json_serial_number::~json_serial_number()
{
	LOG_INFO<<"~json_serial_number";
}

void json_serial_number_init(map <string,shared_ptr<json_op> >  &json_op_map)
{	
	shared_ptr<json_op> ptr_json_serial_number (new json_serial_number());
	RegisterJsonOp(ptr_json_serial_number,json_op_map);
}


