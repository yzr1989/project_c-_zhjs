#ifndef  _JSON_SYSOP_H_
#define _JSON_SYSOP_H_

#include <string>
using namespace std;

class json_op;

class  json_sysop :  public  json_op ,boost::noncopyable
{
public :
	
	json_sysop();
	 virtual ~json_sysop();
	int  json_parse(const char *   json_string ,shared_ptr<thread_object>  ptr_thread_object ,  string &return_string);
	 int  json_encode_sysop(string &return_string);

	
};



#endif  /*_JSON_SYSOP_H_*/
