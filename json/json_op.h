#ifndef  _JSON_OP_H_
#define _JSON_OP_H_

#include <json/json.h>
#include <string>
#include <boost/shared_ptr.hpp>

using  namespace std;
using  namespace boost;

class thread_object;


class  json_op {

public :
	json_op();
	 virtual  ~json_op();
	string json_op_name;
	virtual  const  char *json_parse (string   json_string){
		const   char *msg = "this string is not json format or this ordertype is not  Implementation";
		return msg;
	}

	virtual  int     json_parse(const char *   json_string, string &return_string){
		return_string = "this string is not json format or this ordertype is not  Implementation";
		return 0;
	}

	virtual  int     json_parse(const char *   json_string,shared_ptr<thread_object>  ptr_thread_object , string &return_string){
		return_string = "this string is not json format or this ordertype is not  Implementation";
		return 0;
	}

	virtual const  char *json_encode (char *);
	
};


shared_ptr<json_op>   GetJsonOp( string json_op_name,map <string,shared_ptr<json_op> >  &json_op_map);
int RegisterJsonOp(shared_ptr<json_op> PTJson_op,map <string,shared_ptr<json_op> >  &json_op_map );
int free_json_op(map <string,shared_ptr<json_op> >  &json_op_map);




#endif  /*_JSON_OP_H_*/
