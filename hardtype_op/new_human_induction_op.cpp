//#include <iostream>
#include <mysql++.h>
#include <boost/lexical_cast.hpp>
#include <mysql/mysql.h>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "new_human_induction_op.h"


new_human_induction::new_human_induction()
{
	hardtype = "3";
	hardtype_name ="new_human_induction";
	protocol_name = "ds_bus";	
	baudrate = 9600;
	parity = 'N';
	data_bit = 8;
	stop_bit = 1 ;
	file_no =10200 ;	
	//cout  << "new_human_induction"<<endl;	
	LOG_INFO<<"new_human_induction";
}


new_human_induction::~new_human_induction()
{
	
}

#if 0

int new_human_induction::hardtype_op(shared_ptr<harddevice> Ptrhdharddevice, Connection  conn )
{


#if 0

//	int ret =0;
	cout  << "new_human_induction op "<<endl;	

	string sql_cmd =  "select hd_humtime ,hd_temp  from harddevice   where hdid="+ lexical_cast<string>(Ptrhdharddevice->hdid);

	cout  << "sql_cmd ="<<sql_cmd<<endl; 		
 	float hd_temp  =0.0;
	int hd_humtime;
	
	try {

	Query query = conn.query(sql_cmd);

	if (UseQueryResult res = query.use()) {
			// Display header
			cout.setf(ios::left);
			cout << setw(31) << "hd_temp" <<
					setw(31) << "hd_humtime"<<
					  endl << endl;

			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {
				cout << setw(31) << row["hd_temp"] << ' ' <<
					 setw(31) << row["hd_humtime"] << ' '<<
						endl; 
			

			try{
			 hd_temp = lexical_cast<float >( row["hd_temp"]);
			}catch (bad_lexical_cast & e) {
				cout<< "error:"<<e.what()<<endl;
				hd_temp  =0.0;
			}

			try{
			 hd_humtime = lexical_cast<int >( row["hd_humtime"]);
			}catch (bad_lexical_cast & e) {
				cout<< "error:"<<e.what()<<endl;
				hd_humtime  =0;
			}

			cout << "hd_temp="<<hd_temp<<endl;		
 				
		}

			// Check for error: can't distinguish "end of results" and
			// error cases in return from fetch_row() otherwise.
			if (conn.errnum()) {
				cerr << "Error received in fetching a row: " <<
						conn.error() << endl;
				return  -1;
			}
			
		}	

	
		 if (hd_humtime < 2000){
			// we think is somebody

			//sql_cmd = "update  airc set   airctemp="+lexical_cast<string>(hd_temp)+" where hdid="+lexical_cast<string>(Ptrhdharddevice->hdid);
			sql_cmd = "update  airc set   airctemp="+lexical_cast<string>(hd_temp)+"  , airchum='somebody'  where roomid="+lexical_cast<string>(Ptrhdharddevice->roomid);
			cout  << "sql_cmd ="<<sql_cmd<<endl; 		
		
			query = conn.query(sql_cmd);

			if (query.execute()){

			}else {
				cerr << "error: " << query.error() << endl;
			}	
		 }else  if (hd_humtime >= 2000) {
			//nobody
			
			//sql_cmd = "update  airc set   airctemp="+lexical_cast<string>(hd_temp)+" where hdid="+lexical_cast<string>(Ptrhdharddevice->hdid);
			sql_cmd = "update  airc set   airctemp="+lexical_cast<string>(hd_temp)+"   , airchum='nobody'  where roomid="+lexical_cast<string>(Ptrhdharddevice->roomid);

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

#endif 
	
	return 0;
}


#endif 

int new_human_induction_init(list<shared_ptr<hardtype> > &hardtype_list)
{
	shared_ptr<hardtype> Ptr_new_human_induction ( new  new_human_induction());
	Registerhardtype( Ptr_new_human_induction,hardtype_list);
	return 0;	
}
