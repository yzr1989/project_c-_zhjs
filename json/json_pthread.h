#ifndef _JSON_PTHREAD_H_
#define _JSON_PTHREAD_H_



class json_op;

class  json_thread :  public  json_op ,boost::noncopyable
{
public :
	json_thread();
	~json_thread();
	const   char * json_parse(struct json_object *  obj,shared_ptr<thread_object>  ptr_thread_object);

	 int   json_parse(const char *   json_string,shared_ptr<thread_object>  ptr_thread_object , string &return_string);

	int  json_encode_thread(const char *op_result,string & return_json_string);

	
};



#endif  /*_JSON_PTHREAD_H_*/
