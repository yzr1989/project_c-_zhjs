#ifndef _MEASURE_JACK_OP_H_
#define _MEASURE_JACK_OP_H_

#include <boost/noncopyable.hpp>
#include <mysql/mysql.h>

using namespace mysqlpp ;

class hardtype;
class  measure_jack : public hardtype,boost::noncopyable 
{

public : 
	measure_jack();
	 virtual  ~measure_jack();
//	int hardtype_op(shared_ptr<harddevice> Ptrhdharddevice, Connection  conn);


};

int measure_jack_init(list<shared_ptr<hardtype> > &hardtype_list);


#endif   /*_MEASURE_JACK_OP_H_*/