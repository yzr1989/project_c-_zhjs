#ifndef _JSON_HEARTBEAT_H_
#define _JSON_HEARTBEAT_H_

#include <boost/noncopyable.hpp>
//#include "json_op.h"

class json_op;

class  json_heartbeat :  public  json_op ,boost::noncopyable
{
private:
	string time_string;


public :
	json_heartbeat();
	
	virtual ~json_heartbeat();
	int     json_parse(const char *   json_string,shared_ptr<thread_object>  ptr_thread_object ,  string &return_string);
	int    json_encode_heartbeat(string & return_json_string);	
};


#endif /*_JSON_HEARTBEAT_H_*/