#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <map>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "json_op.h"

using namespace std;

//static   map <string,shared_ptr<json_op> > json_op_map;

int RegisterJsonOp(shared_ptr<json_op> PTJson_op,map <string,shared_ptr<json_op> >  &json_op_map )
{

	if (PTJson_op != NULL){
		json_op_map[PTJson_op->json_op_name] = PTJson_op;
	}else {
		LOG_ERROR<<"PTJson_op  is NULL";
		return -1 ;
	}
	
	return 0;
}

void ShowJson_op(map <string,shared_ptr<json_op> >  &json_op_map )
{	
	 for(map <string, shared_ptr<json_op> >::iterator  iter = json_op_map.begin(); iter != json_op_map.end(); ++iter){
			//cout <<"name"+(*iter).json_op_name<<endl;
	 }

}



shared_ptr<json_op>   GetJsonOp( string json_op_name,map <string,shared_ptr<json_op> >  &json_op_map )
{
	shared_ptr<json_op>  ptTmp = json_op_map[json_op_name];
	return ptTmp;	
}




const char *     json_op::json_encode(char *str)
{
	return NULL;
}


int free_json_op(map <string,shared_ptr<json_op> >  &json_op_map )
{
	json_op_map.clear();
	return 0;
}

json_op::json_op()
{
	
}


json_op::~json_op()
{
	
}