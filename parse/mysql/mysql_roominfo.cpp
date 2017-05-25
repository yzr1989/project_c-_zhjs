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
#include <boost/date_time.hpp>
#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>
#include "config.h"
#include "struct_roominfo.h"
#include "serial_conf.h"
#include "protocol_manager.h"
#include "sting2hex.h"
#include "parse_bit_field.h"
#include "thread_object.h"
#include "mysql_roominfo.h"
#include "mysql_roominfo_jnkz.h"
#include "mysql_roominfo_pcount.h"
#include "get_config_value.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
#include "struct_roominfo.h"


using namespace mysqlpp ;
using namespace boost;
using namespace boost::gregorian;
using namespace std;

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
extern list<shared_ptr<comharddevice> > g_comharddevice_list;

 int   get_roominfo_from_mysql(Connection  conn ,shared_ptr<thread_object>  ptr_thread_object)
{

	string sql_cmd = "select * from roominfo where isused=1 ;";

	LOG_INFO<<"sql_cmd"<<sql_cmd;

	try {

	Query query = conn.query(sql_cmd);

	if (UseQueryResult res = query.use()) {

		
			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {



			LOG_TRACE << "roomid:"<<string(row["roomid"]);
			LOG_TRACE << "roomdm:"<<string(row["roomdm"]);
			LOG_TRACE << "roomname:"<<string(row["roomname"]);
	
			
			shared_ptr<roominfo>  PTRoominfo ( new  roominfo());

			try{	
				PTRoominfo->roomid =   lexical_cast <int>( row["roomid"]);
			}catch (bad_lexical_cast &e){

				LOG_ERROR<<"error:"<<e.what();
				PTRoominfo->roomid =0;
			}	

			PTRoominfo->roomname = string (row["roomname"]);
			// bug  need to fixup 
			
			RegisterRoomInfo( PTRoominfo,ptr_thread_object->roominfo_list);	
 				
		}

			// Check for error: can't distinguish "end of results" and
			// error cases in return from fetch_row() otherwise.
			if (conn.errnum()) {

				LOG_ERROR<<"Error received in fetching a row: "<<conn.error();
				return 1;
			}
			return 0;
		}	

	}
	catch (const mysqlpp::BadQuery& er) {

		LOG_ERROR<< "Query error: " << er.what() ;
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {

		LOG_ERROR<<"Conversion error: " << er.what()  <<" "<<"\tretrieved data size: " << er.retrieved <<
		", actual size: " << er.actual_size ;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {

		LOG_ERROR<< "Error: " << er.what();
		return -1;
	}

	return 0;
}






int get_hdid_info(Connection  conn,shared_ptr<roominfo> PTroominfo,shared_ptr<thread_object>  ptr_thread_object)
{


	
	/*no one*/
	/*hdtype need to fixup*/
	/*  hdtype=2==>  hdtype=4  */

	string sql_cmd = "select   hdid from harddevice  where  roomid ="+  lexical_cast<string>( PTroominfo->roomid) + " ;";
	LOG_INFO<< "sql_cmd="<<sql_cmd;
	try {
	Query query = conn.query(sql_cmd);
	if (UseQueryResult res = query.use()) {		
			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {
			LOG_TRACE<<"hdid:"<<string(row["hdid"] );	
		shared_ptr<harddevice> PTharddevice=Getharddevice_byID(string(row["hdid"] ),ptr_thread_object->harddevice_list);
		if (PTharddevice == NULL){
			LOG_ERROR<<"Getharddevice_byID :"<<string(row["hdid"]);
			return -1;
		}
		LOG_INFO<<	"hdid="<<PTharddevice->hdid<<" "<<"hdtype="<<PTharddevice->hdtype;	
		PTroominfo->harddevice_list.push_back(PTharddevice);		
		}
			// Check for error: can't distinguish "end of results" and
			// error cases in return from fetch_row() otherwise.
			if (conn.errnum()) {
				LOG_ERROR<< "Error received in fetching a row: " <<conn.error();
				return -1;
			}
			return 0;
		}	

	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		LOG_ERROR<< "Query error: " << er.what();
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions
		LOG_ERROR<<"Conversion error: " << er.what() << " " <<
		"\tretrieved data size: " << er.retrieved <<
		", actual size: " << er.actual_size ;
		
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		LOG_ERROR<<"Error: " << er.what();
		return -1;
	}	
	return 0;
	
}

 static int   three_phase_meter_hardtype_op(shared_ptr<harddevice> Ptrhdharddevice,shared_ptr<roominfo> PTroominfo, Connection  conn)
{

	int  hd_power_threshold=30;
	 int ret =  GetProfileInteger(CONFIG_FILE_NAME, PROG_NAME,"hd_power", hd_power_threshold);
	if (ret == -1){
		hd_power_threshold =30;
	}
	LOG_INFO<<"three_phase_meter op ";
	cout <<hd_power_threshold<<endl;
	string sql_cmd =  "select hd_power from harddevice   where hdid="+ lexical_cast<string>(Ptrhdharddevice->hdid);
	LOG_INFO<<"sql_cmd ="<<sql_cmd;
 	float hd_power  =0.0;	
	try {
	Query query = conn.query(sql_cmd);
	if (UseQueryResult res = query.use()) {
			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {
			LOG_TRACE<<"hd_power:"<<string(row["hd_power"]);
			try{
				hd_power = lexical_cast<float >( row["hd_power"]);
			}catch (bad_lexical_cast &e  ){
					LOG_ERROR<<"error:"<<e.what();
					hd_power = 0.0;
			}
			LOG_INFO<<"hd_power="<<hd_power; 				
		}

			// Check for error: can't distinguish "end of results" and
			// error cases in return from fetch_row() otherwise.
			if (conn.errnum()) {
				LOG_ERROR<<"Error received in fetching a row: " <<
						conn.error();
				return  -1;
			}
			
		}	

	
		 if (hd_power < static_cast<float> (hd_power_threshold)){
			// we think air control is power off 
			//sql_cmd = "update  airc set   aircmode=1 where hdid="+lexical_cast<string>(Ptrhdharddevice->hdid);
			sql_cmd = "update  airc set   aircmode=1 where roomid="+lexical_cast<string>(PTroominfo->roomid);
			LOG_INFO<<"sql_cmd ="<<sql_cmd;		
			query = conn.query(sql_cmd);
			if (query.execute()){

			}else {

				LOG_ERROR<<"error: " << query.error();
			}	
		 }else  if (hd_power >=  static_cast<float> (hd_power_threshold)) {
			// air control is power on 
			
			sql_cmd = "update  airc set   aircmode=0 where roomid="+lexical_cast<string>(PTroominfo->roomid);


			LOG_INFO<<"sql_cmd ="<<sql_cmd;

			query = conn.query(sql_cmd);

			if (query.execute()){

			}else {

				LOG_ERROR<< "error: " << query.error();
			}
			
		}	

	

	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		
		LOG_ERROR<<"Query error: " << er.what();
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions


		LOG_ERROR<<"Conversion error: " << er.what() << " " <<
		"\tretrieved data size: " << er.retrieved <<
		", actual size: " << er.actual_size ;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions

		LOG_ERROR<<"Error: " << er.what();
		return -1;
	}

	
	return 0;
}


 static   int new_human_induction_hardtype_op(shared_ptr<harddevice> Ptrhdharddevice,shared_ptr<roominfo> PTroominfo, Connection  conn )
{
 	float hd_temp  =0.0;
	int hd_humtime;
	 int hd_humtime_threshold;

	LOG_INFO<< "new_human_induction op ";

	 int ret =  GetProfileInteger(CONFIG_FILE_NAME, PROG_NAME,"hd_humtime", hd_humtime_threshold);
	if (ret == -1){
		hd_humtime_threshold = 200;
	}

	string sql_cmd =  "select hd_humtime ,hd_temp  from harddevice   where hdid="+ lexical_cast<string>(Ptrhdharddevice->hdid);
	LOG_INFO<<"sql_cmd ="<<sql_cmd;
	try {

	Query query = conn.query(sql_cmd);
	
	if (UseQueryResult res = query.use()) {
	
			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {
			LOG_TRACE<<"hd_temp"<<string(row["hd_temp"]);
			LOG_TRACE<<"hd_humtime"<<string(row["hd_humtime"]);	
			try{
			 hd_temp = lexical_cast<float >( row["hd_temp"]);
			}catch (bad_lexical_cast & e) {		
				LOG_ERROR<<"error:"<<e.what();
				hd_temp  =0.0;
			}
			try{
			 hd_humtime = lexical_cast<int >( row["hd_humtime"]);
			}catch (bad_lexical_cast & e) {		
				LOG_ERROR<<"error:"<<e.what();			
				hd_humtime  =0;
			}	
 				
		}

			// Check for error: can't distinguish "end of results" and
			// error cases in return from fetch_row() otherwise.
			if (conn.errnum()) {	
				LOG_ERROR<< "Error received in fetching a row: " <<conn.error();
				return  -1;
			}
			
		}
	
		 if (hd_humtime < hd_humtime_threshold){
			// we think is somebody
			sql_cmd = "update  airc set   airctemp="+lexical_cast<string>(hd_temp)+"  , airchum='somebody'  where    hdid="+Ptrhdharddevice->hdid +"  and  roomid="+lexical_cast<string>(Ptrhdharddevice->roomid);
			LOG_INFO<<"sql_cmd ="<<sql_cmd;			
			query = conn.query(sql_cmd);
			if (query.execute()){
			}else {
				LOG_ERROR<<"error: " << query.error() ;
			}	
		 }else  if (hd_humtime >= hd_humtime_threshold) {
			//nobody
			
			sql_cmd = "update  airc set   airctemp="+lexical_cast<string>(hd_temp)+"   , airchum='nobody'  where hdid="+Ptrhdharddevice->hdid +" and roomid="+lexical_cast<string>(Ptrhdharddevice->roomid);
			LOG_INFO<< "sql_cmd ="<<sql_cmd;
			query = conn.query(sql_cmd);
			if (query.execute()){
			}else {
				LOG_ERROR << "error: " << query.error() ;
			}			
		}	

	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		LOG_ERROR <<"Query error: " << er.what();
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions

		LOG_ERROR<<"Conversion error: " << er.what() <<" " <<
		"\tretrieved data size: " << er.retrieved <<
		", actual size: " << er.actual_size;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		LOG_ERROR<<"Error: " << er.what() ;
		return -1;
	}
	
	return 0;
}




static    int get_hdid_op(Connection  conn,shared_ptr<roominfo> PTroominfo,shared_ptr<thread_object>  ptr_thread_object)
{

// 更新空调表的状态

	//int ret = 0;
	int size = (PTroominfo->harddevice_list).size();
	for (int i=0;i<size;i++){
			// 根据人体判断房间有人无人			
		if (PTroominfo->harddevice_list[i]->hdtype == "3"  ){
			new_human_induction_hardtype_op(PTroominfo->harddevice_list[i],PTroominfo,conn);	
		}

			//根据三相电表判断空调是否开启
		if (PTroominfo->harddevice_list[i]->hdtype == "1001"){
			three_phase_meter_hardtype_op(PTroominfo->harddevice_list[i],PTroominfo,conn);
		}		
		
	}	

	return 0;

}




int get_timetable_info(Connection  conn,shared_ptr<roominfo> PTroominfo)




{

	date d = day_clock::local_day();
	int week = d.day_of_week();
	// 周日得到的是0
	if (week == 0){
		week =7;
	}

	string sql_cmd = "select timetable.timetable_id ,timetable_detail.detial_timetable_id ,week from timetable, timetable_detail where roomid ="+  lexical_cast<string>(PTroominfo->roomid)+ " and begin_date <= curdate() and end_date>= curdate()  and begin_time <= now() and end_time>= now() and week= "+ lexical_cast<string>(week)+ "    and timetable.timetable_id=timetable_detail.timetable_id and timetable_type = 1 ;";

	LOG_INFO<<"sql_cmd="<<sql_cmd;
	try {
	Query query = conn.query(sql_cmd);
	if (mysqlpp::StoreQueryResult res = query.store()) {
		LOG_INFO<< "size= "<<res.size();
		if (res.size() ==0 ){
			PTroominfo->op_flag = true;
		}		
		mysqlpp::StoreQueryResult::const_iterator it;
		for (it = res.begin(); it != res.end(); ++it) {
			mysqlpp::Row row = *it;
			LOG_INFO<< '\t' << string(row[0]) ;
		}
	}else {
		cerr << "Failed to get item list: " << query.error() << endl;
		LOG_ERROR<<"Failed to get item list: " << query.error();
		return -1;
	}	

	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		LOG_ERROR<< "Query error: " << er.what() ;
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions

		LOG_ERROR<< "Conversion error: " << er.what() <<" "<<
		"\tretrieved data size: " << er.retrieved <<
		", actual size: " << er.actual_size;
		
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		LOG_ERROR<<"Error: " << er.what();
		return -1;
	}
	
	return 0;
	
}




int get_temporary_timetable_info(Connection  conn,shared_ptr<roominfo> PTroominfo)
{

	
	date d = day_clock::local_day();
	int week = d.day_of_week();

	// 周日得到的是0

	if (week == 0){
		week =7;
	}

	//timetable_type 1 正式 2 临时

	string sql_cmd = "select timetable.timetable_id ,timetable_detail.detial_timetable_id ,week from timetable, timetable_detail where roomid ="+  lexical_cast<string>(PTroominfo->roomid)+ " and begin_date <= curdate() and end_date>= curdate()  and begin_time <= now() and end_time>= now() and week= "+ lexical_cast<string>(week)+ "    and timetable.timetable_id=timetable_detail.timetable_id and timetable_type = 2  ;";
	LOG_INFO<<"sql_cmd="<<sql_cmd;
	try {
	Query query = conn.query(sql_cmd);
	if (mysqlpp::StoreQueryResult res = query.store()) {
		LOG_INFO<<"size= "<<res.size();
		if (res.size() ==0 ){
			PTroominfo->temporary_flag = true ;
		}
		mysqlpp::StoreQueryResult::const_iterator it;
		for (it = res.begin(); it != res.end(); ++it) {
			mysqlpp::Row row = *it;
			LOG_INFO<<'\t' <<string( row[0]); 
		}
	}else {
		LOG_ERROR<<"Failed to get item list: " << query.error();
		return -1;
	}	

	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		LOG_ERROR<<"Query error: " << er.what();
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions

		LOG_ERROR<<"Conversion error: " << er.what() << " " <<
		"\tretrieved data size: " << er.retrieved <<
		", actual size: " << er.actual_size;
		
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		LOG_ERROR<<"Error: " << er.what() ;
		
		return -1;
	}
	
	return 0;
	
}


/*

	判断是否有临时课表的自习课


*/

int get_temporary_timetable_self_study_info(Connection  conn,shared_ptr<roominfo> PTroominfo)
{
	date d = day_clock::local_day();
	int week = d.day_of_week();

	// 周日得到的是0

	if (week == 0){
		week =7;
	}

	//timetable_type 1 正式 2 临时

	string sql_cmd = "select timetable.timetable_id ,timetable_detail.detial_timetable_id ,week from timetable, timetable_detail where roomid ="+  lexical_cast<string>(PTroominfo->roomid)+ " and begin_date <= curdate() and end_date>= curdate()  and begin_time <= now() and end_time>= now() and week= "+ lexical_cast<string>(week)+ "    and timetable.timetable_id=timetable_detail.timetable_id and timetable_type = 2  and lesson_type=3;";


	LOG_INFO<<"sql_cmd="<<sql_cmd;

	try {

	Query query = conn.query(sql_cmd);

	if (mysqlpp::StoreQueryResult res = query.store()) {

		//cout << "size= "<<res.size()<<endl;
		LOG_INFO<<"size= "<<res.size();

		if (res.size() >0 ){
			PTroominfo->temporary_timetable_study_self_flag = true ;
		}
		
		mysqlpp::StoreQueryResult::const_iterator it;
		for (it = res.begin(); it != res.end(); ++it) {
			mysqlpp::Row row = *it;
			//cout << '\t' << row[0] << endl;
			LOG_INFO<< '\t' << string(row[0]);
		}
	}else {
		//cerr << "Failed to get item list: " << query.error() << endl;
		LOG_ERROR<< "Failed to get item list: " << query.error();
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
		LOG_ERROR<<"Conversion error: " << er.what() << " " <<
		"\tretrieved data size: " << er.retrieved <<
		", actual size: " << er.actual_size ;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		//cerr << "Error: " << er.what() << endl;
		LOG_ERROR<<"Error: " << er.what() ;
		return -1;
	}


	
	return 0;
	
}



int get_aircpara_info(Connection  conn,shared_ptr<roominfo> PTroominfo,string hdid)
{
	//TODO
	// 不能一个房间有多个插座 fixup 	

	// aircmode = 0  means on 
	
	string sql_cmd  = "select  airckz ,aircmode  from airc , aircpara , harddevice  where    airc.hdid_rt="+hdid+"  and  airc.hdid_rt =harddevice.hdid  and harddevice.roomid="+  lexical_cast<string>( PTroominfo->roomid) + " and  airc.airctype =aircpara.airctype  and aircbm='A02';";

	 LOG_INFO<<"sql_cmd="<<sql_cmd;

	try {

	Query query = conn.query(sql_cmd);

	if (UseQueryResult res = query.use()) {

		
			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {
	
			LOG_TRACE <<"airckz:"<<string(row["airckz"]);
			LOG_TRACE <<"aircmode:"<<string(row["aircmode"]);				
			PTroominfo->airckz = string(row["airckz"]);

			try{	
			
				PTroominfo->aircmode = lexical_cast<int>(row["aircmode"]);

			}catch (bad_lexical_cast &e){
				//cout <<"error:"<<e.what()<<endl;
				LOG_ERROR<<"error:"<<e.what();
				PTroominfo->aircmode = 1;
			}
			

			
		}

			// Check for error: can't distinguish "end of results" and
			// error cases in return from fetch_row() otherwise.
			if (conn.errnum()) {
				//cerr << "Error received in fetching a row: " <<
				//		conn.error() << endl;
				LOG_ERROR<<"Error received in fetching a row: " <<conn.error() ; 
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


 int   op_air_obj_serial(shared_ptr<roominfo> PTroominfo,shared_ptr<protocol_manager>  PTprotocol_manager,int i,
 			shared_ptr<comharddevice>  comharddevicePtr_mutex) 
{
	mutex::scoped_lock lock(comharddevicePtr_mutex->mutex_obj);	
	int  ret =PTprotocol_manager->protocol_init(PTroominfo->harddevice_list[i]);	
	if (ret  == -1){
		LOG_ERROR<<"protocol_init error";
		PTprotocol_manager->protocol_close(PTroominfo->harddevice_list[i]);		
		return -1;
	}

	PTprotocol_manager->protocol_set_slave(PTroominfo->harddevice_list[i]);	
	ret = PTprotocol_manager->protocol_send_ir_cmd(PTroominfo,i);
	PTprotocol_manager->protocol_close(PTroominfo->harddevice_list[i]);
	{
		struct timespec ts;		
		ts.tv_nsec = 0;   
		ts.tv_sec = 3;
		nanosleep(&ts,NULL);				
	}		
	
	return ret ;
	
}




int op_air_obj(shared_ptr<roominfo> PTroominfo,int i,shared_ptr<thread_object>  ptr_thread_object)
{
	int ret =0;
	uint8_t buffer_hex[2000];
	char buffer_hex_temp[2000];
	int length =0;	
	LOG_INFO<<"hdcomname="<<PTroominfo->harddevice_list[i]->hdcom;	
	shared_ptr<comharddevice>  comharddevicePtr	= Gethdcomharddevice(PTroominfo->harddevice_list[i]->hdcom,ptr_thread_object->comharddevice_list);
	if (!comharddevicePtr){
		LOG_ERROR<<"can't get Gethdcomharddevice";
		return   -1;
	}

	shared_ptr<comharddevice>  comharddevicePtr_mutex	= Gethdcomharddevice(PTroominfo->harddevice_list[i]->hdcom,g_comharddevice_list);
	if (!comharddevicePtr_mutex){
		LOG_ERROR<<"can't get comharddevicePtr_mutex";
		return -1;
	}	

	LOG_INFO<<"protocol_name="<<PTroominfo->harddevice_list[i]->protocol_name;	
	shared_ptr<protocol_manager>  PTprotocol_manager = Getprotocol(PTroominfo->harddevice_list[i]->protocol_name,ptr_thread_object->protocol_manager_list);
	if (!PTprotocol_manager){
		LOG_ERROR<<"Getprotocol error";
		return   -1;
	}

	memset(buffer_hex,0,sizeof(buffer_hex));	
	if (PTroominfo->airckz.empty()){
		LOG_ERROR<<"airckz is NULL";
		return -1;
	}	
	
	length = strlen((PTroominfo->airckz).c_str());
	LOG_INFO<<"airckz length="<<strlen((PTroominfo->airckz).c_str());
	if (length == 0){
		LOG_ERROR<<"length=0";
		return -1; 
	}
	
	PTroominfo->airckz_hex_length = length/2;	
	length = hex2bin_trim_len((char  *)buffer_hex,(PTroominfo->airckz).c_str(),strlen((PTroominfo->airckz).c_str()));
	LOG_INFO<<"length="<<length;	
	PTroominfo->airckz_hex_length =length ;
	for ( int  j=0;j<PTroominfo->airckz_hex_length;j++){
		sprintf(buffer_hex_temp,"%x  ",buffer_hex[j]);
	}
	//TODO
	LOG_INFO<<"buffer_hex="<<buffer_hex_temp;
	//puts("\r\n");
	
	//  根据 memwatch 的log 
	// 	overflow: <18> mysql_roominfo.c(456), 1 bytes alloc'd at <17> mysql_roominfo.c(434)
	//   malloc 增加 4字节
	PTroominfo->airckz_hex_buffer = (uint8_t * )malloc(PTroominfo->airckz_hex_length+4);
	memcpy(PTroominfo->airckz_hex_buffer,buffer_hex,PTroominfo->airckz_hex_length);	
	

//add  serial op here 
	ret = op_air_obj_serial(PTroominfo,PTprotocol_manager,i,comharddevicePtr_mutex);	
	free(PTroominfo->airckz_hex_buffer);	
	return ret ;
	
}

int op_relay_obj_serial(shared_ptr<roominfo> PTroominfo,shared_ptr<protocol_manager>  PTprotocol_manager,int i,
 			shared_ptr<comharddevice>  comharddevicePtr_mutex)

{
	mutex::scoped_lock lock(comharddevicePtr_mutex->mutex_obj);
	//io_service ios;	
	int ret =PTprotocol_manager->protocol_init(PTroominfo->harddevice_list[i]);	
	if (ret  == -1){	
		LOG_ERROR<<"protocol_init error";
		PTprotocol_manager->protocol_close(PTroominfo->harddevice_list[i]);	
		return -1;
	}

	PTprotocol_manager->protocol_set_slave(PTroominfo->harddevice_list[i]);
	ret = PTprotocol_manager->relay_off(PTroominfo->harddevice_list[i]);	
	PTprotocol_manager->protocol_close(PTroominfo->harddevice_list[i]);	
	{
		struct timespec ts;		
		ts.tv_nsec = 0;   
		ts.tv_sec = 1;
		nanosleep(&ts,NULL);				
	}	
	
	return ret ;
}


int op_relay_obj(shared_ptr<roominfo> PTroominfo,int i,shared_ptr<thread_object>  ptr_thread_object)
{

	int ret =0;
	LOG_ERROR<<"hdcomname="<<PTroominfo->harddevice_list[i]->hdcom;
	shared_ptr<comharddevice>  comharddevicePtr	= Gethdcomharddevice(PTroominfo->harddevice_list[i]->hdcom,ptr_thread_object->comharddevice_list);
	if (!comharddevicePtr){	
		LOG_ERROR<<"can't get Gethdcomharddevice";
		return   -1;
	}

	shared_ptr<comharddevice>  comharddevicePtr_mutex  = Gethdcomharddevice(PTroominfo->harddevice_list[i]->hdcom,g_comharddevice_list);
	if (!comharddevicePtr_mutex){	
		LOG_ERROR<<"can't get comharddevicePtr_mutex";
		return   -1;
	}
	
	shared_ptr<protocol_manager>  PTprotocol_manager = Getprotocol(PTroominfo->harddevice_list[i]->protocol_name,ptr_thread_object->protocol_manager_list);
	if (!PTprotocol_manager){	
		LOG_ERROR<<"Getprotocol error";
		return   -1;
	}
// here add serial op		
	ret = op_relay_obj_serial(PTroominfo,PTprotocol_manager,i,comharddevicePtr_mutex);
	return ret ;
	
}



int get_relay_info(Connection  conn,shared_ptr<roominfo> PTroominfo)
{

	string sql_cmd  = "select relay_status from  harddevice  where roomid = "+  lexical_cast<string>(PTroominfo->roomid)+" and hdtype = 1002  ;";
	try {
	Query query = conn.query(sql_cmd);
	if (UseQueryResult res = query.use()) {
			// Display header
			cout.setf(ios::left);
			cout << setw(31) << "relay_status" <<				
					 	endl << endl;

			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {
				cout << setw(31) << row["relay_status"] << ' ' <<	
					endl;			

			try{	
				PTroominfo->relay_status =   lexical_cast<int >(row["relay_status"]);
			}catch (bad_lexical_cast &e){
				cout <<"error:"<<e.what()<<endl;
				PTroominfo->relay_status =0;
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







int op_touch_switch_obj_serial(shared_ptr<roominfo> PTroominfo,shared_ptr<protocol_manager>  PTprotocol_manager,int i,
 			shared_ptr<comharddevice>  comharddevicePtr_mutex)

{

	mutex::scoped_lock lock(comharddevicePtr_mutex->mutex_obj);
	int ret =PTprotocol_manager->protocol_init(PTroominfo->harddevice_list[i]);	
	if (ret  == -1){
		//printf("protocol_init error\n");
		LOG_ERROR<<"protocol_init error";
		PTprotocol_manager->protocol_close(PTroominfo->harddevice_list[i]);	
		return -1;
	}

	PTprotocol_manager->protocol_set_slave(PTroominfo->harddevice_list[i]);
	ret = PTprotocol_manager->relay_off(PTroominfo->harddevice_list[i]);	
	PTprotocol_manager->protocol_close(PTroominfo->harddevice_list[i]);	
	{
		struct timespec ts;		
		ts.tv_nsec = 0;   
		ts.tv_sec = 1;
		nanosleep(&ts,NULL);				
	}		
	return ret ;
}



int op_touch_switch_obj(shared_ptr<roominfo> PTroominfo,int i,shared_ptr<thread_object>  ptr_thread_object)
{

	int ret =0;
	LOG_INFO<<"hdcomname="<<PTroominfo->harddevice_list[i]->hdcom;
	shared_ptr<comharddevice>  comharddevicePtr	= Gethdcomharddevice(PTroominfo->harddevice_list[i]->hdcom,ptr_thread_object->comharddevice_list);
	if (!comharddevicePtr){	
		LOG_ERROR<<"can't get Gethdcomharddevice ";
		return   -1;
	}
	shared_ptr<comharddevice>  comharddevicePtr_mutex  = Gethdcomharddevice(PTroominfo->harddevice_list[i]->hdcom,g_comharddevice_list);
	if (!comharddevicePtr_mutex){	
		LOG_ERROR<<"can't get comharddevicePtr_mutex";
		return   -1;	
	}	
	shared_ptr<protocol_manager>  PTprotocol_manager = Getprotocol(PTroominfo->harddevice_list[i]->protocol_name,ptr_thread_object->protocol_manager_list);
	if (!PTprotocol_manager){		
		LOG_ERROR<<"Getprotocol error";
		return   -1;
	}	

// here add serial op		
	ret = op_touch_switch_obj_serial(PTroominfo,PTprotocol_manager,i,comharddevicePtr_mutex);
	return ret ;
	
}





int op_touch_switch_obj_serial_permission_led_clear(shared_ptr<roominfo> PTroominfo,shared_ptr<protocol_manager>  PTprotocol_manager,int i,
 			shared_ptr<comharddevice>  comharddevicePtr_mutex)

{	
	mutex::scoped_lock lock(comharddevicePtr_mutex->mutex_obj);
	int ret =PTprotocol_manager->protocol_init(PTroominfo->harddevice_list[i]);	
	if (ret  == -1){
		//printf("protocol_init error\n");
		LOG_ERROR<<"protocol_init error";
		PTprotocol_manager->protocol_close(PTroominfo->harddevice_list[i]);	
		return -1;
	}

	PTprotocol_manager->protocol_set_slave(PTroominfo->harddevice_list[i]);
	ret = PTprotocol_manager->permission_led_clear(PTroominfo->harddevice_list[i]);	
	PTprotocol_manager->protocol_close(PTroominfo->harddevice_list[i]);	
	{
		struct timespec ts;		
		ts.tv_nsec = 0;   
		ts.tv_sec = 1;
		nanosleep(&ts,NULL);				
	}		
	return ret ;
}


int op_touch_switch_permission_led_obj(shared_ptr<roominfo> PTroominfo,int i,shared_ptr<thread_object>  ptr_thread_object)
{

	int ret =0;
	LOG_INFO<<"hdcomname="<<PTroominfo->harddevice_list[i]->hdcom;
	shared_ptr<comharddevice>  comharddevicePtr	= Gethdcomharddevice(PTroominfo->harddevice_list[i]->hdcom,ptr_thread_object->comharddevice_list);
	if (!comharddevicePtr){	
		LOG_ERROR<<"can't get Gethdcomharddevice ";
		return   -1;
	}
	shared_ptr<comharddevice>  comharddevicePtr_mutex  = Gethdcomharddevice(PTroominfo->harddevice_list[i]->hdcom,g_comharddevice_list);
	if (!comharddevicePtr_mutex){	
		LOG_ERROR<<"can't get comharddevicePtr_mutex";
		return   -1;	
	}	
	shared_ptr<protocol_manager>  PTprotocol_manager = Getprotocol(PTroominfo->harddevice_list[i]->protocol_name,ptr_thread_object->protocol_manager_list);
	if (!PTprotocol_manager){		
		LOG_ERROR<<"Getprotocol error";
		return   -1;
	}	

// here add serial op		
	ret = op_touch_switch_obj_serial_permission_led_clear(PTroominfo,PTprotocol_manager,i,comharddevicePtr_mutex);
	return ret ;	
}



int get_touch_switch_info(Connection  conn,shared_ptr<roominfo> PTroominfo,string hdid)
{

	string sql_cmd  = "select switch_status, manual_permission  from  harddevice  where roomid = "+  lexical_cast<string>(PTroominfo->roomid)+" and hdtype = 5  and hdid ="+hdid;
	LOG_INFO<<"sql_cmd="<<sql_cmd;
	try {
	Query query = conn.query(sql_cmd);
	if (UseQueryResult res = query.use()) {
			// Display header
			cout.setf(ios::left);
			cout << setw(31) << "relay_status" <<
				 	setw(31) << "manual_permission"<<
					 	endl << endl;

			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {
				cout << setw(31) << row["switch_status"] << ' ' <<	
					setw(31) << row["manual_permission"] << ' '<<
					endl;			

			#if 0
			try{	
				PTroominfo->switch_status =   lexical_cast<int >(row["switch_status"]);
			}catch (bad_lexical_cast &e){
				cout <<"error:"<<e.what()<<endl;
				PTroominfo->switch_status =0;
			}	

			#endif 
		
			sscanf(row["switch_status"],"%x",(unsigned  int *)&(PTroominfo->switch_status));
			sscanf(row["manual_permission"],"%x",(unsigned  int *)&(PTroominfo->manual_permission));
			
						
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



#if 0


int op_hddevice_list(Connection  conn ,shared_ptr<roominfo>  PTroominfo,
				shared_ptr<thread_object>  ptr_thread_object)
{

	int ret  = 0; 
	int size = (PTroominfo->harddevice_list).size();
	LOG_INFO<<"size="<<size;
	for (int i=0;i<size;i++){

		cout << "hdid=" <<PTroominfo->harddevice_list[i]->hdid<<endl;
		LOG_INFO<<"hdid=" <<PTroominfo->harddevice_list[i]->hdid;

			// 关空调

					
		if (PTroominfo->harddevice_list[i]->hdtype == "3"  ){

			if ( (PTroominfo->temporary_flag  == true ) || (PTroominfo->op_flag  == true) ){	
			
			cout << "hdtype="<<PTroominfo->harddevice_list[i]->hdtype<<endl;			
			ret = get_aircpara_info(conn,PTroominfo,PTroominfo->harddevice_list[i]->hdid);
			if (ret){				
				LOG_ERROR<<"get_aircpara_info error";
				break;	
			}
			// aircmode = 0  means on 

		       if (PTroominfo->aircmode == 0){
				ret  = op_air_obj(PTroominfo,i,ptr_thread_object);
				if (ret == -1){				
					LOG_ERROR<<"op_air_obj error\n";
					
				}

		       	}

		{
			struct timespec ts;		
			ts.tv_nsec = 0;   
			ts.tv_sec = 1;
			nanosleep(&ts,NULL);	
		}

						

			}
		}
		
					

			//关继电器
		if (PTroominfo->harddevice_list[i]->hdtype == "1002"){
		
			if ( (PTroominfo->temporary_flag  == true ) || (PTroominfo->op_flag  == true) ){
			ret = get_relay_info(conn,PTroominfo);
			if (ret){				
				LOG_ERROR<<"get_relay_info error\n";
				break;	
			}

			if (PTroominfo->relay_status !=0){
			
				ret  = op_relay_obj(PTroominfo,i,ptr_thread_object);
				if (ret ){					
					LOG_ERROR<<"op_relay_obj error";
						
				}
			}

		}

		}


			

				//关触控开关
		if (PTroominfo->harddevice_list[i]->hdtype == "5"){
		
			if ( (PTroominfo->temporary_flag  == true ) || (PTroominfo->op_flag  == true) ){
			ret = get_touch_switch_info(conn,PTroominfo,PTroominfo->harddevice_list[i]->hdid);
			if (ret){				
				LOG_ERROR<<"get_touch_switch_info error";
				break;	
			}

			if (PTroominfo->switch_status !=0){
				
				ret  = op_touch_switch_obj(PTroominfo,i,ptr_thread_object);
				if (ret ){					
					LOG_ERROR<<"op_touch_switch_obj error";					
				}
			}			

		}else {

				/*
					清除禁止灯

				*/

			if (PTroominfo->harddevice_list[i]->hdtype == "5"){
				ret = get_touch_switch_info(conn,PTroominfo,PTroominfo->harddevice_list[i]->hdid);
				if (ret){				
					LOG_ERROR<<"get_touch_switch_info error";
					break;	
				}

				if (PTroominfo->manual_permission !=0xf){
					
					ret  = op_touch_switch_permission_led_obj(PTroominfo,i,ptr_thread_object);
					if (ret ){					
						LOG_ERROR<<"op_touch_switch_obj error";						
					}
				}
			}				


			}
		
	}	

	return 0;
}


#endif 


int op_hddevice_list(Connection  conn ,shared_ptr<roominfo>  PTroominfo,shared_ptr<thread_object>  ptr_thread_object)
{

	int ret  = 0; 
	int size = (PTroominfo->harddevice_list).size();
	LOG_INFO<<"size="<<size;
	for (int i=0;i<size;i++){

		LOG_INFO<<"hdid=" <<PTroominfo->harddevice_list[i]->hdid;

		if (PTroominfo->harddevice_list[i]->hdtype == "3"  ){
			cout <<PTroominfo->temporary_flag<<endl;
			cout <<PTroominfo->op_flag<<endl;	
			
			if ( (PTroominfo->temporary_flag  == true ) || (PTroominfo->op_flag  == true) ){
				ret = get_aircpara_info(conn,PTroominfo,PTroominfo->harddevice_list[i]->hdid);	
				if (ret){				
					LOG_ERROR<<"get_aircpara_info error";
					break;	
				}

				// aircmode = 0  means on 

			       if (PTroominfo->aircmode == 0){
					ret  = op_air_obj(PTroominfo,i,ptr_thread_object);
					if (ret == -1){				
						LOG_ERROR<<"op_air_obj error\n";						
					}
			       	}

				{
					struct timespec ts;		
					ts.tv_nsec = 0;   
					ts.tv_sec = 1;
					nanosleep(&ts,NULL);	
				}
				   
			}
		}



			//关继电器
		if (PTroominfo->harddevice_list[i]->hdtype == "1002"){
			if ( (PTroominfo->temporary_flag  == true ) || (PTroominfo->op_flag  == true) ){
				ret = get_relay_info(conn,PTroominfo);
				if (ret){				
					LOG_ERROR<<"get_relay_info error\n";
					break;	
				}

				if (PTroominfo->relay_status !=0){
				
					ret  = op_relay_obj(PTroominfo,i,ptr_thread_object);
					if (ret ){					
						LOG_ERROR<<"op_relay_obj error";
							
					}
				}
			}

		}


					//关触控开关
		if (PTroominfo->harddevice_list[i]->hdtype == "5"){
			if ( (PTroominfo->temporary_flag  == true ) || (PTroominfo->op_flag  == true) ){

				ret = get_touch_switch_info(conn,PTroominfo,PTroominfo->harddevice_list[i]->hdid);
				if (ret){				
					LOG_ERROR<<"get_touch_switch_info error";
					break;	
				}

				if (PTroominfo->switch_status !=0){					
					ret  = op_touch_switch_obj(PTroominfo,i,ptr_thread_object);
					if (ret ){					
						LOG_ERROR<<"op_touch_switch_obj error";					
					}
				}

			}else {

				ret = get_touch_switch_info(conn,PTroominfo,PTroominfo->harddevice_list[i]->hdid);
				if (ret){				
					LOG_ERROR<<"get_touch_switch_info error";
					break;	
				}

				if (PTroominfo->manual_permission !=0xf){
					
					ret  = op_touch_switch_permission_led_obj(PTroominfo,i,ptr_thread_object);
					if (ret ){					
						LOG_ERROR<<"op_touch_switch_obj error";						
					}
				}

			}
		
		}

		

	}



	return 0;

}






int get_temporary_timetable_isused(Connection  conn,shared_ptr<roominfo> PTroominfo)




{

	string sql_cmd = "select timetable_id from  timetable  where roomid ="+  lexical_cast<string>(PTroominfo->roomid)+" and timetable_type=2 and begin_date <= curdate() and end_date>= curdate() and isused =1;";
	LOG_INFO<< "sql_cmd="<<sql_cmd;
	try {
	Query query = conn.query(sql_cmd);
	if (mysqlpp::StoreQueryResult res = query.store()) {
		cout << "size= "<<res.size()<<endl;
		if (res.size() ==0 ){
			PTroominfo->isused = false ;
		}else {
			PTroominfo->isused = true ;
		}
		mysqlpp::StoreQueryResult::const_iterator it;
		for (it = res.begin(); it != res.end(); ++it) {
			mysqlpp::Row row = *it;
			cout << '\t' << row[0] << endl;
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


int get_normal_timetable_isused(Connection  conn,shared_ptr<roominfo> PTroominfo)
{

	string sql_cmd = "select timetable_id from  timetable  where roomid ="+  lexical_cast<string>(PTroominfo->roomid)+" and timetable_type=1 and begin_date <= curdate() and end_date>= curdate() and isused =1;";
	cout << "sql_cmd="<<sql_cmd<<endl;
	try {
	Query query = conn.query(sql_cmd);
	if (mysqlpp::StoreQueryResult res = query.store()) {
		cout << "size= "<<res.size()<<endl;
		if (res.size() ==0 ){
			PTroominfo->normal_timetable_isused_flag = false ;
		}else {
			PTroominfo->normal_timetable_isused_flag = true ;
		}
		
		mysqlpp::StoreQueryResult::const_iterator it;
		for (it = res.begin(); it != res.end(); ++it) {
			mysqlpp::Row row = *it;
			cout << '\t' << row[0] << endl;
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



int get_normal_timetable_self_study_info(Connection  conn,shared_ptr<roominfo> PTroominfo)
{

	date d = day_clock::local_day();
	int week = d.day_of_week();
	// 周日得到的是0
	if (week == 0){
		week =7;
	}
	string sql_cmd = "select timetable.timetable_id ,timetable_detail.detial_timetable_id ,week from timetable, timetable_detail where roomid ="+  lexical_cast<string>(PTroominfo->roomid)+ " and begin_date <= curdate() and end_date>= curdate()  and begin_time <= now() and end_time>= now() and week= "+ lexical_cast<string>(week)+ "    and timetable.timetable_id=timetable_detail.timetable_id and timetable_type = 1 and lesson_type=3;";
	cout << "sql_cmd="<<sql_cmd<<endl;
	try {
	Query query = conn.query(sql_cmd);
	if (mysqlpp::StoreQueryResult res = query.store()) {
		cout << "size= "<<res.size()<<endl;
		if (res.size() >0 ){
			PTroominfo->normal_timetable_study_self_flag = true;
		}		
		mysqlpp::StoreQueryResult::const_iterator it;
		for (it = res.begin(); it != res.end(); ++it) {
			mysqlpp::Row row = *it;
			cout << '\t' << row[0] << endl;
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



int  op_roominfo_object(Connection  conn ,shared_ptr<roominfo>  PTroominfo,
				shared_ptr<thread_object>  ptr_thread_object)
{
	int ret =0;
	if (PTroominfo == NULL){
		LOG_ERROR<<"PTroominfo is null";
		return  -1;
	}
	
	ret =get_hdid_info(conn,PTroominfo,ptr_thread_object);
	if (ret){
		LOG_ERROR<<"get_hdid_info error";
			return -1;
	}

	ret =get_hdid_op(conn,PTroominfo,ptr_thread_object);
	if (ret){
		LOG_ERROR<<"get_hdid_op error";
		return -1;

	}

	// 如果临时有课程表就优先根据临时课程表操作
	
	get_temporary_timetable_isused(conn,PTroominfo);
	if (PTroominfo->isused ){
		/*    判断是否有自习课    */				
	    ret = get_temporary_timetable_self_study_info(conn,PTroominfo);
		if (PTroominfo->temporary_timetable_study_self_flag){
			/*
				如果是自习课就根据人数来关闭灯光
			*/
			handle_pcount_op(conn,PTroominfo,ptr_thread_object);	
			handle_jnkz_op(conn,PTroominfo,ptr_thread_object);			
		}else {
			
			/*
				无课就关闭灯光和空调
			*/
			
		ret =get_temporary_timetable_info(conn,PTroominfo);	
		if (PTroominfo->temporary_flag ){
			/*
				关灯,关空调,打开禁止灯
			*/
			LOG_INFO<<"temporary_timetable op ";
			op_hddevice_list(conn,PTroominfo,ptr_thread_object);
			}else {
			/*
				关闭禁止灯

			*/
			op_hddevice_list(conn,PTroominfo,ptr_thread_object);

			}
		

		}

	}else {	
		get_normal_timetable_isused(conn,PTroominfo);
		if (PTroominfo->normal_timetable_isused_flag ){
			ret = get_normal_timetable_self_study_info(conn,PTroominfo); 
			if (PTroominfo->normal_timetable_study_self_flag)	{
			/*
				如果是自习课就根据人数来关闭灯光
			*/
			handle_pcount_op(conn,PTroominfo,ptr_thread_object);					
			handle_jnkz_op(conn,PTroominfo,ptr_thread_object);
			}else {
				/*
				无课就关闭灯光和空调
				*/
				ret =get_timetable_info(conn,PTroominfo);	
				if (PTroominfo->op_flag ){
				/*
					关灯,关空调,打开禁止灯
				*/					
					LOG_INFO<<"normal_timetable op ";
					op_hddevice_list(conn,PTroominfo,ptr_thread_object);
				}else {
				/*
				关闭禁止灯
				*/	

					op_hddevice_list(conn,PTroominfo,ptr_thread_object);

				}
			}
		}else {
			PTroominfo->op_flag = false;
			op_hddevice_list(conn,PTroominfo,ptr_thread_object);

		}
	}
	

	/*
		clear harddevice_list of  roominfo 
	*/

	
	free_harddevice_list_of_roominfo(PTroominfo);	
	return ret;
}


int  op_roominfo(Connection  conn,shared_ptr<thread_object>  ptr_thread_object)
{
	
	for (list<shared_ptr<roominfo> >::iterator  pr = ptr_thread_object->roominfo_list.begin(); pr != ptr_thread_object->roominfo_list.end();pr++){
		LOG_INFO<<"roomid="<<(*pr)->roomid<<" "<<"roomname"<< (*pr)->roomname;
		if ((*pr)->roomid   != 0){
			int ret = op_roominfo_object(conn,(*pr),ptr_thread_object);		
			if (ret){
				LOG_ERROR<<"roomid="<<(*pr)->roomid<<" "<<"error  happened ";
			}

		}	
		
	}
	return 0;

}

