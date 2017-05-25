#ifndef   _MYSQL_HDPARAMLIST_H_
#define _MYSQL_HDPARAMLIST_H_


#include <mysql/mysql.h>
#include <mysql++.h>

//#include "struct_hdparamlist.h"
//#include "struct_hardtype.h"

using namespace mysqlpp ;

class  comharddevice;
class ghdparamlist;


int   get_ghdparamlist_object_from_mysql(Connection  conn,list<shared_ptr<ghdparamlist> > &ghdparamlist_list );

 int get_hdparamlist_object_from_mysql(Connection  conn,list<shared_ptr<ghdparamlist> > &ghdparamlist_list);


#endif   /*_MYSQL_HDPARAMLIST_H_*/
