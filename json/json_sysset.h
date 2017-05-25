#ifndef   _JSON_SYSSET_H_
#define  	_JSON_SYSSET_H_

#include <string>
//#include "json_op.h"

using namespace std;

class json_op;

struct  sysset{
	
	int hd_humtime;
	int loop_time;	
	
};

class  json_sysset :  public  json_op , boost::noncopyable
{
public :
	boost::shared_ptr<struct  sysset>  ptr_sysset ;
	json_sysset();
	~json_sysset();
	 int     json_parse(const char *   json_string,shared_ptr<thread_object>  ptr_thread_object ,  string &return_string);

	int   json_encode_sysset( string &return_string);

};


void json_sysset_init(shared_ptr<thread_object>  ptr_thread_object);

  extern "C"    int sys_config_write_file_int(const char *key_string ,int val,const char *config_file,  const char *group_name);



   extern "C"  int sys_config_write_file_string(const char *key_string ,const char * val_string, const  char *config_file,const char *group_name);




#endif     /*_JSON_SYSSET_H_*/
