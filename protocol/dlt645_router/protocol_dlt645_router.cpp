#include <stdio.h> 
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <dlt645_router.h>
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
#include "protocol_dlt645_router.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
#include "struct_roominfo.h"


using namespace std;
using namespace boost;

protocol_dlt645_router::protocol_dlt645_router()
{
	protocol_name = "dlt645_router";
	protocol_size = sizeof(uint8_t);	
	LOG_INFO<<"dlt645_router" ;
}

protocol_dlt645_router::~protocol_dlt645_router()
{
	
}

int protocol_dlt645_router::protocol_init(shared_ptr<harddevice> Ptrhdharddevice)
{

	const int _RESPONSE_TIMEOUT_NEW =2 ;	
	 struct timeval response_timeout;	
	 
	const  char * dlt645_router_device_name = (Ptrhdharddevice->device_name).c_str();
	
      Ptrhdharddevice->ctx = dlt645_router_new(dlt645_router_device_name, Ptrhdharddevice->baudrate,Ptrhdharddevice->parity ,Ptrhdharddevice->data_bit,Ptrhdharddevice->stop_bit);
  	  if (Ptrhdharddevice->ctx == NULL) {
		LOG_ERROR<<"libdlt645_router Error:Unable to allocate libdlt645_router context";
		return -1;
   	 }


    dlt645_router_set_debug((dlt645_router_t *)Ptrhdharddevice->ctx , TRUE);


    if (dlt645_router_connect((dlt645_router_t *)Ptrhdharddevice->ctx ) == -1) {
	LOG_ERROR<<"ibdlt645_router Error:Connection failed:"<< dlt645_router_strerror(errno);
       	return -1;

    }

    /* Define a new and too short timeout */
    response_timeout.tv_sec = _RESPONSE_TIMEOUT_NEW;
    response_timeout.tv_usec = 0;
    dlt645_router_set_response_timeout((dlt645_router_t *)Ptrhdharddevice->ctx, &response_timeout);
    dlt645_router_set_byte_timeout((dlt645_router_t *)Ptrhdharddevice->ctx, &response_timeout);
	return 0;
}
int protocol_dlt645_router::protocol_set_slave( shared_ptr<harddevice> Ptrhdharddevice)
{

	long long server_id_ll;
	uint8_t slave[6]; 
	char temp[100];
	string slave_str;
	int i =0;

	string server_id = Ptrhdharddevice->hdaddr;
	
	 server_id_ll = strtoll(server_id.c_str(),NULL,16);		

	LOG_INFO<<"server_id="<<server_id;

	LOG_INFO<<"server_id_ll id ="<<server_id_ll;

	for (i=0;i<6;i++){
		slave[i] = server_id_ll>>i*8;
		sprintf(temp,"slave[%d] =%x",i,slave[i]);
		LOG_INFO<<temp;
	}

	dlt645_router_set_slave((dlt645_router_t *)Ptrhdharddevice->ctx, slave);	

	return 0;
	
}
int protocol_dlt645_router::protocol_read_registers(shared_ptr<harddevice> Ptrhdharddevice )
{

	int rc =0;
	int header_num =4;
	int addr_num =4;
	int 	addr = 0x8000ff00;
	int list_num = Ptrhdharddevice->list.size();


	uint16_t  *list  = (uint16_t *)malloc(sizeof(uint16_t)*list_num);
	if (list== NULL){
		//printf("malloc list error\n");
		LOG_INFO<<"malloc list error";
		return -1;
	}


 	for (int i=0;i<list_num;i++){
		try{
			list[i] = lexical_cast<uint16_t>(Ptrhdharddevice->list[i]);
		}catch (bad_lexical_cast &e){
			LOG_ERROR<<"error:"<<e.what();
		}
	}  
	struct timeval old_response_timeout;
        struct timeval response_timeout;
	dlt645_router_list  *ptr_dlt645_router_list;	
	
	
	ptr_dlt645_router_list  =(dlt645_router_list  *) malloc (sizeof(dlt645_router_list));
	if (ptr_dlt645_router_list == NULL){
		LOG_INFO<<"malloc ptr_dlt645_router_list error \n";
		return -1;
	}

	ptr_dlt645_router_list->address = 0xfb;
	ptr_dlt645_router_list->function_no = 0x78;
	ptr_dlt645_router_list->recvicer_address = list[0];
	ptr_dlt645_router_list->router_list= list;
	ptr_dlt645_router_list->router_num = list_num;
	ptr_dlt645_router_list->router_length = ptr_dlt645_router_list->router_num +  4;

    /* Save original timeout */
    dlt645_router_get_response_timeout((dlt645_router_t *) Ptrhdharddevice->ctx, &old_response_timeout);

    /* Define a new and too short timeout */
    response_timeout.tv_sec = 5;
    response_timeout.tv_usec = 0;
    dlt645_router_set_response_timeout( (dlt645_router_t *)Ptrhdharddevice->ctx, &response_timeout);

    dlt645_router_set_byte_timeout((dlt645_router_t *)Ptrhdharddevice->ctx, &response_timeout);

	rc = dlt645_router_read_send((dlt645_router_t *)Ptrhdharddevice->ctx,ptr_dlt645_router_list,header_num,addr,addr_num,(uint8_t *)Ptrhdharddevice->device_data);

	free(list);
	free(ptr_dlt645_router_list);

	
	return rc;

	
}
int protocol_dlt645_router::protocol_write_registers(shared_ptr<harddevice> Ptrhdharddevice)
{



	return 0;	
}
int protocol_dlt645_router::protocol_close(shared_ptr<harddevice> Ptrhdharddevice )
{

	if ((dlt645_router_t *)Ptrhdharddevice->ctx != NULL){
		dlt645_router_close((dlt645_router_t *)Ptrhdharddevice->ctx);
		dlt645_router_free((dlt645_router_t *)Ptrhdharddevice->ctx );
	}
	return 0;
}


int protocol_dlt645_router::relay_on(shared_ptr<harddevice> Ptrhdharddevice)
{
	int header_num =4;
	int list_num = Ptrhdharddevice->list.size();


	
	uint16_t  *list  = (uint16_t *)malloc(sizeof(uint16_t)*list_num);
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
	
	dlt645_router_list  *ptr_dlt645_router_list;	
	ptr_dlt645_router_list  = (dlt645_router_list  *)malloc (sizeof(dlt645_router_list));
	if (ptr_dlt645_router_list == NULL){
		LOG_ERROR<<"malloc ptr_dlt645_router_list error";
		return -1;
	}	
	ptr_dlt645_router_list->address = 0xfb;
	ptr_dlt645_router_list->function_no = 0x78;
	ptr_dlt645_router_list->recvicer_address = list[0];
	ptr_dlt645_router_list->router_list= list;
	ptr_dlt645_router_list->router_num = list_num;
	ptr_dlt645_router_list->router_length = ptr_dlt645_router_list->router_num +  4;	
	int rc = dlt645_router_relay_on((dlt645_router_t *)Ptrhdharddevice->ctx,ptr_dlt645_router_list,header_num,RELAY_ON);	

	free(list);
	free(ptr_dlt645_router_list);
	return rc;
}

int protocol_dlt645_router::relay_off(shared_ptr<harddevice> Ptrhdharddevice )
{
	int header_num =4;
	int list_num = Ptrhdharddevice->list.size();



	uint16_t  *list  = (uint16_t *)malloc(sizeof(uint16_t)*list_num);
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
	dlt645_router_list  *ptr_dlt645_router_list;	
	ptr_dlt645_router_list  =(dlt645_router_list  *) malloc (sizeof(dlt645_router_list));
	if (ptr_dlt645_router_list == NULL){
		//printf("malloc ptr_dlt645_router_list error \n");
		LOG_ERROR<<"malloc ptr_dlt645_router_list error";
		return -1;
	}	
	ptr_dlt645_router_list->address = 0xfb;
	ptr_dlt645_router_list->function_no = 0x78;
	ptr_dlt645_router_list->recvicer_address = list[0];
	ptr_dlt645_router_list->router_list= list;
	ptr_dlt645_router_list->router_num = list_num;
	ptr_dlt645_router_list->router_length = ptr_dlt645_router_list->router_num +  4;
	
	 int rc = dlt645_router_relay_on((dlt645_router_t *)Ptrhdharddevice->ctx,ptr_dlt645_router_list,header_num,RELAY_OFF);	
	free(list);
	free(ptr_dlt645_router_list);
	return rc;	
}


void protocol_dlt645_router_init(list<shared_ptr<protocol_manager> > &protocol_manager_list)
{
	shared_ptr<protocol_manager> ptr_protocol_dlt645_router (new  protocol_dlt645_router());		
	Registerprotocol(ptr_protocol_dlt645_router,protocol_manager_list);
}



