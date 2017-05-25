#ifndef  _STRUCT_COMHARDDEVICE_H_
#define  _STRUCT_COMHARDDEVICE_H_

#include <stdint.h>
#include <time.h>
#include <pthread.h> 
#include <stdbool.h>
#include <mysql/mysql.h>
#include <string>
#include <vector>
#include <mysql/mysql.h>
#include <mysql++.h>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
//#include "struct_harddevice.h"
//#include "struct_comharddevice.h"

using namespace std;
using namespace boost;
using namespace mysqlpp ;


class  harddevice;




  
class  comharddevice  :  boost::noncopyable
{
public :
	comharddevice();
	~comharddevice();	
	string com_name;	
	string comid;
	string device_name;
	int nbharddevice;	
	mutex mutex_obj;

	vector< shared_ptr<harddevice> > hdharddevices;

	

} ;

shared_ptr<comharddevice>   Gethdcomharddevice(  string  com_name,list<shared_ptr<comharddevice> > &comharddevice_list);
int Registercomharddevice(shared_ptr<comharddevice> PTcomharddevice,list<shared_ptr<comharddevice> > &comharddevice_list);
void Showcomharddevice(list<shared_ptr<comharddevice> > &comharddevice_list);

void Showharddevice_attach_com(shared_ptr<comharddevice> PTcomharddevice,list<shared_ptr<comharddevice> > &comharddevice_list);
shared_ptr<comharddevice>   Gethdcomharddevice_by_id( string  comid,list<shared_ptr<comharddevice> > &comharddevice_list);
void freecomharddevice(list<shared_ptr<comharddevice> > &comharddevice_list);


void free_harddevice_of_comharddevice(list<shared_ptr<comharddevice> > &comharddevice_list);




#endif   /*_STRUCT_COMHARDDEVICE_H_*/
