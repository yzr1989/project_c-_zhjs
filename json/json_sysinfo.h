#ifndef   _JSON_SYSINFO_H_
#define 	_JSON_SYSINFO_H_

class json_op;

class  json_sysinfo :  public  json_op ,boost::noncopyable
{
public :
	json_sysinfo();
	virtual ~json_sysinfo();
	  int   json_parse(const char *   json_string , shared_ptr<thread_object>  ptr_thread_object , string &return_string);
	   int   json_encode(string &return_json_string);
};



#endif   /*_JSON_SYSINFO_H_*/
