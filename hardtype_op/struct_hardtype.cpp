#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <iostream>
#include <string>
#include <vector>
#include <deque> 
#include <list>
#include <boost/shared_ptr.hpp>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>


#include "struct_hardtype.h"
#include "measure_jack_op.h"
#include "xinke_relay_op.h"
#include "single_phase_meter_op.h"
#include "relay_op.h"
#include "new_human_induction_op.h"
#include "three_phase_meter_op.h"
#include "pcount_tcp_op.h"
#include "pcount_op.h"
#include "touch_switch_op.h"

using namespace std;
using namespace boost;


//list<shared_ptr<hardtype> > hardtype_list;

int Registerhardtype(shared_ptr<hardtype> pthardtype,list<shared_ptr<hardtype> > &hardtype_list)
{	

	hardtype_list.push_back(pthardtype);

	return 0;
}

void Showhardtype(list<shared_ptr<hardtype> > &hardtype_list)
{	 

	cout<<"Showhardtype"<<endl;
	for (list<shared_ptr<hardtype> >::iterator pr = hardtype_list.begin(); pr !=hardtype_list.end();pr++){
		
		//cout<<(*pr)->hardtype<< " "<<(*pr)->hardtype_name<<endl;
		LOG_INFO<<(*pr)->hardtype<< " "<<(*pr)->hardtype_name;
		
	}

}

shared_ptr<hardtype>  Gethardtype_byname(string  hardtype_name,list<shared_ptr<hardtype> > &hardtype_list)
{

	for (list<shared_ptr<hardtype> >::iterator  pr = hardtype_list.begin(); pr != hardtype_list.end();pr++){
		if ((*pr)->hardtype_name == hardtype_name){
		//cout<<(*pr)->hardtype<< (*pr)->hardtype_name<<endl;
		return (*pr);
		}
	}	

	shared_ptr<hardtype> ptr_hardtype;

	return ptr_hardtype;
}


shared_ptr<hardtype>  Gethardtype(  string  hardtype_name,list<shared_ptr<hardtype> > &hardtype_list)
{

	shared_ptr<hardtype> ptr_hardtype;	
	if (hardtype_name.empty()){
		return ptr_hardtype;
	}
	
	for (list<shared_ptr<hardtype> >::iterator  pr = hardtype_list.begin(); pr != hardtype_list.end();pr++){
		if ((*pr)->hardtype == hardtype_name){
		//cout<<(*pr)->hardtype<< (*pr)->hardtype_name<<endl;
		LOG_INFO<<(*pr)->hardtype<< (*pr)->hardtype_name;
		return (*pr);
		}
	}
	
	
	return ptr_hardtype;

}


string get_protocol_name_Byhardtype(string  hardtype_str,list<shared_ptr<hardtype> > &hardtype_list)
{

	string protocol_name ;

	
	for (list<shared_ptr<hardtype> >::iterator  pr = hardtype_list.begin(); pr != hardtype_list.end();pr++){
		if ((*pr)->hardtype == hardtype_str){
		//cout<<(*pr)->hardtype<< (*pr)->hardtype_name<<endl;
		LOG_INFO<<(*pr)->hardtype<< (*pr)->hardtype_name;
		return (*pr)->protocol_name;
		}
	}
	
	return protocol_name ;
}




int hardtype_op_init(list<shared_ptr<hardtype> > &hardtype_list)
{
	measure_jack_init(hardtype_list);
	xinke_relay_init(hardtype_list);
	new_human_induction_init(hardtype_list);
	single_phase_meter_init(hardtype_list);
	relay_init(hardtype_list);
	three_phase_meter_init(hardtype_list);
	touch_switch_init(hardtype_list);
	pcount_tcp_init(hardtype_list);
	pcount_init(hardtype_list);
	return 0;
}

int free_hardtype_op(list<shared_ptr<hardtype> > &hardtype_list)
{
	hardtype_list.clear();
	return 0;
}




hardtype::~hardtype()
{
	//cout <<"hardtype="<<hardtype<<endl;
}

