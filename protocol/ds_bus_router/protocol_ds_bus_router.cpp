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
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <boost/lexical_cast.hpp>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>


#include "config.h"
#include "utils.h"
#include "ds_bus_router.h"
#include "protocol_manager.h"
#include "protocol_ds_bus_router.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
#include "struct_roominfo.h"


using namespace std;
using namespace boost;


protocol_ds_bus_router::protocol_ds_bus_router()
{
	protocol_name = "ds_bus_router";
	protocol_size = sizeof(uint8_t);	
	LOG_INFO<<"ds_bus_router";
}


protocol_ds_bus_router::~protocol_ds_bus_router()
{
	
}

int  protocol_ds_bus_router::protocol_init(shared_ptr<harddevice> Ptrhdharddevice)
{
	const int _RESPONSE_TIMEOUT_NEW =5 ;
	 struct timeval response_timeout;
	 const   char *ds_bus_device_name;	
	
	ds_bus_device_name = (Ptrhdharddevice->device_name).c_str();
   	Ptrhdharddevice->ctx= ds_bus_router_new(ds_bus_device_name, Ptrhdharddevice->baudrate, Ptrhdharddevice->parity, Ptrhdharddevice->data_bit, Ptrhdharddevice->stop_bit);
  	  if (  Ptrhdharddevice->ctx == NULL) {
		LOG_ERROR<<"ds_bus_router Error:Unable to allocate ds_bus_router context";
	   return -1;
   	 }

    ds_bus_router_set_debug( (ds_bus_router_t *)Ptrhdharddevice->ctx , TRUE);

    if (ds_bus_router_connect( (ds_bus_router_t *)Ptrhdharddevice->ctx ) == -1) {
		LOG_ERROR<<"ds_bus_router Error:Connection failed:" <<ds_bus_router_strerror(errno);
		return -1;
    }
	
    /* Define a new and too short timeout */
    response_timeout.tv_sec = _RESPONSE_TIMEOUT_NEW;
    response_timeout.tv_usec = 0;
    ds_bus_router_set_response_timeout((ds_bus_router_t *)Ptrhdharddevice->ctx, &response_timeout);
    ds_bus_router_set_byte_timeout( (ds_bus_router_t *)Ptrhdharddevice->ctx, &response_timeout);


	return 0;
		
}

int protocol_ds_bus_router::protocol_set_slave( shared_ptr<harddevice> Ptrhdharddevice)
{
	int server_id =0;	

	server_id = lexical_cast<int >(Ptrhdharddevice->hdaddr);
	
	LOG_INFO<<"server id ="<<server_id;	
	ds_bus_router_set_slave_For_File_Record((ds_bus_router_t *)Ptrhdharddevice->ctx,server_id);

	return 0;
}


int protocol_ds_bus_router::protocol_read_registers(shared_ptr<harddevice> Ptrhdharddevice )
{
	int rc =0;	

 	File_Record    *File_Record_r[1];
	ds_router_list *ptr_ds_router_list;
	
	int list_num = Ptrhdharddevice->list.size();
	uint16_t * list = (uint16_t *)malloc(sizeof(uint16_t)*list_num);
	if (list== NULL){
		LOG_ERROR<<"malloc list error";
		return -1;
	}
	
	File_Record_r[0]  =(File_Record  *)malloc(sizeof(File_Record )+4);	

	for (int i=0;i<list_num;i++){
	try{
		list[i] = lexical_cast<uint16_t> (Ptrhdharddevice->list[i]);
		cout<<list[i]<<endl;
		}catch (bad_lexical_cast &e){
			LOG_ERROR<<"error:"<<e.what();
		}
	}

	ptr_ds_router_list  = (ds_router_list *)malloc (sizeof(ds_router_list));
	if (ptr_ds_router_list == NULL){
		LOG_ERROR<<"malloc ptr_dlt645_router_list error";
		return -1;
	}

	ptr_ds_router_list->address = 0xfb;
	ptr_ds_router_list->function_no = 0x78;

/*

	if (list_num == 1){
		ptr_ds_router_list->recvicer_address = list[0];
	}else {
		ptr_ds_router_list->recvicer_address = list[1];
	}	
*/

	ptr_ds_router_list->recvicer_address = list[0];

	ptr_ds_router_list->router_list= list;
	ptr_ds_router_list->router_num = list_num;
	ptr_ds_router_list->router_length = ptr_ds_router_list->router_num +  4;	

	File_Record_r[0]->file_no = Ptrhdharddevice->file_no;
	File_Record_r[0]->register_address = Ptrhdharddevice->gparamlist->min_addr_register;
        File_Record_r[0]->register_num = Ptrhdharddevice->gparamlist->addr_register_num;
	File_Record_r[0]->data =(uint8_t*) Ptrhdharddevice->device_data;

	rc = ds_bus_router_File_Record_read_send((ds_bus_router_t *)Ptrhdharddevice->ctx,ptr_ds_router_list
					,File_Record_r ,1);
	

	free(list);
	free(ptr_ds_router_list);	
	free(File_Record_r[0]);
	return rc;	
}


int protocol_ds_bus_router::protocol_write_registers(shared_ptr<harddevice> Ptrhdharddevice)
{

	ds_router_list *ptr_ds_router_list;
 	File_Record    *File_Record_w[1];
 	
        struct timeval response_timeout;	

	File_Record_w[0] =(File_Record  *)malloc(sizeof(File_Record)+4);
	int list_num = Ptrhdharddevice->list.size();


	//uint16_t  *list  = new uint16_t[list_num];
	
	uint16_t * list = (uint16_t *)malloc(sizeof(uint16_t)*list_num);
	if (list== NULL){
		//printf("malloc list error\n");
		LOG_ERROR<<"malloc list error";
		return -1;
	}

 	for (int i=0;i<list_num;i++){
		try{
			list[i] = lexical_cast<uint16_t>(Ptrhdharddevice->list[i]);
		}catch (bad_lexical_cast &e){
			//cout<<"error:"<<e.what()<<endl;
			LOG_ERROR<<"error:"<<e.what();
		}
	}  


	ptr_ds_router_list  = (ds_router_list *)malloc (sizeof(ds_router_list));
	if (ptr_ds_router_list == NULL){
		//printf("malloc ptr_dlt645_router_list error \n");
		LOG_ERROR<<"malloc ptr_dlt645_router_list error";
		return -1;
	}	
	ptr_ds_router_list->address = 0xfb;
	ptr_ds_router_list->function_no = 0x78;
	ptr_ds_router_list->recvicer_address = list[0];
	ptr_ds_router_list->router_list= list;
	ptr_ds_router_list->router_num = list_num;
	ptr_ds_router_list->router_length = ptr_ds_router_list->router_num +  4;	
	
	File_Record_w[0]->file_no = Ptrhdharddevice->file_no;
	File_Record_w[0]->register_address = Ptrhdharddevice->gparamlist->min_addr_register;
        File_Record_w[0]->register_num = Ptrhdharddevice->gparamlist->addr_register_num;
	File_Record_w[0]->data =(uint8_t*) Ptrhdharddevice->device_data;
	
    /* Define a new and too short timeout */
    response_timeout.tv_sec = 5;
    response_timeout.tv_usec = 0;
    ds_bus_router_set_response_timeout( (ds_bus_router_t *)Ptrhdharddevice->ctx, &response_timeout);

    ds_bus_router_set_byte_timeout((ds_bus_router_t *)Ptrhdharddevice->ctx, &response_timeout);

  int  rc = ds_bus_router_File_Record_write_send((ds_bus_router_t *)Ptrhdharddevice->ctx,
		ptr_ds_router_list,	File_Record_w ,1);
   
	free(list);
	free(ptr_ds_router_list);
	free(File_Record_w[0]);
	
   	return rc;
}



int protocol_ds_bus_router::switch_off(shared_ptr<harddevice> Ptrhdharddevice  , int port_num)
{

	File_Record    *File_Record_w[1];
	ds_router_list *ptr_ds_router_list;
	uint8_t register_address_list[] = {11,12,13,14};
	uint8_t device_data[] = {0xc1};
	int list_num = Ptrhdharddevice->list.size();
	uint16_t * list = (uint16_t *)malloc(sizeof(uint16_t)*list_num);
	if (list== NULL){
		//printf("malloc list error\n");
		LOG_ERROR<<"malloc list error";
		return -1;
	}

 	for (int i=0;i<list_num;i++){
		try{
			list[i] = lexical_cast<uint16_t>(Ptrhdharddevice->list[i]);
		}catch (bad_lexical_cast &e){
			//cout<<"error:"<<e.what()<<endl;
			LOG_ERROR<<"error:"<<e.what();
		}
	}  

	ptr_ds_router_list  = (ds_router_list *)malloc (sizeof(ds_router_list));
	if (ptr_ds_router_list == NULL){
		//printf("malloc ptr_dlt645_router_list error \n");
		LOG_ERROR<<"malloc ptr_dlt645_router_list error ";
		return -1;
	}	
	ptr_ds_router_list->address = 0xfb;
	ptr_ds_router_list->function_no = 0x78;
	ptr_ds_router_list->recvicer_address = list[0];
	ptr_ds_router_list->router_list= list;
	ptr_ds_router_list->router_num = list_num;
	ptr_ds_router_list->router_length = ptr_ds_router_list->router_num +  4;
	File_Record_w[0] = (File_Record    *)malloc(sizeof(File_Record)+4);	
	File_Record_w[0]->file_no = 10200;
	File_Record_w[0]->register_address =  register_address_list[port_num-1];
       File_Record_w[0]->register_num =  sizeof(device_data);
	File_Record_w[0]->data =(uint8_t*)  malloc(sizeof(device_data));
	memcpy(File_Record_w[0]->data , device_data,sizeof(device_data));

 	 int  rc = ds_bus_router_File_Record_write_send((ds_bus_router_t *)Ptrhdharddevice->ctx,
		ptr_ds_router_list,	File_Record_w ,1);
	
	free(list);
	free(ptr_ds_router_list);
	free(File_Record_w[0]);	

	return rc;
	
}


struct touch_switch_reg{
	uint8_t sw_ctl1;
	uint8_t sw_ctl2;
	uint8_t sw_ctl3;
	uint8_t sw_ctl4;	
};	


#if 0

int protocol_ds_bus_router::permission_led(shared_ptr<harddevice> Ptrhdharddevice  , int op)
{

	/*¶Á*/	

	int rc =0;	
 	File_Record    *File_Record_r[1];
	ds_router_list *ptr_ds_router_list;	
	int list_num = Ptrhdharddevice->list.size();
	uint16_t * list = (uint16_t *)malloc(sizeof(uint16_t)*list_num);
	if (list== NULL){
		LOG_ERROR<<"malloc list error";
		return -1;
	}

	int register_address = 11;
	int register_num = 4 ;

	uint8_t *device_data = (uint8_t  *)malloc(sizeof(uint8_t)*register_num);
	if (device_data == NULL){
		LOG_ERROR<<"malloc list error";
		return -1;
	}
	
	File_Record_r[0]  =(File_Record  *)malloc(sizeof(File_Record )+4);	
	for (int i=0;i<list_num;i++){
	try{
		list[i] = lexical_cast<uint16_t> (Ptrhdharddevice->list[i]);
		cout<<list[i]<<endl;
		}catch (bad_lexical_cast &e){
			LOG_ERROR<<"error:"<<e.what();
		}
	}

	ptr_ds_router_list  = (ds_router_list *)malloc (sizeof(ds_router_list));
	if (ptr_ds_router_list == NULL){
		//printf("malloc ptr_dlt645_router_list error \n");
		LOG_ERROR<<"malloc ptr_dlt645_router_list error ";
		return -1;
	}
	
	ptr_ds_router_list->address = 0xfb;
	ptr_ds_router_list->function_no = 0x78;
	ptr_ds_router_list->recvicer_address = list[0];
	ptr_ds_router_list->router_list= list;
	ptr_ds_router_list->router_num = list_num;
	ptr_ds_router_list->router_length = ptr_ds_router_list->router_num +  4;	

	File_Record_r[0]->file_no = Ptrhdharddevice->file_no;
	File_Record_r[0]->register_address = register_address;
	File_Record_r[0]->register_num = register_num;
	File_Record_r[0]->data =device_data;

	rc = ds_bus_router_File_Record_read_send((ds_bus_router_t *)Ptrhdharddevice->ctx,ptr_ds_router_list
					,File_Record_r ,1);

	if (rc  == -1){
		free(device_data);
		free(list);
		free(ptr_ds_router_list);		
		free(File_Record_r[0]);	
		return -1;
	}


	struct touch_switch_reg *ptouch_switch_reg = (struct touch_switch_reg *)device_data;
	printf("sw_ctl1=0x%x\n",ptouch_switch_reg->sw_ctl1);	
	printf("sw_ctl2=0x%x\n",ptouch_switch_reg->sw_ctl2);	
	printf("sw_ctl3=0x%x\n",ptouch_switch_reg->sw_ctl3);	
	printf("sw_ctl4=0x%x\n",ptouch_switch_reg->sw_ctl4);
	
	/*
		bit6   0    µãÁÁ ½ûÖ¹µÆ
		bit6   1    Ï¨Ãð ½ûÖ¹µÆ
	*/

	if ( op  == ON){

	//  ¿ª½ûÖ¹µÆ  bit6 Ð´0


	ptouch_switch_reg->sw_ctl1 &=  ~(1<<6);
	ptouch_switch_reg->sw_ctl2 &=  ~(1<<6);
	ptouch_switch_reg->sw_ctl3 &=  ~(1<<6);
	ptouch_switch_reg->sw_ctl4 &=  ~(1<<6);

	}else if ( op == OFF){

	//  ¹Ø½ûÖ¹µÆ  bit6 Ð´1


	ptouch_switch_reg->sw_ctl1 |=  (1<<6);
	ptouch_switch_reg->sw_ctl2 |=  (1<<6);
	ptouch_switch_reg->sw_ctl3 |=  (1<<6);
	ptouch_switch_reg->sw_ctl4 |=  (1<<6);


	}
	
	printf("sw_ctl1=0x%x\n",ptouch_switch_reg->sw_ctl1);	
	printf("sw_ctl2=0x%x\n",ptouch_switch_reg->sw_ctl2);	
	printf("sw_ctl3=0x%x\n",ptouch_switch_reg->sw_ctl3);	
	printf("sw_ctl4=0x%x\n",ptouch_switch_reg->sw_ctl4);	


	File_Record    *File_Record_w[1];	


	 list_num = Ptrhdharddevice->list.size();
	list = (uint16_t *)malloc(sizeof(uint16_t)*list_num);
	if (list== NULL){		
		LOG_ERROR<<"malloc list error";
		return -1;
	}



 	for (int i=0;i<list_num;i++){
		try{
			list[i] = lexical_cast<uint16_t>(Ptrhdharddevice->list[i]);
		}catch (bad_lexical_cast &e){			
			LOG_ERROR<<"error:"<<e.what();
		}
	}  

	ptr_ds_router_list  = (ds_router_list *)malloc (sizeof(ds_router_list));
	if (ptr_ds_router_list == NULL){
		//printf("malloc ptr_dlt645_router_list error \n");
		LOG_ERROR<<"malloc ptr_dlt645_router_list error ";
		return -1;
	}	
	ptr_ds_router_list->address = 0xfb;
	ptr_ds_router_list->function_no = 0x78;
	ptr_ds_router_list->recvicer_address = list[0];
	ptr_ds_router_list->router_list= list;
	ptr_ds_router_list->router_num = list_num;
	ptr_ds_router_list->router_length = ptr_ds_router_list->router_num +  4;	
	

	File_Record_w[0] = (File_Record    *)malloc(sizeof(File_Record)+4);	
	File_Record_w[0]->file_no = 10200;
	File_Record_w[0]->register_address =  register_address;
       File_Record_w[0]->register_num =  register_num;
	File_Record_w[0]->data =device_data;


	memcpy(File_Record_w[0]->data , device_data,sizeof(device_data));
	

	// ds_bus_File_Record_write_send((ds_bus_t *) Ptrhdharddevice->ctx,
	//	File_Record_w ,1);


 	  rc = ds_bus_router_File_Record_write_send((ds_bus_router_t *)Ptrhdharddevice->ctx,
		ptr_ds_router_list,	File_Record_w ,1);
	
	free(list);
	free(ptr_ds_router_list);
	free(File_Record_w[0]);	
	free(File_Record_r[0]);
	free(device_data);

	return  0 ;
		
}


#endif 

int protocol_ds_bus_router::permission_led_clear(shared_ptr<harddevice> Ptrhdharddevice )
{


	/*
		bit6   0    µãÁÁ ½ûÖ¹µÆ 0x0
		bit6   1    Ï¨Ãð ½ûÖ¹µÆ0x40
	*/

	File_Record    *File_Record_w[1];
	ds_router_list *ptr_ds_router_list;
	File_Record_w[0] =(File_Record  *)malloc(sizeof(File_Record)+4);
	int list_num = Ptrhdharddevice->list.size();


	//uint16_t  *list  = new uint16_t[list_num];
	
	uint16_t * list = (uint16_t *)malloc(sizeof(uint16_t)*list_num);
	if (list== NULL){
		printf("malloc list error\n");
		return -1;
	}

 	for (int i=0;i<list_num;i++){
		try{
			list[i] = lexical_cast<uint16_t>(Ptrhdharddevice->list[i]);
		}catch (bad_lexical_cast &e){
			cout<<"error:"<<e.what()<<endl;
		}
	}  


	ptr_ds_router_list  = (ds_router_list *)malloc (sizeof(ds_router_list));
	if (ptr_ds_router_list == NULL){
		printf("malloc ptr_dlt645_router_list error \n");
		return -1;
	}	
	ptr_ds_router_list->address = 0xfb;
	ptr_ds_router_list->function_no = 0x78;
	ptr_ds_router_list->recvicer_address = list[0];
	ptr_ds_router_list->router_list= list;
	ptr_ds_router_list->router_num = list_num;
	ptr_ds_router_list->router_length = ptr_ds_router_list->router_num +  4;	
	
	
	uint8_t device_data[] = {0x40,0x40,0x40,0x40};

	File_Record_w[0] = (File_Record    *)malloc(sizeof(File_Record)+4);
	
	File_Record_w[0]->file_no = 10200;
	File_Record_w[0]->register_address =  11;
       File_Record_w[0]->register_num =  sizeof(device_data);
	File_Record_w[0]->data =(uint8_t*)  malloc(sizeof(device_data));
	memcpy(File_Record_w[0]->data , device_data,sizeof(device_data));	
	/*

 	 int  rc = ds_bus_router_File_Record_write_send((ds_bus_router_t *)Ptrhdharddevice->ctx,
		ptr_ds_router_list,	File_Record_w ,1);
	 */
	ds_bus_router_File_Record_write_send((ds_bus_router_t *)Ptrhdharddevice->ctx,
		ptr_ds_router_list,	File_Record_w ,1);
	
	free(list);
	free(ptr_ds_router_list);
	free(File_Record_w[0]);	


	return 0;
}




int  protocol_ds_bus_router::relay_off(shared_ptr<harddevice> Ptrhdharddevice)
{

	File_Record    *File_Record_w[1];
	ds_router_list *ptr_ds_router_list;
	File_Record_w[0] =(File_Record  *)malloc(sizeof(File_Record)+4);
	int list_num = Ptrhdharddevice->list.size();


	//uint16_t  *list  = new uint16_t[list_num];
	
	uint16_t * list = (uint16_t *)malloc(sizeof(uint16_t)*list_num);
	if (list== NULL){
		printf("malloc list error\n");
		return -1;
	}

 	for (int i=0;i<list_num;i++){
		try{
			list[i] = lexical_cast<uint16_t>(Ptrhdharddevice->list[i]);
		}catch (bad_lexical_cast &e){
			cout<<"error:"<<e.what()<<endl;
		}
	}  


	ptr_ds_router_list  = (ds_router_list *)malloc (sizeof(ds_router_list));
	if (ptr_ds_router_list == NULL){
		printf("malloc ptr_dlt645_router_list error \n");
		return -1;
	}	
	ptr_ds_router_list->address = 0xfb;
	ptr_ds_router_list->function_no = 0x78;
	ptr_ds_router_list->recvicer_address = list[0];
	ptr_ds_router_list->router_list= list;
	ptr_ds_router_list->router_num = list_num;
	ptr_ds_router_list->router_length = ptr_ds_router_list->router_num +  4;	
	uint8_t device_data[] = {0x1,0x1,0x1,0x1};
	File_Record_w[0] = (File_Record    *)malloc(sizeof(File_Record)+4);	
	File_Record_w[0]->file_no = 10200;
	File_Record_w[0]->register_address =  11;
       File_Record_w[0]->register_num =  sizeof(device_data);
	File_Record_w[0]->data =(uint8_t*)  malloc(sizeof(device_data));
	memcpy(File_Record_w[0]->data , device_data,sizeof(device_data));
	
	/*

 	 int  rc = ds_bus_router_File_Record_write_send((ds_bus_router_t *)Ptrhdharddevice->ctx,
		ptr_ds_router_list,	File_Record_w ,1);
	 */
	ds_bus_router_File_Record_write_send((ds_bus_router_t *)Ptrhdharddevice->ctx,
		ptr_ds_router_list,	File_Record_w ,1);
	
	free(list);
	free(ptr_ds_router_list);
	free(File_Record_w[0]);	

	return 0;	
}


int protocol_ds_bus_router::protocol_send_ir_cmd(shared_ptr<roominfo> PTroominfo,int i)
{
	int rc ;
	/*  ir write cmd*/

	uint8_t *buffer ; 
	File_Record    *File_Record_w[1];
	File_Record_w[0] =(File_Record  *)malloc(sizeof(File_Record)+4);	
	int list_num = PTroominfo->harddevice_list[i]->list.size();
	uint16_t * list = (uint16_t *)malloc(sizeof(uint16_t)*list_num);
	if (list== NULL){
		printf("malloc list error\n");
		return -1;
	}	

 	for (int j=0;j<list_num;j++){
		try{
			list[j] = lexical_cast<uint16_t>(PTroominfo->harddevice_list[i]->list[j]);
		}catch (bad_lexical_cast &e){
			cout<<"error:"<<e.what()<<endl;
		}
	}  	
	
	ds_router_list *ptr_ds_router_list;
        struct timeval response_timeout;
	printf("list_num=%d\n",PTroominfo->harddevice_list[i]->list_num);

	 for (int j=0;j<list_num;j++){
		printf("list[%d]=%d  ",j,list[j]);
 	}

   	puts("\r\n");
	ptr_ds_router_list  = (ds_router_list *)malloc (sizeof(ds_router_list));
	if (ptr_ds_router_list == NULL){
		printf("malloc ptr_dlt645_router_list error \n");
		free(File_Record_w[0]);	
		free(list);
		return -1;
	}	
	ptr_ds_router_list->address = 0xfb;
	ptr_ds_router_list->function_no =  0x78;	
	ptr_ds_router_list->recvicer_address = list[0];
	ptr_ds_router_list->router_list= list;
	ptr_ds_router_list->router_num =PTroominfo->harddevice_list[i]->list_num;
	ptr_ds_router_list->router_length = ptr_ds_router_list->router_num +  4;	
	
    /* Define a new and too short timeout */
    response_timeout.tv_sec = 5;
    response_timeout.tv_usec = 0;
    ds_bus_router_set_response_timeout( (ds_bus_router_t*)PTroominfo->harddevice_list[i]->ctx, &response_timeout);

	buffer = (uint8_t *)malloc(PTroominfo->airckz_hex_length+2);
	if(buffer == NULL){
		printf("buffer malloc\n");
		free(File_Record_w[0]);	
		free(ptr_ds_router_list);
		free(list);
		return -1;
	}
	
	File_Record_w[0]->file_no = 10500;
	File_Record_w[0]->register_address = 0;
       File_Record_w[0]->register_num = PTroominfo->airckz_hex_length+2;
	buffer[0] = 0x01;
	buffer[1] = 0x01;
	memcpy(buffer+2,PTroominfo->airckz_hex_buffer,PTroominfo->airckz_hex_length);	
	File_Record_w[0]->data = buffer;	
	rc = ds_bus_router_File_Record_IR_write_send((ds_bus_router_t*)PTroominfo->harddevice_list[i]->ctx,ptr_ds_router_list,File_Record_w ,1);	
	free(buffer);	
	free(list);
	free(ptr_ds_router_list);
	free(File_Record_w[0]);	
	return rc;



}


int protocol_ds_bus_router::protocol_close(shared_ptr<harddevice> Ptrhdharddevice )
{

	if ((ds_bus_router_t *)Ptrhdharddevice->ctx != NULL){

		ds_bus_router_close((ds_bus_router_t *)Ptrhdharddevice->ctx);
		ds_bus_router_free((ds_bus_router_t *)Ptrhdharddevice->ctx );
	}
	
	
	return 0;


}






void protocol_ds_bus_router_init(list<shared_ptr<protocol_manager> > &protocol_manager_list)
{
	shared_ptr<protocol_manager> ptr_protocol_ds_bus_router (new  protocol_ds_bus_router());	
	Registerprotocol(ptr_protocol_ds_bus_router,protocol_manager_list);
}




