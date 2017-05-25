#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <string>
#include <vector>
#include <deque> 
#include <list>
#include <mysql++.h>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp> 
#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>
#include "protocol_manager.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
#include "struct_roominfo.h"
#include "parse_bit_field.h"
#include "thread_object.h"
#include "thread_server_udp.h"
#include "thread_air_control.h"

using namespace std;
using namespace mysqlpp ;
using namespace boost;


extern list<shared_ptr<comharddevice> > g_comharddevice_list;

int   thread_object::get_harddevice(Connection  conn, shared_ptr<comharddevice>   PTcomharddevice,				
				list<shared_ptr<comharddevice> > &comharddevice_list,
				list<shared_ptr<harddevice> > &harddevice_list)
{		


	LOG_INFO<<"com_name="<<PTcomharddevice->com_name;	
	 string com_name = PTcomharddevice->com_name;
	string sql_buffer = "SELECT  *   FROM harddevice  where hdcomname='"+com_name+"'";	
	LOG_INFO<<"sql_buffer="<<sql_buffer;
	Query query = conn.query(sql_buffer);
	if (UseQueryResult res = query.use()) {
			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {

			LOG_TRACE<< "hdid:"<<string(row["hdid"]);
			LOG_TRACE<< "hdaddr:"<<string(row["hdaddr"]);
			LOG_TRACE<< "hdtype:"<<string(row["hdtype"]);			
			LOG_TRACE<< "hdname:"<<string(row["hdname"]);
			LOG_TRACE<< "hdsi:"<<string(row["hdsi"]);
			LOG_TRACE<< "hdcomname:"<<string(row["hdcomname"]);
			LOG_TRACE<< "port:"<<string(row["port"]);
			LOG_TRACE<< "roomid:"<<string(row["roomid"]);				
			shared_ptr<harddevice> PTharddevice ( new harddevice());			
			PTharddevice->hdid 			=  string( row["hdid"]);	
			PTharddevice->hdaddr 			=  string( row["hdaddr"]);			
			PTharddevice->hdtype 			=  string( row["hdtype"]);
			PTharddevice->hdname 		=  string( row["hdname"]);
			PTharddevice->hdsi 			=  string( row["hdsi"]);
			PTharddevice->hdcom 			=  string( row["hdcomname"]);

			try{
				PTharddevice->hdport = lexical_cast<int>(row["port"]);
			}catch (bad_lexical_cast &e  ){			
					LOG_ERROR<<"error:"<<e.what();
					PTharddevice->hdport = 0;
			}
			

			try{
				PTharddevice->roomid = lexical_cast<int>(row["roomid"]);
			}catch (bad_lexical_cast &e  ){
					LOG_ERROR<<"error:"<<e.what();					
					PTharddevice->roomid = 0;
			}

		
			LOG_INFO<<"hdtype="<<PTharddevice->hdtype;
			
			shared_ptr<hardtype>  hardtypePtr = Gethardtype(PTharddevice->hdtype,hardtype_list);
			if (!hardtypePtr){		
				LOG_ERROR<<"Gethardtype error ";
				return -1;
			}

			
			PTharddevice->baudrate = hardtypePtr->baudrate;
			PTharddevice->parity = hardtypePtr->parity;		
			PTharddevice->data_bit = hardtypePtr->data_bit;		
			PTharddevice->stop_bit = hardtypePtr->stop_bit;	
			PTharddevice->file_no = hardtypePtr->file_no;	
			PTharddevice->protocol_name 	= hardtypePtr->protocol_name;	
	
			PTharddevice->list 		=parse_router_list(string (row["relayaddr"])) ;
			PTharddevice->list_num 	= (PTharddevice->list).size(); 		
			LOG_INFO<<"relayaddr="<<string(row["relayaddr"]);			
			if (PTharddevice->list.size() >0){
				LOG_INFO<<"list[0]="<<PTharddevice->list[0];
				LOG_INFO<<"PTharddevice->list.size()="<<PTharddevice->list.size();
				if (  PTharddevice->list[0] != "0"  ){
				
					PTharddevice-> router_enable = true;
					if (protocol_has_router(PTharddevice->protocol_name,protocol_manager_list)){
						PTharddevice->protocol_name = PTharddevice->protocol_name+"_router";
					}
				}
				
			}


			if(PTharddevice-> router_enable){				
				shared_ptr<hardtype>  hardtypePtr = Gethardtype("300",hardtype_list);
				if (!hardtypePtr){
					LOG_ERROR<<"Gethardtype error ";
					return -1;
			}	

			PTharddevice->baudrate = hardtypePtr->baudrate;
			PTharddevice->parity = hardtypePtr->parity;		
			PTharddevice->data_bit = hardtypePtr->data_bit;		
			PTharddevice->stop_bit = hardtypePtr->stop_bit;	
			PTharddevice->file_no = hardtypePtr->file_no;
			}
	
			LOG_INFO<<"hdtype: "<<PTharddevice->hdtype;

			/*   need to add error handler  */
			shared_ptr<ghdparamlist>   ptr_ghdparamlist = Gethdparamlist(PTharddevice->hdtype,ghdparamlist_list);

			if  (!ptr_ghdparamlist){
				LOG_ERROR<<"Gethdparamlist error";
				return  -1;
			}
			PTharddevice->gparamlist =  ptr_ghdparamlist;			

			shared_ptr<comharddevice> Ptrcomharddevice = Gethdcomharddevice(PTharddevice->hdcom,g_comharddevice_list);
			if (!Ptrcomharddevice){	
				LOG_ERROR<<"Ptrcomharddevice error";
				return -1;
			}

			PTharddevice->device_name=	Ptrcomharddevice->device_name;
		
			shared_ptr<protocol_manager> ptr_protocol_manager = Getprotocol(PTharddevice->protocol_name,protocol_manager_list); 
			if (!ptr_protocol_manager){
				LOG_ERROR<<PTharddevice->protocol_name<<"is not found";
				return -1;
			}
			
			PTharddevice->Ptprotocol_manager = ptr_protocol_manager;
			

			LOG_INFO<<"PTharddevice->gparamlist->addr_register_num="<<PTharddevice->gparamlist->addr_register_num;
			LOG_INFO<<"PTharddevice->Ptprotocol_manager->protocol_size="<<PTharddevice->Ptprotocol_manager->protocol_size;
			
			PTharddevice->device_data = malloc((PTharddevice->gparamlist->addr_register_num)*(PTharddevice->Ptprotocol_manager->protocol_size));

			memset(PTharddevice->device_data,0,(PTharddevice->gparamlist->addr_register_num)*(PTharddevice->Ptprotocol_manager->protocol_size));
  			PTharddevice->time_now = time(NULL);
			PTharddevice->record_time_now = time(NULL);

			PTharddevice->connect_times =0;
			PTharddevice->connect_fine_times = 0;		
			PTharddevice->hd_connect = 0xffff;
			print_harddevice(PTharddevice);
 			PTcomharddevice->hdharddevices.push_back(PTharddevice);
			
			
		}

			// Check for error: can't distinguish "end of results" and
			// error cases in return from fetch_row() otherwise.
			if (conn.errnum()) {
				LOG_ERROR<<"Error received in fetching a row: "<<conn.error() ;
				return -1;
			}
			
		}	



	LOG_INFO<<"com_name="<<com_name;
	PTcomharddevice->nbharddevice = PTcomharddevice->hdharddevices.size();

	LOG_INFO<<"nbharddevice="<<(PTcomharddevice->nbharddevice);
	
		
	return 0;
}



#if 0

int   thread_server_udp_object::get_harddevice(Connection  conn, shared_ptr<comharddevice>   PTcomharddevice,				
				list<shared_ptr<comharddevice> > &comharddevice_list,
				list<shared_ptr<harddevice> > &harddevice_list)
{	

	LOG_INFO<<"com_name="<<PTcomharddevice->com_name;	
	 string com_name = PTcomharddevice->com_name;
	string sql_buffer = "SELECT  *   FROM harddevice  where hdcomname='"+com_name+"'";	
	LOG_INFO<<"sql_buffer="<<sql_buffer;
	Query query = conn.query(sql_buffer);
	if (UseQueryResult res = query.use()) {
			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {
			LOG_TRACE <<"hdid:"<<string(row["hdid"]);
			LOG_TRACE <<"hdaddr:"<<string(row["hdaddr"]);
			LOG_TRACE <<"hdtype:"<<string(row["hdtype"]);
			LOG_TRACE <<"hdname:"<<string(row["hdname"]);
			LOG_TRACE <<"hdsi:"<<string(row["hdsi"]);
			LOG_TRACE <<"hdcomname:"<<string(row["hdcomname"]);
			LOG_TRACE <<"port:"<<string(row["port"]);
			
				
			shared_ptr<harddevice> PTharddevice ( new harddevice());
			
			PTharddevice->hdid 			=  string( row["hdid"]);	
			PTharddevice->hdaddr 			=  string( row["hdaddr"]);			
			PTharddevice->hdtype 			=  string( row["hdtype"]);
			PTharddevice->hdname 		=  string( row["hdname"]);
			PTharddevice->hdsi 			=  string( row["hdsi"]);
			PTharddevice->hdcom 			=  string( row["hdcomname"]);


			
			try{
				PTharddevice->hdport = lexical_cast<int>(row["port"]);
			}catch (bad_lexical_cast &e  ){
					LOG_ERROR<<"error:"<<e.what();
					PTharddevice->hdport = 0;
			}
			

			PTharddevice->list 		=parse_router_list(string (row["relayaddr"])) ;
			PTharddevice->list_num 	= (PTharddevice->list).size(); 
		
			LOG_INFO<<"relayaddr="<<string(row["relayaddr"]);
			
			if (PTharddevice->list.size() >0){

				LOG_INFO<<"list[0]="<<PTharddevice->list[0];
				LOG_INFO<<"PTharddevice->list.size()"<<PTharddevice->list.size();
				if (  PTharddevice->list[0] != "0"  ){				
					PTharddevice-> router_enable = true;
					if (protocol_has_router(PTharddevice->protocol_name,protocol_manager_list)){
						PTharddevice->protocol_name = PTharddevice->protocol_name+"_router";
					}
				}				
			}


			if(PTharddevice-> router_enable){				
				shared_ptr<hardtype>  hardtypePtr = Gethardtype("300",hardtype_list);
				if (!hardtypePtr){
					LOG_ERROR<<"Gethardtype error ";
					return -1;
			}	

			PTharddevice->baudrate = hardtypePtr->baudrate;
			PTharddevice->parity = hardtypePtr->parity;		
			PTharddevice->data_bit = hardtypePtr->data_bit;		
			PTharddevice->stop_bit = hardtypePtr->stop_bit;	
			PTharddevice->file_no = hardtypePtr->file_no;
			}



			
			LOG_INFO<<"hdtype="<<PTharddevice->hdtype;
			shared_ptr<hardtype>  hardtypePtr = Gethardtype(PTharddevice->hdtype,hardtype_list);
			if (!hardtypePtr){
				LOG_ERROR<<"Gethardtype error ";
				return -1;
			}

			
			PTharddevice->baudrate = hardtypePtr->baudrate;
			PTharddevice->parity = hardtypePtr->parity;		
			PTharddevice->data_bit = hardtypePtr->data_bit;		
			PTharddevice->stop_bit = hardtypePtr->stop_bit;	
			PTharddevice->file_no = hardtypePtr->file_no;	
			PTharddevice->protocol_name 	= hardtypePtr->protocol_name;	
	
	

			LOG_INFO<<"hdtype: "<<PTharddevice->hdtype;

			shared_ptr<comharddevice> Ptrcomharddevice = Gethdcomharddevice(PTharddevice->hdcom,comharddevice_list);
			if (!Ptrcomharddevice){
				LOG_ERROR<<"Ptrcomharddevice error";
				return -1;
			}

			PTharddevice->device_name=	Ptrcomharddevice->device_name;
 		
			Registerharddevice(PTharddevice,harddevice_list);
			
		}

			// Check for error: can't distinguish "end of results" and
			// error cases in return from fetch_row() otherwise.
			if (conn.errnum()) {
				LOG_ERROR<<"Error received in fetching a row: "<<conn.error() ;
				return -1;
			}
			
		}	



	LOG_INFO<<"com_name="<<com_name;
	LOG_INFO<<"harddevice_list size="<<harddevice_list.size();
	return 0;
}

#endif 


int   thread_server_udp_object::get_harddevice(Connection  conn, shared_ptr<comharddevice>   PTcomharddevice,				
				list<shared_ptr<comharddevice> > &comharddevice_list,
				list<shared_ptr<harddevice> > &harddevice_list)
{	
	LOG_INFO<<"com_name="<<PTcomharddevice->com_name;
	
	 string com_name = PTcomharddevice->com_name;

	string sql_buffer = "SELECT  *   FROM harddevice  where hdcomname='"+com_name+"'";
	
	LOG_INFO<<"sql_buffer="<<sql_buffer;

	Query query = conn.query(sql_buffer);

	if (UseQueryResult res = query.use()) {
			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {


			LOG_TRACE<<"hdid:"<<string(row["hdid"]);
			LOG_TRACE<<"hdaddr:"<<string(row["hdaddr"]);
			LOG_TRACE<<"hdtype:"<<string(row["hdtype"]);
			LOG_TRACE<<"hdname:"<<string(row["hdname"]);
			LOG_TRACE<<"hdsi:"<<string(row["hdsi"]);
			LOG_TRACE<<"hdcomname:"<<string(row["hdcomname"]);
			LOG_TRACE<<"hdcomname:"<<string(row["hdcomname"]);
						
			
			shared_ptr<harddevice> PTharddevice ( new harddevice());
			
			PTharddevice->hdid 			=  string( row["hdid"]);	
			PTharddevice->hdaddr 			=  string( row["hdaddr"]);			
			PTharddevice->hdtype 			=  string( row["hdtype"]);
			PTharddevice->hdname 		=  string( row["hdname"]);
			PTharddevice->hdsi 			=  string( row["hdsi"]);
			PTharddevice->hdcom 			=  string( row["hdcomname"]);

			LOG_INFO<<"hdtype="<<PTharddevice->hdtype;

			shared_ptr<hardtype>  hardtypePtr = Gethardtype(PTharddevice->hdtype,hardtype_list);
			if (!hardtypePtr){
				LOG_ERROR<<"Gethardtype error ";
				return -1;
			}

			
			PTharddevice->baudrate = hardtypePtr->baudrate;
			PTharddevice->parity = hardtypePtr->parity;		
			PTharddevice->data_bit = hardtypePtr->data_bit;		
			PTharddevice->stop_bit = hardtypePtr->stop_bit;	
			PTharddevice->file_no = hardtypePtr->file_no;	
			PTharddevice->protocol_name 	= hardtypePtr->protocol_name;	
	
			PTharddevice->list 		=parse_router_list(string (row["relayaddr"])) ;
			PTharddevice->list_num 	= (PTharddevice->list).size(); 
			LOG_INFO<<"relayaddr"<<string(row["relayaddr"]);
			
			if (PTharddevice->list.size() >0){
				LOG_INFO<<"list[0]="<<PTharddevice->list[0];
				LOG_INFO<<"PTharddevice->list.size()="<<PTharddevice->list.size();
				
				if (  PTharddevice->list[0] != "0"  ){
				
					PTharddevice-> router_enable = true;
					if (protocol_has_router(PTharddevice->protocol_name,protocol_manager_list)){
						PTharddevice->protocol_name = PTharddevice->protocol_name+"_router";
					}
				}
				
			}

			if(PTharddevice-> router_enable){				
			
				
				shared_ptr<hardtype>  hardtypePtr = Gethardtype("300",hardtype_list);
				if (!hardtypePtr){

					LOG_ERROR<<"Gethardtype error ";
					return -1;
				}	

			PTharddevice->baudrate = hardtypePtr->baudrate;
			PTharddevice->parity = hardtypePtr->parity;		
			PTharddevice->data_bit = hardtypePtr->data_bit;		
			PTharddevice->stop_bit = hardtypePtr->stop_bit;	
			PTharddevice->file_no = hardtypePtr->file_no;	


			}		



			LOG_INFO<<"hdtype: "<<PTharddevice->hdtype;

			/*   need to add error handler  */
			shared_ptr<ghdparamlist>   ptr_ghdparamlist = Gethdparamlist(PTharddevice->hdtype,ghdparamlist_list);

			if  (!ptr_ghdparamlist){
				LOG_ERROR<<"Gethdparamlist error";
				return  -1;
			}
			PTharddevice->gparamlist =  ptr_ghdparamlist;			

			shared_ptr<comharddevice> Ptrcomharddevice = Gethdcomharddevice(PTharddevice->hdcom,g_comharddevice_list);
			if (!Ptrcomharddevice){
				LOG_ERROR<<"Ptrcomharddevice error";
				return -1;
			}

			PTharddevice->device_name=	Ptrcomharddevice->device_name;
		
			shared_ptr<protocol_manager> ptr_protocol_manager = Getprotocol(PTharddevice->protocol_name,protocol_manager_list); 
			if (!ptr_protocol_manager){
				LOG_ERROR<<PTharddevice->protocol_name<<"is not found";
				return -1;
			}
			
			PTharddevice->Ptprotocol_manager = ptr_protocol_manager;
			

			LOG_INFO<<"PTharddevice->gparamlist->addr_register_num="<<PTharddevice->gparamlist->addr_register_num;
			LOG_INFO<<"PTharddevice->Ptprotocol_manager->protocol_size="<<PTharddevice->Ptprotocol_manager->protocol_size;
			
			PTharddevice->device_data = malloc((PTharddevice->gparamlist->addr_register_num)*(PTharddevice->Ptprotocol_manager->protocol_size));

			memset(PTharddevice->device_data,0,(PTharddevice->gparamlist->addr_register_num)*(PTharddevice->Ptprotocol_manager->protocol_size));
  			PTharddevice->time_now = time(NULL);
			PTharddevice->record_time_now = time(NULL);

			PTharddevice->connect_times =0;
			PTharddevice->connect_fine_times = 0;		
			PTharddevice->hd_connect = 0xffff;
			print_harddevice(PTharddevice);
			Registerharddevice(PTharddevice,harddevice_list);
			
		}

			// Check for error: can't distinguish "end of results" and
			// error cases in return from fetch_row() otherwise.
			if (conn.errnum()) {
				LOG_ERROR<<"Error received in fetching a row: " <<conn.error();
				return -1;
			}
			
		}	



	PTcomharddevice->nbharddevice = PTcomharddevice->hdharddevices.size();

	LOG_INFO<<	"com_name="<<com_name;
	LOG_INFO<<"nbharddevice="<<(PTcomharddevice->nbharddevice);
	
	
	return 0;
}



int   thread_air_control_object::get_harddevice(Connection  conn, shared_ptr<comharddevice>   PTcomharddevice,				
				list<shared_ptr<comharddevice> > &comharddevice_list,
				list<shared_ptr<harddevice> > &harddevice_list)

{	
	LOG_INFO<<"com_name="<<PTcomharddevice->com_name;
	 string com_name = PTcomharddevice->com_name;
	string sql_buffer = "SELECT  *   FROM harddevice  where hdcomname='"+com_name+"'";
	LOG_INFO<<"sql_buffer="<<sql_buffer;
	Query query = conn.query(sql_buffer);
	if (UseQueryResult res = query.use()) {
			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {
			LOG_TRACE<<"hdid:"<<string(row["hdid"]);
			LOG_TRACE<<"hdaddr:"<<string(row["hdaddr"]);
			LOG_TRACE<<"hdtype:"<<string(row["hdtype"]);
			LOG_TRACE<<"hdname:"<<string(row["hdname"]);
			LOG_TRACE<<"hdsi:"<<string(row["hdsi"]);
			LOG_TRACE<<"hdcomname:"<<string(row["hdcomname"]);
			LOG_TRACE<<"hdcomname:"<<string(row["hdcomname"]);
			shared_ptr<harddevice> PTharddevice ( new harddevice());
			PTharddevice->hdid 			=  string( row["hdid"]);	
			PTharddevice->hdaddr 			=  string( row["hdaddr"]);			
			PTharddevice->hdtype 			=  string( row["hdtype"]);
			PTharddevice->hdname 		=  string( row["hdname"]);
			PTharddevice->hdsi 			=  string( row["hdsi"]);
			PTharddevice->hdcom 			=  string( row["hdcomname"]);
			LOG_INFO<<"hdtype="<<PTharddevice->hdtype;
			shared_ptr<hardtype>  hardtypePtr = Gethardtype(PTharddevice->hdtype,hardtype_list);
			if (!hardtypePtr){
				LOG_ERROR<<"Gethardtype error ";
				return -1;
			}
			
			PTharddevice->baudrate = hardtypePtr->baudrate;
			PTharddevice->parity = hardtypePtr->parity;		
			PTharddevice->data_bit = hardtypePtr->data_bit;		
			PTharddevice->stop_bit = hardtypePtr->stop_bit;	
			PTharddevice->file_no = hardtypePtr->file_no;	
			PTharddevice->protocol_name 	= hardtypePtr->protocol_name;	
	
			PTharddevice->list 		=parse_router_list(string (row["relayaddr"])) ;
			PTharddevice->list_num 	= (PTharddevice->list).size(); 
			LOG_INFO<<"relayaddr"<<string(row["relayaddr"]);
			
			if (PTharddevice->list.size() >0){
				LOG_INFO<<"list[0]="<<PTharddevice->list[0];
				LOG_INFO<<"PTharddevice->list.size()="<<PTharddevice->list.size();
				
				if (  PTharddevice->list[0] != "0"  ){
				
					PTharddevice-> router_enable = true;
					if (protocol_has_router(PTharddevice->protocol_name,protocol_manager_list)){
						PTharddevice->protocol_name = PTharddevice->protocol_name+"_router";
					}
				}
				
			}

			if(PTharddevice-> router_enable){				
				shared_ptr<hardtype>  hardtypePtr = Gethardtype("300",hardtype_list);
				if (!hardtypePtr){
					LOG_ERROR<<"Gethardtype error ";
					return -1;
				}	

			PTharddevice->baudrate = hardtypePtr->baudrate;
			PTharddevice->parity = hardtypePtr->parity;		
			PTharddevice->data_bit = hardtypePtr->data_bit;		
			PTharddevice->stop_bit = hardtypePtr->stop_bit;	
			PTharddevice->file_no = hardtypePtr->file_no;	


			}		



			LOG_INFO<<"hdtype: "<<PTharddevice->hdtype;

			/*   need to add error handler  */
			shared_ptr<ghdparamlist>   ptr_ghdparamlist = Gethdparamlist(PTharddevice->hdtype,ghdparamlist_list);

			if  (!ptr_ghdparamlist){
				LOG_ERROR<<"Gethdparamlist error";
				return  -1;
			}
			PTharddevice->gparamlist =  ptr_ghdparamlist;			

			shared_ptr<comharddevice> Ptrcomharddevice = Gethdcomharddevice(PTharddevice->hdcom,g_comharddevice_list);
			if (!Ptrcomharddevice){
				LOG_ERROR<<"Ptrcomharddevice error";
				return -1;
			}

			PTharddevice->device_name=	Ptrcomharddevice->device_name;
		
			shared_ptr<protocol_manager> ptr_protocol_manager = Getprotocol(PTharddevice->protocol_name,protocol_manager_list); 
			if (!ptr_protocol_manager){
				LOG_ERROR<<PTharddevice->protocol_name<<"is not found";
				return -1;
			}
			
			PTharddevice->Ptprotocol_manager = ptr_protocol_manager;
			

			LOG_INFO<<"PTharddevice->gparamlist->addr_register_num="<<PTharddevice->gparamlist->addr_register_num;
			LOG_INFO<<"PTharddevice->Ptprotocol_manager->protocol_size="<<PTharddevice->Ptprotocol_manager->protocol_size;
			
			PTharddevice->device_data = malloc((PTharddevice->gparamlist->addr_register_num)*(PTharddevice->Ptprotocol_manager->protocol_size));

			memset(PTharddevice->device_data,0,(PTharddevice->gparamlist->addr_register_num)*(PTharddevice->Ptprotocol_manager->protocol_size));
  			PTharddevice->time_now = time(NULL);
			PTharddevice->record_time_now = time(NULL);

			PTharddevice->connect_times =0;
			PTharddevice->connect_fine_times = 0;		
			PTharddevice->hd_connect = 0xffff;
			print_harddevice(PTharddevice);
			Registerharddevice(PTharddevice,harddevice_list);
			
		}

			// Check for error: can't distinguish "end of results" and
			// error cases in return from fetch_row() otherwise.
			if (conn.errnum()) {
				LOG_ERROR<<"Error received in fetching a row: " <<conn.error();
				return -1;
			}
			
		}	



	PTcomharddevice->nbharddevice = PTcomharddevice->hdharddevices.size();

	LOG_INFO<<	"com_name="<<com_name;
	LOG_INFO<<"nbharddevice="<<(PTcomharddevice->nbharddevice);
	
	
	return 0;
}

