#ifndef   _THREAD_AIR_CONTROL_H_
#define 	_THREAD_AIR_CONTROL_H_

#include <boost/noncopyable.hpp>

//#include "thread_object.h"

class thread_object;

class thread_air_control_object :  public thread_object, boost::noncopyable
{
public :
	thread_air_control_object();
	virtual ~thread_air_control_object();
	int   get_harddevice(Connection  conn, shared_ptr<comharddevice>   PTcomharddevice,				
				list<shared_ptr<comharddevice> > &comharddevice_list,list<shared_ptr<harddevice> > &harddevice_list);

	int parse_comharddevice(list<shared_ptr<comharddevice> > &comharddevice_list ,
			list<shared_ptr<harddevice> > &harddevice_list);
	
};


#endif  /*_THREAD_AIR_CONTROL_H_*/
