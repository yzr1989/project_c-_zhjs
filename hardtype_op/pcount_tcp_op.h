#ifndef _PCOUNT_TCP_OP_H_
#define _PCOUNT_TCP_OP_H_

class pcount_tcp  : public  hardtype{
public :

	pcount_tcp();
	 virtual  ~pcount_tcp();
};


int pcount_tcp_init(list<shared_ptr<hardtype> > &hardtype_list);



#endif   /*_PCOUNT_TCP_OP_H_*/