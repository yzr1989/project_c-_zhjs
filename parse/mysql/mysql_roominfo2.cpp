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
#include <mysql++/ssqls.h>
#include <boost/lexical_cast.hpp>
#include <boost/date_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/lexical_cast.hpp>  
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "get_config_value.h"
#include "config.h"
#include "struct_roominfo.h"
#include "serial_conf.h"
#include "protocol_manager.h"
#include "sting2hex.h"
#include "parse_bit_field.h"
#include "thread_object.h"
#include "struct_roominfo.h"
#include "config.h"
#include "serial_conf.h"
#include "protocol_manager.h"
#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "struct_comharddevice.h"
#include "struct_hdparamlist.h"
#include "struct_roominfo.h"



#include "contextmodel.h"
#include "contextmodeldetail.h"

using namespace std;
using namespace mysqlpp ;
using namespace boost;
using namespace boost::gregorian;

using namespace boost::posix_time;



#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

extern list<shared_ptr<comharddevice> > g_comharddevice_list;

void asyncOutput(const char* msg, int len);


#define OP_ON    1
#define OP_OFF  2 



static int get_hdid_info(Connection  conn,shared_ptr<roominfo> PTroominfo,shared_ptr<thread_object>  ptr_thread_object);

 int get_aircpara_info(Connection  conn,shared_ptr<roominfo> PTroominfo);

 int op_air_obj(shared_ptr<roominfo> PTroominfo,int i,shared_ptr<thread_object>  ptr_thread_object);

static int op_hddevice_list(Connection  conn ,shared_ptr<roominfo>  PTroominfo,
				shared_ptr<thread_object>  ptr_thread_object);


#if 0


 static   int   get_roominfo_from_mysql(Connection  conn ,shared_ptr<thread_object>  ptr_thread_object)
{

	muduo::Logger::setOutput(asyncOutput);

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


#endif 



static int get_hdid_info(Connection  conn,shared_ptr<roominfo> PTroominfo,shared_ptr<thread_object>  ptr_thread_object)
{

	
	/*no one*/
	/*hdtype need to fixup*/
	/*  hdtype=2==>  hdtype=4  */

	/*

		hd_right     1    手动控制
		hd_right     0     自动控制     


	*/

	string sql_cmd = "select   hdid from harddevice  where     roomid ="+  lexical_cast<string>( PTroominfo->roomid) + " ;";


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

static  int   three_phase_meter_hardtype_op(shared_ptr<harddevice> Ptrhdharddevice,shared_ptr<roominfo> PTroominfo, Connection  conn)
{

	



	LOG_INFO<<"three_phase_meter op ";

	string sql_cmd =  "select hdmode from harddevice   where hdid="+ lexical_cast<string>(Ptrhdharddevice->hdid);

	
	LOG_INFO<<"sql_cmd ="<<sql_cmd;
 
	
	
	
	try {

	Query query = conn.query(sql_cmd);

	if (UseQueryResult res = query.use()) {


			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {


			LOG_TRACE<<"hdmode:"<<string(row["hdmode"]);

			try{

				PTroominfo->hdmode = lexical_cast<int >( row["hdmode"]);
			}catch (bad_lexical_cast &e  ){

					LOG_ERROR<<"error:"<<e.what();
					PTroominfo->hdmode = 0;
			}
			 

			LOG_INFO<<"hdmode="<<PTroominfo->hdmode;
 				
		}

			// Check for error: can't distinguish "end of results" and
			// error cases in return from fetch_row() otherwise.
			if (conn.errnum()) {

				LOG_ERROR<<"Error received in fetching a row: " <<
						conn.error();
				return  -1;
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




static int   single_phase_meter_op(shared_ptr<harddevice> Ptrhdharddevice,shared_ptr<roominfo> PTroominfo, Connection  conn)
{

	



	LOG_INFO<<"single_phase_meter op ";

	string sql_cmd =  "select hdmode from harddevice   where hdid="+ lexical_cast<string>(Ptrhdharddevice->hdid);

	
	LOG_INFO<<"sql_cmd ="<<sql_cmd;
 	
	
	
	
	try {

	Query query = conn.query(sql_cmd);

	if (UseQueryResult res = query.use()) {


			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {


			LOG_TRACE<<"hdmode:"<<string(row["hdmode"]);

			try{

				PTroominfo->hdmode = lexical_cast<int >( row["hdmode"]);
			}catch (bad_lexical_cast &e  ){

					LOG_ERROR<<"error:"<<e.what();
					PTroominfo->hdmode = 0;
			}


			 

			LOG_INFO<<"hdmode="<<PTroominfo->hdmode;
 				
		}

			// Check for error: can't distinguish "end of results" and
			// error cases in return from fetch_row() otherwise.
			if (conn.errnum()) {

				LOG_ERROR<<"Error received in fetching a row: " <<
						conn.error();
				return  -1;
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


static int op_relay_obj_serial(shared_ptr<roominfo> PTroominfo,shared_ptr<protocol_manager>  PTprotocol_manager,int i,
 			shared_ptr<comharddevice>  comharddevicePtr_mutex,int op_name)

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
	if (op_name == OP_ON){
		ret = PTprotocol_manager->relay_on(PTroominfo->harddevice_list[i]);	
	}else if (op_name == OP_OFF){
		ret = PTprotocol_manager->relay_off(PTroominfo->harddevice_list[i]);
	}

	PTprotocol_manager->protocol_close(PTroominfo->harddevice_list[i]);	
	
	{
		struct timespec ts;		
		ts.tv_nsec = 0;   
		ts.tv_sec = 1;
		nanosleep(&ts,NULL);				
	}	
	
	return ret ;
}


/* 
	op_name 
		on  1 
		off  2 

*/


static int op_relay_obj(shared_ptr<roominfo> PTroominfo,int i,shared_ptr<thread_object>  ptr_thread_object,int op_name)
{

	
	int ret =0;

	LOG_INFO<<"hdcomname="<<PTroominfo->harddevice_list[i]->hdcom;


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
	ret = op_relay_obj_serial(PTroominfo,PTprotocol_manager,i,comharddevicePtr_mutex,op_name);
	return ret ;
	
}


static int op_hddevice_list(Connection  conn ,shared_ptr<roominfo>  PTroominfo,
				shared_ptr<thread_object>  ptr_thread_object)
{
	int size = (PTroominfo->harddevice_list).size();
	
	LOG_INFO<<"size="<<size;
	for (int i=0;i<size;i++){


		LOG_INFO<<"hdid=" <<PTroominfo->harddevice_list[i]->hdid;

		
		//判断单相相表继电器是否打开
		if (PTroominfo->harddevice_list[i]->hdtype == "1000"){
			LOG_INFO<<"op :"<<PTroominfo->op_name<<"  hdid="<<PTroominfo->harddevice_list[i]->hdid;	
			single_phase_meter_op(PTroominfo->harddevice_list[i],PTroominfo,conn);

			// 需要打开而没有打开 发送打开命令
			if ( (PTroominfo->op_name == "on" ) && ( PTroominfo->hdmode == 1) ){
				LOG_INFO  << "on op";
				op_relay_obj(PTroominfo,i,ptr_thread_object,OP_ON);

			}


			//需要关闭而没有关闭发送关闭命令

			if ((PTroominfo->op_name == "off" ) && ( PTroominfo->hdmode == 0) ){

				LOG_INFO  << "on off";
				op_relay_obj(PTroominfo,i,ptr_thread_object,OP_OFF);
			}
			

			
		}


			//判断三相表继电器是否打开
		if (PTroominfo->harddevice_list[i]->hdtype == "1001"){
		LOG_INFO<<"op :"<<PTroominfo->op_name<<"  hdid="<<PTroominfo->harddevice_list[i]->hdid;		
			three_phase_meter_hardtype_op(PTroominfo->harddevice_list[i],PTroominfo,conn);
			// hdmode  1 关闭  拉闸 
			// hdmode  0 打开 合闸

			// 需要打开而没有打开 发送打开命令
			if (PTroominfo->op_name == "on"  &&  PTroominfo->hdmode == 1 ){
				LOG_INFO  << "op on ";
				op_relay_obj(PTroominfo,i,ptr_thread_object,OP_ON);

			}


			//需要关闭而没有关闭发送关闭命令

			if (PTroominfo->op_name == "off"  &&  PTroominfo->hdmode == 0 ){

				LOG_INFO  << "op off";
				op_relay_obj(PTroominfo,i,ptr_thread_object,OP_OFF);
			}
			
		}

		
		
	}	

	return 0;
}









static int get_weekcycle_info(Connection  conn ,shared_ptr<roominfo>  PTroominfo)
{
	string sql_cmd ;


	LOG_INFO<<"get_weekcycle_info";		

	 sql_cmd =  "select weekcycle ,contextmodel.modelid     from     contextmodel, contextmodelrooms ,contextmodeldetail where roomid ="+  lexical_cast<string>( PTroominfo->roomid)+"   and contextmodelrooms.modelid =contextmodel.modelid   and dtbegin <= curdate() and dtend>= curdate() and isused =1  group by modelid ;";	
		
	LOG_INFO<<sql_cmd;

	try {


		// Retrieve a subset of the stock table's columns, and store
		// the data in a vector of 'stock' SSQLS structures.  See the
		// user manual for the consequences arising from this quiet
		// ability to store a subset of the table in the stock SSQLS.
		mysqlpp::Query query = conn.query(sql_cmd);
		vector<contextmodel> res;
		query.storein(res);

		// Display the items
		
		vector<contextmodel>::iterator it;
		for (it = res.begin(); it != res.end(); ++it) {
			cout << '\t' << it->modelid<<"\t"<<it->weekcycle<<"\t"<<endl;

			//contextmodel_res res(it->modelid,it->weekcycle);
			//shared_ptr<contextmodel_res> PTcontextmodel_res ( new contextmodel_res( ));
			shared_ptr<contextmodel_res>   PTcontextmodel_res(new contextmodel_res( ) );

			PTcontextmodel_res->modelid = it->modelid;
			PTcontextmodel_res->weekcycle = it->weekcycle;
		
			PTroominfo->contextmodel_vec.push_back(PTcontextmodel_res);
			
		}

		
		
	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions; e.g. type mismatch populating 'stock'
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


	cout << "contextmodel_vec  size="<<PTroominfo->contextmodel_vec.size() <<endl;

	return 0;

}


static bool  check_week_enabled(string bin_string,int week)
{
	char string_line[8];	
	bool  temp  =false;

	memcpy(string_line,bin_string.c_str(),sizeof(string_line));
	
	//printf("%s\n",string_line);	

	//cout << string(string_line)<<endl;

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


static  int    get_contextmodeldetail_info(Connection  conn ,shared_ptr<roominfo>  PTroominfo , int modelid)
{

	LOG_INFO<<"get_time_info";

	string sql_cmd  ;

	sql_cmd  =   "select  detailid , bgtime ,edtime ,ctrltype from  contextmodeldetail  where modelid="+lexical_cast<string>(modelid);

	try {


		// Retrieve a subset of the stock table's columns, and store
		// the data in a vector of 'stock' SSQLS structures.  See the
		// user manual for the consequences arising from this quiet
		// ability to store a subset of the table in the stock SSQLS.
		mysqlpp::Query query = conn.query(sql_cmd);
		vector<contextmodeldetail> res;
		query.storein(res);

		// Display the items
		
		vector<contextmodeldetail>::iterator it;
		for (it = res.begin(); it != res.end(); ++it) {
			cout << '\t' << it->detailid<<"\t"<<it->bgtime<<"\t"<<it->edtime<<"\t"<<it->ctrltype<<"\t"<<endl;


			
			shared_ptr<contextmodeldetail_res>   PTcontextmodeldetail_res(new contextmodeldetail_res( ) );

			PTcontextmodeldetail_res->detailid = it->detailid;
			//PTcontextmodeldetail_res->bgtime = it->bgtime;
			//PTcontextmodeldetail_res->edtime = it->edtime;

			int  hours_bg =    static_cast<int> ( (it->bgtime).hour());
			int  minute_bg =    static_cast<int> ( (it->bgtime).minute());
			int  second_bg =    static_cast<int> ( (it->bgtime).second());
	
			cout <<"hours_bg:"<<hours_bg<<endl;
				
			int hours_end = static_cast<int> ( (it->edtime).hour());
			int  minute_end =    static_cast<int> ( (it->edtime).minute());
			int  second_end =    static_cast<int> ( (it->edtime).second());
			
			cout  <<"hours_end"<<hours_end<<endl;

			date today = boost::gregorian::day_clock::local_day();


			ptime  bgtime = ptime(today,hours(hours_bg)+minutes(minute_bg)+seconds(second_bg));
			ptime  edtime = ptime(today,hours(hours_end)+minutes(minute_end)+seconds(second_end));

			PTcontextmodeldetail_res->bgtime = ptime(today,hours(hours_bg)+minutes(minute_bg)+seconds(second_bg));

			PTcontextmodeldetail_res->edtime = ptime(today,hours(hours_end)+minutes(minute_end)+seconds(second_end));


			cout << PTcontextmodeldetail_res->bgtime<<endl;
			cout << PTcontextmodeldetail_res->edtime <<endl;

			/*
				如果当前时间

			*/


			ptime p1 = second_clock::local_time();
			


			
			if ((hours_bg  >hours_end) && (p1 > bgtime)) {
				//  end  日期 加一天
				PTcontextmodeldetail_res->edtime = ptime(today+days(1),hours(hours_end)+minutes(minute_end)+seconds(second_end));


			}else if ((hours_bg  >hours_end) && (p1 < edtime)) {

				// begin 日期 减一天
				PTcontextmodeldetail_res->bgtime = ptime(today-days(1),hours(hours_bg)+minutes(minute_bg)+seconds(second_bg));
	
		
			}
			

			cout << PTcontextmodeldetail_res->bgtime<<endl;
			cout << PTcontextmodeldetail_res->edtime <<endl;
			
			
			PTcontextmodeldetail_res->ctrltype = it->ctrltype;
			
			PTroominfo->contextmodeldetail_res_vec.push_back(PTcontextmodeldetail_res);

			
			
		}
		
		
	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions; e.g. type mismatch populating 'stock'
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


	cout << "contextmodel_vec  size="<<PTroominfo->contextmodel_vec.size() <<endl;	

	return 0;

}

int check_week(Connection  conn ,shared_ptr<roominfo>  PTroominfo)

{
	date d = day_clock::local_day();
	int week = d.day_of_week();

	if (week == 0){
		week =7;
	}

	LOG_INFO <<"week="<<week;




	for ( uint32_t  i =  0 ;i <PTroominfo->contextmodel_vec.size();i++){

		cout <<"weekcycle="<<PTroominfo->contextmodel_vec[i]->weekcycle<<endl;
		PTroominfo->contextmodel_vec[i]->enabled = check_week_enabled(PTroominfo->contextmodel_vec[i]->weekcycle,week);

		cout << "enabled:"<<PTroominfo->contextmodel_vec[i]->enabled <<endl;
		
		if (PTroominfo->contextmodel_vec[i]->enabled){


		get_contextmodeldetail_info( conn , PTroominfo,PTroominfo->contextmodel_vec[i]->modelid);
			
		}

	}


	return 0;
	
}



static int check_time(Connection  conn ,shared_ptr<roominfo>  PTroominfo)
{


	LOG_INFO<<"check_time";

	bool op_flag =false;

	for (uint32_t i = 0 ;i < PTroominfo->contextmodeldetail_res_vec.size();i++){

		ptime p1 = second_clock::local_time();

		cout <<p1<<endl;

		cout <<PTroominfo->contextmodeldetail_res_vec[i]->bgtime<<endl;
		cout <<PTroominfo->contextmodeldetail_res_vec[i]->edtime<<endl;
		
		time_period tp1(PTroominfo->contextmodeldetail_res_vec[i]->bgtime,PTroominfo->contextmodeldetail_res_vec[i]->edtime);

		cout<<tp1.contains(p1)<<endl;

		
		
		//PTroominfo->op_flag = tp1.contains(p1);
		op_flag = tp1.contains(p1);

		if (op_flag){
		PTroominfo->op_flag = op_flag; 
		PTroominfo->op_name = PTroominfo->contextmodeldetail_res_vec[i]->ctrltype;

		cout <<"op_flag:"<<PTroominfo->op_flag<<endl;
		cout <<"op_name:"<<PTroominfo->op_name<<endl;

		}

	}


	return 0;
	
}



static int handle_jnkz_op(Connection  conn ,shared_ptr<roominfo>  PTroominfo,
				shared_ptr<thread_object>  ptr_thread_object)
{
	
	LOG_INFO<< "handle_jnkz_op ";

	get_weekcycle_info(conn,PTroominfo);
	check_week(conn,PTroominfo);

	check_time(conn,PTroominfo);
	if (PTroominfo->op_flag ){				
				
		LOG_INFO<<"normal_timetable op ";
		op_hddevice_list(conn,PTroominfo,ptr_thread_object);

	}

	return 0;
}




static int  op_roominfo_object(Connection  conn ,shared_ptr<roominfo>  PTroominfo,
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




	handle_jnkz_op(conn,PTroominfo,ptr_thread_object);


	/*
		clear harddevice_list of  roominfo 
	*/

	
	free_harddevice_list_of_roominfo(PTroominfo);	
	return ret;
}


int  op_roominfo2(Connection  conn,shared_ptr<thread_object>  ptr_thread_object)
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

