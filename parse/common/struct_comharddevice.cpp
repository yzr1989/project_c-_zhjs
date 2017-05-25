#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <iostream>
#include <vector>
#include <deque> 
#include <list>
#include <string>
#include <mysql/mysql.h>
#include <mysql++.h>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>


#include "struct_comharddevice.h"
#include "struct_harddevice.h"
#include "protocol_manager.h"



list<shared_ptr<comharddevice> > g_comharddevice_list;

int Registercomharddevice(shared_ptr<comharddevice> PTcomharddevice,list<shared_ptr<comharddevice> > &comharddevice_list)
{
	comharddevice_list.push_back(PTcomharddevice);
	return 0;
}


void Showcomharddevice(list<shared_ptr<comharddevice> > &comharddevice_list)
{

	//cout<<"Showcomharddevice"<<endl;
	
	for (list<shared_ptr<comharddevice> >::iterator pr = comharddevice_list.begin(); pr != comharddevice_list.end();pr++){
		
		//cout<<(*pr)->com_name<<" "<<(*pr)->device_name<<endl;
		LOG_INFO<<(*pr)->com_name<<" "<<(*pr)->device_name;
		
	}
	
}

//void freeharddevice_of_hdharddevices_vect(shared_ptr<comharddevice>   PTcomharddevice);
//int free_hardtype_op(shared_ptr<comharddevice>  comharddevicePtr);

void free_harddevice_of_comharddevice(list<shared_ptr<comharddevice> > &comharddevice_list)
{

	//cout<<"Showcomharddevice"<<endl;
	LOG_INFO<<"Showcomharddevice";
	
	for (list<shared_ptr<comharddevice> >::iterator pr = comharddevice_list.begin(); pr != comharddevice_list.end();pr++){
		
		//cout<<(*pr)->com_name<<" "<<(*pr)->device_name<<endl;
		LOG_INFO<<(*pr)->com_name<<" "<<(*pr)->device_name;
		
		//free_ghdparamlist((*pr));
		//free_hardtype_op((*pr));	
		//free_protocol((*pr));
		freeharddevice_of_hdharddevices_vect((*pr));
	}
	
}


shared_ptr<comharddevice>   Gethdcomharddevice_by_id( string  comid,list<shared_ptr<comharddevice> > &comharddevice_list)
{

	
	shared_ptr<comharddevice>  ptr_comharddevice;

	if (comid.empty()){
		return ptr_comharddevice;	
	}
		
	for (list<shared_ptr<comharddevice> >::iterator  pr = comharddevice_list.begin(); pr != comharddevice_list.end();pr++){
		
		//cout<<(*pr)->com_name<<" "<<(*pr)->device_name<<endl;
		if ((*pr)->comid == comid){
			//cout<<"com_name="<<(*pr)->com_name<<endl;
			LOG_INFO<<"com_name="<<(*pr)->com_name;
			return *pr;
		}
		
	}	
	
	return ptr_comharddevice;	
}



shared_ptr<comharddevice>   Gethdcomharddevice( string com_name,list<shared_ptr<comharddevice> > &comharddevice_list)
{
	
	shared_ptr<comharddevice>  ptr_comharddevice;
	if (com_name.empty()){
		return ptr_comharddevice;		
	}
	for (list<shared_ptr<comharddevice> >::iterator pr = comharddevice_list.begin(); pr != comharddevice_list.end();pr++){
		
		if ((*pr)->com_name == com_name){
			LOG_INFO<<"com_name="<<(*pr)->com_name;
			return *pr;
		}
		
	}	
	
	return ptr_comharddevice;	
}


shared_ptr<comharddevice>   Gethdcomharddevice_by_name(string  device_name,list<shared_ptr<comharddevice> > &comharddevice_list)
{
	
	
	shared_ptr<comharddevice>  ptr_comharddevice;
	
	if (device_name.empty()){
		return ptr_comharddevice;		
	}
	
	for (list<shared_ptr<comharddevice> >::iterator pr = comharddevice_list.begin(); pr != comharddevice_list.end();pr++){
		
		//cout<<(*pr)->com_name<<" "<<(*pr)->device_name<<endl;
		if ((*pr)->device_name == device_name){
			//cout<<"device_name="<<(*pr)->device_name<<endl;
			LOG_INFO<<"device_name="<<(*pr)->device_name;
			return *pr;
		}
		
	}
	
	return ptr_comharddevice;	
}



void freecomharddevice(list<shared_ptr<comharddevice> > &comharddevice_list)
{
	comharddevice_list.clear();
}


comharddevice::~comharddevice()
{
	//cout <<"com_name="<< com_name<<endl;
}


comharddevice::comharddevice()
{
	
}