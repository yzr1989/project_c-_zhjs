#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <mysql/mysql.h>
#include <mysql++.h>
#include <boost/shared_ptr.hpp>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>


#include "mysql_hardtype.h"
#include "struct_hardtype.h"

using namespace mysqlpp ;
using namespace std;
using namespace boost;




int   get_hardtype_from_mysql(Connection  conn , const  char *table_name)
{	

	
	string sql_buffer = "select * from hardtype as t1 inner join (select hdtype from harddevice ) as t2 where t1.hdtype = t2.hdtype";

	try {

		Query query = conn.query(sql_buffer);

		if (UseQueryResult res = query.use()) {
				// Display header
				cout.setf(ios::left);
				cout << setw(31) << "hdtype" <<
						setw(15) << "hdtypename" <<
						setw(15) << "hardtypever" <<
						setw(15) << "isused" <<
						"hdtype" << endl << endl;

				// Get each row in result set, and print its contents
			while (Row row = res.fetch_row()) {
					cout << setw(31) << row["hdtype"] << ' ' <<
							setw(15) << row["hdtypename"] << ' ' <<
							setw(15) << row["hardtypever"] << ' ' <<
							setw(15) << row["isused"] << ' ' <<
							setw(15) << row["hdtype"] <<
							endl;
		
				shared_ptr<hardtype> Ptr_hardtype ( new  hardtype());
				Ptr_hardtype->hardtype =  string( row["hdtype"]);
				Ptr_hardtype->hardtype_name =  string( row["hdtypename"]) ;
				Registerhardtype( Ptr_hardtype);	
	 				
			}

				// Check for error: can't distinguish "end of results" and
				// error cases in return from fetch_row() otherwise.
				if (conn.errnum()) {
					cerr << "Error received in fetching a row: " <<
							conn.error() << endl;
					return -1;
				}
				
			}	

	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions
		cerr << "Conversion error: " << er.what() << endl <<
		"\tretrieved data size: " << er.retrieved <<
		", actual size: " << er.actual_size << endl;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		cerr << "Error: " << er.what() << endl;
		return -1;
	}

	return 0;
  
}





