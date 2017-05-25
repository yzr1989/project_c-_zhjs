#include <signal.h>
#include <time.h>
#include <pthread.h>  
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <boost/lexical_cast.hpp>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "config.h"
#include "utils.h"

#include "protocol_manager.h"
#include "struct_hardtype.h"
#include "protocol_ds_bus.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
//#include "struct_roominfo.h"

using namespace std;
using namespace boost;




struct str_time{
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
};

void protocol_ds_bus_handle_hd_register_length(uint32_t hd_register_length,  shared_ptr<harddevice>   PTharddevice ,string  & sql_cmd,int i)
{
	uint32_t   temp =0;
	int  signed_temp =0;
	uint64_t   temp64 =0;
	float  temp_f = 0.0;
	char temp_string[10];
	char temp_buffer[100];

	struct str_time  str_time;

	// here
	uint8_t *device_data =(uint8_t *) PTharddevice->device_data;

	int min_register = PTharddevice->gparamlist->min_addr_register;

	int ptr =  (PTharddevice->gparamlist->hdparamlists[i]->hdaddr_register)-min_register;

	LOG_INFO <<"ptr="<<ptr;		
	LOG_INFO <<"the register num="<<i;		
	LOG_INFO << "the register =" <<PTharddevice->gparamlist->hdparamlists[i]->hdaddr_register ;	
	LOG_INFO <<"hd_register_length="<<hd_register_length;
	
	switch(hd_register_length){
	case 1:
		temp =device_data[ptr];		
		sprintf(temp_buffer,"hd_register_length=%d hex=0x%x\t",hd_register_length,temp);
		LOG_TRACE<<temp_buffer;

		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype == "hex"){
			temp =temp/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);
			sprintf(temp_string,"%x",temp);
			cout <<temp_string<<endl;
			try {
				sql_cmd = sql_cmd + temp_string;
			}catch (bad_lexical_cast &e){
				LOG_ERROR<< "error:"<<e.what();
			}
			temp =0;
		}

		
		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype == "int"){
			temp =temp/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);
			try {
				sql_cmd = sql_cmd + lexical_cast<string>(temp);
			}catch (bad_lexical_cast &e){
				LOG_ERROR<< "error:"<<e.what();
			}
			temp =0;
		}
		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype == "float"){
			temp_f =(float)temp/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);
			try {
				sql_cmd = sql_cmd + lexical_cast<string> (temp_f);
			}catch (bad_lexical_cast &e){
				LOG_ERROR<< "error:"<<e.what();
			}
			temp_f = 0.0;
		}
		
		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype  == "bit"){
		
			sprintf(temp_buffer," hex=%d\t,temp=0x%x",temp,temp);
			LOG_TRACE<<temp_buffer;
			
			if ((PTharddevice->gparamlist->hdparamlists[i]->hd_bit_field).size() ==1){
				int bit_f  = lexical_cast<int> (PTharddevice->gparamlist->hdparamlists[i]->hd_bit_field[0]);
				temp =   (temp&(1<<bit_f))>>bit_f;
			}else if ((PTharddevice->gparamlist->hdparamlists[i]->hd_bit_field).size() ==2){
			int offset = lexical_cast<int>(PTharddevice->gparamlist->hdparamlists[i]->hd_bit_field[0])  - lexical_cast<int>(PTharddevice->gparamlist->hdparamlists[i]->hd_bit_field[1]);
			int offset_int =  pow(2,abs(offset)+1)-1;
			int bit_f = lexical_cast<int>(PTharddevice->gparamlist->hdparamlists[i]->hd_bit_field[1]);
			 temp =   (temp&(offset_int<<bit_f))>>bit_f;
			//cout  <<temp <<endl;
			}			

			sql_cmd  = sql_cmd + lexical_cast<string>(temp);
			temp = 0;
		}
		
		break;
	case 2:
		
		temp = (uint32_t)device_data[ptr+1]<<8|device_data[ptr];
	
		sprintf(temp_buffer,"hd_register_length=%d hex=0x%x\t%x\t 0x%x\t",hd_register_length,device_data[ptr],device_data[ptr+1],temp);
		LOG_TRACE<<temp_buffer;
		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype == "int"){
			temp =temp/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);	
			try{
				sql_cmd = sql_cmd + lexical_cast<string>(temp);
			}catch (bad_lexical_cast &e){
				LOG_ERROR<< "error:"<<e.what();
			}
			temp = 0;
		}
		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype == "float" ){
			temp_f =(float)temp/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);
			try{
				sql_cmd  =sql_cmd + lexical_cast<string>(temp_f);
			}catch (bad_lexical_cast &e){
				LOG_ERROR<< "error:"<<e.what();
			}
			temp_f =0.0;
		}	
	
		break;
	case 3:

			temp =(uint64_t)device_data[ptr];
			temp |=(uint64_t) device_data[ptr+1]<<8;
			temp |= (uint64_t) device_data[ptr+2]<<8*2;				
			temp  = temp/atol((char *)PTharddevice->gparamlist->hdparamlists[i]->hddatalen);			
			//sprintf("hd_register_length=%d\n",hd_register_length);
			LOG_TRACE<<"hd_register_length="<<hd_register_length;
			if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype =="int"){
				temp =temp/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);	
				try {
					sql_cmd  =sql_cmd + lexical_cast<string>(temp);
				}catch (bad_lexical_cast &e){
					//cout<< "error:"<<e.what()<<endl;
					LOG_ERROR<< "error:"<<e.what();
				}
				temp = 0;
			}
			if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype  == "float"){
				temp_f =(float)temp/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);
				try {
					sql_cmd   = sql_cmd + lexical_cast<string>(temp_f);
				}catch (bad_lexical_cast &e  ){
					//cout<< "error:"<<e.what()<<endl;
					LOG_ERROR<< "error:"<<e.what();
				}
				temp_f = 0.0;
				
			}	
		
		
		break;
	case 4:		
		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype == "int"){
			temp =(uint32_t)device_data[ptr];
			temp |=(uint32_t) device_data[ptr+1]<<8;
			temp |= (uint32_t) device_data[ptr+2]<<8*2;	
			temp |= (uint32_t) device_data[ptr+3]<<8*3;	
			sprintf(temp_buffer,"hd_register_length=%d hex=0x%x\t",hd_register_length,temp);
			LOG_TRACE<<temp_buffer;
			temp =temp/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);
			try {
				sql_cmd = sql_cmd + lexical_cast<string>(temp);
			}catch (bad_lexical_cast &e){
				//cout<< "error:"<<e.what()<<endl;
				LOG_ERROR<< "error:"<<e.what();
			}
			temp = 0;
		}

		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype == "float" ){
			temp =(uint32_t)device_data[ptr];
			temp |=(uint32_t) device_data[ptr+1]<<8;
			temp |= (uint32_t) device_data[ptr+2]<<8*2;	
			temp |= (uint32_t) device_data[ptr+3]<<8*3;			
			temp_f =(float)temp/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);
			try{
				sql_cmd  =sql_cmd + lexical_cast<string>(temp_f);
			}catch (bad_lexical_cast &e){
				LOG_ERROR<< "error:"<<e.what();
			}
			temp_f =0.0;
		}


		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype == "signed_float" ){
			signed_temp =(uint32_t)device_data[ptr];
			signed_temp |=(uint32_t) device_data[ptr+1]<<8;
			signed_temp |= (uint32_t) device_data[ptr+2]<<8*2;	
			signed_temp |= (uint32_t) device_data[ptr+3]<<8*3;			
			temp_f =(float)signed_temp/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);
			try{
				sql_cmd  =sql_cmd + lexical_cast<string>(temp_f);
			}catch (bad_lexical_cast &e){
				LOG_ERROR<< "error:"<<e.what();
			}
			temp_f =0.0;
		}

		
		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype  == "bcd"){
			temp =(uint32_t)bcd_code_2int(device_data[ptr]);
			temp +=(uint32_t)(bcd_code_2int(device_data[ptr+1]))*10000;
			temp +=((uint32_t) (bcd_code_2int(device_data[ptr+2])))*10000*10000;	
			temp +=((uint32_t) (bcd_code_2int(device_data[ptr+3])))*10000*10000*10000;				
			sprintf(temp_buffer,"bcd code =%ld\n",(long int)temp);
			LOG_TRACE<<temp_buffer;			
			sprintf(temp_string,"%u",temp);		
			LOG_TRACE<<temp_string;		
			sql_cmd  = sql_cmd + string(temp_string);
			temp = 0;
		}

	case 6:
		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype == "bcdtime"){

			sprintf(temp_buffer,"hex 0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t",device_data[ptr],
				device_data[ptr+1],device_data[ptr+2],device_data[ptr+3],device_data[ptr+4],
				device_data[ptr+5]);
			LOG_TRACE<<temp_buffer;	
			str_time.year =  bcd_code_l2int(device_data[ptr]);
			if (str_time.year  == 0){
				str_time.year = 1;
			}
			str_time.month=  bcd_code_l2int(device_data[ptr+1]);
			if (str_time.month == 0){
				str_time.month =1;
			}

			str_time.day=  bcd_code_l2int(device_data[ptr+2]);
			if (str_time.day ==0) {
				str_time.day = 1;				
			}
			
			str_time.hour=   bcd_code_l2int(device_data[ptr+3]);
			if (str_time.hour  ==0){
				str_time.hour =1 ;
			}

			str_time.min=  bcd_code_l2int(device_data[ptr+4]);
			if  (str_time.min ==0){
				str_time.min =1;	
			}	
			str_time.sec=   bcd_code_l2int(device_data[ptr+5]);
			if  (str_time.sec  ==0){
				str_time.sec =1 ;
			}
			sprintf(temp_buffer,"sec=%u\n",str_time.sec);		
			LOG_TRACE<<temp_buffer;
			
			sprintf(temp_string,"'20%d-%d-%d %d:%d:%d'",str_time.year,str_time.month,str_time.day,str_time.hour,str_time.min,str_time.sec);
			//puts(temp_string);
			LOG_TRACE<<temp_string;	
			sql_cmd = sql_cmd + string (temp_string);
			temp = 0;
		}
		break;

	case 8:
		temp64 =(uint64_t)device_data[ptr];
		temp64 |=(uint64_t) device_data[ptr+1]<<8;
		temp64 |= (uint64_t) device_data[ptr+2]<<8*2;	
		temp64 |= (uint64_t) device_data[ptr+3]<<8*3;
		temp64 |= (uint64_t) device_data[ptr+4]<<8*4;
		temp64 |= (uint64_t) device_data[ptr+5]<<8*5;
		temp64 |= (uint64_t) device_data[ptr+6]<<8*6;
		sprintf(temp_buffer,"hd_register_length=%d hex=0x%x\t",hd_register_length,(unsigned int)temp64);
		LOG_TRACE<<temp_buffer;	
		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype == "int"){

			temp64 =temp64/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);	
			try {
				sql_cmd = sql_cmd + lexical_cast<string>(temp);
			}catch (bad_lexical_cast &e){
				//cout<< "error:"<<e.what()<<endl;
				LOG_ERROR<< "error:"<<e.what();
			}
			temp = 0;
		}	
	default:
		sprintf(temp_buffer,"can't support this hd_register_length type %d\n",hd_register_length);
		LOG_TRACE<<temp_buffer;	
		sql_cmd  =sql_cmd + "0";
		
	}	
	
}



void protocol_ds_bus::handle_hd_register_length(uint32_t hd_register_length,  shared_ptr<harddevice>   PTharddevice ,string  & sql_cmd,int i)
{
	protocol_ds_bus_handle_hd_register_length(hd_register_length,PTharddevice,sql_cmd,i);
}

