#include <stdio.h> 
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <pthread.h> 
#include <stdio.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <boost/lexical_cast.hpp>  
#include <boost/shared_ptr.hpp>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


#include "config.h"

#include "parse_json.h"
#include "json_op.h"
#include "json_sysset.h"
#include "thread_object.h"
#include "get_config_value.h"

#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace std;
using namespace rapidjson;

json_sysset::json_sysset()
{
	json_op_name = "sysset";

     ptr_sysset  =boost::shared_ptr<struct  sysset> (new sysset);
	
	LOG_INFO<<"json_sysset";
}


json_sysset::~json_sysset()
{
	
	
	LOG_INFO<<"~json_sysset";
}


int   json_sysset::json_encode_sysset(  string &return_string)
{
	StringBuffer s;	

	GetProfileInteger(CONFIG_FILE_NAME, PROG_NAME,"hd_humtime", ptr_sysset->hd_humtime);
	
	GetProfileInteger(CONFIG_FILE_NAME, PROG_NAME,"loop_time", ptr_sysset->loop_time);
	
	
	Writer<StringBuffer> writer(s);
	writer.StartObject();  
	writer.Key("ordertype");   
	writer.String(json_op_name.c_str()); 
	writer.Key("hd_humtime");   
	writer.Int(ptr_sysset->hd_humtime); 
	writer.Key("loop_time");   
	writer.Int(ptr_sysset->loop_time); 	
	writer.EndObject();
	return_string= s.GetString();
	LOG_INFO<<return_string;


	return 0;
}







 int     json_sysset::json_parse(const char *   json_string,shared_ptr<thread_object>  ptr_thread_object ,  string &return_string)
{


	  Document d;
	  d.Parse(json_string);	


	 if ( d.HasMember("hd_humtime") ) {

	 	ptr_sysset->hd_humtime  = d["hd_humtime"].GetInt();

		cout <<ptr_sysset->hd_humtime<<endl;

		sys_config_write_file_int("hd_humtime",ptr_sysset->hd_humtime,CONFIG_FILE_NAME,PROG_NAME);		
		
	 }

	 if ( d.HasMember("loop_time") ) {

	 	ptr_sysset->loop_time  = d["loop_time"].GetInt();

		cout <<ptr_sysset->hd_humtime<<endl;

		sys_config_write_file_int("loop_time",ptr_sysset->loop_time,CONFIG_FILE_NAME,PROG_NAME);		
		
	 }

	json_encode_sysset(return_string);

	return 0;

}




void json_sysset_init(map <string,shared_ptr<json_op> >  &json_op_map)
{
	shared_ptr<json_op> ptrjson_sysset (new json_sysset());
	
	RegisterJsonOp(ptrjson_sysset,json_op_map);	
}





