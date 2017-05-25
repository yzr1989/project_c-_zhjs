#ifndef _SINGLE_PHASE_METER_H_
#define _SINGLE_PHASE_METER_H_

#include <boost/noncopyable.hpp>

class single_phase_meter  : public  hardtype,boost::noncopyable
{
public :

	single_phase_meter();
	virtual   ~single_phase_meter();
	// int hardtype_op(shared_ptr<harddevice> Ptrhdharddevice, Connection  conn);
	
};


int single_phase_meter_init(list<shared_ptr<hardtype> > &hardtype_list);


#endif  /*_SINGLE_PHASE_METER_H_*/

