#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql/mysql.h>
//#include <iostream>
#include <string>
#include <vector>
#include <deque> 
#include <list>
#include <mysql/mysql.h>
#include <mysql++.h>
#include <boost/shared_ptr.hpp>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>


#include "protocol_manager.h"
#include "struct_comharddevice.h"
#include "struct_harddevice.h"
#include "struct_hardtype.h"


using namespace mysqlpp ;
using namespace std;
using  namespace boost;

extern  list<shared_ptr<comharddevice> > comharddevice_list;


int   get_comharddevice_object_from_mysql(Connection  conn, list<shared_ptr<comharddevice> >  &comharddevice_list)
{


	
	string sql_buffer = "select * from hardcom as t1 inner join (select hdcomname from harddevice ) as t2 where t1.hdcomname = t2.hdcomname   group by comid";
	//cout<< sql_buffer<<endl;
	LOG_TRACE<<"sql ="<<sql_buffer;
	Query query = conn.query(sql_buffer);

	if (UseQueryResult res = query.use()) {

			#if 0
			// Display header
			cout.setf(ios::left);
			cout << setw(10) << "hdcomname" <<
				setw(10) << "comid"<<
				setw(10)<<"hdcomdevice"<<
				endl << endl;
			#endif 
			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {

			#if 0
				cout << setw(10) << row["hdcomname"] << ' ' <<
				setw(10) << row["comid"] << ' ' <<
				setw(10) << row["hdcomdevice"] << ' ' <<
						endl;
			#endif 

			//LOG_TRACE<<string(row["hdcomname"]) << ' '<<string(row["comid"]) << ' '<<string(row["hdcomdevice"]) ;
	
			LOG_TRACE<<"hdcomname:"<<string(row["hdcomname"]) ;
			LOG_TRACE<<"comid:"<<string(row["comid"]) ;			
			LOG_TRACE<<"hdcomdevice:"<<string(row["hdcomdevice"]) ;	
			
			//comharddevice   *PTcomharddevice  = new comharddevice();	
			shared_ptr<comharddevice> PTcomharddevice ( new comharddevice());
			
			PTcomharddevice->com_name 		=  string( row["hdcomname"]);	
			PTcomharddevice->comid 			=  string( row["comid"]);			
			PTcomharddevice->device_name 		=  string( row["hdcomdevice"]);
			
			Registercomharddevice(PTcomharddevice,comharddevice_list);			
		}

			// Check for error: can't distinguish "end of results" and
			// error cases in return from fetch_row() otherwise.
			if (conn.errnum()) {
				//cerr << "Error received in fetching a row: " <<
				//		conn.error() << endl;

				LOG_ERROR<<"Error received in fetching a row: " <<conn.error() ;
				return -1;
			}
			
		}	
	return 0;
  
}
