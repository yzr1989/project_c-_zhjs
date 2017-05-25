#ifndef  _PARSE_JSON_H_
#define  _PARSE_JSON_H_

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

using namespace boost;
using namespace std;

class thread_object ;
class json_op;

 int    parse_json( const  char *  json_string,shared_ptr<thread_object>  ptr_thread_object , string &return_string );
void json_heartbeat_init(map <string,shared_ptr<json_op> >  &json_op_map);
void json_setorder_init(map <string,shared_ptr<json_op> >  &json_op_map);
int parse_json_init(map <string,shared_ptr<json_op> >  &json_op_map);
void json_thread_init(map <string,shared_ptr<json_op> >  &json_op_map);
void json_sysinfo_init(map <string,shared_ptr<json_op> >  &json_op_map);
void json_netinfo_init(map <string,shared_ptr<json_op> >  &json_op_map);
void json_serial_number_init(map <string,shared_ptr<json_op> >  &json_op_map);
void json_netset_init(map <string,shared_ptr<json_op> >  &json_op_map);
void json_probe_init(map <string,shared_ptr<json_op> >  &json_op_map);
int   free_json_object(map <string,shared_ptr<json_op> >  &json_op_map);
void json_sysop_init(map <string,shared_ptr<json_op> >  &json_op_map);
void json_cmd_init(map <string,shared_ptr<json_op> >  &json_op_map);
  int json_error_msg(const char *ordertype, string &return_string);
void json_battery_init(map <string,shared_ptr<json_op> >  &json_op_map);
void json_sysset_init(map <string,shared_ptr<json_op> >  &json_op_map);
void json_update_init(map <string,shared_ptr<json_op> >  &json_op_map);
void json_prog_name_init(map <string,shared_ptr<json_op> >  &json_op_map);
  int json_error_msg(const char *ordertype,  const char *msg,  string &return_string);
	
#endif  /*_PARSE_JSON_H_*/
