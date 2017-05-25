#ifndef _THREAD_SERVER_UDP_H_
#define _THREAD_SERVER_UDP_H_

#include <map>
#include <string>
#include <boost/noncopyable.hpp>

class thread_object;
class json_op;

class thread_server_udp_object : public  thread_object , boost::noncopyable
{
public :
	std::map <std::string,shared_ptr<json_op> > json_op_map;	
	thread_server_udp_object();
	~thread_server_udp_object();
	 int   get_harddevice(Connection  conn, shared_ptr<comharddevice>   PTcomharddevice,				
				list<shared_ptr<comharddevice> > &comharddevice_list,list<shared_ptr<harddevice> > &harddevice_list);


};



#endif /*_THREAD_SERVER_UDP_H_*/

