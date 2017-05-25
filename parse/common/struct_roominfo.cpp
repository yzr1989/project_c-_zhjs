#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <deque> 
#include <list>
#include <string>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "struct_roominfo.h"
#include "struct_harddevice.h"

int RegisterRoomInfo(shared_ptr<roominfo> ptroominfo,list<shared_ptr<roominfo> > &roominfo_list)
{
	roominfo_list.push_back(ptroominfo);
	return 0;
}



void ShowRoomInfo(list<shared_ptr<roominfo> > &roominfo_list)
{

	cout<<"ShowRoomInfo"<<endl;
	list<shared_ptr<roominfo> >::iterator pr;  	
	for (pr = roominfo_list.begin(); pr != roominfo_list.end();pr++){
		LOG_INFO<<"roomid="<<(*pr)->roomid<<" "<<"roomname"<< (*pr)->roomname ;		
	}
	
}



shared_ptr<roominfo>   GetRoomInfo_by_roomid(int  roomid,list<shared_ptr<roominfo> > &roominfo_list)
{
	list<shared_ptr<roominfo> >::iterator pr;   
	shared_ptr<roominfo> ptr_roominfo;
	if (roomid == 0){
		return ptr_roominfo;	
	}
	for (pr = roominfo_list.begin(); pr != roominfo_list.end();pr++){	
		if ((*pr)->roomid == roomid){		
			LOG_INFO<<"roomid="<<(*pr)->roomid;
			return *pr;
		}		
	}	
	return ptr_roominfo;
}	

void freeroominfo(list<shared_ptr<roominfo> > &roominfo_list)
{
	roominfo_list.clear();
}


void free_harddevice_list_of_roominfo(shared_ptr<roominfo> PTRoominfo)
{
	PTRoominfo->harddevice_list.clear();
}


void show_hddevice_within_roominfo(shared_ptr<roominfo> PTroominfo)
{
	LOG_INFO<<"show_hddevice_within_roominfo";
	int size = (PTroominfo->harddevice_list).size();
	cout << "size="<<size<<endl;
	for (int i=0;i<size;i++){
	      LOG_INFO <<"hdid="<<PTroominfo->harddevice_list[i]->hdid<<" "<<"hdtype="<<PTroominfo->harddevice_list[i]->hdtype;
	}
	
}

roominfo::~roominfo()
{
	LOG_INFO<<"roomid="<<roomid;
}