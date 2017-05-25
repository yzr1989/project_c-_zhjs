#ifndef  _THREE_PHASE_METER_OP_H_
#define _THREE_PHASE_METER_OP_H_


#include <boost/noncopyable.hpp>

class three_phase_meter  : public  hardtype,boost::noncopyable
{
public :

	three_phase_meter();
	 virtual  ~three_phase_meter();
	//int hardtype_op(shared_ptr<harddevice> Ptrhdharddevice, Connection  conn);

};


int three_phase_meter_init(list<shared_ptr<hardtype> > &hardtype_list);


#endif   /*_THREE_PHASE_METER_OP_H_*/
