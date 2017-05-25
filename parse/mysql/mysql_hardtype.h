#ifndef  _MYSQL_HARDTYPE_H_
#define  _MYSQL_HARDTYPE_H_


#include <mysql++.h>
#include <mysql/mysql.h>
#include <string>

using namespace mysqlpp;
using namespace std;

int   get_hardtype_from_mysql(Connection  conn , const  char *table_name);


#endif  /*_MYSQL_HARDTYPE_H_*/