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

#include "protocol_manager.h"
#include "struct_harddevice.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
#include "struct_roominfo.h"
using namespace std;
using namespace boost;

int Registerharddevice(shared_ptr<harddevice> ptharddevice,list<shared_ptr<harddevice> > &harddevice_list)
{
	harddevice_list.push_back(ptharddevice);
	return 0;
}

void Showharddevice(list<shared_ptr<harddevice> > &harddevice_list)
{
	LOG_INFO<<"Showharddevice";
	for (list<shared_ptr<harddevice> >::iterator pr = harddevice_list.begin(); pr != harddevice_list.end();pr++){
		LOG_INFO<<(*pr)->hdid;
	}
}


shared_ptr<harddevice>  Getharddevice_byID(  string  hdid,list<shared_ptr<harddevice> > &harddevice_list)
{
	shared_ptr<harddevice>  ptr_harddevice;
	for (list<shared_ptr<harddevice> >::iterator  pr = harddevice_list.begin(); pr != harddevice_list.end();pr++){
		if ((*pr)->hdid == hdid){
	
		LOG_INFO<<(*pr)->hdid;
		return (*pr);
		}
	}
	
	return ptr_harddevice;
}


 void  print_harddevice(shared_ptr<harddevice> PTharddevice) 
{

	if (PTharddevice == NULL) {		
		return;
	}
	LOG_INFO<<"=======  harddevice==begin";
	LOG_INFO<<"hdid: "<<PTharddevice->hdid;
	LOG_INFO<<"hdaddr: "<<PTharddevice->hdaddr;
	LOG_INFO<<"hdtype: "<< PTharddevice->hdtype;
	LOG_INFO<<"hdsi: "<<PTharddevice->hdsi;
	LOG_INFO<<"hddelay_time:"<<PTharddevice->hddelay_time;
	LOG_INFO<<"record_time:"<<PTharddevice->record_time;
	LOG_INFO<<"hdparamlist_name:"<< PTharddevice->gparamlist->hdparamlist_name;
	 print_ghdparamlist(PTharddevice->gparamlist);
	LOG_INFO<<"protocol_name: "<<PTharddevice->Ptprotocol_manager->protocol_name;
	LOG_INFO<<"=======  harddevice==end";
}



harddevice::~harddevice()
{
	LOG_INFO<< "hdid="<<hdid;
	if (this->device_data != NULL){
		 free( this->device_data);
	}
}


void freeharddevice( list<shared_ptr<harddevice> > &harddevice_list)
{
	harddevice_list.clear();
}

void freeharddevice_of_hdharddevices_vect(shared_ptr<comharddevice>   PTcomharddevice)
{
	PTcomharddevice->hdharddevices.clear();
}