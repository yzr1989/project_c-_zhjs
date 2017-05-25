//#include <iostream>
#include <mysql++.h>
#include <boost/lexical_cast.hpp>
#include <mysql/mysql.h>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "pcount_tcp_op.h"

pcount_tcp::pcount_tcp()
{
	hardtype = "7";
	hardtype_name ="pcount_tcp";
	protocol_name = "modbus_tcp";
	baudrate = 9600;
	parity = 'N';
	data_bit = 8;
	stop_bit = 1 ;
	file_no =10200 ;	
	//cout  << "pcount_tcp"<<endl;		
	LOG_INFO<<"pcount_tcp";
}

pcount_tcp::~pcount_tcp()
{

}


int pcount_tcp_init(list<shared_ptr<hardtype> > &hardtype_list)
{
	shared_ptr<hardtype> Ptr_pcount_tcp ( new  pcount_tcp());
	Registerhardtype( Ptr_pcount_tcp,hardtype_list);
	return 0;	
}
