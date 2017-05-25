#include <mysql++.h>
#include <boost/lexical_cast.hpp>
#include <mysql/mysql.h>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "pcount_op.h"

pcount::pcount()
{
	hardtype = "6";
	hardtype_name ="pcount";
	protocol_name = "modbus";
	baudrate = 9600;
	parity = 'N';
	data_bit = 8;
	stop_bit = 1 ;
	file_no =10200 ;	

	LOG_INFO<<"pcount_tcp";
}

pcount::~pcount()
{

}


int pcount_init(list<shared_ptr<hardtype> > &hardtype_list)
{
	shared_ptr<hardtype> Ptr_pcount ( new  pcount());
	Registerhardtype( Ptr_pcount,hardtype_list);
	return 0;	
}

