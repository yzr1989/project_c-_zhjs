//#include <iostream>
#include <mysql++.h>
#include <boost/lexical_cast.hpp>
#include <mysql/mysql.h>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "config.h"
#include "get_config_value.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "measure_jack_op.h"




using namespace std ;
using namespace mysqlpp ;
using namespace boost;

measure_jack::measure_jack()
{
	hardtype = "4";
	hardtype_name ="measure_jack";
	protocol_name = "modbus";
	baudrate = 9600;
	parity = 'N';
	data_bit = 8;
	stop_bit = 1 ;
	file_no =0 ;		
	//cout  << "measure_jack"<<endl;
	LOG_INFO<<"measure_jack";
}

measure_jack::~measure_jack()
{
	
}

// todo   get  mysql handler from main thread 

#if 0


int measure_jack::hardtype_op(shared_ptr<harddevice> Ptrhdharddevice, Connection  conn )
{

	int  hd_power_threshold;

	 int ret =  GetProfileInteger(CONFIG_FILE_NAME, PROG_NAME,"hd_power", hd_power_threshold);
	if (ret == -1){
		hd_power_threshold =20;
	}

//	int ret =0;
	cout  << "measure_jack op "<<endl;	

	string sql_cmd =  "select hd_power from harddevice   where hdid="+ lexical_cast<string>(Ptrhdharddevice->hdid);

	cout  << "sql_cmd ="<<sql_cmd<<endl; 		
 	float hd_power  =0.0;
	try {

	Query query = conn.query(sql_cmd);

	if (UseQueryResult res = query.use()) {
			// Display header
			cout.setf(ios::left);
			cout << setw(31) << "hd_power" 							 
					 << endl << endl;

			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {
				cout << setw(31) << row["hd_power"] << ' ' <<											 
						endl;
	
			 hd_power = lexical_cast<float >( row["hd_power"]);
			cout << "hd_power="<<hd_power<<endl;			
 				
		}

			// Check for error: can't distinguish "end of results" and
			// error cases in return from fetch_row() otherwise.
			if (conn.errnum()) {
				cerr << "Error received in fetching a row: " <<
						conn.error() << endl;
				return  -1;
			}
			
		}	

	
		 if (hd_power <  static_cast<float> (hd_power_threshold)){
			// we think air control is power off 

			sql_cmd = "update  airc set   aircmode=1 where roomid="+lexical_cast<string>(Ptrhdharddevice->roomid);
			cout  << "sql_cmd ="<<sql_cmd<<endl; 		
		
			query = conn.query(sql_cmd);

			if (query.execute()){

			}else {
				cerr << "error: " << query.error() << endl;
			}	
		 }else  if (hd_power >=  static_cast<float>(hd_power_threshold)) {
			// air control is power on 
			
			sql_cmd = "update  airc set   aircmode=0 where roomid="+lexical_cast<string>(Ptrhdharddevice->roomid);
			cout  << "sql_cmd ="<<sql_cmd<<endl; 		

			query = conn.query(sql_cmd);

			if (query.execute()){

			}else {
				cerr << "error: " << query.error() << endl;
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

#endif 

int measure_jack_init(list<shared_ptr<hardtype> > &hardtype_list)
{
	shared_ptr<hardtype> Ptr_measure_jack ( new  measure_jack());
	Registerhardtype( Ptr_measure_jack,hardtype_list);
	return 0;
}

