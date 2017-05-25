#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <string>
#include <vector>
#include <deque> 
#include <list>
#include <mysql/mysql.h>
#include <mysql++.h>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>
#include "struct_hdparamlist.h"
#include "struct_hardtype.h"
#include "parse_bit_field.h"
#include "mysql_hdparamlist.h"
#include "struct_comharddevice.h"

using namespace mysqlpp ;
using namespace std;
using namespace boost;


int   get_ghdparamlist_object_from_mysql(Connection  conn,list<shared_ptr<ghdparamlist> > &ghdparamlist_list )
{


	string sql_cmd = "SELECT hdtype FROM hdparamlist  group by hdtype;";
	LOG_INFO<<"sql_cmd="<<sql_cmd;
	Query query = conn.query(sql_cmd);
	if (UseQueryResult res = query.use()) {
			// Display header
			//cout.setf(ios::left);
			//cout << setw(31) << "hdtype" << endl << endl;

			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {
				//cout << setw(31) << row["hdtype"]  <<endl;	
			LOG_TRACE<<"hdtype:"<<string(row["hdtype"]);	
			shared_ptr<ghdparamlist> Ptr_ghdparamlist ( new ghdparamlist());
			Ptr_ghdparamlist->hdparamlist_name =  string( row["hdtype"]);	
			Registerghdparamlist(Ptr_ghdparamlist,ghdparamlist_list); 				
		}

			// Check for error: can't distinguish "end of results" and
			// error cases in return from fetch_row() otherwise.
			if (conn.errnum()) {
				//cerr << "Error received in fetching a row: " <<
				//		conn.error() << endl;
				LOG_ERROR<<"Error received in fetching a row: " <<conn.error();
				return -1;
			}
			
		}	
	return 0;
}


 
 int     get_hdparamlist (Connection  conn,shared_ptr<ghdparamlist> PTghdparamlist )
{

	string  sql_buffer;	

	if (!PTghdparamlist){
		LOG_ERROR<<"get_hdparamlist error";
		return -1;
	}
	sql_buffer = "SELECT  * FROM  hdparamlist  where hdtype= "+PTghdparamlist->hdparamlist_name;
	LOG_INFO<<"sql_buffer="<<sql_buffer;
	Query query = conn.query(sql_buffer);
	if (UseQueryResult res = query.use()) {
			// Display header

			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {
			LOG_TRACE<<"hdtype:"<<string(row["hdtype"]);
			LOG_TRACE<<"hdpara:"<<string(row["hdpara"]);			
			LOG_TRACE<<"hdtypename:"<<string(row["hdtypename"]);
			LOG_TRACE<<"hdaddr_register:"<<string(row["hdaddr_register"]);
			LOG_TRACE<<"hdlen:"<<string(row["hdlen"]);
			LOG_TRACE<<"hddatatype:"<<string(row["hddatatype"]);
			LOG_TRACE<<"hddatalen:"<<string(row["hddatalen"]);
			LOG_TRACE<<"colum:"<<string(row["colum"]);			
			shared_ptr<hdparamlist> PThdparamlist ( new hdparamlist() ) ;			
			PThdparamlist->hdtype 			=  string( row["hdtype"]);	
			PThdparamlist->hdpara 			=  string( row["hdpara"]);			
			PThdparamlist->hdparaname 		=  string( row["hdtypename"]);			
			try{
			PThdparamlist->hdaddr_register  	= lexical_cast<int>(row["hdaddr_register"]);	
			}catch (bad_lexical_cast & e) {			
				LOG_ERROR<<"error:"<<e.what();
				PThdparamlist->hdaddr_register   =0;
			}
			
			try {
				PThdparamlist->hd_register_length 	= lexical_cast<int  >(row["hdlen"]);	
			}catch (bad_lexical_cast &e){			
				LOG_ERROR<<"error:"<<e.what();
				PThdparamlist->hd_register_length  =0;
			}
			
			PThdparamlist->hddatatype 			=  string( row["hddatatype"]);
			
			try {
			
			PThdparamlist->hddatalen  			= lexical_cast<int>(row["hddatalen"]);	
			}catch (bad_lexical_cast &e){				
				LOG_ERROR<<"error:"<<e.what();
				PThdparamlist->hddatalen =0;
			}
			
			PThdparamlist->colum	 			=  string( row["colum"]);
			PThdparamlist->hd_bit_field	 		=  parse_bit_field(string( row["hd_bit_field"]));	
 	
			PTghdparamlist->hdparamlists.push_back(PThdparamlist);
			
		}

			// Check for error: can't distinguish "end of results" and
			// error cases in return from fetch_row() otherwise.
			if (conn.errnum()) {
				//cerr << "Error received in fetching a row: " <<
				//		conn.error() << endl;
				LOG_ERROR<<"Error received in fetching a row: " <<	conn.error();
				return -1;
			}
			
		}	
	PTghdparamlist->nbhdparamlist = PTghdparamlist->hdparamlists.size();
	find_addr_register_num(PTghdparamlist);
	print_ghdparamlist(PTghdparamlist);	
	return 0;
}


//extern   list<shared_ptr<ghdparamlist> > ghdparamlist_list;

 int get_hdparamlist_object_from_mysql(Connection  conn,list<shared_ptr<ghdparamlist> > &ghdparamlist_list)
{	

	int i =0;	
	int ret =0;
	LOG_INFO<<"Showghdparamlist";
	for (list<shared_ptr<ghdparamlist> >::iterator  pr = ghdparamlist_list.begin(); pr != ghdparamlist_list.end();pr++){
		LOG_INFO<<i++<<" "<<(*pr)->hdparamlist_name;
		ret = get_hdparamlist(conn,*pr);
		if (ret){
			LOG_INFO<<"get_hdparamlist";
			return ret ;
		}
	}

	return 0;	
}
