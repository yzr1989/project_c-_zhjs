#include <stdio.h> 
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>

#include <curl/curl.h>
#include <curl/easy.h>


#include <fstream> 


#include <mysql/mysql.h>
#include <mysql++.h>

#include <boost/shared_ptr.hpp>
#include <iostream>
#include <boost/lexical_cast.hpp>  
#include <boost/filesystem.hpp>
#include <boost/format.hpp>  
#include <boost/tokenizer.hpp>  
#include <boost/algorithm/string.hpp>  
#include <boost/filesystem.hpp>

#include <boost/format.hpp>  
#include <boost/tokenizer.hpp>  
#include <boost/algorithm/string.hpp>  
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>   

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>


#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "config.h"
#include "utils.h"
#include "parse_json.h"
#include "json_op.h"
#include "json_update.h"
#include "thread_object.h"
#include "get_config_value.h"

using namespace std;
using namespace rapidjson;
using namespace mysqlpp ;
using namespace  boost;  
using namespace boost::filesystem;
using namespace  boost::algorithm;

char  *json_error_msg(const char *ordertype);


struct FtpFile   
{ 
      const   char *filename; 
        FILE *stream; 
}; 

int my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream) 
{ 
        struct FtpFile *out=(struct FtpFile *)stream;
		
        if(out && !out->stream) 
        { 
                out->stream=fopen(out->filename, "wb"); 
                if(!out->stream) 
                return -1; 
        } 
        return fwrite(buffer, size, nmemb, out->stream); 
} 

json_update::json_update()
{
	json_op_name = "update";

	LOG_INFO<<"json_update";
}

json_update::~json_update()
{
	
}

int    json_update::json_encode_update(shared_ptr<thread_object>  ptr_thread_object,string & return_json_string)
{

	string json_string;
	StringBuffer s;
	Writer<StringBuffer> writer(s);
	writer.StartObject();  
	writer.Key("ordertype");   
	writer.String(json_op_name.c_str()); 
	writer.Key("result");   
	writer.String("ok"); 
	writer.EndObject();
	return_json_string= string(s.GetString());
	LOG_INFO<<return_json_string;
	return  0;
	
}








extern "C"   int update_prog_watchdog_name(const  char * progname);


vector<string>    parse_sql_cmd( string   strTag  )
{
	
	 boost::char_separator<char> sep(";");  
	typedef boost::tokenizer<boost::char_separator<char> >  
	CustonTokenizer;  
	CustonTokenizer tok(strTag,sep);  
        vector<string> vecSegTag;  
        for(CustonTokenizer::iterator beg=tok.begin(); beg!=tok.end();++beg)  
        {  
            vecSegTag.push_back(*beg);  
        }  

	return vecSegTag;
}


//create database yourdb DEFAULT CHARACTER SET gbk COLLATE gbk_chinese_ci;


int json_update::create_tables()
{

	
	cout <<sql_path<<endl;
	int filesize;
	
	if (exists(sql_path)){

		 filesize = file_size(sql_path);
		
		cout <<file_size(sql_path)<<endl;

	}	

	char * buffer = new char [filesize];

	

	 ifstream file(sql_path.c_str(),  std::ifstream::binary);
	 if (file){

	 	file.read(buffer, filesize);

	 }

	//cout <<buffer<<endl;


	vector<string>  sql_cmd_vec = parse_sql_cmd(buffer);

/*
	最后sql 一条是空行

*/

	for (uint32_t i =0 ;i < sql_cmd_vec.size()-1;i++){

		//cout <<sql_cmd_vec[i]<<endl;
		mysqlpp::Query query = conn.query(sql_cmd_vec[i]);
		
		if (query.execute()){

		}else {
			cerr << "error: " << query.error() << endl;
			
			return -1;
		}
	}

	


	 file.close();
	delete []buffer;

	return 0;
}

int     json_update::drop_database()
{
		string sql_cmd = "DROP DATABASE  if exists "+mysql_database;


	try {

		conn.connect(NULL, mysql_hostname.c_str(),mysql_user_name.c_str(), mysql_passwd.c_str());

		mysqlpp::Query query = conn.query(sql_cmd);
		
		if (query.execute()){

		}else {
			cerr << "error: " << query.error() << endl;
			
			return -1;
		}	


	}catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		cerr << "Query error: " << er.what() << endl;
		return -1;
		
	}catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		cerr << "Error: " << er.what() << endl;
		return -1;
	}

	return 0;
		
}


int json_update::create_database()
{

	 string   sql_cmd = "create database  "+mysql_database+"  DEFAULT CHARACTER SET gbk COLLATE gbk_chinese_ci";

	conn.connect(NULL, mysql_hostname.c_str(),mysql_user_name.c_str(), mysql_passwd.c_str());

		mysqlpp::Query query = conn.query(sql_cmd);
		
		if (query.execute()){

		}else {
			cerr << "error: " << query.error() << endl;
			
			return -1;
		}	


	return 0;
}


 int   json_update::mysql_update()
{	
	GetProfileString(sys_conf_path.string(), prog_name,"mysql_user_name", mysql_user_name);
	GetProfileString(sys_conf_path.string(), prog_name,"mysql_passwd", mysql_passwd);
	GetProfileString(sys_conf_path.string(), prog_name,"mysql_database_name", mysql_database);
	GetProfileString(sys_conf_path.string(), prog_name,"mysql_hostname", mysql_hostname);

	if (mysql_user_name.empty()){
		mysql_user_name ="root";
	}

	if (mysql_passwd.empty()){
		mysql_passwd ="123456";
	}

	if (mysql_database.empty()){
		mysql_database =prog_name;
	}	

	if (mysql_hostname.empty()){
		mysql_hostname ="localhost";
	}

	drop_database();
	create_database();

	try{

    	 conn.connect(mysql_database.c_str(), mysql_hostname.c_str(),mysql_user_name.c_str(), mysql_passwd.c_str());
	create_tables();
		return 0;
	
	}catch (const ConnectionFailed& err) {
		cerr << "Failed to connect to database server: " <<
		err.what() << endl;		
		
	}			
	create_tables();	

	return 0;

 }


extern "C"  int update_prog_watchdog_name(const  char * progname);

int json_update::do_update_scrip()
{

	path scrip_example_path  = "/etc/rc.d/init.d/example";

	path scrip_path = "/etc/rc.d/init.d/"+prog_name;

	cout <<scrip_example_path<<endl;
	cout <<scrip_path<<endl;
	
	int filesize;
	
	if (exists(scrip_example_path)){

		 filesize = file_size(scrip_example_path);
		
		cout <<file_size(scrip_example_path)<<endl;

	}else {

		return -1;
	}

	char * buffer = new char [filesize];


	 ifstream file(scrip_example_path.c_str(),  std::ifstream::binary);
	 if (file){

	 	file.read(buffer, filesize);

	 }

	//cout <<buffer<<endl;

	string s(buffer);

	 string result_string =   replace_all_copy(s, "example", prog_name) ;

	//cout <<result_string<<endl;

	ofstream fout(scrip_path.c_str(), std::ifstream::binary);

	if (fout.is_open()){
		fout<<result_string; 
		cout <<"write ok"<<endl;
	}else {
		cout<<"fout error"<<endl;

	}	
	
	 file.close();
	 fout.close();
	 delete []buffer;


	chmod(scrip_path.c_str(),777);

	
	//status(scrip_path).permissions(all_all);	

	return 0;
}


int json_update::do_update_local()
{

	/**/	

	   prog_path  = local_path+"/"+prog_name+"/"+prog_name;

	cout <<prog_path  <<endl;

	   conf_path =local_path+"/"+prog_name+"/"+prog_name+".conf";

	cout <<conf_path  <<endl;

	 sql_path  = local_path+"/"+prog_name+"/"+prog_name+".sql";

	cout <<sql_path<<endl;

	if (exists(prog_path) ){

		cout << "copy prog_path"	<<endl;

		path des_path = "/usr/bin/"+prog_name;

		cout <<des_path<<endl;
		
		if (exists(des_path)){
			remove(des_path);			
		}

		
		copy_file(prog_path,des_path);
		chmod(des_path.c_str(),777);
		
		
		do_update_scrip();		
		
	}


	if (exists(conf_path)) {

		cout << "copy prog_path"	<<endl;

		path des_path = "/etc/"+prog_name+".conf";

		cout <<des_path<<endl;
		
		if (exists(des_path)){
			remove(des_path);
			
		}
		copy_file(conf_path,des_path);

	}


	if (exists(sql_path)){
		
		 sys_conf_path = "/etc/"+prog_name+".conf";
		cout <<sys_conf_path<<endl;
	if (exists(sys_conf_path)){

		cout << "sql op"<<endl;

		mysql_update();
	}


	}

	update_prog_watchdog_name(prog_name.c_str());	

	return 0;
}

int  json_update::http_down_file(string  file_name,string url)
{

	 CURL *curl; 
        CURLcode res;  

         struct FtpFile ftpfile={file_name.c_str(),NULL}; //初始化一个FtpFile结构   
     
	
        curl_global_init(CURL_GLOBAL_DEFAULT); 
 
        curl = curl_easy_init(); 
        if(curl) 
        { 
                curl_easy_setopt(curl, CURLOPT_URL,url.c_str());  
 
             
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite); 
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);  
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);  
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile); 
                curl_easy_setopt(curl, CURLOPT_VERBOSE, TRUE);
 
                res = curl_easy_perform(curl); 
                curl_easy_cleanup(curl); 
 
                if(CURLE_OK != res) {
                        fprintf(stderr, "curl told us %d\n", res); 
				
			}
        } 
        if(ftpfile.stream) 
        fclose(ftpfile.stream); 
        curl_global_cleanup(); 


	return res;
}


int  json_update::ftp_down_file(string  file_name,string url)
{

	 CURL *curl; 
        CURLcode res;  

         struct FtpFile ftpfile={file_name.c_str(),NULL}; //初始化一个FtpFile结构   
     
	
        curl_global_init(CURL_GLOBAL_DEFAULT); 
 
        curl = curl_easy_init(); 
        if(curl) 
        { 
                curl_easy_setopt(curl, CURLOPT_URL,url.c_str());  
 
             
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite); 

                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile); 
                curl_easy_setopt(curl, CURLOPT_VERBOSE, TRUE);
 
                res = curl_easy_perform(curl); 
                curl_easy_cleanup(curl); 
 
                if(CURLE_OK != res) {
                        fprintf(stderr, "curl told us %d\n", res); 
				
			}
        } 
        if(ftpfile.stream) 
        fclose(ftpfile.stream); 
        curl_global_cleanup(); 


	return res;
}



int json_update::do_update_http()
{

	local_path="/var/update";

	path dir_path =  local_path+"/"+prog_name;

	cout <<dir_path  <<endl;
	
	if (exists(dir_path) ){

		remove_all(dir_path);
	}
   

	string url = http_path+"/"+prog_name+"/"+prog_name;

	cout <<url<<endl;

	

	path  local_save_path = local_path;

	// mkdir 

	 if (  !exists(local_save_path) ){
		create_directory(local_save_path);
	 }

	local_save_path = local_path+"/"+prog_name;


	if (  !exists(local_save_path) ){
		create_directory(local_save_path);
	 }else {

		if ( ! is_directory(local_save_path)){
			remove(local_save_path);
			create_directory(local_save_path);
		}

	 }

	   prog_path  = local_path+"/"+prog_name+"/"+prog_name;

	cout <<prog_path  <<endl;

	   conf_path =local_path+"/"+prog_name+"/"+prog_name+".conf";

	cout <<conf_path  <<endl;

	 sql_path  = local_path+"/"+prog_name+"/"+prog_name+".sql";


	cout <<sql_path<<endl;

	

	http_down_file(prog_path.string(),url);

	url = http_path+"/"+prog_name+"/"+prog_name+".conf";
	cout <<url<<endl;
	http_down_file(conf_path.string(),url);
	
	url = http_path+"/"+prog_name+"/"+prog_name+".sql";
	cout <<url<<endl;
	http_down_file(sql_path.string(),url);

	return 0;
}


int json_update::do_update_ftp()
{


	local_path="/var/update";

	path dir_path =  local_path+"/"+prog_name;

	cout <<dir_path  <<endl;
	
	if (exists(dir_path) ){

		remove_all(dir_path);
	}
   

	string url = ftp_path+"/"+prog_name+"/"+prog_name;

	cout <<url<<endl;

	

	path  local_save_path = local_path;

	// mkdir 

	 if (  !exists(local_save_path) ){
		create_directory(local_save_path);
	 }

	local_save_path = local_path+"/"+prog_name;


	if (  !exists(local_save_path) ){
		create_directory(local_save_path);
	 }else {

		if ( ! is_directory(local_save_path)){
			remove(local_save_path);
			create_directory(local_save_path);
		}

	 }

	   prog_path  = local_path+"/"+prog_name+"/"+prog_name;

	cout <<prog_path  <<endl;

	   conf_path =local_path+"/"+prog_name+"/"+prog_name+".conf";

	cout <<conf_path  <<endl;

	 sql_path  = local_path+"/"+prog_name+"/"+prog_name+".sql";


	cout <<sql_path<<endl;	

	ftp_down_file(prog_path.string(),url);

	url = ftp_path+"/"+prog_name+"/"+prog_name+".conf";
	cout <<url<<endl;
	ftp_down_file(conf_path.string(),url);
	
	url = ftp_path+"/"+prog_name+"/"+prog_name+".sql";
	cout <<url<<endl;
	ftp_down_file(sql_path.string(),url);

	return 0;
}


int   json_update::json_parse(const char *   json_string,shared_ptr<thread_object>  ptr_thread_object , string &return_string)
{
	
	
	  Document d;
	  d.Parse(json_string);
	   string  ordertype = d["ordertype"].GetString();

	 cout <<	json_op_name <<endl;

	 if ( d.HasMember("prog_name") ) {

	 prog_name = d["prog_name"].GetString();

	cout << prog_name <<endl;
	 }

	 if ( d.HasMember("local_path") ) {

	 local_path = d["local_path"].GetString();

	cout << local_path <<endl;
	do_update_local();
	 }	


	 if ( d.HasMember("http_path") ) {

	 http_path = d["http_path"].GetString();

	cout << http_path <<endl;
	do_update_http();
	do_update_local();
	 }	


	 if ( d.HasMember("ftp_path") ) {

	 ftp_path = d["ftp_path"].GetString();

	cout << ftp_path <<endl;
	do_update_ftp();
	do_update_local();
	 }	
	 

	 json_encode_update(ptr_thread_object,return_string);
	cout <<return_string<<endl;

	return 0;
	
}



void json_update_init(shared_ptr<thread_object>  ptr_thread_object)
{	

	shared_ptr<json_op> ptrjson_update (new json_update());
	
	RegisterJsonOp(ptrjson_update,ptr_thread_object);
}

