#ifndef _NEW_HUMAN_INDUCTION_OP_H_
#define _NEW_HUMAN_INDUCTION_OP_H_


#include <boost/noncopyable.hpp>



class new_human_induction : public hardtype ,boost::noncopyable
{
public :
	new_human_induction();
 	 virtual   ~new_human_induction();
	//int hardtype_op(shared_ptr<harddevice> Ptrhdharddevice, Connection  conn );
};


int new_human_induction_init(list<shared_ptr<hardtype> > &hardtype_list);

#endif  /*_NEW_HUMAN_INDUCTION_OP_H_*/

