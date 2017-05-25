#ifndef  _XINKE_RELAY_OP_H
#define _XINKE_RELAY_OP_H

#include <boost/noncopyable.hpp>
#include <mysql/mysql.h>

using namespace mysqlpp ;
class hardtype;

class  xinke_relay : public hardtype , boost::noncopyable
{

public : 
	xinke_relay();	
	 virtual  ~xinke_relay();	
	int hardtype_op(shared_ptr<harddevice> Ptrhdharddevice, Connection  conn);


};

int xinke_relay_init(list<shared_ptr<hardtype> > &hardtype_list);


#endif  /*_XINKE_RELAY_OP_H*/
