#ifndef _MYSQL_PTHREAD_H_
#define _MYSQL_PTHREAD_H_

#include <mysql++.h>
//#include "thread_object.h"

using namespace mysqlpp ;

class thread_object;
class comharddevice;

class  mysql_pthread_object : public thread_object
{
public:
	mysql_pthread_object(shared_ptr<comharddevice>  comharddevicePtr);	

};


#endif   /*_MYSQL_PTHREAD_H_*/