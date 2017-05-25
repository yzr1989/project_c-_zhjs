#ifndef _RELAY_H_
#define  _RELAY_H_

class relay  : public  hardtype{
public :

	relay();
	 virtual  ~relay();
	
};


int relay_init(list<shared_ptr<hardtype> > &hardtype_list);

#endif  /*_RELAY_H_*/