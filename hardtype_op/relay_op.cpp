//#include <iostream>
#include <mysql++.h>
#include <boost/lexical_cast.hpp>
#include <mysql/mysql.h>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "relay_op.h"

relay::relay()
{
	hardtype = "300";
	hardtype_name ="relay";
	protocol_name = "ds_bus";
	baudrate = 9600;
	parity = 'N';
	data_bit = 8;
	stop_bit = 1 ;
	file_no =10200 ;	
	//cout  << "relay"<<endl;	
	LOG_INFO<<"relay";
}

relay::~relay()
{
	
}

int relay_init(list<shared_ptr<hardtype> > &hardtype_list)
{
	shared_ptr<hardtype> Ptr_relay ( new  relay());
	Registerhardtype( Ptr_relay,hardtype_list);
	return 0;	
}