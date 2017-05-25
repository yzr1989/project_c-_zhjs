#ifndef _TOUCH_SWITCH_OP_H_
#define  _TOUCH_SWITCH_OP_H_

#include <boost/noncopyable.hpp>

class touch_switch  : public  hardtype,boost::noncopyable
{
public :

	touch_switch();
	 virtual  ~touch_switch();
	int hardtype_op(shared_ptr<harddevice> Ptrhdharddevice, Connection  conn);
	
	

	
};


int touch_switch_init(list<shared_ptr<hardtype> > &hardtype_list);



#endif /*_TOUCH_SWITCH_OP_H_*/
