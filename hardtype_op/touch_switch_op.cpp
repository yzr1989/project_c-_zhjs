//#include <iostream>
#include <mysql++.h>
#include <boost/lexical_cast.hpp>
#include <mysql/mysql.h>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "struct_hardtype.h"
#include "struct_harddevice.h"
#include "touch_switch_op.h"

using namespace mysqlpp ;
using namespace std;
using namespace boost;

touch_switch::touch_switch()
{
	hardtype = "5";
	hardtype_name ="touch_switch";
	protocol_name = "ds_bus";
	baudrate = 9600;
	parity = 'N';
	data_bit = 8;
	stop_bit = 1 ;
	file_no =10200 ;	
	LOG_INFO<<"relay";
}

touch_switch::~touch_switch()
{
	
}

struct switch_control{
	uint8_t switch_status:4;
	uint8_t reserve:2;
	uint8_t manual_permission:1;	
	uint8_t ctl_mode:1;
};


int touch_switch::hardtype_op(shared_ptr<harddevice> Ptrhdharddevice, Connection  conn )
{

	char  sql_buffer[200];

	struct switch_control *pswitch_control[4];
	uint8_t  sw_ctl[4];
	string sql_cmd = "select sw_ctl1,sw_ctl2,sw_ctl3,sw_ctl4 from harddevice where hdid="+Ptrhdharddevice->hdid+";";
	cout << "sql_cmd="<<sql_cmd<<endl;	
	Query query = conn.query(sql_cmd);
	if (UseQueryResult res = query.use()) {
			// Display header
			cout.setf(ios::left);
			cout << setw(31) << "sw_ctl1" 
					<<setw(31) << "sw_ctl2"
					<<setw(31) << "sw_ctl3"
					<<setw(31) << "sw_ctl4"
						<<endl << endl;

			// Get each row in result set, and print its contents
		while (Row row = res.fetch_row()) {
				cout << setw(31) << row["sw_ctl1"]  
					 <<setw(31) << row["sw_ctl2"] 
					  <<setw(31) << row["sw_ctl3"]
					  <<setw(31) << row["sw_ctl4"] <<endl;					  

#if 0

			try{
			//PThdparamlist->hdaddr_register  	= lexical_cast<int>(row["hdaddr_register"]);	
			sw_ctl[0]=  lexical_cast<int>(row["sw_ctl1"]);
			}catch (bad_lexical_cast & e) {
				cout<< "error:"<<e.what()<<endl;
				sw_ctl[0]  =0;
			}

			try{
			//PThdparamlist->hdaddr_register  	= lexical_cast<int>(row["hdaddr_register"]);	
			sw_ctl[1]=  lexical_cast<int>(row["sw_ctl2"]);
			}catch (bad_lexical_cast & e) {
				cout<< "error:"<<e.what()<<endl;
				sw_ctl[1]  =0;
			}
			try{
			//PThdparamlist->hdaddr_register  	= lexical_cast<int>(row["hdaddr_register"]);	
			sw_ctl[2]=  lexical_cast<int>(row["sw_ctl3"]);
			}catch (bad_lexical_cast & e) {
				cout<< "error:"<<e.what()<<endl;
				sw_ctl[2]  =0;
			}
			try{
			//PThdparamlist->hdaddr_register  	= lexical_cast<int>(row["hdaddr_register"]);	
			sw_ctl[3]=  lexical_cast<int>(row["sw_ctl4"]);
			}catch (bad_lexical_cast & e) {
				cout<< "error:"<<e.what()<<endl;
				sw_ctl[3]  =0;
			}


#endif 

			sscanf(row["sw_ctl1"],"%x",(unsigned  int *)&sw_ctl[0]);
			sscanf(row["sw_ctl2"],"%x",(unsigned  int *)&sw_ctl[1]);
			sscanf(row["sw_ctl3"],"%x",(unsigned  int *)&sw_ctl[2]);
			sscanf(row["sw_ctl4"],"%x",(unsigned  int *)&sw_ctl[3]);
			
				
		}

			// Check for error: can't distinguish "end of results" and
			// error cases in return from fetch_row() otherwise.
			if (conn.errnum()) {
				cerr << "Error received in fetching a row: " <<
						conn.error() << endl;
				return -1;
			}
			
		}	


		//cout <<"sw_ctl1="<<(int )sw_ctl1<<endl;		
		pswitch_control[0] = (struct switch_control *)&sw_ctl[0];
		pswitch_control[1] = (struct switch_control *)&sw_ctl[1];
		pswitch_control[2] = (struct switch_control *)&sw_ctl[2];
		pswitch_control[3] = (struct switch_control *)&sw_ctl[3];				
	
	printf("switch_status1=%u\n",pswitch_control[0]->switch_status);
	printf("manual_permission1=%u\n",pswitch_control[0]->manual_permission);
	printf("ctl_mode1=%u\n",pswitch_control[0]->ctl_mode);
	
	printf("switch_status2=%u\n",pswitch_control[1]->switch_status);
	printf("manual_permission2=%u\n",pswitch_control[1]->manual_permission);
	printf("ctl_mode2=%u\n",pswitch_control[1]->ctl_mode);
	
	printf("switch_status3=%u\n",pswitch_control[2]->switch_status);
	printf("manual_permission3=%u\n",pswitch_control[2]->manual_permission);
	printf("ctl_mode3=%u\n",pswitch_control[2]->ctl_mode);
	
	printf("switch_status4=%u\n",pswitch_control[3]->switch_status);
	printf("manual_permission4=%u\n",pswitch_control[3]->manual_permission);
	printf("ctl_mode4=%u\n",pswitch_control[3]->ctl_mode);


/*
	int switch_status:4;
	int reserve:2;
	int manual_permission:1;
	int ctl_mode:1;

*/
	//uint16_t manual_permission=0;
	//uint16_t ctl_mode  =0;
	uint16_t switch_status=0;
//	uint16_t switch_status = ((pswitch_control[0]->switch_status)<<0)|((pswitch_control[1]->switch_status)<<4)|((pswitch_control[2]->switch_status)<<8)|((pswitch_control[3]->switch_status)<<12);


/*
	switch_status  [0]       	1  开 0 关
				[1]		1  开 0 关
				[2]		1  开 0 关
				[3]		1  开 0 关
*/
	
	if (((pswitch_control[0]->switch_status)&0xf) == 0xf){        //  f 为闭合
		switch_status |= (1<<0);
	}else if (((pswitch_control[0]->switch_status)&0xf) == 0){     //上电断开
		switch_status &= ~(1<<0);
	}else if (((pswitch_control[0]->switch_status)&0xf )== 1){   // 断开
		switch_status &= ~(1<<0);
	}

	
	if (((pswitch_control[1]->switch_status)&0xf) == 0xf){        //  f 为闭合
		switch_status |= (1<<1);
	}else if (((pswitch_control[1]->switch_status)&0xf )== 0){     //上电断开
		switch_status &= ~(1<<1);
	}else if (((pswitch_control[1]->switch_status)&0xf) == 1){   // 断开
		switch_status &= ~(1<<1);
	}	


	if (((pswitch_control[2]->switch_status)&0xf) == 0xf){        //  f 为闭合
		switch_status |= (1<<2);
	}else if (((pswitch_control[2]->switch_status)&0xf) == 0){     //上电断开
		switch_status &= ~(1<<2);
	}else if (((pswitch_control[2]->switch_status)&0xf )== 1){   // 断开
		switch_status &= ~(1<<2);
	}

	
	if (((pswitch_control[3]->switch_status)&0xf )== 0xf){        //  f 为闭合
		switch_status |= (1<<3);
	}else if (((pswitch_control[3]->switch_status)&0xf )== 0){     //上电断开
		switch_status &= ~(1<<3);
	}else if (((pswitch_control[3]->switch_status)&0xf )== 1){   // 断开
		switch_status &= ~(1<<3);
	}	

	char switch_status_buffer[100];
	sprintf(switch_status_buffer,"%x",switch_status);

	/*
		manual_permission   [0]   1 手动  2 远程
						 
	*/

	char manual_permission_buffer[100];
	uint16_t manual_permission = ((pswitch_control[0]->manual_permission)<<0)|((pswitch_control[1]->manual_permission)<<1)|((pswitch_control[2]->manual_permission)<<2)|((pswitch_control[3]->manual_permission)<<3);
	sprintf(manual_permission_buffer,"%x",manual_permission);

	/*
		ctl_mode  [0]     1手动权限 0无手动权限
	*/

	char ctl_mode_buffer[100];
	uint16_t ctl_mode =  ((pswitch_control[0]->ctl_mode)<<0)|((pswitch_control[1]->ctl_mode)<<1)|((pswitch_control[2]->ctl_mode)<<2)|((pswitch_control[3]->ctl_mode)<<3);
	sprintf(ctl_mode_buffer,"%x",ctl_mode);
	//sql_cmd = "update harddevice set switch_status='"+lexical_cast<string>(switch_status)+"'  ,  manual_permission='"+lexical_cast<string>(manual_permission) +"'  , ctl_mode='"+lexical_cast<string>(ctl_mode)+"'   where hdid="+Ptrhdharddevice->hdid;
	sprintf(sql_buffer,"update harddevice set switch_status='%x' ,  manual_permission='%x' , ctl_mode='%x'  where hdid=%s",switch_status,manual_permission,ctl_mode,Ptrhdharddevice->hdid.c_str());
	

	cout<<"cmd="<<sql_buffer<<endl;
	query = conn.query(sql_buffer);
	if (query.execute()){
	}else {
		cerr << "error: " << query.error() << endl;
		return -1;
	}


#if 0
	
	sql_cmd = "update harddevice set switch_status2='"+lexical_cast<string>((int )pswitch_control[1]->switch_status)+"'  ,  manual_permission2='"+lexical_cast<string>((int )pswitch_control[1]->manual_permission) +"'  , ctl_mode2='"+lexical_cast<string>((int )pswitch_control[1]->ctl_mode)+"'   where hdid="+Ptrhdharddevice->hdid;
	
	cout<<"cmd="<<sql_cmd<<endl;
	
	query = conn.query(sql_cmd);
	
	if (query.execute()){

	}else {
		cerr << "error: " << query.error() << endl;
		return -1;
	}


	sql_cmd = "update harddevice set switch_status3='"+lexical_cast<string>((int )pswitch_control[2]->switch_status)+"'  ,  manual_permission3='"+lexical_cast<string>((int )pswitch_control[2]->manual_permission) +"'  , ctl_mode3='"+lexical_cast<string>((int )pswitch_control[2]->ctl_mode)+"'   where hdid="+Ptrhdharddevice->hdid;
	
	cout<<"cmd="<<sql_cmd<<endl;
	
	query = conn.query(sql_cmd);
	
	if (query.execute()){

	}else {
		cerr << "error: " << query.error() << endl;
		return -1;
	}


	sql_cmd = "update harddevice set switch_status4='"+lexical_cast<string>((int )pswitch_control[3]->switch_status)+"'  ,  manual_permission4='"+lexical_cast<string>((int )pswitch_control[3]->manual_permission) +"'  , ctl_mode4='"+lexical_cast<string>((int )pswitch_control[3]->ctl_mode)+"'   where hdid="+Ptrhdharddevice->hdid;
	
	cout<<"cmd="<<sql_cmd<<endl;
	
	query = conn.query(sql_cmd);
	
	if (query.execute()){

	}else {
		cerr << "error: " << query.error() << endl;
		return -1;
	}
#endif 
	  return 0;
}


int touch_switch_init(list<shared_ptr<hardtype> > &hardtype_list)
{
	shared_ptr<hardtype> Ptr_touch_switch ( new  touch_switch());
	Registerhardtype( Ptr_touch_switch,hardtype_list);
	return 0;	
}


