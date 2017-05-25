#ifndef _PROTOCOL_MODBUS_ROUTER_H_
#define _PROTOCOL_MODBUS_ROUTER_H_

#include <boost/noncopyable.hpp>

class  harddevice;
class protocol_manager;

class  protocol_modbus_router : public  protocol_manager,boost::noncopyable
{
public :
	 protocol_modbus_router();
	 ~protocol_modbus_router();
	void handle_hd_register_length(uint32_t hd_register_length,shared_ptr<harddevice> PTharddevice ,string  & sql_cmd,int i);

	int protocol_init(shared_ptr<harddevice> Ptrhdharddevice);
	int protocol_set_slave( shared_ptr<harddevice> Ptrhdharddevice);
	int protocol_read_registers(shared_ptr<harddevice> Ptrhdharddevice );
	int protocol_write_registers(shared_ptr<harddevice> Ptrhdharddevice);
	int protocol_close(shared_ptr<harddevice> Ptrhdharddevice );
	int relay_on(shared_ptr<harddevice> Ptrhdharddevice);
	 int relay_off(shared_ptr<harddevice> Ptrhdharddevice );

};


void protocol_modbus_router_init(list<shared_ptr<protocol_manager> > &protocol_manager_list);



#endif    /*_PROTOCOL_MODBUS_ROUTER_H_*/
