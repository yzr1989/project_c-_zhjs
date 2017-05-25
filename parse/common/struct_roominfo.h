#ifndef _STRUCT_ROOMINFO_H_
#define _STRUCT_ROOMINFO_H_

#include <iostream>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

//#include "struct_harddevice.h"
//#include "serial_conf.h"

#include "contextmodel_res.h"
#include "contextmodeldetail_res.h"

using namespace std;
using  namespace boost;



class  harddevice;
class  pcount_op_obj;
class pcount_num_obj ;

class  roominfo  : boost::noncopyable
{
public :
	~roominfo();
	 int   roomid;
	 string roomname;
	 int relay_status;
	 int switch_status;
	 int manual_permission;
	 int hd_temp;
	 int hd_humtime;
	 int hd_light;
	 int light_on;
	 int light_off;
	string airckz;
	string startupschema;
	string tempcompare;
	uint8_t *airckz_hex_buffer;
	int  airckz_hex_length;
	int aircmode;
	bool  isused;
	bool  op_flag;
	bool temporary_flag ;
	bool has_nobody_flag;
	int   modelid;
	int  detailid;
	int hdmode;
	string op_name;
	bool some_flag;
	bool nobody_shutdown_flag;
	bool  normal_control_flag;
	bool temporary_timetable_study_self_flag;
	bool normal_timetable_study_self_flag;
	bool normal_timetable_isused_flag;
	string weekcycle;
	int temperature;
	int temperature_presetting;
	int pcount_num;
	 vector< shared_ptr<harddevice> > harddevice_list;
	// vector <int>  hdid_pcount_list;
	 vector< shared_ptr<pcount_op_obj> > pcount_op_obj_list;
	 vector< shared_ptr<pcount_num_obj> > pcount_num_obj_list;
	  vector <shared_ptr<contextmodel_res> >  contextmodel_vec;
	 vector <shared_ptr<contextmodeldetail_res> >  contextmodeldetail_res_vec;
} ;



int RegisterRoomInfo(shared_ptr<roominfo> ptroominfo,list<shared_ptr<roominfo> >   &roominfo_list);
void ShowRoomInfo(list<shared_ptr<roominfo> > &roominfo_list);
shared_ptr<roominfo>   GetRoomInfo_by_roomid( string  roomid,list<shared_ptr<roominfo> > &roominfo_list);
void free_roominfo_object(shared_ptr<roominfo> PTRoominfo,list<shared_ptr<roominfo> > &roominfo_list);
void freeroominfo(list<shared_ptr<roominfo> > &roominfo_list);
void show_hddevice_within_roominfo(shared_ptr<roominfo> PTroominfo);
void free_harddevice_list_of_roominfo(shared_ptr<roominfo> PTRoominfo);

#endif  /*_STRUCT_ROOMINFO_H_*/