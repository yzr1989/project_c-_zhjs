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
#include "protocol_modbus.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
//#include "struct_roominfo.h"

using  namespace std;
using namespace boost;

struct Serial_No{
	uint16_t temp1;
	uint16_t temp2;
	uint16_t temp3;
	uint16_t temp4;	
};


struct str_time{
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
};


void protocol_modbus_handle_hd_register_length(uint32_t hd_register_length,  shared_ptr<harddevice>   PTharddevice ,  string  & sql_cmd,int i)
{
	uint32_t   temp =0;
	uint64_t  temp64 =0;
	double  temp_f = 0.0f;
	char temp_string[10];
	char temp_buffer[100];
	struct str_time  str_time;

	//memset(temp_string,0,sizeof(temp_string));
	// hear 
	uint16_t *device_data =(uint16_t *) PTharddevice->device_data;

	int min_register = PTharddevice->gparamlist->min_addr_register;

	int ptr =  PTharddevice->gparamlist->hdparamlists[i]->hdaddr_register-min_register;

	
	LOG_TRACE << " ptr="<<ptr;	
	
	LOG_TRACE << " the register num="<<i  ;

	LOG_TRACE << " the register ="<<PTharddevice->gparamlist->hdparamlists[i]->hdaddr_register;
	switch(hd_register_length){
	case 1:
		temp =device_data[ptr];		
		
		sprintf(temp_buffer,"hd_register_length=%d hex=0x%x\t",hd_register_length,temp);
		LOG_TRACE<<temp_buffer;
				
		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype  == "int"){
			temp =temp/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);		
			sprintf(temp_buffer," int\ttemp=%u\n",temp);
			LOG_TRACE<<temp_buffer;				
			sql_cmd = sql_cmd + lexical_cast<string>(temp);			
			temp =0;
		}
		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype == "float"){
			temp_f = (float)temp / (PTharddevice->gparamlist->hdparamlists[i]->hddatalen);
		
			sprintf(temp_buffer," dobule\ttemp=%f\n",temp_f);
			LOG_TRACE<<temp_buffer;		
			sql_cmd = sql_cmd + lexical_cast<string>(temp_f);
			
			temp_f = 0.0f;
		}
		
		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype  == "bit"){
		
			sprintf(temp_buffer," hex=%d\t,temp=0x%x\n",temp,temp);
			LOG_TRACE<<temp_buffer;	
			
			if ((PTharddevice->gparamlist->hdparamlists[i]->hd_bit_field).size() ==1){

				int bit_f  = lexical_cast<int> (PTharddevice->gparamlist->hdparamlists[i]->hd_bit_field[0]);
		
				temp =   (temp&(1<<bit_f))>>bit_f;
				cout << temp<<endl;
				
			}else if ((PTharddevice->gparamlist->hdparamlists[i]->hd_bit_field).size() ==2){

			int offset = lexical_cast<int>(PTharddevice->gparamlist->hdparamlists[i]->hd_bit_field[0])  - lexical_cast<int>(PTharddevice->gparamlist->hdparamlists[i]->hd_bit_field[1]);
			int offset_int =  pow(2,abs(offset)+1)-1;
			int bit_f = lexical_cast<int>(PTharddevice->gparamlist->hdparamlists[i]->hd_bit_field[1]);
			 temp =   (temp&(offset_int<<bit_f))>>bit_f;
			cout  <<temp <<endl;
			}			

			sql_cmd  = sql_cmd + lexical_cast<string>(temp);
			temp = 0;
		}
		

		
		
		break;
	case 2:
		temp =device_data[ptr]<<16;
		temp |= device_data[ptr+1];
		sprintf(temp_buffer," hd_register_length=%d hex=0x%x\t",hd_register_length,temp);
		LOG_TRACE<<temp_buffer;		
		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype  =="int"){
			temp =temp/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);	
			sprintf(temp_buffer," int\ttemp=%d\n",temp);
			LOG_TRACE<<temp_buffer;			
			sql_cmd = sql_cmd + lexical_cast<string>(temp);
			temp = 0;
		}
		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype == "float"){
			temp_f =(double)temp/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);					
			sprintf(temp_buffer," dobule\ttemp=%lf\n",temp_f);
			LOG_TRACE<<temp_buffer;			
			sql_cmd = sql_cmd + lexical_cast<string>(temp_f);
			temp_f =0.0f;
		}	
	
		break;
	case 3:
		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype == "bcdtime"){

			sprintf(temp_buffer," hex 0x%x\t0x%x\t0x%x\t",device_data[ptr],
				device_data[ptr+1],device_data[ptr+2]);
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
			sprintf(temp_buffer," sec=%d\n",str_time.sec);		
			LOG_TRACE<<temp_buffer;					
			sprintf(temp_string,"' 20%d-%d-%d %d:%d:%d'",str_time.year,str_time.month,str_time.day,str_time.hour,str_time.min,str_time.sec);
		
			LOG_TRACE<<temp_string;
	
			sql_cmd = sql_cmd + string (temp_string);
	
			
			temp64 = 0;
			
		}else {
			temp64 =(uint64_t)device_data[ptr]<<16*2;
			temp64 |=(uint64_t) device_data[ptr+1]<<16;
			temp64 |= (uint64_t) device_data[ptr+2];				
			temp64  = temp64/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);			
			sprintf(temp_buffer,"hd_register_length=%d\n",hd_register_length);
			LOG_TRACE<<temp_buffer;		
			if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype == "int"){
				temp64 =temp64/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);					
				
				sprintf(temp_buffer,"int\ttemp=%llu\n",temp64);
				LOG_TRACE<<temp_buffer;		
				sql_cmd = sql_cmd + lexical_cast<string>(temp64);
				temp = 0;
			}
			if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype == "float"){
				temp_f =(float)temp/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);						
			
				sprintf(temp_buffer,"flaot\ttemp=%f\n",temp_f);
				LOG_TRACE<<temp_buffer;				
	
				sql_cmd = sql_cmd + lexical_cast<string>(temp64);
				temp_f = 0.0f;
			}	
		}
		
		break;
	case 4:
		
		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype == "bcd"){
			sprintf(temp_buffer,"device_data[ptr]=0x%x\t",device_data[ptr]);
				LOG_TRACE<<temp_buffer;
			temp64 =(uint64_t)bcd_code_2int(device_data[ptr])*10000*10000*10000;
			sprintf(temp_buffer,"device_data[ptr+1]=0x%x\t",device_data[ptr+1]);
				LOG_TRACE<<temp_buffer;
			temp64 +=(uint64_t) (bcd_code_2int(device_data[ptr+1]))*10000*10000;
			sprintf(temp_buffer,"device_data[ptr+2]=0x%x\t",device_data[ptr+2]);
				LOG_TRACE<<temp_buffer;
			temp64 +=(uint64_t) (bcd_code_2int(device_data[ptr+2]))*10000;
			sprintf(temp_buffer,"device_data[ptr+3]=0x%x\t",device_data[ptr+3]);
				LOG_TRACE<<temp_buffer;
			temp64 +=(uint64_t) (bcd_code_2int(device_data[ptr+3]));				
			sprintf(temp_buffer,"bcd code =%llu\n",temp64);
			LOG_TRACE<<temp_buffer;			
			LOG_TRACE<< "temp_string="<<temp_string ;			
			sql_cmd = sql_cmd + lexical_cast<string> (temp64);
			
			temp64 = 0;
		}	
					
		break;
	default:
		sprintf(temp_buffer,"can't support this hd_register_length type %d\n",hd_register_length);
		LOG_TRACE<<temp_buffer;
		sql_cmd  =sql_cmd + "0";
	}	

	
}


void protocol_modbus::handle_hd_register_length(uint32_t hd_register_length,  shared_ptr<harddevice>   PTharddevice ,  string  & sql_cmd,int i)
{
	protocol_modbus_handle_hd_register_length(hd_register_length,PTharddevice,sql_cmd,i);
}








