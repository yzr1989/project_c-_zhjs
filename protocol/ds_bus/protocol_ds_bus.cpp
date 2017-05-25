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
//#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "config.h"
#include "ds-bus.h"
#include "utils.h"
#include "protocol_manager.h"
#include "protocol_modbus.h"
#include "protocol_ds_bus.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
#include "struct_roominfo.h"



using namespace std;
using namespace boost;

protocol_ds_bus::protocol_ds_bus()
{
	protocol_name = "ds_bus";
	protocol_size = sizeof(uint8_t);
	//cout << "protocol_ds_bus" <<endl;
	LOG_INFO<<"protocol_ds_bus";
}

protocol_ds_bus::~protocol_ds_bus()
{
	
}

int protocol_ds_bus::protocol_init(shared_ptr<harddevice> Ptrhdharddevice)
{
	
	const  char *modbus_device_name;	
	const int _RESPONSE_TIMEOUT_NEW =1 ;
	
	 struct timeval response_timeout;	
	modbus_device_name = (Ptrhdharddevice->device_name).c_str();
   	Ptrhdharddevice->ctx= ds_bus_new(modbus_device_name, Ptrhdharddevice->baudrate, Ptrhdharddevice->parity, Ptrhdharddevice->data_bit, Ptrhdharddevice->stop_bit);
  	  if (  Ptrhdharddevice->ctx == NULL) {
   	    LOG_ERROR<<"ds_bus Error:Unable to allocate ds_bus context";
    	    return -1;
   	 }


    ds_bus_set_debug( (ds_bus_t*)Ptrhdharddevice->ctx , TRUE);

    if (ds_bus_connect( (ds_bus_t *)Ptrhdharddevice->ctx ) == -1) {
	LOG_ERROR<<"ds_bus Error:Connection failed:"<< ds_bus_strerror(errno);
	   return -1;
    	}


    /* Define a new and too short timeout */
    response_timeout.tv_sec = _RESPONSE_TIMEOUT_NEW;
    response_timeout.tv_usec = 0;
    ds_bus_set_response_timeout((ds_bus_t *)Ptrhdharddevice->ctx, &response_timeout);
    ds_bus_set_byte_timeout((ds_bus_t *)Ptrhdharddevice->ctx, &response_timeout);
	
	return 0;	
}

int protocol_ds_bus::protocol_set_slave( shared_ptr<harddevice> Ptrhdharddevice)
{	
	
	int server_id;
	
	try{	
	 server_id = lexical_cast<int>(Ptrhdharddevice->hdaddr);
	} catch ( bad_lexical_cast &e){
		//cout<< "error"<<e.what()<<endl;
		LOG_ERROR<<"error"<<e.what();
		return -1;
	}	
	
	//cout<<"server id ="<<server_id<<endl;
	LOG_INFO<<"server id ="<<server_id;
	
	ds_bus_set_slave_For_File_Record((ds_bus_t *)Ptrhdharddevice->ctx,server_id);	

	return 0;
}



int protocol_ds_bus::protocol_read_registers(shared_ptr<harddevice> Ptrhdharddevice )
{
	int rc =0;	

	File_Record    *File_Record_r[1];	
	File_Record_r[0]  = (File_Record    *) malloc(sizeof(File_Record )+4);

	
	File_Record_r[0]->file_no = Ptrhdharddevice->file_no;
	File_Record_r[0]->register_address = Ptrhdharddevice->gparamlist->min_addr_register;
        File_Record_r[0]->register_num = Ptrhdharddevice->gparamlist->addr_register_num;
	File_Record_r[0]->data =(uint8_t*) Ptrhdharddevice->device_data;

	rc = ds_bus_File_Record_read_send((ds_bus_t *)Ptrhdharddevice->ctx,File_Record_r ,1);

	free(File_Record_r[0]);

	return rc;	
}
int protocol_ds_bus::protocol_write_registers(shared_ptr<harddevice> Ptrhdharddevice)
{

	int rc;
	File_Record    *File_Record_w[1];

	File_Record_w[0] = (File_Record    *)malloc(sizeof(File_Record)+4);
	
	File_Record_w[0]->file_no = Ptrhdharddevice->file_no;
	File_Record_w[0]->register_address =  Ptrhdharddevice->gparamlist->min_addr_register;
       File_Record_w[0]->register_num =  Ptrhdharddevice->gparamlist->addr_register_num;
	File_Record_w[0]->data =(uint8_t*)  Ptrhdharddevice->device_data;

	rc = ds_bus_File_Record_write_send((ds_bus_t *) Ptrhdharddevice->ctx,
		File_Record_w ,1);
	free(File_Record_w[0]);	

	return rc;
}

int  protocol_ds_bus::relay_off(shared_ptr<harddevice> Ptrhdharddevice)
{
	
	File_Record    *File_Record_w[1];
	uint8_t device_data[] = {0xc1,0xc1,0xc1,0xc1};

	File_Record_w[0] = (File_Record    *)malloc(sizeof(File_Record)+4);
	
	File_Record_w[0]->file_no = 10200;
	File_Record_w[0]->register_address =  11;
       File_Record_w[0]->register_num =  sizeof(device_data);
	File_Record_w[0]->data =(uint8_t*)  malloc(sizeof(device_data));


	memcpy(File_Record_w[0]->data , device_data,sizeof(device_data));
	

	 ds_bus_File_Record_write_send((ds_bus_t *) Ptrhdharddevice->ctx,
		File_Record_w ,1);


	free(File_Record_w[0]->data );
	free(File_Record_w[0]);	

	return 0;	
}



int protocol_ds_bus::switch_off(shared_ptr<harddevice> Ptrhdharddevice  , int port_num)
{

	File_Record    *File_Record_w[1];

	uint8_t register_address_list[] = {11,12,13,14};

	
	//uint8_t device_data[] = {0xc1,0xc1,0xc1,0xc1};
	uint8_t device_data[] = {0xc1};

	File_Record_w[0] = (File_Record    *)malloc(sizeof(File_Record)+4);
	
	File_Record_w[0]->file_no = 10200;
	File_Record_w[0]->register_address =  register_address_list[port_num-1];
       File_Record_w[0]->register_num =  sizeof(device_data);
	File_Record_w[0]->data =(uint8_t*)  malloc(sizeof(device_data));


	memcpy(File_Record_w[0]->data , device_data,sizeof(device_data));
	

	 ds_bus_File_Record_write_send((ds_bus_t *) Ptrhdharddevice->ctx,
		File_Record_w ,1);


	free(File_Record_w[0]->data );
	free(File_Record_w[0]);	

	return 0;
	
}


int protocol_ds_bus::protocol_close(shared_ptr<harddevice> Ptrhdharddevice )
{

	if ((ds_bus_t *)Ptrhdharddevice->ctx  != NULL){
		ds_bus_close((ds_bus_t *)Ptrhdharddevice->ctx);
		ds_bus_free((ds_bus_t *)Ptrhdharddevice->ctx );
	}

	return 0;
}




void protocol_ds_bus_init(list<shared_ptr<protocol_manager> > &protocol_manager_list)
{
	shared_ptr<protocol_manager> ptr_protocol_ds_bus (new  protocol_ds_bus());	
	Registerprotocol(ptr_protocol_ds_bus,protocol_manager_list);
}




