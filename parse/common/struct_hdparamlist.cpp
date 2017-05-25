#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <string>
#include <vector>
#include <deque> 
#include <list>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>
#include "struct_hdparamlist.h"
#include "struct_comharddevice.h"

using namespace std;
using namespace boost;

int Registerghdparamlist(shared_ptr<ghdparamlist> ptghdparamlist,list<shared_ptr<ghdparamlist> > &ghdparamlist_list)
{
	ghdparamlist_list.push_back(ptghdparamlist);
	return 0;
}

void Showghdparamlist(list<shared_ptr<ghdparamlist> > &ghdparamlist_list)
{
	int i = 0;	
	LOG_INFO<<"Showghdparamlist";	
	for (list<shared_ptr<ghdparamlist> >::iterator  pr = ghdparamlist_list.begin(); pr != ghdparamlist_list.end();pr++){
		LOG_INFO<<i++<<" "<<(*pr)->hdparamlist_name;
	}
}






shared_ptr<ghdparamlist>   Gethdparamlist(  string   hdparamlist_name,list<shared_ptr<ghdparamlist> > &ghdparamlist_list)
{
	
	shared_ptr<ghdparamlist>  ptr_ghdparamlist;
	if (hdparamlist_name.empty()){
		return ptr_ghdparamlist;
	}
	int i=0;
	LOG_INFO<<"hdparamlist_name="<<hdparamlist_name;
	for (list<shared_ptr<ghdparamlist> >::iterator  pr = ghdparamlist_list.begin(); pr != ghdparamlist_list.end();pr++){
			LOG_INFO<<i++<<" "<<(*pr)->hdparamlist_name;
		if ((*pr)->hdparamlist_name == hdparamlist_name){
			return (*pr);
		}
	}
	return ptr_ghdparamlist;
	
}


void Removehdparamlist( string dparamlist_name,list<shared_ptr<ghdparamlist> > &ghdparamlist_list)
{
	//todo
}


int   find_addr_register_num(shared_ptr<ghdparamlist> PTghdparamlist) 
{

	int max_addr_register =0;
	int min_addr_register=32767;   /*set the max value*/
	int the_max_register_lenght =0;
	int the_max_register_num =0;
	int i=0;
    	if (PTghdparamlist == NULL) {		
		return -1;
    	}
	LOG_INFO<<"find_addr_register_num";
    	LOG_INFO<<"hdparamlist_name="<<PTghdparamlist->hdparamlist_name;
	LOG_INFO<<"nbhdparamlist="<<PTghdparamlist->nbhdparamlist;
    	for (i = 0;i < PTghdparamlist->nbhdparamlist;i++){
			
		print_hdparamlist(PTghdparamlist->hdparamlists[i]);
		LOG_INFO<<"hdaddr_register="<<PTghdparamlist->hdparamlists[i]->hdaddr_register;
		if (max_addr_register <=  ((PTghdparamlist->hdparamlists[i]->hdaddr_register))){			
			max_addr_register= (PTghdparamlist->hdparamlists[i]->hdaddr_register);
			the_max_register_num =i;
		}
		
		if (min_addr_register >=  ((PTghdparamlist->hdparamlists[i]->hdaddr_register))){
			min_addr_register= (PTghdparamlist->hdparamlists[i]->hdaddr_register);
		}
		
    	}
	LOG_INFO<<"the_max_register_num="<<the_max_register_num;
	LOG_INFO<<"hd_register_length="<<PTghdparamlist->hdparamlists[the_max_register_num]->hd_register_length;
	the_max_register_lenght =(PTghdparamlist->hdparamlists[the_max_register_num]->hd_register_length);
	max_addr_register = max_addr_register+ the_max_register_lenght-1;
	PTghdparamlist->max_addr_register = 	max_addr_register;
	PTghdparamlist->min_addr_register = 	min_addr_register;	
	PTghdparamlist->addr_register_num = max_addr_register-min_addr_register+1;
	return 0;
	
}



 void  print_hdparamlist(  shared_ptr<hdparamlist> PThdparamlist) 
 {

    if (PThdparamlist == NULL) {		
	return;
    }		
	LOG_TRACE<<"=======  hdparamlist==begin";
	LOG_TRACE<<"hdtype:"<<PThdparamlist->hdtype;
	LOG_TRACE<<"hdpara:"<<PThdparamlist->hdpara;
	LOG_TRACE<<"hdparaname: "<<PThdparamlist->hdparaname;
	LOG_TRACE<<"hdaddr_register:"<<PThdparamlist->hdaddr_register;
	LOG_TRACE<<"hd_register_length: "<<PThdparamlist->hd_register_length;
	LOG_TRACE<<"=======  hdparamlist==end";

}




 void  print_ghdparamlist( shared_ptr <ghdparamlist> PTghdparamlist) 
{
    int i;
    if (PTghdparamlist == NULL) {		
	return;
    }		
    LOG_TRACE<<"=======  ghdparamlist==begin";
   LOG_TRACE<<"hdparamlist_name="<<PTghdparamlist->hdparamlist_name;
   LOG_TRACE<<"nbhdparamlist="<<PTghdparamlist->nbhdparamlist;
    for (i = 0;i < PTghdparamlist->nbhdparamlist;i++){
		print_hdparamlist(PTghdparamlist->hdparamlists[i]);		
    }	
	LOG_TRACE<<"max_addr_register="<<PTghdparamlist->max_addr_register;
	LOG_TRACE<<"min_addr_register="<<PTghdparamlist->min_addr_register;
	LOG_TRACE<<"=======  ghdparamlist==end";	
}




 int free_hdparamlist(shared_ptr<ghdparamlist> PTghdparamlist)
{
	PTghdparamlist->hdparamlists.clear();
	return 0;
}

int free_ghdparamlist(list<shared_ptr<ghdparamlist> > &ghdparamlist_list)
{

	for (list<shared_ptr<ghdparamlist> >::iterator  pr = ghdparamlist_list.begin(); pr != ghdparamlist_list.end();pr++){
		LOG_INFO<<(*pr)->hdparamlist_name;
		free_hdparamlist(*pr);
	}
	ghdparamlist_list.clear();
	return 0;
}


ghdparamlist::~ghdparamlist()
{
	LOG_INFO<<"~ghdparamlist="<<hdparamlist_name;
}


 hdparamlist::~hdparamlist()
{
	LOG_INFO<<"~hdparaname="<<hdparaname;
}

