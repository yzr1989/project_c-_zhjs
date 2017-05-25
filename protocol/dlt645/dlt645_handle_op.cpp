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
#include "protocol_dlt645.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
//#include "struct_roominfo.h"


using namespace std;
using namespace boost;

static int swap(int   a)
{
	int h = a>>4;
	int l = a&0xf;

	return  (l<<4)|h;
}


static int bcd2dec(int bcd)
{
	int h = bcd>>4;
	int l = bcd&0xf;
	return h*10+l;
}

 void    protocol_dlt645_handle_hd_register_length(uint32_t hd_register_length,  shared_ptr<harddevice>   PTharddevice ,string  & sql_cmd,int i)
{
	uint32_t   temp =0;
	uint64_t  temp64 =0;
	float   temp_f = 0.0;
	char temp_buffer[100];
	
	// here  
	uint8_t *device_data =(uint8_t *) PTharddevice->device_data;

	int min_register = PTharddevice->gparamlist->min_addr_register;

	int ptr =  (PTharddevice->gparamlist->hdparamlists[i]->hdaddr_register)-min_register;


	LOG_TRACE<<"ptr="  <<ptr;
	LOG_TRACE<< "the register num=" <<i ;
	LOG_TRACE<<  "the register ="<<PTharddevice->gparamlist->hdparamlists[i]->hdaddr_register;

	switch(hd_register_length){
	case 1:
		//为继电器状态而修正，需要高低字节颠倒
		
		temp =swap (device_data[ptr]);
	
		sprintf(temp_buffer,"device_data=%x",device_data[ptr]);
		LOG_TRACE<<temp_buffer;
		//temp  = swap(temp);
		
		sprintf(temp_buffer,"hd_register_length=%d hex=0x%x\t",hd_register_length,temp);
		LOG_TRACE<<temp_buffer;
			
		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype  == "int"){
			temp =temp/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);			
			try{
			sql_cmd = sql_cmd + lexical_cast<string>(temp);
			}catch ( bad_lexical_cast &e ){
				LOG_ERROR<< "error:"<<e.what();
			}
			temp =0;
		}		
		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype == "float"){
			temp_f =(float)temp/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);					

			try {
			sql_cmd = sql_cmd +  lexical_cast<string>(temp_f);
			} catch (bad_lexical_cast &e){
				LOG_ERROR<< "error:"<<e.what();
			}
			temp_f =0.0;
		}	
		
		
		
		break;
	case 2:
		temp =bcd2dec (device_data[ptr]);
		temp += bcd2dec (device_data[ptr+1])*100;

		sprintf(temp_buffer,"device_data[%d]=%d",ptr,bcd2dec(device_data[ptr]));
		LOG_TRACE<<temp_buffer;
		
		sprintf(temp_buffer,"device_data[%d]=%d",ptr+1,bcd2dec(device_data[ptr+1]));
		LOG_TRACE<<temp_buffer;
		
		LOG_TRACE<<"temp="<<temp;
		
		LOG_TRACE<<"hd_register_length="<<hd_register_length;
		
		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype =="int"){
			temp =temp/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);			
	
			try {
				sql_cmd  = sql_cmd + lexical_cast<string>(temp);
			}catch (bad_lexical_cast &e){
				LOG_ERROR<< "error:"<<e.what();
			}
			temp = 0;
		}
		if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype =="float"){
			temp_f =(float)temp/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);					

			try{
				sql_cmd = sql_cmd + lexical_cast<string>(temp_f);
			}catch (bad_lexical_cast &e){
				LOG_ERROR<< "error:"<<e.what();
			}
			temp_f =0.0;
		}	
	
		break;
	case 3:

			temp64 =(uint64_t) bcd2dec (device_data[ptr]);
			temp64 +=(uint64_t) bcd2dec (device_data[ptr+1])*100;
			temp64 += (uint64_t) bcd2dec (device_data[ptr+2])*10000;	
			
			sprintf(temp_buffer,"device_data[%d]=%d",ptr,bcd2dec(device_data[ptr]));
			LOG_TRACE<<temp_buffer;
			sprintf(temp_buffer,"device_data[%d]=%d",ptr+1,bcd2dec(device_data[ptr+1]));
			LOG_TRACE<<temp_buffer;
			sprintf(temp_buffer,"device_data[%d]=%d", ptr+1, bcd2dec(device_data[ptr+2]));
			LOG_TRACE<<temp_buffer;
			sprintf(temp_buffer,"temp64=%lld\n",temp64);
			LOG_TRACE<<temp_buffer;
			
					

			LOG_TRACE<<"hd_register_length="<<hd_register_length;
			
			if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype == "int"){
				temp64 =temp64/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);					

				try{
					sql_cmd = sql_cmd + lexical_cast<string> (temp);
				}catch (bad_lexical_cast &e){
		
					LOG_ERROR<< "error:"<<e.what();
				}
				temp = 0;
			}
			if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype == "float"){
				temp_f =(float)temp64/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);					

				try{
					sql_cmd = sql_cmd + lexical_cast<string>(temp_f);
				}catch (bad_lexical_cast &e ){		
					LOG_ERROR<< "error:"<<e.what();
				}
				temp_f =0.0;
			}		
		
		
		break;
	case 4:
		
			temp64 =(uint64_t)  bcd2dec (device_data[ptr]);
			temp64 +=(uint64_t) bcd2dec(device_data[ptr+1])*100;
			temp64 += (uint64_t) bcd2dec (device_data[ptr+2])*10000;	
			temp64 += (uint64_t) bcd2dec (device_data[ptr+3])*1000000;	
			
			sprintf(temp_buffer,"device_data[%d]=%x", ptr, device_data[ptr]);
			LOG_TRACE<<temp_buffer;			
			sprintf(temp_buffer,"device_data[%d]=%x",ptr+1,device_data[ptr+1]);
			LOG_TRACE<<temp_buffer;		
			sprintf(temp_buffer,"device_data[%d]=%x",ptr+2,device_data[ptr+2]);
			LOG_TRACE<<temp_buffer;		
			sprintf(temp_buffer,"device_data[%d]=%x",ptr+3,device_data[ptr+3]);
			LOG_TRACE<<temp_buffer;
			
			sprintf(temp_buffer,"device_data[%d]=%d",ptr,bcd2dec(device_data[ptr]));
			LOG_TRACE<<temp_buffer;			
			sprintf(temp_buffer,"device_data[%d]=%d",ptr+1,bcd2dec(device_data[ptr+1]));
			LOG_TRACE<<temp_buffer;			
			sprintf(temp_buffer,"device_data[%d]=%d",ptr+2,bcd2dec(device_data[ptr+2]));
			LOG_TRACE<<temp_buffer;		
			sprintf(temp_buffer,"device_data[%d]=%d",ptr+3,bcd2dec(device_data[ptr+3]));
			LOG_TRACE<<temp_buffer;		
			
			sprintf(temp_buffer,"temp64=%lld",temp64);
			LOG_TRACE<<temp_buffer;		

			LOG_TRACE<<"hd_register_length="<<hd_register_length;
			
			if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype =="int"){
				temp64 =temp64/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);					

				try{
					sql_cmd = sql_cmd + lexical_cast<string>(temp64);
				}catch (bad_lexical_cast &e){
					//cout<< "error:"<<e.what()<<endl;
					LOG_ERROR<<"error:"<<e.what();
				}
				temp = 0;
			}	
			if (PTharddevice->gparamlist->hdparamlists[i]->hddatatype =="float"){
				
				temp_f =(float)temp64/(PTharddevice->gparamlist->hdparamlists[i]->hddatalen);					
				try{
					sql_cmd = sql_cmd +lexical_cast<string>(temp_f);
				}catch (bad_lexical_cast &e){
					LOG_ERROR<<"error:"<<e.what();	
				}
				temp_f =0.0;
			}	
					
		break;
	default:
		LOG_ERROR<<"can't support this hd_register_length type "<<hd_register_length;
		
	}
	
}


void protocol_dlt645::handle_hd_register_length(uint32_t hd_register_length,  shared_ptr<harddevice>   PTharddevice ,string  & sql_cmd,int i)
{
	 protocol_dlt645_handle_hd_register_length(hd_register_length,PTharddevice,sql_cmd,i);
}





