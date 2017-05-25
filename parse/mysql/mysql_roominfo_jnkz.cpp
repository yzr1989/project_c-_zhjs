#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <string.h>  
#include <stdbool.h>
#include <netdb.h>  
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/file.h>
#include <mysql/mysql.h>
#include	<semaphore.h>	
#include <mysql++.h>
#include <boost/lexical_cast.hpp>




#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include <iostream>
#include "get_config_value.h"
#include "config.h"
#include "struct_roominfo.h"
#include "serial_conf.h"
#include "protocol_manager.h"
#include "sting2hex.h"
#include "parse_bit_field.h"
#include "thread_object.h"
#include "mysql_roominfo_jnkz.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
#include "struct_roominfo.h"
#include "mysql_roominfo.h"

using namespace mysqlpp ;
using namespace boost;
using namespace boost::gregorian;
using namespace std;
int get_hdid_info(Connection  conn,shared_ptr<roominfo> PTroominfo,shared_ptr<thread_object>  ptr_thread_object);
int get_aircpara_info(Connection  conn,shared_ptr<roominfo> PTroominfo,string hdid );
int op_air_obj(shared_ptr<roominfo> PTroominfo,int i,shared_ptr<thread_object>  ptr_thread_object);

int  bin_string2int(string bin_string)
{
	char string_line[8];	
	int temp=0;
	memcpy(string_line,bin_string.c_str(),sizeof(string_line));

	/*
		下标为0是在最高位
	*/
	for (int i =0;i<=7;i++){
		if (string_line[i] == '1'){			
			temp |= (1<<(6-i));
		}
	}
	cout << temp<<endl;
	return temp;
}






int get_jnkz_modelid_isused(Connection  conn,shared_ptr<roominfo> PTroominfo)
{
	string sql_cmd = "select contextmodelrooms.modelid , modelname  from contextmodelrooms, contextmodel  where roomid = "+  lexical_cast<string>(PTroominfo->roomid)+" and  contextmodelrooms.modelid =contextmodel.modelid and dtbegin <= curdate() and dtend>= curdate() and isused =1;";
	LOG_INFO<<"sql_cmd="<<sql_cmd;
	try {
	Query query = conn.query(sql_cmd);
	if (mysqlpp::StoreQueryResult res = query.store()) {
		LOG_INFO<<"size= "<<res.size();
		if (res.size() ==0 ){
			PTroominfo->isused = false ;
		}else {
			PTroominfo->isused = true ;
		}
		mysqlpp::StoreQueryResult::const_iterator it;
		for (it = res.begin(); it != res.end(); ++it) {
			mysqlpp::Row row = *it;
			LOG_INFO<<'\t' << string(row[0]);
			
		try{
			PTroominfo->modelid= lexical_cast<int>( row[0]);
		}catch (bad_lexical_cast & e) {
			LOG_ERROR<<"error:"<<e.what();
			PTroominfo->modelid=0;
		}

			
		}
	}else {
		LOG_ERROR<<"Failed to get item list: " << query.error() ;
		return -1;
	}	

	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		//cerr << "Query error: " << er.what() << endl;
		LOG_ERROR<<"Query error: " << er.what();
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions
		//cerr << "Conversion error: " << er.what() << endl <<
		//"\tretrieved data size: " << er.retrieved <<
		//", actual size: " << er.actual_size << endl;

		LOG_ERROR<< "Conversion error: " << er.what() << " "<<
		"\tretrieved data size: " << er.retrieved <<
		", actual size: " << er.actual_size ;
		
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		//cerr << "Error: " << er.what() << endl;
		LOG_ERROR<< "Error: " << er.what() ;
		return -1;
	}

	return 0;

}



int get_jnkz_nobody_info(Connection  conn,shared_ptr<roominfo> PTroominfo)
{	

	date d = day_clock::local_day();
	int week = d.day_of_week();
	if (PTroominfo->modelid == 0){
		return -1;
	}
	if (week == 0){
		week =7;
	}

	string sql_cmd = "select   detailid  from contextmodeldetail ,harddevice  where contextmodeldetail.modelid ="+lexical_cast<string>(PTroominfo->modelid)+"  and bgtime <= now() and edtime>= now()  and humen='nobody'  and  ctrltype='shutdown'  and harddevice.roomid = "+lexical_cast<string>(PTroominfo->roomid)+" and  hd_humtime > 200;";
	LOG_INFO<<"sql_cmd="<<sql_cmd;
	try {
	Query query = conn.query(sql_cmd);
	if (mysqlpp::StoreQueryResult res = query.store()) {
		LOG_INFO<<"size= "<<res.size();
		if (res.size() ==0 ){
			PTroominfo->has_nobody_flag = false ;
		}else {
			PTroominfo->has_nobody_flag = true ;
		}
		mysqlpp::StoreQueryResult::const_iterator it;
		for (it = res.begin(); it != res.end(); ++it) {
			mysqlpp::Row row = *it;
			LOG_INFO<< '\t' << string(row[0]);
		}
	}else {
		LOG_ERROR<<"Failed to get item list: " << query.error() ;
		return -1;
	}	

	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		LOG_ERROR<< "Query error: " << er.what();
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions
		//cerr << "Conversion error: " << er.what() << endl <<
		//"\tretrieved data size: " << er.retrieved <<
		//", actual size: " << er.actual_size << endl;
		
		LOG_ERROR<<"Conversion error: " << er.what() << " " <<
		"\tretrieved data size: " << er.retrieved <<
		", actual size: " << er.actual_size ;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		//cerr << "Error: " << er.what() << endl;
		LOG_ERROR<< "Error: " << er.what() ;
		return -1;
	}
	return 0;
	
}






int op_hddevice_list_jnkz_shutdown(Connection  conn ,shared_ptr<roominfo>  PTroominfo,
				shared_ptr<thread_object>  ptr_thread_object)
{
	int ret =0;
	int size = (PTroominfo->harddevice_list).size();
	cout << "size="<<size<<endl;
	for (int i=0;i<size;i++){

			// 关空调
			
		if (PTroominfo->harddevice_list[i]->hdtype == "3"  ){
			ret = get_aircpara_info(conn,PTroominfo,PTroominfo->harddevice_list[i]->hdid);
			if (ret){
				LOG_ERROR<<"get_aircpara_info error";
				break;	
			}
			// aircmode = 0  means on 

		       if (PTroominfo->aircmode == 0){
				ret  = op_air_obj(PTroominfo,i,ptr_thread_object);
				if (ret == -1){
					LOG_ERROR<<"op_air_obj error";
					break;	
				}

			 }
						

		}


		bool op_flag = false;

		for (uint32_t  j =0 ;j< PTroominfo->harddevice_list.size();j++){
			if (PTroominfo->harddevice_list[j]->hdtype == "7"){
				/*有人数计数就true*/
				op_flag = true;
			}
		}

		/*没有人数计数*/

		if (op_flag == false){	
				//关触控开关
		if (PTroominfo->harddevice_list[i]->hdtype == "5"){
			ret = get_touch_switch_info(conn,PTroominfo,PTroominfo->harddevice_list[i]->hdid);
			if (ret){
				LOG_ERROR<<"get_touch_switch_info error";
				break;	
			}
			if (PTroominfo->switch_status !=0){
				ret  = op_touch_switch_obj(PTroominfo,i,ptr_thread_object);
				if (ret ){
					LOG_ERROR<<"op_touch_switch_obj error";
					break;		
				}
			}

		}

			}	
	}	

	return 0;
}



int get_aircpara_info_normal_control(Connection  conn,shared_ptr<roominfo> PTroominfo)
{	
		//TODO
		// 不能一个房间有多个插座 fixup 	

	// aircmode = 0  means on 
	
	string air_mode;
	if (PTroominfo->startupschema  ==  string("hot")){
		{
			air_mode = "B";
			air_mode+=string("01");
			if (PTroominfo->temperature != 0){

				air_mode+= lexical_cast<string>(PTroominfo->temperature_presetting);
				air_mode+=string("03");
			}

			
		}
		
	}


	if (PTroominfo->startupschema  ==  string("cold")){
		{
			air_mode = "B";
			air_mode+=string("02");
			if (PTroominfo->temperature != 0){

				air_mode+= lexical_cast<string>(PTroominfo->temperature_presetting);
				air_mode+=string("03");
			}

			
		}
		
	}


	if (air_mode.empty()   == true){
		LOG_ERROR<< " air_mode is empty";
		return -1;
	}

	LOG_INFO<< "air_mode="<<air_mode;
	string sql_cmd = "select  airckz ,aircmode  from airc , aircpara   where    roomid="+  lexical_cast<string>( PTroominfo->roomid) + " and  airc.airctype =aircpara.airctype  and aircbm='"+air_mode+"';";
	LOG_INFO<<"sql_cmd="<<sql_cmd;
	try {
	Query query = conn.query(sql_cmd);
	if (UseQueryResult res = query.use()) {
			// Display header
			cout.setf(ios::left);
			cout << setw(31) << "airckz" <<	
					setw(31) <<"aircmode"<<
					 	endl << endl;

			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {
				cout << setw(31) << row["airckz"] << ' ' <<	
					setw(31) << row["aircmode"] << ' ' <<
					endl;			
			PTroominfo->airckz = string(row["airckz"]);
			try{	
			
				PTroominfo->aircmode = lexical_cast<int>(row["aircmode"]);

			}catch (bad_lexical_cast &e){
				cout <<"error:"<<e.what()<<endl;
				PTroominfo->aircmode = 1;
			}
			

			
		}

			// Check for error: can't distinguish "end of results" and
			// error cases in return from fetch_row() otherwise.
			if (conn.errnum()) {
				cerr << "Error received in fetching a row: " <<
						conn.error() << endl;
				return -1;
			}
			return 0;
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






int op_hddevice_list_jnkz_normal_control(Connection  conn ,shared_ptr<roominfo>  PTroominfo,
				shared_ptr<thread_object>  ptr_thread_object)
{
	int ret =0;
	int size = (PTroominfo->harddevice_list).size();
	cout << "size="<<size<<endl;
	for (int i=0;i<size;i++){

			//发送恒温控制命令
			
		if (PTroominfo->harddevice_list[i]->hdtype == "3"  ){
		LOG_INFO<<"get_aircpara_info_normal_control";
			  
			ret = get_aircpara_info_normal_control(conn,PTroominfo);
			if (ret){
				LOG_ERROR<<"get_aircpara_info error";
				break;	
			}
			// aircmode = 0  means on 

		       if (PTroominfo->aircmode == 0){
			   	LOG_INFO<<"op_air_obj";
				ret  = op_air_obj(PTroominfo,i,ptr_thread_object);
				if (ret == -1){
					LOG_INFO<<"op_air_obj error";
					break;	
				}
			 }
		}		
	}	
	return 0;
}





/*
	some_flag 有人为true
*/




int   is_someone_detect(Connection  conn ,shared_ptr<roominfo>  PTroominfo,
				shared_ptr<thread_object>  ptr_thread_object)

{
	 int hd_humtime_threshold;
	 int ret =  GetProfileInteger(CONFIG_FILE_NAME, PROG_NAME,"hd_humtime", hd_humtime_threshold);
	if (ret == -1){
		hd_humtime_threshold = 200;
	}
	vector<int>  vector_temp ;
	vector<int> vector_humtime;
 	int hd_humtime;
	int hd_temp;		
	string sql_cmd = "select   hd_humtime  ,hd_temp   from harddevice where roomid="+lexical_cast<string>(PTroominfo->roomid)+" and hdtype = '3';";
	LOG_INFO<<"sql_cmd="<<sql_cmd;	
	//int hd_humtime;
	try {

		Query query = conn.query(sql_cmd);
		if (UseQueryResult res = query.use()) {
				// Display header
				cout.setf(ios::left);
				cout << setw(31) << "hd_humtime" 
					<< setw(31) << "hd_temp"
					<<endl;

				// Get each row in result set, and print its contents
			while (Row row = res.fetch_row()) {
					cout << setw(31) << row["hd_humtime"] 
						<< setw(31) << row["hd_temp"]
							<<endl;

				try{			
				hd_humtime = lexical_cast<int>(row["hd_humtime"]);
				}catch (bad_lexical_cast & e) {
					cout<< "error:"<<e.what()<<endl;
					hd_humtime =65535;
				}
				vector_humtime.push_back(hd_humtime);
				try{
			
				hd_temp = lexical_cast<float>(row["hd_temp"]);
				}catch (bad_lexical_cast & e) {
					cout<< "error:"<<e.what()<<endl;
					hd_temp  =0;
				}
				vector_temp.push_back(hd_temp);
	 				
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
	vector<int>::iterator min_hd_humtime = min_element(vector_humtime.begin(), vector_humtime.end()); 
	PTroominfo->hd_humtime = *min_hd_humtime;
	
	/*达不到阈值 认为是有人*/

	if (PTroominfo->hd_humtime  <hd_humtime_threshold ){
		PTroominfo->some_flag = true;
		cout << "some_flag="<<PTroominfo->some_flag<<endl;
	}else if (PTroominfo->hd_humtime  >=hd_humtime_threshold){
		PTroominfo->some_flag = false;
		cout << "some_flag="<<PTroominfo->some_flag<<endl;
	}
	    for(vector<int>::iterator it = vector_temp.begin(); it != vector_temp.end(); ++it) {
	        hd_temp += *it; 
	    }   
	    hd_temp /= vector_temp.size();
	PTroominfo->hd_temp = hd_temp;	
	return 0;

}


int get_jnkz_nobody_shutdown_info(Connection  conn ,shared_ptr<roominfo>  PTroominfo)
{
	string sql_cmd = "select weekcycle    from     contextmodel, contextmodelrooms ,contextmodeldetail where roomid = "+  lexical_cast<string>( PTroominfo->roomid)+"  and bgtime <= now() and edtime>= now()    and humen='nobody'  and  ctrltype='shutdown'  and contextmodelrooms.modelid =contextmodel.modelid and contextmodeldetail.modelid=contextmodel.modelid  and dtbegin <= curdate() and dtend>= curdate() and isused =1;";
	LOG_INFO<<"sql_cmd="<<sql_cmd;
	try {
	Query query = conn.query(sql_cmd);
	if (mysqlpp::StoreQueryResult res = query.store()) {
		cout << "size= "<<res.size()<<endl;
		/*启用了无人关闭空调的模式*/
		if (res.size() ==0 ){
			PTroominfo->nobody_shutdown_flag = false ;
		}else {
			PTroominfo->nobody_shutdown_flag = true ;
			mysqlpp::StoreQueryResult::const_iterator it;
			for (it = res.begin(); it != res.end(); ++it) {
				mysqlpp::Row row = *it;
				cout << '\t' << row[0] << endl;
				PTroominfo->weekcycle =string( row[0]);
			}	
		}
		

	}else {
		cerr << "Failed to get item list: " << query.error() << endl;
		return -1;
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



int get_jnkz_nobody_normal_control_flag(Connection  conn ,shared_ptr<roominfo>  PTroominfo,  bool all_flag )
{
	string sql_cmd ;
	if (all_flag){
		sql_cmd = "select weekcycle    from     contextmodel, contextmodelrooms ,contextmodeldetail where roomid ="+  lexical_cast<string>( PTroominfo->roomid)+"  and bgtime <= now() and edtime>= now()    and humen='all'  and  ctrltype='normal_control'  and contextmodelrooms.modelid =contextmodel.modelid and contextmodeldetail.modelid=contextmodel.modelid   and dtbegin <= curdate() and dtend>= curdate() and isused =1;";
	}else {
		sql_cmd = "select weekcycle    from     contextmodel, contextmodelrooms ,contextmodeldetail where roomid = "+  lexical_cast<string>( PTroominfo->roomid)+"  and bgtime <= now() and edtime>= now()    and humen='nobody'  and  ctrltype='normal_control'  and contextmodelrooms.modelid =contextmodel.modelid  and contextmodeldetail.modelid=contextmodel.modelid  and dtbegin <= curdate() and dtend>= curdate() and isused =1;";
	}

	LOG_INFO<<"sql_cmd="<<sql_cmd;
	try {
	Query query = conn.query(sql_cmd);
	if (mysqlpp::StoreQueryResult res = query.store()) {
		cout << "size= "<<res.size()<<endl;
		/*启用了无人恒温控制的模式*/

		if (res.size() ==0 ){
			PTroominfo->normal_control_flag = false ;
		}else {
			PTroominfo->normal_control_flag = true ;
			mysqlpp::StoreQueryResult::const_iterator it;
			for (it = res.begin(); it != res.end(); ++it) {
				mysqlpp::Row row = *it;
				cout << '\t' << row[0] << endl;
				PTroominfo->weekcycle =string( row[0]);
			}	
		}
		

	}else {
		cerr << "Failed to get item list: " << query.error() << endl;
		return -1;
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



int get_jnkz_somebody_normal_control_flag(Connection  conn ,shared_ptr<roominfo>  PTroominfo,  bool all_flag )
{
	string sql_cmd ;
	if (all_flag){
		sql_cmd = "select weekcycle    from     contextmodel, contextmodelrooms ,contextmodeldetail where roomid ="+  lexical_cast<string>( PTroominfo->roomid)+"  and bgtime <= now() and edtime>= now()    and humen='all'  and  ctrltype='normal_control'  and contextmodelrooms.modelid =contextmodel.modelid  and contextmodeldetail.modelid=contextmodel.modelid  and dtbegin <= curdate() and dtend>= curdate() and isused =1;";
	}else {
		sql_cmd = "select weekcycle    from     contextmodel, contextmodelrooms ,contextmodeldetail where roomid = "+  lexical_cast<string>( PTroominfo->roomid)+"  and bgtime <= now() and edtime>= now()    and humen='somebody'  and  ctrltype='normal_control'  and contextmodelrooms.modelid =contextmodel.modelid  and contextmodeldetail.modelid=contextmodel.modelid and dtbegin <= curdate() and dtend>= curdate() and isused =1;";
	}
	LOG_INFO<<"sql_cmd="<<sql_cmd;
	try {
	Query query = conn.query(sql_cmd);
	if (mysqlpp::StoreQueryResult res = query.store()) {
		cout << "size= "<<res.size()<<endl;
		/*启用了有人恒温控制的模式*/
		if (res.size() ==0 ){
			PTroominfo->normal_control_flag = false ;
		}else {
			PTroominfo->normal_control_flag = true ;
			mysqlpp::StoreQueryResult::const_iterator it;
			for (it = res.begin(); it != res.end(); ++it) {
				mysqlpp::Row row = *it;
				cout << '\t' << row[0] << endl;
				PTroominfo->weekcycle =string( row[0]);
			}	
		}
	}else {
		cerr << "Failed to get item list: " << query.error() << endl;
		return -1;
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


int get_jnkz_nobody_normal_control_info(Connection  conn ,shared_ptr<roominfo>  PTroominfo,  bool all_flag )
{

	string sql_cmd ;
	sql_cmd = "select  modelname , weekcycle , startupschema ,tempcompare , temperature from contextmodelrooms, contextmodel ,contextmodeldetail   where roomid = "+lexical_cast<string>( PTroominfo->roomid)+"    and  contextmodelrooms.modelid =contextmodel.modelid and dtbegin <= curdate() and dtend>= curdate() and isused =1;";
	LOG_INFO<< "sql_cmd="<<sql_cmd;
	try {
		Query query = conn.query(sql_cmd);
		if (UseQueryResult res = query.use()) {
				// Display header
				cout.setf(ios::left);
				cout << setw(31) << "startupschema" 
					<< setw(31) << "tempcompare"
					<< setw(31) << "temperature"
				<<endl;

				// Get each row in result set, and print its contents
			while (Row row = res.fetch_row()) {
					cout << setw(31) << row["startupschema"] 
						<<setw(31) << row["tempcompare"] 
						<<setw(31) << row["temperature"] 						
							<<endl;
			PTroominfo->startupschema = string(row["startupschema"] );
			PTroominfo->tempcompare = string(row["tempcompare"] );
			try{
			PTroominfo->temperature =  lexical_cast<int>(row["temperature"] );
			}catch (bad_lexical_cast &e){
				cout<< "error:"<<e.what()<<endl;
				PTroominfo->temperature = 0;
			}
	 				
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


int get_jnkz_normal_control_info(Connection  conn ,shared_ptr<roominfo>  PTroominfo)

{
	string sql_cmd ;
	sql_cmd = "select  modelname , weekcycle , startupschema ,tempcompare , temperature from contextmodelrooms, contextmodel ,contextmodeldetail   where roomid = "+lexical_cast<string>( PTroominfo->roomid)+"    and  contextmodelrooms.modelid =contextmodel.modelid and dtbegin <= curdate() and dtend>= curdate() and isused =1 group by temperature ;";
	LOG_INFO<<"sql_cmd="<<sql_cmd;
	try {
		Query query = conn.query(sql_cmd);
		if (UseQueryResult res = query.use()) {
				// Display header
				cout.setf(ios::left);
				cout << setw(31) << "startupschema" 
					<< setw(31) << "tempcompare"
					<< setw(31) << "temperature"
				<<endl;

				// Get each row in result set, and print its contents
			while (Row row = res.fetch_row()) {
					cout << setw(31) << row["startupschema"] 
						<<setw(31) << row["tempcompare"] 
						<<setw(31) << row["temperature"] 						
							<<endl;
			PTroominfo->startupschema = string(row["startupschema"] );
			PTroominfo->tempcompare = string(row["tempcompare"] );

			try{
			PTroominfo->temperature =  lexical_cast<int>(row["temperature"] );
			}catch (bad_lexical_cast &e){
				cout<< "error:"<<e.what()<<endl;
				PTroominfo->temperature = 0;
			}
	 				
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



int get_jnkz_temp_info(Connection  conn ,shared_ptr<roominfo>  PTroominfo)
{

	string sql_cmd = "select   airctemp , airctemp_ys  from airc  where    roomid="+  lexical_cast<string>( PTroominfo->roomid) + " ; ";
	LOG_INFO<<"sql_cmd="<<sql_cmd;
	try {
		Query query = conn.query(sql_cmd);
		if (UseQueryResult res = query.use()) {
				// Display header
				cout.setf(ios::left);
				cout << setw(31) << "airctemp" 
					<< setw(31) << "airctemp_ys"
					
				<<endl;

				// Get each row in result set, and print its contents
			while (Row row = res.fetch_row()) {
					cout << setw(31) << row["airctemp"] 
						<<setw(31) << row["airctemp_ys"] 
							<<endl;
			try{
			PTroominfo->temperature_presetting =  lexical_cast<int>(row["airctemp_ys"] );
			}catch (bad_lexical_cast &e){
				cout<< "error:"<<e.what()<<endl;
				PTroominfo->temperature = 0;
			}
	
				
	 				
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



bool  check_week_enabled(string bin_string,int week)
{
	char string_line[8];	
	bool  temp  =false;
	memcpy(string_line,bin_string.c_str(),sizeof(string_line));

	/*
		下标为0是在最高位
	*/

	for (int i =0;i<=7;i++){
		if (string_line[i] == '1'){
			int j = i+1;
			if(j== week){
				temp = true;
			}
			
		}
	}	

	return temp;
}


int check_week(shared_ptr<roominfo>  PTroominfo)
{

	
	date d = day_clock::local_day();
	int week = d.day_of_week();

	if (week == 0){
		week =7;
	}

	LOG_INFO<<"week="<<week;
	LOG_INFO<<"weekcycle="<<PTroominfo->weekcycle;
	if (PTroominfo->nobody_shutdown_flag  == true){
		if(check_week_enabled(PTroominfo->weekcycle,week) ){
			PTroominfo->nobody_shutdown_flag = true;
		}else {
			PTroominfo->nobody_shutdown_flag = false;
		}
	}

	if (PTroominfo->normal_control_flag  == true){
		if(check_week_enabled(PTroominfo->weekcycle,week) ){
			PTroominfo->normal_control_flag = true;
		}else {
			PTroominfo->normal_control_flag = false;
		}
	}

	return 0;
	
}


int check_temp(shared_ptr<roominfo>  PTroominfo)
{

	LOG_INFO << "tempcompare="<<PTroominfo->tempcompare;
	LOG_INFO <<"hd_temp="<<PTroominfo->hd_temp;
	LOG_INFO <<"temperature="<<PTroominfo->temperature;
	LOG_INFO <<"normal_control_flag="<<PTroominfo->normal_control_flag;
	if (PTroominfo->normal_control_flag  == true){
		if(PTroominfo->startupschema == "hot" ){
			//制热模式
			/*
				要求空调温度要低于设定温度
				但当前温度高于设定温度所以要发送红外控制命令
			*/
			if (PTroominfo->hd_temp > PTroominfo->temperature){
				LOG_INFO <<"超过设定温度";
				PTroominfo->normal_control_flag = true;
			}else if (PTroominfo->hd_temp <= PTroominfo->temperature) {
				LOG_INFO <<"没有达到设定温度";
				PTroominfo->normal_control_flag = false;
			}
			
		}else  if(PTroominfo->startupschema == "cold")  {
			//制冷模式
		/*
				要求空调温度要高于设定温度
				但当前温度低于设定温度所以要发送红外控制命令
			*/
			
		
			if (PTroominfo->hd_temp  <PTroominfo->temperature){
				LOG_INFO <<"低于设定温度";
				PTroominfo->normal_control_flag = true;
			}else {
				LOG_INFO <<"没有达到设定温度";

				PTroominfo->normal_control_flag = false;
			}
		}else {
			
			PTroominfo->normal_control_flag = false;
		}

	}


	return 0;
}


int handle_jnkz_op(Connection  conn ,shared_ptr<roominfo>  PTroominfo,
				shared_ptr<thread_object>  ptr_thread_object)
{
   	is_someone_detect(conn,PTroominfo,ptr_thread_object);
	if(!(PTroominfo->some_flag)) {
		printf("nobody\n");	
		printf("无人关闭空调模式启用\n");
		LOG_INFO<<"shutdown aircontrol when nobody enabled";
		/*通过人体发送红外关闭空调命令*/
	      LOG_INFO<<"op_hddevice_list_jnkz_shutdown";
	      op_hddevice_list_jnkz_shutdown(conn,PTroominfo,ptr_thread_object);			
	}
	return 0;
}
