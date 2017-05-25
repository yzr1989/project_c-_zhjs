#ifndef   _PROTOCOL_DS_BUS_ROUTER_H_
#define _PROTOCOL_DS_BUS_ROUTER_H_

#include <boost/noncopyable.hpp>
//#include "protocol_manager.h"
//#include "struct_hardtype.h"
//#include "struct_roominfo.h"


class protocol_manager;
class harddevice;


class  protocol_ds_bus_router : public  protocol_manager ,boost::noncopyable
{
public :
	 protocol_ds_bus_router();
	  ~protocol_ds_bus_router();
	 void handle_hd_register_length(uint32_t hd_register_length,  shared_ptr<harddevice>   PTharddevice ,string  & sql_cmd,int i);
/*for roominfo */



// new 
	int protocol_send_ir_cmd(shared_ptr<roominfo> PTroominfo,int i);
	int protocol_init(shared_ptr<harddevice> Ptrhdharddevice);
	int protocol_set_slave( shared_ptr<harddevice> Ptrhdharddevice);
 	int protocol_read_registers(shared_ptr<harddevice> Ptrhdharddevice );
	int protocol_write_registers(shared_ptr<harddevice> Ptrhdharddevice);
	int protocol_close(shared_ptr<harddevice> Ptrhdharddevice );
	int switch_off(shared_ptr<harddevice> Ptrhdharddevice  , int port_num);
	int  relay_off(shared_ptr<harddevice> Ptrhdharddevice);
	int permission_led_clear(shared_ptr<harddevice> Ptrhdharddevice );

};


void protocol_ds_bus_router_init(list<shared_ptr<protocol_manager> > &protocol_manager_list);





#endif    /*_PROTOCOL_DS_BUS_ROUTER_H_*/
