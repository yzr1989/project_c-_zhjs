#ifndef  _STRUCT_HARDTYPE_H_
#define _STRUCT_HARDTYPE_H_


#include <string>
#include <boost/shared_ptr.hpp>
#include <mysql/mysql.h>
#include <mysql++.h>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

//#include "struct_comharddevice.h"

using namespace std ;
using namespace boost;
using namespace mysqlpp ;

class  harddevice ;
class  roominfo;

class  hardtype{
public :	
	virtual  ~hardtype();
	string  hardtype;
	string hardtype_name;
	string protocol_name;
	int  baudrate;
	char parity;
	int data_bit;
	int stop_bit;
	int file_no ;	
	 virtual  int hardtype_op(shared_ptr<harddevice> Ptrhdharddevice, Connection  conn){
		//cout << " not Implement yet!  "<<endl;
		LOG_INFO<< " not Implement yet!  ";
		return 0;
	 }


	 virtual  int hardtype_op(shared_ptr<harddevice> Ptrhdharddevice){
		//cout << " not Implement yet!  "<<endl;
		LOG_INFO<<" not Implement yet!  ";
		
		return 0;
	 }


	virtual  int hardtype_init(shared_ptr<harddevice> Ptrhdharddevice){
		//cout<<"protocol_init not implement yet "<<endl;
		LOG_INFO<<"protocol_init not implement yet ";
		return -1;		
	}   	
	
	virtual int hardtype_set_slave( shared_ptr<harddevice> Ptrhdharddevice){
		//cout<<"protocol_set_slave not implement yet "<<endl;
		LOG_INFO<<"protocol_set_slave not implement yet ";
		return -1;		
	}  


	virtual int hardtype_read_registers(shared_ptr<harddevice> Ptrhdharddevice ){
		//cout<<"protocol_read_registers not implement yet "<<endl;
		LOG_INFO<<"protocol_read_registers not implement yet ";
		return -1;			
	}


	virtual int hardtype_write_registers(shared_ptr<harddevice> Ptrhdharddevice){
		cout<<"protocol_write_registers not implement yet "<<endl;
		return -1;		
	}


	virtual int hardtype_close(shared_ptr<harddevice> Ptrhdharddevice ){
		cout<<"protocol_close not implement yet "<<endl;
		return -1;	
	}


	virtual int hardtype_send_ir_cmd(shared_ptr<harddevice> Ptrhdharddevice){
		cout<<"protocol_send_ir_cmd for roominfo not implement yet "<<endl;
		return -1;			
	}

	virtual int hardtype_send_ir_cmd(shared_ptr<roominfo> PTroominfo,int i){
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

	 
} ;


void Showhardtype(list<shared_ptr<hardtype> > &hardtype_list);
shared_ptr<hardtype>  Gethardtype(string hardtype_name, list<shared_ptr<hardtype> > &hardtype_list);
int Registerhardtype(shared_ptr<hardtype> pthardtype, list<shared_ptr<hardtype> > &hardtype_list);
void freehardtype(void);
int hardtype_op_init(list<shared_ptr<hardtype> > &hardtype_list);

string get_protocol_name_Byhardtype(string  hardtype_str,list<shared_ptr<hardtype> > &hardtype_list);

int free_hardtype_op(list<shared_ptr<hardtype> > &hardtype_list);

#endif   /*_STRUCT_HARDTYPE_H_*/   
