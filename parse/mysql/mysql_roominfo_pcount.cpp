#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <string.h>  
#include <stdbool.h>
#include <netdb.h>  
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/file.h>
#include <mysql/mysql.h>
#include	<semaphore.h>	
#include <mysql++.h>
#include <boost/lexical_cast.hpp>


#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include <iostream>
#include "get_config_value.h"
#include "config.h"
#include "struct_roominfo.h"
#include "serial_conf.h"
#include "protocol_manager.h"
#include "sting2hex.h"
#include "parse_bit_field.h"
#include "thread_object.h"
#include "mysql_roominfo_pcount.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
#include "struct_roominfo.h"


using namespace mysqlpp ;
using namespace boost;
using namespace boost::gregorian;
using namespace std;


extern list<shared_ptr<comharddevice> > g_comharddevice_list;
class  pcount_op_obj 
{

public:

	int hdid ;
	int port;
	pcount_op_obj()	{
	

	}
	
	pcount_op_obj(int hdid ,  int port){
		this->hdid = hdid ;
		this->port = port;
	}


	~pcount_op_obj(){

	}
};



class pcount_num_obj 
{
public :
	int in_num;
	int out_num;
	pcount_num_obj(){

		
	}

	pcount_num_obj(int in_num , int out_num){

	this->in_num = in_num;
	this->out_num =  out_num;

	}


	~pcount_num_obj(){


	}

};


int   get_pcount_num(Connection  conn ,shared_ptr<roominfo>  PTroominfo,
				shared_ptr<thread_object>  ptr_thread_object)
				
{	
	
	string sql_cmd = "select   in_num,  out_num   from harddevice where roomid="+lexical_cast<string>(PTroominfo->roomid)+" and hdtype = '7';";
	LOG_INFO<<"sql_cmd="<<sql_cmd;
	//int hd_humtime;
	try {

		Query query = conn.query(sql_cmd);
		if (UseQueryResult res = query.use()) {
				// Display header
				cout.setf(ios::left);
				cout << setw(31) << "in_num" 
					 <<setw(31) << "out_num"
					<<endl;

				// Get each row in result set, and print its contents
			while (Row row = res.fetch_row()) {
					cout << setw(31) << row["in_num"] 
						 << setw(31) << row["out_num"]
							<<endl;
				int  in_num ; 
				int  out_num;
				try{
			
				in_num = lexical_cast<int>(row["in_num"]);
				}catch (bad_lexical_cast & e) {
					cout<< "error:"<<e.what()<<endl;
					in_num =0;
				}

			try{
			
				 out_num = lexical_cast<int>(row["out_num"]);
				}catch (bad_lexical_cast & e) {
					cout<< "error:"<<e.what()<<endl;
					out_num =0;
				}			


			shared_ptr<pcount_num_obj>  Ptr_pcount_num_obj (new pcount_num_obj(in_num,out_num)); ;
			PTroominfo->pcount_num_obj_list.push_back(Ptr_pcount_num_obj);
				
	 				
			}

				// Check for error: can't distinguish "end of results" and
				// error cases in return from fetch_row() otherwise.
				if (conn.errnum()) {
					cerr << "Error received in fetching a row: " <<
							conn.error() << endl;
					return -1;
				}
				
			}	

	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions
		cerr << "Conversion error: " << er.what() << endl <<
		"\tretrieved data size: " << er.retrieved <<
		", actual size: " << er.actual_size << endl;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		cerr << "Error: " << er.what() << endl;
		return -1;
	}

//correction_factor

	sql_cmd =  "select   fix_num  ,correction_factor   from roominfo  where roomid="+ lexical_cast<string>(PTroominfo->roomid);
	cout  << "sql_cmd ="<<sql_cmd<<endl; 		
 	int fix_num;
	int correction_factor;
	
	try {

	Query query = conn.query(sql_cmd);

	if (UseQueryResult res = query.use()) {
			// Display header
			cout.setf(ios::left);
			cout << setw(31) << "fix_num"<<" "	
				<< setw(31) << "correction_factor"<<" "	
					 << endl << endl;

			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {
				cout << setw(31) << row["fix_num"] << ' ' 
					<< setw(31) << row["correction_factor"] << ' ' 
					<<endl;
	
			// hd_power = lexical_cast<float >( row["hd_power"]);
			try{
			
				fix_num = lexical_cast<int>(row["fix_num"]);
			}catch ( bad_lexical_cast &e ){
				cout<< "error:"<<e.what()<<endl;
				fix_num = 0;
			}				
	

			try{
			
				correction_factor = lexical_cast<int>(row["correction_factor"]);
			}catch ( bad_lexical_cast &e ){
				cout<< "error:"<<e.what()<<endl;
				correction_factor = 1;
			}
			
 				
		}

			// Check for error: can't distinguish "end of results" and
			// error cases in return from fetch_row() otherwise.
			if (conn.errnum()) {
				cerr << "Error received in fetching a row: " <<
						conn.error() << endl;
				return  -1;
			}
			
		}	


	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions
		cerr << "Conversion error: " << er.what() << endl <<
		"\tretrieved data size: " << er.retrieved <<
		", actual size: " << er.actual_size << endl;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		cerr << "Error: " << er.what() << endl;
		return -1;
	}
	
	int in_num = 0;
	int out_num =0;


	for (uint32_t j = 0 ;j <PTroominfo->pcount_num_obj_list.size();j++ ){

		cout << "in_num="<<PTroominfo->pcount_num_obj_list[j]->in_num<<endl;	
		cout << "out_num="<<PTroominfo->pcount_num_obj_list[j]->out_num<<endl;		
		
		in_num += PTroominfo->pcount_num_obj_list[j]->in_num;
		out_num += PTroominfo->pcount_num_obj_list[j]->out_num;
	}
	
	cout << "fix_num="<<fix_num<<endl;	
	cout << "in_num="<<in_num<<endl;	
	cout << "out_num="<<out_num<<endl;	

	PTroominfo->pcount_num = (in_num-out_num+fix_num)*correction_factor;


	PTroominfo->pcount_num_obj_list.clear();

	cout << "pcount_num="<<PTroominfo->pcount_num<<endl;

		try{
		sql_cmd = "update  roominfo set   pcount_num="+lexical_cast<string>(PTroominfo->pcount_num)+" where roomid="+lexical_cast<string>(PTroominfo->roomid);
		cout  << "sql_cmd ="<<sql_cmd<<endl; 		
	
		Query query = conn.query(sql_cmd);

		if (query.execute()){

		}else {
			cerr << "error: " << query.error() << endl;
		}


	}catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions
		cerr << "Conversion error: " << er.what() << endl <<
		"\tretrieved data size: " << er.retrieved <<
		", actual size: " << er.actual_size << endl;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		cerr << "Error: " << er.what() << endl;
		return -1;
	}
	

	return 0;

}


int   get_hid_list_by_pcount(Connection  conn ,shared_ptr<roominfo>  PTroominfo,
				shared_ptr<thread_object>  ptr_thread_object)

{
	
	string sql_cmd = "select   hdid ,port from  pcount_conf where cmd_type=1 and  pcount_num>="+lexical_cast<string>(PTroominfo->pcount_num)+" and roomid="+lexical_cast<string>(PTroominfo->roomid) ;
	cout << "sql_cmd="<<sql_cmd<<endl;
	//int hd_humtime;
	try {
		Query query = conn.query(sql_cmd);
		if (UseQueryResult res = query.use()) {
				// Display header
				cout.setf(ios::left);
				cout << setw(31) << "hdid" 
					<< setw(31) << "port" 
					<<endl;

				// Get each row in result set, and print its contents
			while (Row row = res.fetch_row()) {
					cout << setw(31) << row["hdid"] 
						<< setw(31) << row["port"]
							<<endl;
				int hdid ; 
				int port;
				try{
			
				hdid = lexical_cast<int>(row["hdid"] );
				}catch (bad_lexical_cast &e  ){
					cout<< "error:"<<e.what()<<endl;
					hdid  = 0;
				}


			try{
			
				port = lexical_cast<int>(row["port"] );
				}catch (bad_lexical_cast &e  ){
					cout<< "error:"<<e.what()<<endl;
					hdid  = 0;
				}
				shared_ptr<pcount_op_obj>  Ptr_pcount_op_obj (new pcount_op_obj(hdid,port)); ;
				PTroominfo->pcount_op_obj_list.push_back(Ptr_pcount_op_obj);
			}

				// Check for error: can't distinguish "end of results" and
				// error cases in return from fetch_row() otherwise.
				if (conn.errnum()) {
					cerr << "Error received in fetching a row: " <<
							conn.error() << endl;
					return -1;
				}
				
			}	

	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions
		cerr << "Conversion error: " << er.what() << endl <<
		"\tretrieved data size: " << er.retrieved <<
		", actual size: " << er.actual_size << endl;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		cerr << "Error: " << er.what() << endl;
		return -1;
	}

	return 0;

}



int op_switch_obj_serial(shared_ptr<harddevice> Ptrhdharddevice,shared_ptr<protocol_manager>  PTprotocol_manager,
		shared_ptr<comharddevice>  comharddevicePtr_mutex,shared_ptr<pcount_op_obj>  Ptrpcount_op_obj)

{
	
	cout << "op_switch_obj_socket"<<endl;
	cout << "PTprotocol_manager name="<<PTprotocol_manager->protocol_name<<endl;
	mutex::scoped_lock lock(comharddevicePtr_mutex->mutex_obj);
	int ret =PTprotocol_manager->protocol_init(Ptrhdharddevice);	
	if (ret  == -1){
		printf("protocol_init error\n");
		PTprotocol_manager->protocol_close(Ptrhdharddevice);	
		return -1;
	}
	PTprotocol_manager->protocol_set_slave(Ptrhdharddevice);
	ret = PTprotocol_manager->switch_off(Ptrhdharddevice,Ptrpcount_op_obj->port);	
	PTprotocol_manager->protocol_close(Ptrhdharddevice);	
	{
		struct timespec ts;		
		ts.tv_nsec = 0;   
		ts.tv_sec = 1;
		nanosleep(&ts,NULL);				
	}	


	
	return ret  ;
}



int op_swtich_obj(shared_ptr<roominfo> PTroominfo,int i,shared_ptr<thread_object>  ptr_thread_object)
{
	int ret =0;
	
	LOG_INFO<< "op_swtich_obj";
	cout <<"hdid="<<PTroominfo->pcount_op_obj_list[i]->hdid<<endl;
	cout <<"port="<<PTroominfo->pcount_op_obj_list[i]->port<<endl;
	LOG_INFO<<"hdid="<<PTroominfo->pcount_op_obj_list[i]->hdid;
	LOG_INFO<<"port="<<PTroominfo->pcount_op_obj_list[i]->port;
	shared_ptr<harddevice> Ptrhdharddevice = Getharddevice_byID( lexical_cast<string>( PTroominfo->pcount_op_obj_list[i]->hdid),ptr_thread_object->harddevice_list);
	if (!Ptrhdharddevice){
		LOG_ERROR<<"Getharddevice_byID error";
		return -1;
	}	
	cout << "hdcom="<<Ptrhdharddevice->hdcom<<endl;
	shared_ptr<comharddevice>  comharddevicePtr	= Gethdcomharddevice(Ptrhdharddevice->hdcom,ptr_thread_object->comharddevice_list);
	if (!comharddevicePtr){
		LOG_ERROR<<"can't get Gethdcomharddevice ";
		return   -1;
	}
	shared_ptr<comharddevice>  comharddevicePtr_mutex  = Gethdcomharddevice(Ptrhdharddevice->hdcom,g_comharddevice_list);
	if (!comharddevicePtr_mutex){
		LOG_ERROR<<"can't get comharddevicePtr_mutex";
		return   -1;
	}
	LOG_INFO<<"protocol_manager_list size="<<ptr_thread_object->protocol_manager_list.size();
	shared_ptr<protocol_manager>  PTprotocol_manager = Getprotocol(Ptrhdharddevice->protocol_name,ptr_thread_object->protocol_manager_list);
	if (!PTprotocol_manager){
		LOG_ERROR<<"Getprotocol error";
		return   -1;
	}	
	LOG_INFO<<"harddevice_list  size="<<ptr_thread_object->harddevice_list.size();
// here add serial op		
	ret = op_switch_obj_serial(Ptrhdharddevice,PTprotocol_manager,comharddevicePtr_mutex,PTroominfo->pcount_op_obj_list[i]);
	return ret ;
	
}




int   pcount_obj_op(Connection  conn ,shared_ptr<roominfo>  PTroominfo,
				shared_ptr<thread_object>  ptr_thread_object)				

{
	
	cout <<PTroominfo->pcount_op_obj_list.size()<<endl;
	for (uint32_t   i = 0 ;i< PTroominfo->pcount_op_obj_list.size();i++){
		LOG_INFO<<"hdid="<<PTroominfo->pcount_op_obj_list[i]->hdid;
		LOG_INFO<<"port="<<PTroominfo->pcount_op_obj_list[i]->port;			
		 int  ret  = op_swtich_obj(PTroominfo,i,ptr_thread_object);
		if (ret ){
			LOG_ERROR<<"op_swtich_obj error";
		}		
	}
		PTroominfo->pcount_op_obj_list.clear();
		return 0;
}


int handle_pcount_op(Connection  conn ,shared_ptr<roominfo>  PTroominfo,
				shared_ptr<thread_object>  ptr_thread_object)
{
	LOG_INFO<< "handle_pcount_op ";
	get_pcount_num(conn,PTroominfo,ptr_thread_object);
	get_hid_list_by_pcount(conn,PTroominfo,ptr_thread_object);
	pcount_obj_op(conn,PTroominfo,ptr_thread_object);
	return 0;
}

