#ifndef _JSON_NETSET_H_
#define _JSON_NETSET_H_


#include <boost/noncopyable.hpp>

class json_op;

class  json_netset :  public  json_op ,boost::noncopyable
{

public :
	json_netset();
	  virtual   ~json_netset();
 	 int     json_parse(const char *   json_string, shared_ptr<thread_object>  ptr_thread_object , string &return_string);	
};




#endif   /*_JSON_NETSET_H_*/
