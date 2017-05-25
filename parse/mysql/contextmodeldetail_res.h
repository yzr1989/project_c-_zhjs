#ifndef   _CONTEXTMODELDETAIL_RES_H_
#define  _CONTEXTMODELDETAIL_RES_H_

#include <string>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;


class contextmodeldetail_res{
public :
		int detailid;
		ptime  bgtime;
		ptime edtime;
		string ctrltype;
		int light_on;
		int light_off;
};



#endif   /*_CONTEXTMODELDETAIL_RES_H_*/ 
