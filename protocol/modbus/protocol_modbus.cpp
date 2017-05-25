#include <stdio.h> 
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus.h>
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
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "config.h"


#include "utils.h"
#include "protocol_manager.h"
#include "protocol_modbus.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
#include "struct_roominfo.h"

using namespace std;
using namespace boost;

#define _RESPONSE_TIMEOUT    1

protocol_modbus::protocol_modbus()
{
	protocol_name = string("modbus");
	protocol_size	= sizeof(uint16_t);
	//cout<<"protocol_name="<<protocol_name<<endl;
	LOG_INFO<<"protocol_name="<<protocol_name;
}

protocol_modbus::~protocol_modbus()
{
	
}

// new 

int protocol_modbus::protocol_init(shared_ptr<harddevice> Ptrhdharddevice)
{

	const int _RESPONSE_TIMEOUT_NEW =1 ;
	 struct timeval response_timeout;	
    const char *  modbus_device_name = (Ptrhdharddevice->device_name).c_str();
    Ptrhdharddevice->ctx = (modbus_t *)modbus_new_rtu(modbus_device_name, Ptrhdharddevice->baudrate,Ptrhdharddevice->parity ,Ptrhdharddevice->data_bit,Ptrhdharddevice->stop_bit);
    if (Ptrhdharddevice->ctx == NULL) {
   	   LOG_ERROR<<"libmodbus Error:Unable to allocate libmodbus context";
    	    	return -1;
     }
	
    modbus_set_debug((modbus_t *)Ptrhdharddevice->ctx , TRUE);

    if (modbus_connect((modbus_t *)Ptrhdharddevice->ctx ) == -1) {
        LOG_ERROR<<"ibmodbus Error:Connection failed:" << modbus_strerror(errno);
	 LOG_ERROR<<"modbus_device_name"<<modbus_device_name;
       	return -1;	
    }

    /* Define a new and too short timeout */
    response_timeout.tv_sec = _RESPONSE_TIMEOUT_NEW;
    response_timeout.tv_usec = 0;
    modbus_set_response_timeout((modbus_t *)Ptrhdharddevice->ctx, &response_timeout);
    modbus_set_byte_timeout((modbus_t *)Ptrhdharddevice->ctx, &response_timeout);

	return 0;	
}




int protocol_modbus::protocol_set_slave( shared_ptr<harddevice> Ptrhdharddevice)
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
	
	modbus_set_slave((modbus_t *)Ptrhdharddevice->ctx,server_id);	

	return 0;
}
int protocol_modbus::protocol_read_registers(shared_ptr<harddevice> Ptrhdharddevice )
{		


	 int rc = modbus_read_registers((modbus_t *)Ptrhdharddevice->ctx,
				Ptrhdharddevice->gparamlist->min_addr_register,
				Ptrhdharddevice->gparamlist->addr_register_num,
				(uint16_t *)Ptrhdharddevice->device_data);	


	return rc;
}
int protocol_modbus::protocol_write_registers(shared_ptr<harddevice> Ptrhdharddevice)
{

	 int rc = modbus_write_registers((modbus_t *)Ptrhdharddevice->ctx,
				Ptrhdharddevice->gparamlist->min_addr_register,
				Ptrhdharddevice->gparamlist->addr_register_num,
				(uint16_t *)Ptrhdharddevice->device_data);	


	return rc;
}
int protocol_modbus::protocol_close(shared_ptr<harddevice> Ptrhdharddevice )
{

	if ((modbus_t *)Ptrhdharddevice->ctx != NULL){
		modbus_close((modbus_t *)Ptrhdharddevice->ctx);
		modbus_free((modbus_t *)Ptrhdharddevice->ctx );
	}
	return 0;
}


int protocol_modbus::relay_on(shared_ptr<harddevice> Ptrhdharddevice)
{
	int rc ;
	int start_addr_register = 100;
	int register_num = 1;
	uint16_t data = 1<<0;
	rc = modbus_write_registers((modbus_t *)Ptrhdharddevice->ctx,
				start_addr_register,
				register_num,
				&data);		
	return rc;
}


int protocol_modbus::relay_off(shared_ptr<harddevice> Ptrhdharddevice)
{
	int rc ;
	int start_addr_register = 100;
	int register_num = 1;
	uint16_t data = 1<<1;
	rc = modbus_write_registers((modbus_t *)Ptrhdharddevice->ctx,
				start_addr_register,
				register_num,
				&data);	
	return rc;
}



void protocol_modbus_init(list<shared_ptr<protocol_manager> > &protocol_manager_list)
{
	shared_ptr<protocol_manager> ptr_protocol_modbus (new  protocol_modbus());
	Registerprotocol(ptr_protocol_modbus,protocol_manager_list);

}



