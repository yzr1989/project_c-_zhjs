#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <boost/shared_ptr.hpp>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
#include "protocol_manager.h"
#include "protocol_modbus.h"
#include "protocol_modbus_router.h"
#include "protocol_ds_bus.h"
#include "protocol_ds_bus_router.h"
#include "protocol_dlt645_router.h"
#include "protocol_dlt645.h"


using namespace std;
using namespace boost;


//list<shared_ptr<protocol_manager> >protocol_manager_list;
int Registerprotocol(  shared_ptr<protocol_manager>  Ptprotocol_manager,list<shared_ptr<protocol_manager> > &protocol_manager_list)
{
	if (Ptprotocol_manager){
		protocol_manager_list.push_back(Ptprotocol_manager);
		return 0;
	}else {
		return -1;
	}
}


void Showprotocol(list<shared_ptr<protocol_manager> > &protocol_manager_list)
{
	for (list<shared_ptr<protocol_manager> >::iterator  pr = protocol_manager_list.begin(); pr != protocol_manager_list.end();pr++){
		
		//cout<<(*pr)->com_name<<" "<<(*pr)->device_name<<endl;
		//cout << "protocol_name " <<(*pr)->protocol_name <<endl;
		LOG_INFO<<"protocol_name " <<(*pr)->protocol_name;
	}
}


shared_ptr<protocol_manager> Getprotocol(string protocol_name,list<shared_ptr<protocol_manager> > &protocol_manager_list)
{
	shared_ptr<protocol_manager>  ptr_protocol_manager;

	if (protocol_name.empty()){
		return ptr_protocol_manager;	
	}
		
	for ( list<shared_ptr<protocol_manager> >::iterator  pr = protocol_manager_list.begin(); pr != protocol_manager_list.end();pr++){
		
		if ((*pr)->protocol_name == protocol_name){
			//cout<<"protocol_name="<<(*pr)->protocol_name<<endl;
			LOG_INFO<<"protocol_name="<<(*pr)->protocol_name;
			return *pr;
		}
		
	}	
	
	return ptr_protocol_manager;	
	
}


bool protocol_has_router(string protocol_name,list<shared_ptr<protocol_manager> > &protocol_manager_list)
{
	string   protocol_name_router = protocol_name+"_router";

	 shared_ptr<protocol_manager>   Ptprotocol_manager = Getprotocol(protocol_name_router,protocol_manager_list);
	 if (Ptprotocol_manager->protocol_name.empty()){
		// has no router
 		return false;
	 }else {	 	
		// has router
		return true;
		
	 }

	
}


void protocol_init(list<shared_ptr<protocol_manager> > &protocol_manager_list)
{
	protocol_modbus_init(protocol_manager_list);
	protocol_ds_bus_init(protocol_manager_list);
	protocol_dlt645_init(protocol_manager_list);
	protocol_dlt645_router_init(protocol_manager_list);
	protocol_ds_bus_router_init(protocol_manager_list);
	protocol_modbus_router_init(protocol_manager_list);
}


void free_protocol(list<shared_ptr<protocol_manager> > &protocol_manager_list)
{
	protocol_manager_list.clear();
}


protocol_manager::protocol_manager()
{
	
}

protocol_manager::~protocol_manager()
{
	
}