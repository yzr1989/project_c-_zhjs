//#include <iostream>
#include <mysql++.h>
#include <boost/lexical_cast.hpp>
#include <mysql/mysql.h>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "single_phase_meter_op.h"

single_phase_meter::single_phase_meter()
{
	hardtype = "1000";
	hardtype_name ="single_phase_meter";
	protocol_name = "dlt645";
	baudrate = 2400;
	parity = 'E';
	data_bit = 8;
	stop_bit = 1 ;
	file_no =0 ;	
	//cout  << "single_phase_meter"<<endl;	
	LOG_INFO<< "single_phase_meter";
}

single_phase_meter::~single_phase_meter()
{
	
}

#if 0

int single_phase_meter::hardtype_op(shared_ptr<harddevice> Ptrhdharddevice, Connection  conn )
{


//	int ret =0;
	cout  << "single_phase_meter op "<<endl;	

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

	
		 if (hd_power < 10.0){
			// we think air control is power off 

			sql_cmd = "update  airc set   aircmode=1 where hdid="+lexical_cast<string>(Ptrhdharddevice->hdid);
			cout  << "sql_cmd ="<<sql_cmd<<endl; 		
		
			query = conn.query(sql_cmd);

			if (query.execute()){

			}else {
				cerr << "error: " << query.error() << endl;
			}	
		 }else  if (hd_power > 10.0) {
			// air control is power on 
			
			sql_cmd = "update  airc set   aircmode=0 where hdid="+lexical_cast<string>(Ptrhdharddevice->hdid);
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


int single_phase_meter_init(list<shared_ptr<hardtype> > &hardtype_list)
{
	shared_ptr<hardtype> Ptr_single_phase_meter ( new  single_phase_meter());
	Registerhardtype( Ptr_single_phase_meter,hardtype_list);
	return 0;	
}

