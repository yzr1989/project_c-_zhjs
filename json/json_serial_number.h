#ifndef   _JSON_SERIAL_NUMBER_H_
#define 	_JSON_SERIAL_NUMBER_H_

class json_op;

class  json_serial_number :  public  json_op 
{
private:


public :
	json_serial_number();	
	virtual ~json_serial_number();	

	 int   json_parse(const char *   json_string, shared_ptr<thread_object>  ptr_thread_object , string &return_string);
	int    json_encode_get_serial_number(string & return_json_string);
	 
};


  extern "C"   int   json_encode_write_serial_number(const char *string_serial_number);


#endif   /*_JSON_SERIAL_NUMBER_H_*/