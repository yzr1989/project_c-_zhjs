#ifndef  _JSON_UPDATE_H
#define _JSON_UPDATE_H


#include <mysql/mysql.h>
#include <mysql++.h>
#include <boost/noncopyable.hpp>
#include <boost/filesystem.hpp>


using namespace mysqlpp ;
using namespace boost::filesystem;

class json_op;

class  json_update :  public  json_op ,boost::noncopyable
{

private:
	string prog_name;
	string local_path;
	string http_path;
	string ftp_path;
	path   prog_path;
	path   conf_path;
	path sql_path ;
	path sys_conf_path;
	Connection conn;
	string mysql_user_name;
	string  mysql_passwd;
	string mysql_database;	
	string mysql_hostname;	
public :

	json_update();
	~json_update();
	 int     json_parse(const char *   json_string,shared_ptr<thread_object>  ptr_thread_object , string &return_string);
    	int    json_encode_update(shared_ptr<thread_object>  ptr_thread_object,string & return_json_string);

	int do_update_local();
	
	 int   mysql_update();
	 int create_tables();
	 int create_database();

	int drop_database();
	int do_update_scrip();
	int do_update_http();
	int do_update_ftp();
	int  ftp_down_file(string  file_name,string url);
	int  http_down_file(string  file_name,string url);
	
};



#endif  /*_JSON_UPDATE_H*/
