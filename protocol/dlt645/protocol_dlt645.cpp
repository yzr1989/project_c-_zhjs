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

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "dlt645.h"
#include "config.h"
#include "utils.h"
#include "protocol_manager.h"
#include "protocol_dlt645.h"

#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
#include "struct_roominfo.h"


protocol_dlt645::protocol_dlt645()
{
	protocol_name = string("dlt645");
	protocol_size	= sizeof(uint8_t);
	LOG_TRACE<<"protocol_name="<<protocol_name;
}


protocol_dlt645::~protocol_dlt645()
{
	
}
int protocol_dlt645::protocol_init(shared_ptr<harddevice> Ptrhdharddevice)
{
	const char *dlt645_device_name; 
	const int _RESPONSE_TIMEOUT_NEW =1 ;
	
	 struct timeval response_timeout;		
	dlt645_device_name =( Ptrhdharddevice->device_name).c_str();
	
       Ptrhdharddevice->ctx = dlt645_new(dlt645_device_name,  Ptrhdharddevice->baudrate, Ptrhdharddevice->parity , Ptrhdharddevice->data_bit, Ptrhdharddevice->stop_bit);
  	  if ( Ptrhdharddevice->ctx == NULL) {

   	     LOG_ERROR<<"libdlt645 Error:Unable to allocate libdlt645 context";

    	    	return -1;
   	 }
	
    dlt645_set_debug((dlt645_t *) Ptrhdharddevice->ctx , TRUE);
 
    if (dlt645_connect((dlt645_t *) Ptrhdharddevice->ctx ) == -1) {

    	LOG_ERROR<<"ibdlt645 Error:Connection failed:"<< dlt645_strerror(errno);
	LOG_ERROR<<"dlt645_device_name"<<dlt645_device_name;	 		
       	return -1;
 
    } 

    /* Define a new and too short timeout */
    response_timeout.tv_sec = _RESPONSE_TIMEOUT_NEW;
    response_timeout.tv_usec = 0;
    dlt645_set_response_timeout((dlt645_t *)Ptrhdharddevice->ctx, &response_timeout);
    dlt645_set_byte_timeout((dlt645_t *)Ptrhdharddevice->ctx, &response_timeout);

	return 0;

}

int protocol_dlt645::protocol_set_slave( shared_ptr<harddevice> Ptrhdharddevice)
{
	long long server_id_ll;
	uint8_t slave[6]; 
	char slave_temp[100];
	int i =0;

	string server_id = Ptrhdharddevice->hdaddr;
	
	 server_id_ll = strtoll(server_id.c_str(),NULL,16);		


	LOG_INFO<<"server_id="<<server_id;


	LOG_INFO<<"server_id_ll id ="<<server_id_ll;

	for (i=0;i<6;i++){
		slave[i] = server_id_ll>>i*8;		
		sprintf(slave_temp,"slave[%d] =%x",i,slave[i]);
		LOG_INFO<<slave_temp;
	}

	dlt645_set_slave((dlt645_t *)Ptrhdharddevice->ctx, slave);	

	return 0;

}


int protocol_dlt645::protocol_read_registers(shared_ptr<harddevice> Ptrhdharddevice )
{
	int rc =0;
	int header_num =4;
	int addr_num =4;
	int 	addr = 0x8000ff00;

	rc = dlt645_read_send((dlt645_t *)Ptrhdharddevice->ctx,header_num,addr,addr_num,(uint8_t *)Ptrhdharddevice->device_data);

	return rc;
}


int protocol_dlt645::protocol_write_registers(shared_ptr<harddevice> Ptrhdharddevice)
{
	return 0;
}

int protocol_dlt645::protocol_close(shared_ptr<harddevice> Ptrhdharddevice )
{

	if ((dlt645_t *)Ptrhdharddevice->ctx != NULL){

		dlt645_close((dlt645_t *)Ptrhdharddevice->ctx);
		dlt645_free((dlt645_t *)Ptrhdharddevice->ctx );
	}
	return 0;
}



int  protocol_dlt645::relay_on(shared_ptr<harddevice> Ptrhdharddevice)
{
	int header_num =4;
	 int rc = dlt645_relay_on((dlt645_t *)Ptrhdharddevice->ctx,header_num,RELAY_ON);	
	return rc;	
}
int  protocol_dlt645::relay_off(shared_ptr<harddevice> Ptrhdharddevice )
{
	int header_num =4;
	 int rc = dlt645_relay_on((dlt645_t *)Ptrhdharddevice->ctx,header_num,RELAY_OFF);	
	return rc ;	
}


void protocol_dlt645_init(list<shared_ptr<protocol_manager> > &protocol_manager_list)
{
	shared_ptr<protocol_manager> ptr_protocol_dlt645 (new  protocol_dlt645());
	Registerprotocol(ptr_protocol_dlt645,protocol_manager_list);
	
}



