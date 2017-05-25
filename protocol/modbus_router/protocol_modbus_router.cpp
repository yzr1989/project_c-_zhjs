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
#include <iostream>
#include <boost/lexical_cast.hpp>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "config.h"
#include "utils.h"
#include "protocol_manager.h"
#include "protocol_modbus_router.h"
#include "modbus_router.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
#include "struct_roominfo.h"

using namespace std;
using namespace boost;




protocol_modbus_router::protocol_modbus_router()
{	
	protocol_name = "modbus_router";
	protocol_size = sizeof(uint16_t);
	LOG_INFO<<"modbus_router";
	
}

protocol_modbus_router::~protocol_modbus_router()
{
	
}

int protocol_modbus_router::protocol_init(shared_ptr<harddevice> Ptrhdharddevice)
{	

	const int _RESPONSE_TIMEOUT_NEW =5 ;
	struct timeval response_timeout;
    const char * modbus_router_device_name =( Ptrhdharddevice->device_name).c_str();
      Ptrhdharddevice->ctx = modbus_router_new(modbus_router_device_name, Ptrhdharddevice->baudrate,Ptrhdharddevice->parity ,Ptrhdharddevice->data_bit,Ptrhdharddevice->stop_bit);
  	  if (Ptrhdharddevice->ctx == NULL) {
   	     LOG_ERROR<<"libmodbus_router Error:Unable to allocate libmodbus_router context";
    	    	return -1;
   	 }

	
    modbus_router_set_debug((modbus_router_t *)Ptrhdharddevice->ctx , TRUE);


    if (modbus_router_connect((modbus_router_t *)Ptrhdharddevice->ctx ) == -1) {
      	LOG_ERROR<<"ibmodbus_router Error:Connection failed:" << modbus_router_strerror(errno);
       	return -1;	
    }



    /* Define a new and too short timeout */
    response_timeout.tv_sec = _RESPONSE_TIMEOUT_NEW;
    response_timeout.tv_usec = 0;
    modbus_router_set_response_timeout((modbus_router_t *)Ptrhdharddevice->ctx, &response_timeout);
    modbus_router_set_byte_timeout((modbus_router_t *)Ptrhdharddevice->ctx, &response_timeout);
	return 0;
}

int protocol_modbus_router::protocol_set_slave( shared_ptr<harddevice> Ptrhdharddevice)
{

	int server_id;	
	
	try{
	server_id = lexical_cast<int>(Ptrhdharddevice->hdaddr);
	} catch ( bad_lexical_cast &e){
		LOG_ERROR<< "error"<<e.what();
		return -1;
	}	
	LOG_INFO<<"server id ="<<server_id;

	modbus_router_set_slave((modbus_router_t *)Ptrhdharddevice->ctx,server_id);

	return 0;
}

int protocol_modbus_router::protocol_read_registers(shared_ptr<harddevice> Ptrhdharddevice )
{

	int rc =0;
	modbus_router_list  *ptr_modbus_router_list ;
	int list_num =Ptrhdharddevice->list.size();


	//uint16_t  *list  = new uint16_t[list_num];
	uint16_t  *list  =(uint16_t *) malloc(sizeof(uint16_t)*list_num);
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
	
	ptr_modbus_router_list  =(modbus_router_list  *) malloc (sizeof(modbus_router_list));
	if (ptr_modbus_router_list == NULL){
		printf("malloc ptr_dlt645_router_list error \n");
		return -1;
	}

	ptr_modbus_router_list->address = 0xfb;
	ptr_modbus_router_list->function_no = 0x78;

/*

	if (list_num == 1){
		ptr_modbus_router_list->recvicer_address = list[0];
	}else {
		ptr_modbus_router_list->recvicer_address = list[1];
	}	
*/

	ptr_modbus_router_list->recvicer_address = list[0];

	ptr_modbus_router_list->router_list= list;
	ptr_modbus_router_list->router_num = list_num;
	ptr_modbus_router_list->router_length = ptr_modbus_router_list->router_num +  4;	
	
	
	rc = modbus_router_read_registers((modbus_router_t *)Ptrhdharddevice->ctx,
				Ptrhdharddevice->gparamlist->min_addr_register,
				Ptrhdharddevice->gparamlist->addr_register_num,
				(uint16_t *)Ptrhdharddevice->device_data,ptr_modbus_router_list);	

	free(list );
	free(ptr_modbus_router_list);
	return rc;	


}

int protocol_modbus_router::protocol_write_registers(shared_ptr<harddevice> Ptrhdharddevice)
{
	int rc =0;
	modbus_router_list  *ptr_modbus_router_list ;
	int list_num = Ptrhdharddevice->list.size();


	//uint16_t  *list  = new uint16_t[list_num];
	uint16_t  *list  = (uint16_t *)malloc(sizeof(uint16_t)*list_num);
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
	
	ptr_modbus_router_list  = (modbus_router_list  *)malloc (sizeof(modbus_router_list));
	if (ptr_modbus_router_list == NULL){
		printf("malloc ptr_dlt645_router_list error \n");
		return -1;
	}

	ptr_modbus_router_list->address = 0xfb;
	ptr_modbus_router_list->function_no = 0x78;

	if (list_num == 1){
		ptr_modbus_router_list->recvicer_address = list[0];
	}else {
		ptr_modbus_router_list->recvicer_address = list[1];
	}	
	
	ptr_modbus_router_list->router_list= list;
	ptr_modbus_router_list->router_num = list_num;
	ptr_modbus_router_list->router_length = ptr_modbus_router_list->router_num +  4;		
	rc = modbus_router_write_registers((modbus_router_t *)Ptrhdharddevice->ctx,
				Ptrhdharddevice->gparamlist->min_addr_register,
				Ptrhdharddevice->gparamlist->addr_register_num,
				(uint16_t *)Ptrhdharddevice->device_data,ptr_modbus_router_list);	

	free(list );
	free(ptr_modbus_router_list);
	
	return rc;	
}

int protocol_modbus_router::protocol_close(shared_ptr<harddevice> Ptrhdharddevice )
{

	if ((modbus_router_t *)Ptrhdharddevice->ctx != NULL){
		modbus_router_close((modbus_router_t *)Ptrhdharddevice->ctx);
		modbus_router_free((modbus_router_t *)Ptrhdharddevice->ctx );
	}
	return 0;	
}


int protocol_modbus_router::relay_on(shared_ptr<harddevice> Ptrhdharddevice)
{
	int rc ;
	int start_addr_register = 100;
	int register_num = 1;
	uint16_t data = 1<<0;
	modbus_router_list  *ptr_modbus_router_list ;
	int list_num = Ptrhdharddevice->list.size();


	//uint16_t  *list  = new uint16_t[list_num];
	
	uint16_t  *list  = (uint16_t *)malloc(sizeof(uint16_t)*list_num);
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
	
	ptr_modbus_router_list  = (modbus_router_list  *)malloc (sizeof(modbus_router_list));
	if (ptr_modbus_router_list == NULL){
		printf("malloc ptr_dlt645_router_list error \n");
		return -1;
	}

	ptr_modbus_router_list->address = 0xfb;
	ptr_modbus_router_list->function_no = 0x78;

	if (list_num == 1){
		ptr_modbus_router_list->recvicer_address = list[0];
	}else {
		ptr_modbus_router_list->recvicer_address = list[1];
	}	
	
	ptr_modbus_router_list->router_list= list;
	ptr_modbus_router_list->router_num = list_num;
	ptr_modbus_router_list->router_length = ptr_modbus_router_list->router_num +  4;	
	
	rc = modbus_router_write_registers((modbus_router_t *)Ptrhdharddevice->ctx,
				start_addr_register,
				register_num,
				&data,ptr_modbus_router_list);	

	free(list);
	free(ptr_modbus_router_list);	
	return rc;
}


int protocol_modbus_router::relay_off(shared_ptr<harddevice> Ptrhdharddevice)
{
	int rc ;
	int start_addr_register = 100;
	int register_num = 1;
	uint16_t data = 1<<1;
	modbus_router_list  *ptr_modbus_router_list ;
	int list_num = Ptrhdharddevice->list.size();


	//uint16_t  *list  = new uint16_t[list_num];
	
	uint16_t  *list  = (uint16_t *)malloc(sizeof(uint16_t)*list_num);
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
	ptr_modbus_router_list  =(modbus_router_list  *) malloc (sizeof(modbus_router_list));
	if (ptr_modbus_router_list == NULL){
		printf("malloc ptr_dlt645_router_list error \n");
		return -1;
	}

	ptr_modbus_router_list->address = 0xfb;
	ptr_modbus_router_list->function_no = 0x78;

	if (list_num == 1){
		ptr_modbus_router_list->recvicer_address = list[0];
	}else {
		ptr_modbus_router_list->recvicer_address = list[1];
	}	
	
	ptr_modbus_router_list->router_list= list;
	ptr_modbus_router_list->router_num = list_num;
	ptr_modbus_router_list->router_length = ptr_modbus_router_list->router_num +  4;	
	
	rc = modbus_router_write_registers((modbus_router_t *)Ptrhdharddevice->ctx,
				start_addr_register,
				register_num,
				&data,ptr_modbus_router_list);

	free(list);
	free(ptr_modbus_router_list);
	return rc;

}



void protocol_modbus_router_init(list<shared_ptr<protocol_manager> > &protocol_manager_list)
{
	shared_ptr<protocol_manager> ptr_protocol_modbus_router (new  protocol_modbus_router());	
	Registerprotocol(ptr_protocol_modbus_router,protocol_manager_list);
}



