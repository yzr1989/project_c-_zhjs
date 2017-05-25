#ifndef _PROTOCOL_MANAGER_H_
#define _PROTOCOL_MANAGER_H_

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <mysql++.h>

//#include "struct_hardtype.h"
//#include "struct_harddevice.h"
//#include "struct_comharddevice.h"
//#include "struct_hdparamlist.h"
//#include "struct_roominfo.h"

class comharddevice ;
class harddevice ;

using namespace std;
using namespace boost;
using namespace mysqlpp ;

#define  RELAY_ON	1
#define  RELAY_OFF    0

#ifndef OFF
#define OFF 0
#endif

#ifndef ON
#define ON 1
#endif

class  roominfo;


class  protocol_manager {
public :
	string protocol_name;	
	int protocol_size;

	protocol_manager();
	 virtual ~protocol_manager();

//new 

	virtual  int protocol_init(shared_ptr<harddevice> Ptrhdharddevice){
		cout<<"protocol_init not implement yet "<<endl;
		return -1;		
	}   	
	
	virtual int protocol_set_slave( shared_ptr<harddevice> Ptrhdharddevice){
		cout<<"protocol_set_slave not implement yet "<<endl;
		return -1;		
	}  


	virtual int protocol_read_registers(shared_ptr<harddevice> Ptrhdharddevice ){
		cout<<"protocol_read_registers not implement yet "<<endl;
		return -1;			
	}


	virtual int protocol_write_registers(shared_ptr<harddevice> Ptrhdharddevice){
		cout<<"protocol_write_registers not implement yet "<<endl;
		return -1;		
	}


	virtual int protocol_close(shared_ptr<harddevice> Ptrhdharddevice ){
		cout<<"protocol_close not implement yet "<<endl;
		return -1;	
	}


	virtual int protocol_send_ir_cmd(shared_ptr<harddevice> Ptrhdharddevice){
		cout<<"protocol_send_ir_cmd for roominfo not implement yet "<<endl;
		return -1;			
	}

	virtual int protocol_send_ir_cmd(shared_ptr<roominfo> PTroominfo,int i){
		cout<<"protocol_send_ir_cmd for roominfo not implement yet "<<endl;
		return -1;		
	}


	virtual int relay_on(shared_ptr<harddevice> Ptrhdharddevice){
		cout<<"relay_on not implement yet "<<endl;
		return -1;	
	}
	virtual int relay_off(shared_ptr<harddevice> Ptrhdharddevice ){
		cout<<"relay_off not implement yet "<<endl;
		return -1;			
	}


	virtual int switch_off(shared_ptr<harddevice> Ptrhdharddevice  , int port_num){
		cout<<"switch_off not implement yet "<<endl;
		return -1;			
	}


	virtual int permission_led_clear(shared_ptr<harddevice> Ptrhdharddevice){
		cout<<"permission not implement yet "<<endl;
		return -1;			
	}

	virtual void handle_hd_register_length(uint32_t hd_register_length,shared_ptr<harddevice> PTharddevice ,string  & sql_cmd,int i){
		cout<<"handle_hd_register_length not implement yet "<<endl;
					
	}
	   void  handle_harddevice(  shared_ptr<harddevice> PTharddevice) ;
	    int     sql_insert_handle(shared_ptr<harddevice> PTharddevice,shared_ptr<protocol_manager>  PTprotocol_manager,  Connection  conn) ;
 	  int    update_hd_connect_times ( shared_ptr<harddevice> PTharddevice,  Connection  conn);

	   int    update_lastconnecttime ( shared_ptr<harddevice> PTharddevice,  Connection  conn);
	 int    update_hd_connect ( shared_ptr<harddevice> PTharddevice,  Connection  conn);


	
} ;




void protocol_init(list<shared_ptr<protocol_manager> > &protocol_manager_list);
int Registerprotocol(  shared_ptr<protocol_manager>  Ptprotocol_manager,list<shared_ptr<protocol_manager> > &protocol_manager_list);
shared_ptr<protocol_manager> Getprotocol(string protocol_name,list<shared_ptr<protocol_manager> > &protocol_manager_list);
bool protocol_has_router(string protocol_name,list<shared_ptr<protocol_manager> > &protocol_manager_list);

void free_protocol(list<shared_ptr<protocol_manager> > &protocol_manager_list);

#endif  /*_PROTOCOL_MANAGER_H_*/
