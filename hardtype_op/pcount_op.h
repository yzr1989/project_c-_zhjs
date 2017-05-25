#ifndef  _PCOUNT_OP_H_
#define   _PCOUNT_OP_H_

class pcount  : public  hardtype{
public :

	pcount();
	 virtual  ~pcount();
};


int pcount_init(list<shared_ptr<hardtype> > &hardtype_list);




#endif   /*_PCOUNT_OP_H_*/