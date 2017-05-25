#ifndef  _STRUCT_HARDDEVICE_H_
#define   _STRUCT_HARDDEVICE_H_


#include <stdint.h>
#include <time.h>
#include <pthread.h> 
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
using namespace std;
using  namespace boost;
class   protocol_manager;
class  ghdparamlist;
class  comharddevice;
 
class  harddevice  : boost::noncopyable
{
public :
	~harddevice();
	string hdid;
	string hdaddr;
	string hdtype;
	string hdsi;
	string hdcom;
	string hdname;
	string device_name;
	int  baudrate;
	char parity;
	int data_bit;
	int stop_bit;
	int fisrt_read;
	int record_time;
	int roomid;
	int hdport;	
	string protocol_name;
	time_t    record_time_now;
	shared_ptr<ghdparamlist> gparamlist;
    	void *device_data; 
	time_t    time_now;
	int 		hddelay_time;
	uint16_t  hd_connect;
	uint32_t connect_times;
	uint32_t connect_fine_times;
	bool  router_enable ;
	int file_no ;
	void *ctx;
	int op_flag;
	vector<string> list;
	uint16_t list_num;
	shared_ptr<protocol_manager> Ptprotocol_manager;
} ;




shared_ptr<harddevice>     Getharddevice_byID( string  hdid,list<shared_ptr<harddevice> > &harddevice_list);
 void  print_harddevice(shared_ptr<harddevice>   PTharddevice) ;
int Registerharddevice(shared_ptr<harddevice>  ptharddevice,list<shared_ptr<harddevice> > &harddevice_list);
void Showharddevice(list<shared_ptr<harddevice> > &harddevice_list);
void freeharddevice(list<shared_ptr<harddevice> > &harddevice_list);

void freeharddevice_of_hdharddevices_vect(shared_ptr<comharddevice>   PTcomharddevice);


#endif  /*_STRUCT_HARDDEVICE_H_*/
