#ifndef _PROTOCOL_DLT645_ROUTER_H_
#define _PROTOCOL_DLT645_ROUTER_H_

#include <boost/noncopyable.hpp>

//#include "protocol_manager.h"
//#include "struct_hardtype.h"

class protocol_manager;
class harddevice;

class  protocol_dlt645_router : public  protocol_manager,boost::noncopyable
{
public :
	 protocol_dlt645_router();
	 ~protocol_dlt645_router();
	void handle_hd_register_length(uint32_t hd_register_length,  shared_ptr<harddevice>   PTharddevice ,string  & sql_cmd,int i);

//new 

	int protocol_init(shared_ptr<harddevice> Ptrhdharddevice);
	int protocol_set_slave( shared_ptr<harddevice> Ptrhdharddevice);
 	int protocol_read_registers(shared_ptr<harddevice> Ptrhdharddevice );
	int protocol_write_registers(shared_ptr<harddevice> Ptrhdharddevice);
	int protocol_close(shared_ptr<harddevice> Ptrhdharddevice );
	int relay_on(shared_ptr<harddevice> Ptrhdharddevice);
	 int relay_off(shared_ptr<harddevice> Ptrhdharddevice );

};


void protocol_dlt645_router_init(list<shared_ptr<protocol_manager> > &protocol_manager_list);


#endif    /*_PROTOCOL_DLT645_ROUTER_H_*/
