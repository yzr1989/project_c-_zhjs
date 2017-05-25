#ifndef  _JSON_BATTERY_H_
#define _JSON_BATTERY_H_

//#include "json_op.h"

#include <boost/noncopyable.hpp>


class json_op;

class  json_battery :  public  json_op ,boost::noncopyable
{
public :
	
	json_battery();
	~json_battery();
	int  json_parse(const char *   json_string, shared_ptr<thread_object>  ptr_thread_object , string &return_string);
	int    json_encode_battery(string & return_json_string);

};



#endif  /*_JSON_BATTERY_H_*/
