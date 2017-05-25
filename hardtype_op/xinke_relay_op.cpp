//#include <iostream>
#include <mysql++.h>
#include <boost/lexical_cast.hpp>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>


#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "xinke_relay_op.h"


using namespace std ;
using namespace mysqlpp ;
using namespace boost;


string bin2str(uint8_t bin){
	string str;

	for (int i=7; i>=0;i--){

		if (bin&(1<<i)){
			str = str+"1";
		}else {
			str = str+"0";
		}

	}

	return str;
}



xinke_relay::xinke_relay()
{
	hardtype = "1002";
	hardtype_name ="xinke_relay";
	protocol_name = "xinke_relay";	
	baudrate = 9600;
	parity = 'N';
	data_bit = 8;
	stop_bit = 1 ;
	file_no =0 ;		
	//cout  << "xinke_relay"<<endl;
	LOG_INFO <<"xinke_relay";
}


xinke_relay::~xinke_relay()
{
	
}


int xinke_relay::hardtype_op(shared_ptr<harddevice> Ptrhdharddevice, Connection  conn )
{
	  uint8_t *hd_mode = (uint8_t*)Ptrhdharddevice->device_data ;
	  //cout << "hd_mode"<<hd_mode[0]<<endl;
	  LOG_INFO<< "hd_mode"<<hd_mode[0];

	 string hd_mode_string = bin2str(hd_mode[0]);

	 string sql_cmd = "update harddevice  set  hdmode='"+hd_mode_string+"'  where hdid="+ lexical_cast<string>( Ptrhdharddevice->hdid);
	 
	
	//cout << "sql_cmd="<<sql_cmd<<endl;
	LOG_INFO<<"sql_cmd="<<sql_cmd;
	
	mysqlpp::Query query = conn.query(sql_cmd);
	
	if (query.execute()){

	}else {
		//cerr << "error: " << query.error() << endl;
		LOG_ERROR<< "error: " << query.error();
		return -1;
	}
	  
	  return 0;
}



int xinke_relay_init(list<shared_ptr<hardtype> > &hardtype_list)
{
	shared_ptr<hardtype> Ptr_xinke_relay ( new  xinke_relay());
	Registerhardtype( Ptr_xinke_relay,hardtype_list);
	return 0;	
}