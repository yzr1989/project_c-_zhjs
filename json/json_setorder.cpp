
#include <stdio.h> 
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <time.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <sstream> 
#include <string>
#include <vector>
#include <boost/thread.hpp>
#include <boost/format.hpp>  
#include <boost/tokenizer.hpp> 
#include <boost/algorithm/string.hpp>  
#include <boost/date_time.hpp>

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
#include "serial_op.h"
#include "json_setorder.h"
#include "thread_object.h"
#include "sting2hex.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
#include "struct_roominfo.h"

using namespace std;	
using namespace boost; 
using namespace rapidjson;


extern list<shared_ptr<comharddevice> > g_comharddevice_list;

static int hex_to_bin(char ch)
{
	if ((ch >= '0') && (ch <= '9'))
		return ch - '0';
	ch = tolower(ch);
	if ((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	return 0;
}

static   int hex2bin_zero(const char  src[2])
{

	
	char   dst =0;
	//cout << src[1]<<endl;
	
	//printf("0x%x\n",src[1]);

	if (src[1] ==0){
		int lo = hex_to_bin(src[0]);	
		dst = lo;
	}else {			
	
		int hi = hex_to_bin(src[0]);
		int lo = hex_to_bin(src[1]);	
		dst = (hi << 4) | lo;
	}
	
	
	return dst;
}


vector<string>    parse_setorder( string   strTag  )
{
	
	boost::char_separator<char> sep(" ");  
	typedef boost::tokenizer<boost::char_separator<char> >  
	CustonTokenizer;  
	CustonTokenizer tok(strTag,sep);  
        vector<string> vecSegTag;  
        for(CustonTokenizer::iterator beg=tok.begin(); beg!=tok.end();++beg)  
        {  
            vecSegTag.push_back(*beg);  
        }  

	return vecSegTag;
}




char  *json_error_msg(const char *ordertype);

static 	uint8_t  hex_ret_buffer[1000];
static 	char  hex_dump_buffer[1000];



 static int   json_setorder_serial_op(shared_ptr<comharddevice>  comharddevicePtr, shared_ptr<harddevice> PTharddevice,char *buffer  , int datalen)
{
	int ret =0;
	int read_length=0;
	struct timeval tv; 
	 fd_set rdfds;  
	 int max_fd;
	bool setorder_flag = true;
	const   char *str = "timeout!\0";
	mutex::scoped_lock lock(comharddevicePtr->mutex_obj);	
	serial_opPtr PTserial_op   =  (serial_op *) malloc(sizeof(serial_op));
	 if (PTserial_op == NULL){
		printf("malloc PTserial_op error\n");
		return -1;
	 }

	PTserial_op->device_name = (comharddevicePtr->device_name).c_str();
	printf("PTserial_op->device_name =%s\n",PTserial_op->device_name );
	
	
	PTserial_op->data_bit=PTharddevice->data_bit;
	PTserial_op->parity = PTharddevice->parity;
	PTserial_op->baud =  PTharddevice->baudrate;
	PTserial_op->stop_bit = PTharddevice->stop_bit;

	cout <<"device_name="<<PTharddevice->device_name<<endl;
	cout <<"data_bit="<<PTharddevice->data_bit<<endl;
	cout <<"baud="<<PTharddevice->baudrate<<endl;
	cout <<"stop_bit="<<PTharddevice->stop_bit<<endl;	
	cout <<"parity="<<PTharddevice->parity<<endl;
	memset(hex_dump_buffer,0,sizeof(hex_dump_buffer));
	memset(hex_ret_buffer,0,sizeof(hex_ret_buffer));
	
	ret = open_serial(PTserial_op);
	if (ret){
		printf("open_serial error\n");
		return  -1;
	}
	
	ret = write(PTserial_op->fd,buffer,datalen);

	 FD_ZERO(&rdfds);  
        FD_SET(PTserial_op->fd,&rdfds);
	tv.tv_sec = 2;  
        tv.tv_usec = 0;  
	max_fd=PTserial_op->fd+1;

	while(setorder_flag){

	
        ret = select(max_fd,&rdfds,NULL,NULL,&tv);  
        if(ret<0) { 
              printf("selcet   error\n");
		close_serial(PTserial_op);
		free(PTserial_op);			  
		return -1;
        } else if(ret == 0){  
              printf("timeout \n");
		
		//memset(hex_dump_buffer,0,sizeof(hex_dump_buffer));
		//memcpy(hex_dump_buffer,str,strlen(str));
		
		setorder_flag = false;
        } else  {  
	
	//setorder_flag = true;
	
		 
	ret  = read(PTserial_op->fd,hex_ret_buffer+read_length,sizeof(hex_ret_buffer));
	printf("reading ret=%d\n",ret); 
	read_length += ret;
	
	
        }

	}
	
	printf("reading read_length=%d\n",read_length); 
	if (read_length == 0){
		memcpy(hex_dump_buffer,str,strlen(str));	
	}else {
		hex_dump_to_buffer(hex_ret_buffer, read_length,  hex_dump_buffer, sizeof(hex_dump_buffer));	

	}
	printf("hex_dump_buffer=%s\n",hex_dump_buffer);	
	close_serial(PTserial_op);
	free(PTserial_op);

	return 0;
}







int     json_setorder::json_encode_setorder(string & return_json_string)
{
	return 0;
}


 int      json_setorder::json_parse(const char *   json_string , shared_ptr<thread_object>  ptr_thread_object , string &return_string)
{

	 int  i;	
	LOG_INFO<<"json_parse_setorder";
	  Document d;
	  d.Parse(json_string);	
	string orderdata;
	vector<string>  vector_orderdata;
	string hdid;
	 if ( d.HasMember("orderdata") ) {

	 	 orderdata = d["orderdata"].GetString();			
		 cout <<"orderdata:"<<orderdata<<endl;	
		 vector_orderdata =   parse_setorder(orderdata);
		cout << "vector_orderdata size"<<vector_orderdata.size()<<endl; 
	 }

	

	if ( d.HasMember("hdid") ) {

 	 	hdid = d["hdid"].GetString();			
		 cout <<"hdid:"<<hdid<<endl;	
	}	



	shared_ptr<harddevice> PTharddevice=Getharddevice_byID(hdid,ptr_thread_object->harddevice_list);
	
	if (!PTharddevice){
		//printf("Getharddevice_byID error\n");
		LOG_INFO<<"Getharddevice_byID error";
		json_error_msg(json_op_name.c_str(),"Getharddevice_byID error",return_string);
		return -1;
	}	
	cout << "get PTharddevice ok"<<endl;
	cout << PTharddevice->hdid<<" "<<PTharddevice->hdcom<<endl;

	// here  wait serial thread init ok;	
	//condtion_wait_by_name(PTharddevice->hdcom,comharddevice_list);
	
	// 为了得到串口的互斥锁	
	shared_ptr<comharddevice>  comharddevicePtr=Gethdcomharddevice(PTharddevice->hdcom,g_comharddevice_list);
	if(!comharddevicePtr){
		//printf("Gethdcomharddevice_by_id error\n");
		//return NULL;
		LOG_INFO<<"Gethdcomharddevice_by_id error";
		json_error_msg(json_op_name.c_str(),"Gethdcomharddevice_by_id error",return_string);
		return -1;		
	}

	
	int data_len = vector_orderdata.size();
	char *hex_buffer= (char *) malloc(data_len);
	memset(hex_buffer,0,data_len);		
	
	char *buffer = (char *)malloc(sizeof(char)*data_len);

	memset(buffer,0,sizeof(sizeof(char)*data_len));	
	
	for (   i=0 ;i < data_len;i++){		
		buffer[i] = hex2bin_zero(vector_orderdata[i].c_str());
	}
	
	for (i=0;i<data_len;i++){

		printf("0x%x\t",buffer[i]);
	}

        puts("\n");

	//op here 

	 int ret =  json_setorder_serial_op(comharddevicePtr,PTharddevice,buffer,data_len);
        if (ret == -1 ){
		LOG_INFO<<"json_setorder_serial_op error";
		json_error_msg(json_op_name.c_str(),"json_setorder_serial_op error",return_string);
		return -1;	
	}

///json_encode_setorder
// 避免传参


	StringBuffer s;	
	
	Writer<StringBuffer> writer(s);
	writer.StartObject();  
	writer.Key("ordertype");   
	writer.String(json_op_name.c_str()); 
	writer.Key("orderdata");   
	writer.String(hex_dump_buffer); 
	writer.Key("hdid");   
	writer.String(hdid.c_str()); 	
	writer.EndObject();
	return_string= s.GetString();
	LOG_INFO<<return_string;

	free(buffer);
	free(hex_buffer);
	
	return 0;

}


json_setorder::json_setorder()
{
	json_op_name = "setorder";

	LOG_INFO<<"json_setorder";
}
	
json_setorder::~json_setorder()
{

}


void json_setorder_init(map <string,shared_ptr<json_op> >  &json_op_map)
{	
	shared_ptr<json_op> ptrjson_setorder (new json_setorder());
	RegisterJsonOp(ptrjson_setorder,json_op_map);
}


