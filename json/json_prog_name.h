#ifndef  _JSON_PROG_NAME_H_
#define _JSON_PROG_NAME_H_



#include <boost/noncopyable.hpp>


class json_op;

class  json_prog_name :  public  json_op ,boost::noncopyable
{
public :
	
	json_prog_name();
	~json_prog_name();
	 int     json_parse(const char *   json_string,shared_ptr<thread_object>  ptr_thread_object , string &return_string);
	  int  json_encode_prog_name(shared_ptr<thread_object>  ptr_thread_object, string &return_json_string);
};



#endif  /*_JSON_PROG_NAME_H_*/