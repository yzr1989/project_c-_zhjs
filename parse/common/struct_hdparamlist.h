#ifndef  _STRUCT_HDPARAMLIST_H_
#define _STRUCT_HDPARAMLIST_H_

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

using namespace std;
using namespace boost;
class  comharddevice;

class  hdparamlist  : boost::noncopyable
{

public:
	~hdparamlist();
	string hdtype;
	string hdpara;
	string hdparaname;
	int  hdaddr_register;
	vector<string>  hd_bit_field;
	int      hd_bit_offset;
	int    hd_register_length;
	string hddatatype;
	int  hddatalen;
	string colum;

} ; 



  class  ghdparamlist {
 public:
 	~ghdparamlist();
	int nbhdparamlist;
	string hdparamlist_name;	
	int max_addr_register;
	int min_addr_register;
	int addr_register_num;
	vector< shared_ptr<hdparamlist> >hdparamlists;
	
} ;

void  print_ghdparamlist(shared_ptr<ghdparamlist> PTghdparamlist) ;
int   find_addr_register_num(shared_ptr<ghdparamlist> PTghdparamlist) ;
 void  print_hdparamlist(  shared_ptr<hdparamlist>  PThdparamlist) ;
shared_ptr<ghdparamlist>  Gethdparamlist(  string  hdparamlist_name,list<shared_ptr<ghdparamlist> > &ghdparamlist_list);
int Registerghdparamlist( shared_ptr<ghdparamlist> ptghdparamlist,list<shared_ptr<ghdparamlist> > &ghdparamlist_list);
int free_ghdparamlist(list<shared_ptr<ghdparamlist> > &ghdparamlist_list);

void Showghdparamlist(list<shared_ptr<ghdparamlist> > &ghdparamlist_list);

#endif   /*_STRUCT_HDPARAMLIST_H_*/
