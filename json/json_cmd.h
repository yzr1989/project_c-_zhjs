#ifndef _JSON_CMD_H_
#define _JSON_CMD_H_


#include <boost/noncopyable.hpp>

class json_op;

class  json_cmd :  public  json_op ,boost::noncopyable
{
public :
	 json_cmd();
	virtual  ~json_cmd();
	 int   json_parse(const char *   json_string ,shared_ptr<thread_object>  ptr_thread_object ,  string &return_string);

  	int    json_encode_cmd(  int string_len,string & return_json_string);
	 
	
};





#endif   /*_JSON_CMD_H_*/
