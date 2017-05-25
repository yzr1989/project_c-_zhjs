#ifndef _XML_PARSE_MODBUS_H_
#define _XML_PARSE_MODBUS_H_

#include <mysql/mysql.h>
#include <mysql++.h>
#include <string>
#include <boost/shared_ptr.hpp>

using namespace mysqlpp ;
using namespace std;
using namespace boost;

class  comharddevice ;

int get_comharddevice_list( list<shared_ptr<comharddevice> >  &comharddevice_list);

#endif  /*_XML_PARSE_MODBUS_H_*/
