#ifndef  _JSON_SETORDER_H_
#define 	_JSON_SETORDER_H_

class json_op;

class  json_setorder :  public  json_op ,boost::noncopyable
{
public :
	json_setorder();
	~json_setorder();
 	int     json_parse(const char *   json_string,shared_ptr<thread_object>  ptr_thread_object ,  string &return_string);	 
	int    json_encode_setorder(string & return_json_string);



};



#endif /*_JSON_SETORDER_H_*/
