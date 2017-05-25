#ifndef _PROTOCOL_DLT645_H_
#define _PROTOCOL_DLT645_H_

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
//#include "protocol_manager.h"
//#include "struct_hardtype.h"

using namespace  boost;

class protocol_manager;
class harddevice;


class  protocol_dlt645  : public  protocol_manager , boost::noncopyable  
														  
{
public :
	protocol_dlt645();	
	~protocol_dlt645();
	 void handle_hd_register_length(uint32_t hd_register_length,  shared_ptr<harddevice>   PTharddevice ,string  & sql_cmd,int i);

// new 

	int protocol_init(shared_ptr<harddevice> Ptrhdharddevice);
	int protocol_set_slave( shared_ptr<harddevice> Ptrhdharddevice);
 	int protocol_read_registers(shared_ptr<harddevice> Ptrhdharddevice );
	int protocol_write_registers(shared_ptr<harddevice> Ptrhdharddevice);
	int protocol_close(shared_ptr<harddevice> Ptrhdharddevice );
	int relay_on(shared_ptr<harddevice> Ptrhdharddevice);
	 int relay_off(shared_ptr<harddevice> Ptrhdharddevice );
 
};


void protocol_dlt645_init(list<shared_ptr<protocol_manager> > &protocol_manager_list);


#endif    /*_PROTOCOL_DLT645_H_*/
