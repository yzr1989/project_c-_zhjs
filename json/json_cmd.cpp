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

#include <boost/lexical_cast.hpp>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


#include "config.h"
#include "utils.h"
#include "thread_object.h"
#include "parse_json.h"
#include "json_op.h"
#include "json_cmd.h"
#include "serial_op.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
#include "struct_roominfo.h"
#include "mysql_parse_modbus.h"

using namespace std;
using namespace rapidjson;

extern list<shared_ptr<comharddevice> > g_comharddevice_list;


static 	char   hex_ret_buffer[1000];


  int    json_cmd::json_encode_cmd(  int string_len,string & return_json_string)
{
	StringBuffer s;
	
	
	Writer<StringBuffer> writer(s);
	writer.StartObject();  
	writer.Key("ordertype");   
	writer.String(json_op_name.c_str()); 
	writer.Key("string");   
	writer.String(hex_ret_buffer); 
	writer.EndObject();
	return_json_string= s.GetString();
	LOG_INFO<<return_json_string;

	return 0;		
}


 static int   json_cmd_serial_op(shared_ptr<comharddevice>  comharddevicePtr,const char *buffer  , int datalen,serial_op *  PTserial_op)
{
	int ret =0;
	struct timeval tv; 
	 fd_set rdfds;  
	 int max_fd;
	
	 const   char *str = "timeout!\0";
	 mutex::scoped_lock lock(comharddevicePtr->mutex_obj);

	ret = open_serial(PTserial_op);
	if (ret){
		printf("open_serial error\n");
		return  -1;
	}
	
	ret = write(PTserial_op->fd,buffer,datalen);

	 FD_ZERO(&rdfds);  
        FD_SET(PTserial_op->fd,&rdfds);
	tv.tv_sec = 5;  
        tv.tv_usec = 0;  
	max_fd=PTserial_op->fd+1;
        ret = select(max_fd,&rdfds,NULL,NULL,&tv);  
        if(ret<0) { 
              //printf("selcet   error\n");
              LOG_INFO<<"selcet   error\n";
		close_serial(PTserial_op);
		free(PTserial_op);			  
		return -1;
        } else if(ret == 0){  
              //printf("timeout \n");
              LOG_ERROR<<"timeout";
		memset(hex_ret_buffer,0,sizeof(hex_ret_buffer));
		memcpy(hex_ret_buffer,str,strlen(str));
		
        } else  {  
         memset(hex_ret_buffer,0,sizeof(hex_ret_buffer));   
	ret = read(PTserial_op->fd,hex_ret_buffer,sizeof(hex_ret_buffer));
	//printf("reading ret=%d\n",ret); 	
	LOG_INFO<<"reading ret="<<ret;
        }

	close_serial(PTserial_op);
	free(PTserial_op);

	return ret;
}





 int   json_cmd::json_parse(const char *   json_string ,shared_ptr<thread_object>  ptr_thread_object ,  string &return_string)
{

	  Document d;
	  d.Parse(json_string);	
	int baudrate=115200;
	const  char *baudrate_str;

	const char *comname ;
	const char *string;
	const char * parity_str;
	char parity = 'N';		

	 if ( d.HasMember("comname") ) {

	 	 comname = d["comname"].GetString();
		 cout <<comname<<endl;
		
	 }else {
	 
		json_error_msg(json_op_name.c_str(),return_string);
		return -1;
	 }


	if (g_comharddevice_list.size()  == 0){

		get_comharddevice_list(g_comharddevice_list);	

	}

	cout <<"g_comharddevice_list="<<g_comharddevice_list.size()<<endl;


	shared_ptr<comharddevice>  comharddevicePtr=Gethdcomharddevice(comname,g_comharddevice_list);
	if(!comharddevicePtr){
		json_error_msg(json_op_name.c_str(),return_string);
		return -1;
	}
	

	 if ( d.HasMember("parity") ) {

	 	 parity_str = d["parity"].GetString();
		 cout <<parity_str<<endl;
		 parity = parity_str[0];
		
	 }else {
	 
		json_error_msg(json_op_name.c_str(),return_string);
		return -1;
	 }


	 if ( d.HasMember("baudrate") ) {

	 	 baudrate_str = d["baudrate"].GetString();
		 cout <<baudrate_str<<endl;
		 // add try 
		 baudrate = lexical_cast<int>(baudrate_str);
		
	 }else {
	 
		json_error_msg(json_op_name.c_str(),return_string);
		return -1;
	 }




	 if ( d.HasMember("string") ) {

	 	 string = d["string"].GetString();
		 cout <<string<<endl;

		
	 }else {
	 
		json_error_msg(json_op_name.c_str(),return_string);
		return -1;
	 }

	



	serial_op *  PTserial_op   =  (serial_op *) malloc(sizeof(serial_op));
	 if (PTserial_op == NULL){
		LOG_ERROR<<"malloc PTserial_op error";
		return  NULL;
	 }


	PTserial_op->device_name = (comharddevicePtr->device_name).c_str();
	LOG_INFO<<"PTserial_op->device_name ="<<PTserial_op->device_name;
	
	PTserial_op->data_bit=8;
	PTserial_op->parity = parity;
	PTserial_op->baud =  baudrate;
	PTserial_op->stop_bit = 1;
	
	 int string_len =  json_cmd_serial_op(comharddevicePtr,string,strlen(string),PTserial_op);
        if (string_len == -1 ){
		return NULL;
	}

	json_encode_cmd(string_len,return_string);




	return 0;

}


json_cmd::json_cmd()
{
	json_op_name = "cmd";
	LOG_INFO<<"json_cmd";
}

json_cmd::~json_cmd()
{
	LOG_INFO<<"~json_cmd";
}

void json_cmd_init(map <string,shared_ptr<json_op> >  &json_op_map)
{	
	shared_ptr<json_op> ptrjson_cmd (new json_cmd());
	RegisterJsonOp(ptrjson_cmd,json_op_map);
}


