#ifndef _PROTOCOL_DS_BUS_H_
#define _PROTOCOL_DS_BUS_H_

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

//#include "protocol_manager.h"
//#include "struct_hardtype.h"
//#include "struct_roominfo.h"

using namespace std;
using namespace boost;

class protocol_manager;
class roominfo;
class harddevice;

class  protocol_ds_bus : public  protocol_manager,boost::noncopyable
{
public :
	protocol_ds_bus();	
	~protocol_ds_bus();
	void handle_hd_register_length(uint32_t hd_register_length,shared_ptr<harddevice> PTharddevice ,string  & sql_cmd,int i);

// new 

	int protocol_init(shared_ptr<harddevice> Ptrhdharddevice);
	int protocol_set_slave( shared_ptr<harddevice> Ptrhdharddevice);
 	int protocol_read_registers(shared_ptr<harddevice> Ptrhdharddevice );
	int protocol_write_registers(shared_ptr<harddevice> Ptrhdharddevice);
	int protocol_close(shared_ptr<harddevice> Ptrhdharddevice );
	int relay_off(shared_ptr<harddevice> Ptrhdharddevice);
	int switch_off(shared_ptr<harddevice> Ptrhdharddevice  , int port_num);
	
};



void protocol_ds_bus_init(list<shared_ptr<protocol_manager> > &protocol_manager_list);




#endif    /*_PROTOCOL_DS_BUS_H_*/
